#include "prog21.h"

volatile sig_atomic_t interrupt;

int main(int argc, char **argv)
{
    int n, k;
    char *filename;
    read_args(&n, &k, &filename, argc, argv);

    int file = TEMP_FAILURE_RETRY(open(filename, O_RDWR));
    if (file < 0)
        ERR("File opening error");

    off_t block_size = filesize(file) / n;
    if (block_size < 1)
        usage();

    printf("File size: %ld\n", filesize(file));
    printf("Block size: %ld\n", block_size);

    char *buf[LEVEL];
    buf_init(buf, LEVEL, block_size);

    struct aiocb aiocbs[LEVEL];
    aiocbs_init(aiocbs, buf, file, LEVEL, block_size);

    srand(time(NULL));
    interrupt = 0;
    sethandler(sigint_handler, SIGINT);

    process_file(aiocbs, n, k, block_size);

    sethandler(SIG_IGN, SIGINT);
    if (interrupt)
        cancel(file, aiocbs);

    for (int i = 0; i < LEVEL; i++)
        free(buf[i]);

    if (TEMP_FAILURE_RETRY(fsync(file)) < 0)
        ERR("File sync error");

    if (TEMP_FAILURE_RETRY(close(file)) < 0)
        ERR("File closure error");

    return EXIT_SUCCESS;
}

void process_file(struct aiocb *aiocbs,  int nblock, int times, off_t block_size)
{
    int src, dest;
    read_off(&aiocbs[0], (rand() % nblock) * block_size);
    suspend(&aiocbs[0]);

    int i;
    for (i = 0; i < times; i++)
    {
        draw_indexes(&src, &dest, nblock);

        if (i > 0)
            write_off(&aiocbs[(i + 2) % LEVEL], dest * block_size);

        if (i < times - 1)
            read_off(&aiocbs[(i + 1) % LEVEL], src * block_size);

        reverse_buffer((char *)aiocbs[i % LEVEL].aio_buf, aiocbs[i % LEVEL].aio_nbytes);

        if (i > 0)
            data_sync(&aiocbs[(i + 2) % LEVEL]);

        if (i < times - 1)
            suspend(&aiocbs[(i + 1) % LEVEL]);
    }

    write_off(&aiocbs[(i + 2) % LEVEL], dest * block_size);
    suspend(&aiocbs[(i + 2) % LEVEL]);
}

void suspend(struct aiocb *aiocb)
{
    if (interrupt)
        return;

    while (aio_suspend((const struct aiocb *const *)&aiocb, 1, NULL) == -1)
    {
        if (interrupt)
            return;
        if (errno == EINTR)
            continue;
        ERR("Suspend error");
    }
    if (aio_error(aiocb) != 0)
        ERR("Suspend error");
    if (aio_return(aiocb) == -1)
        ERR("Return error");
}

void read_off(struct aiocb *aiocb, off_t offset)
{
    if (interrupt)
        return;
    aiocb->aio_offset = offset;
    aio_read(aiocb);
}

void write_off(struct aiocb *aiocb, off_t offset)
{
    if (interrupt)
        return;
    aiocb->aio_offset = offset;
    aio_write(aiocb);
}

void data_sync(struct aiocb *aiocb)
{
    if (interrupt)
        return;
    suspend(aiocb);
    if (aio_fsync(O_SYNC, aiocb) == -1)
        ERR("AIO sync error\n");
    suspend(aiocb);
}