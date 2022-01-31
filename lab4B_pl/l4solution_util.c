#include "l4solution.h"

void usage()
{
    fprintf(stderr, "USAGE: out1 out2 out3 in buf\n");
    fprintf(stderr, "out1 - output filename 1\n");
    fprintf(stderr, "out2 - output filename 2\n");
    fprintf(stderr, "out3 - output filename 3\n");
    fprintf(stderr, "in - input filename\n");
    fprintf(stderr, "buf - buffer size (buf >= 1)\n");
    exit(EXIT_FAILURE);
}

void read_args(int argc, char **argv, char **ofn1, char **ofn2, char **ofn3, char **ifn, int *bs)
{
    if (argc != NUM_OF_ARGS + 1)
        usage();

    *ofn1 = argv[1];
    *ofn2 = argv[2];
    *ofn3 = argv[3];
    *ifn = argv[4];

    *bs = atol(argv[5]);
    if (*bs < 1)
        usage();
}

int open_file(const char *filename, int flags, int mask)
{
    int filedes;
    if (mask >= 0)
        filedes = open(filename, flags, mask);
    else
        filedes = open(filename, flags, mask);

    if (filedes < 0)
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

void aiocb_init(struct aiocb *aiocb, char *buf, int filedes, size_t bufsize, off_t offset)
{
    memset(aiocb, 0, sizeof *aiocb);
    aiocb->aio_fildes = filedes;
    aiocb->aio_offset = offset;
    aiocb->aio_nbytes = bufsize;
    aiocb->aio_buf = buf;
    aiocb->aio_sigevent.sigev_notify = SIGEV_NONE;
}