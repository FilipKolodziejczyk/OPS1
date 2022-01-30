#include "l4solution.h"

/// Constants

const int num_of_args = 4;
const int num_of_files = 3;
const int bufsize = 192;

int main(int argc, char **argv)
{
    char *filename[num_of_files];
    int level;
    read_args(argc, argv, filename, &level);

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

    char buffer[num_of_files * 2][bufsize];
    struct aiocb aiocbs[num_of_files * 2];
    for (int i = 0; i < num_of_files * 2; i++)
        aiocb_init(&aiocbs[i], file[i % 3], 0, buffer[i], bufsize, LIO_NOP);

    process_files(aiocbs, num_of_files, bytes, bufsize);

    for (int i = 0; i < num_of_files; i++)
        if (TEMP_FAILURE_RETRY(close(file[i])) == -1)
            ERR("close");
}

void process_files(struct aiocb aiocbs[], int n, int bytes, int bufsize)
{
    /// 0 - 2 : read
    struct aiocb *aio_read_list[n];
    off_t read_pos = 0;
    int read_bytes = bytes - read_pos < bufsize ? bytes - read_pos : bufsize;
    /// 3 - 5 : write
    struct aiocb *aio_write_list[n];
    off_t write_pos = -1;
    int write_bytes;

    /// Queue Reading
    for (int i = 0; i < n; i++)
        aiocb_init(&aiocbs[i], aiocbs[i].aio_fildes, read_pos, aiocbs[i].aio_buf, read_bytes, LIO_READ);
    aiolist_init(aio_read_list, aiocbs, n);
    if (lio_listio(LIO_NOWAIT, aio_read_list, n, 0) == -1)
        ERR("lio_listio");

    while (read_pos < bytes)
    {
        /// Wait for reading
        for (int i = 0; i < n; i++)
        {
            suspend_result result = suspend(aio_read_list, n);
            if (result.retval != read_bytes)
            {
                fprintf(stderr, "Invalid number of characters read");
                exit(EXIT_FAILURE);
            }
            aio_read_list[result.n] = NULL;
        }

        /// Sorting
        sort(aiocbs, n, read_bytes);

        /// Wait for writing
        if (write_pos >= 0)
        {
            for (int i = 0; i < n; i++)
            {
                suspend_result result = suspend(aio_write_list, n);
                if (result.retval != write_bytes)
                {
                    fprintf(stderr, "Invalid number of characters written");
                    exit(EXIT_FAILURE);
                }
                aio_write_list[result.n] = NULL;
            }
        }

        ///Updating positions
        write_pos = read_pos;
        write_bytes = read_bytes;
        read_pos += read_bytes;
        read_bytes = bytes - read_pos < bufsize ? bytes - read_pos : bufsize;

        /// Swapping buffers and setting control blocks
        for (int i = 0; i < n; i++)
        {
            volatile void *temp = aiocbs[i].aio_buf;
            aiocb_init(&aiocbs[i], aiocbs[i].aio_fildes, read_pos, aiocbs[i + n].aio_buf, read_bytes, LIO_READ);
            aiocb_init(&aiocbs[i + n], aiocbs[i + n].aio_fildes, write_pos, temp, write_bytes, LIO_WRITE);
        }

        /// Queue reading
        if (read_pos < bytes)
        {
            aiolist_init(aio_read_list, aiocbs, n);
            if (lio_listio(LIO_NOWAIT, aio_read_list, n, 0) == -1)
                ERR("lio_listio");
        }

        /// Queue writing
        aiolist_init(aio_write_list, aiocbs + n, n);
        if (lio_listio(LIO_NOWAIT, aio_write_list, n, 0) == -1)
            ERR("lio_listio");
    }

    /// Wait for writing
    for (int i = 0; i < n; i++)
    {
        suspend_result result = suspend(aio_write_list, n);
        if (result.retval != write_bytes)
        {
            fprintf(stderr, "Invalid number of characters written");
            exit(EXIT_FAILURE);
        }
        aio_write_list[result.n] = NULL;
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