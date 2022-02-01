#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

typedef struct suspend_result
{
    int n;
    ssize_t retval;
} suspend_result;

void usage()
{
    fprintf(stderr, "USAGE: fA fB nb sb ...\n");
    fprintf(stderr, "fA -input file\n");
    fprintf(stderr, "fB - output file\n");
    fprintf(stderr, "nb - number of blocks\n");
    fprintf(stderr, "sb - size of blocks\n");
    exit(EXIT_FAILURE);
}

void aiocb_init(struct aiocb *aiocb, char *buf, int filedes, size_t bufsize, off_t offset, int lio_code)
{
    memset(aiocb, 0, sizeof *aiocb);
    aiocb->aio_fildes = filedes;
    aiocb->aio_offset = offset;
    aiocb->aio_nbytes = bufsize;
    aiocb->aio_buf = buf;
    aiocb->aio_sigevent.sigev_notify = SIGEV_NONE;
    aiocb->aio_lio_opcode = lio_code;
}

void aiolist_init(struct aiocb *aiolist[], struct aiocb aiocbs[], int n)
{
    for (int i = 0; i < n; i++)
        aiolist[i] = &aiocbs[i];
}

int open_file(const char *filename, int flags, int mask)
{
    int filedes = open(filename, flags, mask);
    if (filedes == -1)
        ERR("open");
    return filedes;
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

void run_aio(struct aiocb aiocbs[], int n)
{
    struct aiocb **aio_list = malloc(n * (sizeof *aio_list));
    aiolist_init(aio_list, aiocbs, n);
    if (lio_listio(LIO_NOWAIT, aio_list, n, 0) == -1)
        ERR("lio_listio");

    for (int i = 0; i < n; i++)
    {
        suspend_result result = suspend(aio_list, n);
        aio_list[result.n] = NULL;
    }
    free(aio_list);
}

void cycle(int nb, int sb, int pos, int fA, int fB)
{
    char **buf = malloc(nb * (sizeof *buf));
    if (buf == NULL)
        ERR("malloc");

    struct aiocb *aiocbs = malloc(nb * (sizeof *aiocbs));
    if (aiocbs == NULL)
        ERR("malloc");

    for (int i = 0; i < nb; i++)
    {
        if ((buf[i] = malloc(sb)) == NULL)
            ERR("malloc");
        aiocb_init(&aiocbs[i], buf[i], fA, sb, pos + i * sb, LIO_READ);
    }

    run_aio(aiocbs, nb);

    for (int i = 0; i < nb / 2; i++)
    {
        char *temp = buf[nb - i - 1];
        buf[nb - i - 1] = buf[i];
        buf[i] = temp;
    }

    for (int i = 0; i < nb; i++)
        aiocb_init(&aiocbs[i], buf[i], fB, sb, pos + sb * i, LIO_WRITE);

    run_aio(aiocbs, nb);

    free(aiocbs);
    for (int i = 0; i < nb; i++)
        free(buf[i]);
    free(buf);
}

int main(int argc, char **argv)
{
    if ((argc + 1) % 2 != 0 || argc < 3)
        usage();

    char *fA_name = argv[1];
    char *fB_name = argv[2];

    int fA = open_file(fA_name, O_RDONLY, 0);
    int fB = open_file(fB_name, O_WRONLY | O_TRUNC | O_CREAT, 0777);

    int pos = 0;
    for (int i = 3; i < argc; i += 2)
    {
        int nb = atoi(argv[i]);
        int sb = atoi(argv[i + 1]);
        if (nb < 1 || sb < 1)
            usage();

        cycle(nb, sb, pos, fA, fB);

        pos += nb * sb;
    }

    if (close(fA) == -1)
        ERR("close");
    if (close(fB) == -1)
        ERR("close");

    return EXIT_SUCCESS;
}