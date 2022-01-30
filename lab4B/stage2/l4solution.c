#include "l4solution.h"

/// Constants

const int num_of_args = 4;
const int num_of_files = 3;
const int bufsize = 5;

int main(int argc, char **argv)
{
    char *filename[num_of_files];
    int level;
    read_args(argc, argv, filename, &level);

    char buffer[num_of_files][bufsize];
    int file[num_of_files];
    for (int i = 0; i < num_of_files; i++)
        file[i] = open_file(filename[i], O_RDWR, 0);

    off_t bytes = LONG_MAX;
    for (int i = 0; i < num_of_files; i++)
    {
        int len = filelen(file[i]);
        if (len < bytes)
            bytes = len;
    }

    struct aiocb aiocbs[num_of_files];
    for (int i = 0; i < num_of_files; i++)
        aiocb_init(&aiocbs[i], file[i], 0, buffer[i], bufsize, LIO_NOP);

    process_files(aiocbs, num_of_files, bytes, bufsize);

    for (int i = 0; i < num_of_files; i++)
        if (TEMP_FAILURE_RETRY(close(file[i])) == -1)
            ERR("close");
}

void process_files(struct aiocb aiocbs[], int n, int bytes, int bufsize)
{
    struct aiocb *aiolist[n];

    off_t pos = 0;
    while (pos < bytes)
    {
        int nbytes = bytes - pos < bufsize ? bytes - pos : bufsize;

        for (int i = 0; i < n; i++)
            aiocb_init(&aiocbs[i], aiocbs[i].aio_fildes, pos, aiocbs[i].aio_buf, nbytes, LIO_READ);
        aiolist_init(aiolist, aiocbs, n);
        if (lio_listio(LIO_NOWAIT, aiolist, n, 0) == -1)
            ERR("lio_listio");

        for (int i = 0; i < n; i++)
        {
            suspend_result result = suspend(aiolist, n);
            if (result.retval != nbytes)
            {
                fprintf(stderr, "Invalid number of characters read");
                exit(EXIT_FAILURE);
            }
            aiolist[result.n] = NULL;
        }

        sort(aiocbs, n, nbytes);

        for (int i = 0; i < n; i++)
            aiocb_init(&aiocbs[i], aiocbs[i].aio_fildes, pos, aiocbs[i].aio_buf, nbytes, LIO_WRITE);
        aiolist_init(aiolist, aiocbs, n);
        if (lio_listio(LIO_NOWAIT, aiolist, n, 0) == -1)
            ERR("lio_listio");

        for (int i = 0; i < n; i++)
        {
            suspend_result result = suspend(aiolist, n);
            if (result.retval != nbytes)
            {
                fprintf(stderr, "Invalid number of characters written");
                exit(EXIT_FAILURE);
            }
            aiolist[result.n] = NULL;
        }

        pos += nbytes;
    }
}

void sort(struct aiocb aiocbs[], int n, int nbytes)
{
    char *buf[n];
    for (int i = 0; i < n; i++)
        buf[i] = (char *)aiocbs[i].aio_buf;

    for (int pos = 0; pos < nbytes; pos++)
    {
        char c[n];
        for (int i = 0; i < n; i++)
            c[i] = buf[i][pos];
        insertion_sort(c, n);
        for (int i = 0; i < n; i++)
            buf[i][pos] = c[i];
    }
}