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

}


void read_and_modify(__sigval_t sigval)
{
    
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

    while ()

    for (; file_counter > 1; file_counter--)
        if (close(fd[file_counter - 1]) == -1)
            ERR("close");

    for (int i = 0; i < parallelism; i++)
        free(buffer[i]);

    return EXIT_SUCCESS;
}