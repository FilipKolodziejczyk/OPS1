#include "l4solution.h"

void usage()
{
    fprintf(stderr, "USAGE: file1 file2 file3 level\n");
    fprintf(stderr, "file1 -  path fo file 1\n");
    fprintf(stderr, "file2 -  path fo file 2\n");
    fprintf(stderr, "file3 -  path fo file 3\n");
    fprintf(stderr, "level - parallelism level (level >= 1)\n");
    exit(EXIT_FAILURE);
}

void read_args(int argc, char **argv, char *filename[num_of_args], int *level)
{
    if (argc != num_of_args + 1)
        usage();

    for (int i = 0; i < num_of_files; i++)
        filename[i] = argv[i + 1];

    *level = atoi(argv[num_of_args]);
    if (*level < 1)
        usage();
}

int open_file(const char *filename, int flags, int mask)
{
    int filedes = TEMP_FAILURE_RETRY(open(filename, flags, mask));

    if (filedes == -1)
        ERR("open");

    return filedes;
}

off_t filelen(int filedes)
{
    struct stat filestat;
    if (fstat(filedes, &filestat) == -1)
        ERR("fstat");
    return filestat.st_size;
}

void aiocb_init(struct aiocb *aiocb, int fd, off_t offset, volatile void *buf, size_t nbytes, int lio_code)
{
    memset(aiocb, 0, sizeof *aiocb);
    aiocb->aio_fildes = fd;
    aiocb->aio_offset = offset;
    aiocb->aio_buf = buf;
    aiocb->aio_nbytes = nbytes;
    aiocb->aio_lio_opcode = lio_code;
    aiocb->aio_sigevent.sigev_notify = SIGEV_NONE;
}

void aiolist_init(struct aiocb *aiolist[], struct aiocb aiocbs[], int n)
{
    for (int i = 0; i < n; i++)
        aiolist[i] = &aiocbs[i];
}

suspend_result suspend(struct aiocb *aiolist[], int n)
{
    suspend_result result;

    while (aio_suspend((const struct aiocb *const *)aiolist, n, NULL) == -1)
        if (errno != EINTR)
            ERR("aio_suspend");

    for (int i = 0; i < n; i++)
    {
        if (aiolist[i] == NULL)
            continue;

        int error = aio_error(aiolist[i]);
        if (error == 0)
        {
            result.n = i;
            result.retval = aio_return(aiolist[i]);
            if (result.retval == -1)
                ERR("aio_return");
            break;
        }

        if (error != EINPROGRESS)
            ERR("aio_error");
    }

    return result;
}

void insertion_sort(char c[], int n)
{
    for (int i = 1; i < n; i++)
    {
        for (int j = i; j > 0 && c[j - 1] > c[j]; j--)
        {
            char temp = c[j - 1];
            c[j - 1] = c[j];
            c[j] = temp;
        }
    }
}