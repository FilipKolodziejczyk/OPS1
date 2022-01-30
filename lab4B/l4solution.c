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

    char **buffer = malloc(level * num_of_files * (sizeof *buffer));
    if (buffer == NULL)
        ERR("malloc");
    struct aiocb *aiocbs = malloc(level * num_of_files * (sizeof *aiocbs));
    if (aiocbs == NULL)
        ERR("malloc");
    for (int i = 0; i < num_of_files * level; i++)
    {
        if ((buffer[i] = malloc(bufsize)) == NULL)
            ERR("malloc");
        aiocb_init(&aiocbs[i], file[i % 3], 0, buffer[i], bufsize, LIO_NOP);
    }
    
    process_files(aiocbs, num_of_files, bytes, bufsize, level);

    for (int i = 0; i < num_of_files * level; i++)
        free(buffer[i]);
    free(buffer);
    free(aiocbs);

    for (int i = 0; i < num_of_files; i++)
        if (TEMP_FAILURE_RETRY(close(file[i])) == -1)
            ERR("close");
}

void process_files(struct aiocb aiocbs[], int n, int bytes, int bufsize, int level)
{
    struct aiocb **aio_list = malloc(level * n * (sizeof *aio_list));
    if (aio_list == NULL)
        ERR("malloc");

    off_t pos = 0;
    int nbytes = bytes - pos < bufsize ? bytes - pos : bufsize;

    for (int i = 0; i < level * n; i++)
    {
        aiocb_init(&aiocbs[i], aiocbs[i].aio_fildes, pos, aiocbs[i].aio_buf, nbytes, LIO_READ);

        if (i % n == n - 1)
        {
            pos += nbytes;
            nbytes = bytes - pos < bufsize ? bytes - pos : bufsize;
        }
    }

    aiolist_init(aio_list, aiocbs, n * level);
    if (lio_listio(LIO_NOWAIT, aio_list, n * level, 0) == -1)
        ERR("lio_listio");

    int active_chunks = level;

    int *chunk_status = calloc(level, sizeof *chunk_status);
    if (chunk_status == NULL)
        ERR("malloc");

    while (active_chunks > 0)
    {
        suspend_result result = suspend(aio_list, n * level);
        if (result.retval != aiocbs[result.n].aio_nbytes)
        {
            fprintf(stderr, "Invalid number of characters processed\n");
            exit(EXIT_FAILURE);
        }
        aio_list[result.n] = NULL;
        chunk_status[result.n / n]++;

        if (chunk_status[result.n / n] == n)
        {
            chunk_status[result.n / n] = 0;
            int i = result.n - (result.n % n);

            if (aiocbs[result.n].aio_lio_opcode == LIO_READ)
            {
                sort(&aiocbs[i], n, result.retval);
                for (int j = 0; j < n; i++, j++)
                {
                    aiocb_init(&aiocbs[i], aiocbs[i].aio_fildes, aiocbs[i].aio_offset, aiocbs[i].aio_buf, aiocbs[i].aio_nbytes, LIO_WRITE);
                    aio_write(&aiocbs[i]);
                }

                i--;
                for (int j = 0; j < n; j++, i--)
                    aio_list[i] = &aiocbs[i];
            }
            else if (pos < bytes)
            {
                for (int j = 0; j < n; i++, j++)
                {
                    aiocb_init(&aiocbs[i], aiocbs[i].aio_fildes, pos, aiocbs[i].aio_buf, nbytes, LIO_READ);
                    aio_read(&aiocbs[i]);
                }

                pos += nbytes;
                nbytes = bytes - pos < bufsize ? bytes - pos : bufsize;
                i--;
                for (int j = 0; j < n; j++, i--)
                    aio_list[i] = &aiocbs[i];
            }
            else
            {
                active_chunks--;
            }
        }
    }

    free(chunk_status);
    free(aio_list);
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