#include "task4.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <aio.h>
#include <string.h>
#include <signal.h>

void usage()
{
    fprintf(stderr, "USAGE: n s\n");
    fprintf(stderr, "n - dir name\n");
    fprintf(stderr, "s - block size [1-10kB]\n");
    exit(EXIT_FAILURE);
}

void open_files(char *dirname, int fd[], int *files_opened, int max_fd, int max_size)
{
    int dirdes = open(dirname, O_RDONLY);
    if (dirdes == -1)
        ERR("open");

    DIR *dir = fdopendir(dirdes);
    if (dir == NULL)
        ERR("fdopendir");

    *files_opened = 0;
    struct dirent *dirent;

    while ((dirent = readdir(dir)) != NULL && *files_opened < max_fd)
    {
        int filedes = openat(dirdes, dirent->d_name, O_RDONLY);
        if (filedes == -1)
            ERR("openat");

        struct stat filestat;
        if (fstat(filedes, &filestat) == -1)
            ERR("fstat");

        if (S_ISREG(filestat.st_mode) && (filestat.st_size <= max_size))
        {
            fd[*files_opened] = filedes;
            (*files_opened)++;
            printf("%s [%ld B]\n", dirent->d_name, filestat.st_size);
        }
        else if (close(filedes) == -1)
                ERR("close");
    }

    if (closedir(dir) == -1)
        ERR("closedir");
}

void aiocb_init(struct aiocb *aiocb, char *buf, int filedes, size_t bufsize, off_t offset, void *fun(union sigval))
{
    memset(aiocb, 0, sizeof *aiocb);
    aiocb->aio_fildes = filedes;
    aiocb->aio_offset = offset;
    aiocb->aio_nbytes = bufsize;
    aiocb->aio_buf = buf;

    if (fun == NULL)
        aiocb->aio_sigevent.sigev_notify = SIGEV_NONE;
    else
    {
        aiocb->aio_sigevent.sigev_notify = SIGEV_THREAD;
        aiocb->aio_sigevent.sigev_value.sival_ptr = fun;
    }
}

void attrib_init(attrib *attr ,struct aiocb *aiocb, pthread_t main_tid, int *aio_progress, pthread_mutex_t *aio_progress_mx)
{
    attr->aiocb = aiocb;
    attr->main_tid = main_tid;
    attr->aio_progress = aio_progress;
    attr->aio_progress_mx = aio_progress_mx;
}

int is_vowel(char c)
{
    if (c == 'a' || c == 'A' || c == 'e' || c == 'E' || c == 'i' || c == 'I' || c == 'o' || c == 'O' || c == 'u' || c == 'U')
        return 1;
    return 0;
}

void sethandler(void (*f)(int), int sig_num)
{
    struct sigaction sig_act;
    memset(&sig_act, 0, sizeof(struct sigaction));
    sig_act.sa_handler = f;
    if (-1 == sigaction(sig_num, &sig_act, NULL))
        ERR("sigaction");
}