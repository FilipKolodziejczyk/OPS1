#include "prog21.h"

void usage()
{
    fprintf(stderr, "USAGE: n k filename\n");
    fprintf(stderr, "n - filename is split into n blocks of equal size\n");
    fprintf(stderr, "k - number of exchange operations\n");
    fprintf(stderr, "filesize >= n >= 2\n");
    fprintf(stderr, "k >= 1\n");
    exit(EXIT_FAILURE);
}

void read_args(int *n, int *k, char **filename, int argc, char **argv)
{
    if (argc != NUM_OF_ARGS + 1)
        usage();

    *n = atol(argv[1]);
    if (*n < 2)
        usage();

    *k = atol(argv[2]);
    if (*k < 1)
        usage();

    *filename = argv[3];
}

off_t filesize(int filedes)
{
    off_t size = lseek(filedes, 0, SEEK_END);
    if (size < 0)
        ERR("File size not obtained (lseek error)");
    return size;
}

void buf_init(char *buf[], int n, int size)
{
    for (int i = 0; i < n; i++)
    {
        buf[i] = malloc(size * (sizeof *buf));
        if (NULL == buf[i])
            ERR("Buffer allocation error");
    }
}

void aiocbs_init(struct aiocb *aiocbs, char *buf[], int filedes, int n, int size)
{
    for (int i = 0; i < LEVEL; i++)
    {
        memset(aiocbs + i, 0, sizeof *aiocbs);
        aiocbs[i].aio_fildes = filedes;
        aiocbs[i].aio_offset = 0;
        aiocbs[i].aio_nbytes = size;
        aiocbs[i].aio_buf = buf[i];
        aiocbs[i].aio_sigevent.sigev_notify = SIGEV_NONE;
    }
}

void draw_indexes(int *i1, int *i2, int max)
{
    *i1 = rand() % max;
    *i2 = rand() % max;
    if (*i1 == *i2)
    {
        if (*i2 == max - 1)
            *i2 = 0;
        else
            (*i2)++;
    }
}

void reverse_buffer(char *buf, int size)
{
    for (int i = 0; i < size / 2; i++)
    {
        char c = buf[i];
        buf[i] = buf[size - i - 1];
        buf[size - i - 1] = c;
    }
}

void sethandler(void (*handler)(int), int signo)
{
    struct sigaction action;
    memset(&action, 0, sizeof action);
    action.sa_handler = handler;
    if (sigaction(signo, &action, NULL) == -1)
        ERR("Error setting signal handler");
}

extern volatile sig_atomic_t interrupt;
void sigint_handler(int signo)
{
    interrupt = 1;
}

void cancel(int filedes, struct aiocb *aiocbs)
{
    int status = aio_cancel(filedes, NULL);
    if (-1 == status)
        ERR("AIO cancellation error");
    if (AIO_NOTCANCELED == status)
    {
        for (int i = 0; i < LEVEL; i++)
        {
            int error = aio_error(&aiocbs[i]);
            if (error == EINPROGRESS)
                suspend(&aiocbs[i]);
            else if (error != ECANCELED && error != 0)
                ERR("Suspend error");
        }
    }
}