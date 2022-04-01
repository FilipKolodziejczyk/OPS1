#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <aio.h>
#include <string.h>

void usage()
{
    fprintf(stderr, "USAGE: fA fB nb sb ...\n");
    fprintf(stderr, "fA -input file\n");
    fprintf(stderr, "fB - output file\n");
    fprintf(stderr, "nb - number of blocks\n");
    fprintf(stderr, "sb - size of blocks\n");
    exit(EXIT_FAILURE);
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