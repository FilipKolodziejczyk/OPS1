#include "task4.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <aio.h>
#include <signal.h>
#include <pthread.h>

const int num_of_args = 2;
const int parallelism = 3;
const int kb = 1000;
const int max_files_opened = 50;

void write_to_file(__sigval_t sigval)
{
    attrib *attr = (attrib *)sigval.sival_ptr;

    int errval = aio_error(attr->aiocb);
    if (errval == -1)
        ERR("aio_error");
    else if (errval != 0)
    {
        fprintf(stderr, "%d\n", errval);
        exit(EXIT_FAILURE);
    }

    if (aio_return(attr->aiocb) == -1)
        ERR("aio_return");

    pthread_mutex_lock(attr->aio_progress_mx);
    (*attr->aio_progress)++;
    pthread_mutex_unlock(attr->aio_progress_mx);

    pthread_kill(attr->main_tid, SIGUSR1);
}

void read_and_modify(__sigval_t sigval)
{
    attrib *attr = (attrib *)sigval.sival_ptr;

    int errval = aio_error(attr->aiocb);
    if (errval == -1)
        ERR("aio_error");
    else if (errval != 0)
    {
        perror("aio_error");
        exit(EXIT_FAILURE);
    }

    int retval = aio_return(attr->aiocb);
    if (retval == -1)
        ERR("aio_return");

    char *buf = (char *)attr->aiocb->aio_buf;
    for (int i = 0; i < retval; i++)
        if (is_vowel(buf[i]))
            buf[i] = ' ';

    aiocb_init(attr->aiocb, buf, attr->aiocb->aio_fildes, retval, 0, write_to_file);
    attr->aiocb->aio_sigevent.sigev_value.sival_ptr = attr;
    aio_write(attr->aiocb);
}

int main(int argc, char **argv)
{
    if (argc != num_of_args + 1)
        usage();

    char *dirname = argv[1];
    int bufsize = atoi(argv[2]);
    if (bufsize < 1 || bufsize > 10)
        usage();

    int fd[max_files_opened];
    int file_counter;

    open_files(dirname, fd, &file_counter, max_files_opened, bufsize * kb);

    pthread_t main_tid = pthread_self();
    int aio_progress = 0;
    pthread_mutex_t aio_progress_mx = PTHREAD_MUTEX_INITIALIZER;

    sethandler(SIG_IGN, SIGUSR1);
    sigset_t new_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    if (pthread_sigmask(SIG_BLOCK, &new_mask, NULL) != 0)
        ERR("pthread_sigmask");

    char *buffer[parallelism];
    for (int i = 0; i < parallelism; i++)
        if ((buffer[i] = malloc(bufsize * kb)) == NULL)
            ERR("malloc");

    struct aiocb aiocbs[parallelism];
    attrib attr[parallelism];
    for (int i = 0; i < parallelism; i++)
    {
        aiocb_init(&aiocbs[i], buffer[i], fd[i], bufsize * kb, 0, read_and_modify);
        attrib_init(&attr[i], &aiocbs[i], main_tid, &aio_progress, &aio_progress_mx);
        aiocbs[i].aio_sigevent.sigev_value.sival_ptr = &attr[i];
        aio_read(&aiocbs[i]);
    }

    while (1)
    {
        if (sigwaitinfo(&new_mask, NULL) == -1)
            ERR("sigwait");
        pthread_mutex_lock(&aio_progress_mx);
        printf("%d\n", aio_progress);
        if (aio_progress == parallelism)
            break;
        pthread_mutex_unlock(&aio_progress_mx);
    }
    pthread_mutex_unlock(&aio_progress_mx);

    for (; file_counter > 1; file_counter--)
        if (close(fd[file_counter - 1]) == -1)
            ERR("close");

    for (int i = 0; i < parallelism; i++)
        free(buffer[i]);

    return EXIT_SUCCESS;
}