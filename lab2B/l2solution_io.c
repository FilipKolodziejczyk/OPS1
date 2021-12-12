#include "l2solution.h"

void usage(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    kill(0, SIGKILL);
    exit(EXIT_FAILURE);
}

int invalid_filename(const char *filename)
{
    if (strlen(filename) > MAX_FILENAME)
        return 1;

    if (!isalnum(filename[0]))
        return 1;

    return 0;
}

int read_line(char *dest, int size)
{
    char c = fgetc(stdin);
    int i = 0;
    while (c != EOF && c != '\n')
    {
        if ((c != '0' && c != '1') || i >= size)
        {
            clear_stdin();
            i = 0;
            break;
        }
        dest[i++] = c;
        c = fgetc(stdin);
    }
    if (i >= 1)
        dest[i++] = '\n';
    dest[i] = '\0';

    if (ferror(stdin))
        ERR("fgetc");

    if (feof(stdin))
        return EOF;

    return 0;
}

void clear_stdin()
{
    char c = fgetc(stdin);
    while (c != EOF && c != '\n')
        c = fgetc(stdin);

    if (ferror(stdin))
        ERR("fgetc");
}

extern volatile sig_atomic_t last_sig;

void write_last_signal(int fd)
{
    if (last_sig == SIGINT)
    {
        if (write(fd, "Q", 1) == -1)
            ERR("write");
        if (close(fd) == -1)
            ERR("close");
        exit(EXIT_SUCCESS);
    }

    if (last_sig == SIGUSR1)
    {
        printf("%d: Last signal SIGUSR1\n", getpid());
        if (write(fd, "1", 1) == -1)
            ERR("write");
    }
    else if (last_sig == SIGUSR2)
    {
        printf("%d: Last signal SIGUSR2\n", getpid());
        if (write(fd, "2", 1) == -1)
            ERR("write");
    }
}

void save(char *filename)
{
    int dest = open(filename, O_WRONLY | O_APPEND | O_TRUNC | O_CREAT, S_IRWXU);
    if (dest < 0)
        ERR("open");

    pid_t pid;
    int len;
    char *buf = malloc(MAX_BUF);
    if(NULL == buf)
        ERR("malloc");

    while ((pid = wait(NULL)) > 0)
    {
        char src_name[MAX_FILENAME];
        if (snprintf(src_name, MAX_FILENAME, "%d.txt", pid) == -1)
            ERR("snprintf");

        int src = open(src_name, O_RDONLY);
        if (src < 0)
            ERR("open");

        if ((len = read(src, buf, MAX_BUF)) < 0)
            ERR("read");

        if (write(dest, buf, len) < 0)
            ERR("write");

        if (close(src) == -1)
            ERR("close");
    }

    if (errno != ECHILD)
        ERR("wait");

    free(buf);

    if (close(dest) == -1)
        ERR("close");
}