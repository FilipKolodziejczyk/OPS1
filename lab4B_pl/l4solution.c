#include "l4solution.h"

int main(int argc, char **argv)
{
    char *outfilename1, *outfilename2, *outfilename3, *infilename;
    int bufsize;
    read_args(argc, argv, &outfilename1, &outfilename2, &outfilename3, &infilename, &bufsize);

    int outfile1 = open_file(outfilename1, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0777);
    int outfile2 = open_file(outfilename2, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0777);
    int outfile3 = open_file(outfilename3, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0777);
    int infile = open_file(infilename, O_RDONLY, -1);

    char *buf[4];
    for (int i = 0; i < 4; i++)
    {
        buf[i] = malloc(bufsize);
        if (buf[i] == NULL)
            ERR("malloc");
    }

    struct aiocb out1_aiocb, out2_aiocb, out3_aiocb, in_aiocb;
    
    //aiocb_init(&out1_aiocb, buf[0], outfile1, bufsize, 0);

    for (int i = 0; i < filelen(infile); i += bufsize)
    {
        aiocb_init(&in_aiocb, buf[0], infile, bufsize, i);
        aio_read(&in_aiocb);
        suspend(&in_aiocb);
    }

    printf("\n");

    for (int i = 0; i < 4; i++)
        free(buf[i]);

    if (close(outfile1) < 0)
        ERR("close");
    if (close(outfile2) < 0)
        ERR("close");
    if (close(outfile3) < 0)
        ERR("close");
    if (close(infile) < 0)
        ERR("close");
}

void suspend(struct aiocb *aiocb)
{
    struct aiocb **aiolist = &aiocb;
    while (aio_suspend((const struct aiocb *const *)aiolist, 1, NULL) == -1)
    {
        if (errno == EINTR)
            continue;
        ERR("aio_suspend");
    }

    int aio_err = aio_error(aiocb);
    if (aio_err != 0)
        ERR("aio_error");

    ssize_t ret = aio_return(aiocb);
    if (ret < 0)
        ERR("aio_return");

    write(STDOUT_FILENO, (const void *)aiocb->aio_buf, aiocb->aio_nbytes);
}