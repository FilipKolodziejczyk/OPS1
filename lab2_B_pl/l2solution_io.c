#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "l2solution.h"

void usage(const char *msg)
{
    fprintf(stderr, "USAGE: %s\n", msg);
    kill(0, SIGKILL);
    exit(EXIT_FAILURE);
}

int stdin_getline(char *buf, int s)
{
    int i = 0;
    char c = fgetc(stdin);
    while (c != '\n' && c != EOF)
    {
        if (i >= s - 1)
            usage("Too many characters in stdin");
        buf[i] = c;
        i++;
        c = fgetc(stdin);
    }
    buf[i] = '\0';

    if (ferror(stdin))
        ERR("fgetc");

    if (feof(stdin))
        return EOF;
    else
        return 0;
}

void rewrite(const char *path, const char *buf)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (fd < 0)
        ERR("open");

    if (write(fd, buf, strlen(buf)) < 0)
        ERR("write");

    if (close(fd) < 0)
        ERR("close");
}

void buf_update(char *buf, int s)
{
    int fd = open("stdin.txt", O_RDONLY);
    if (fd < 0)
        ERR("open");

    int len = lseek(fd, 0L, SEEK_END);
    lseek(fd, 0L, SEEK_SET);

    if (len >= s - 1)
        usage("Buffer overflow");

    if (read(fd, buf, len) < 0)
        ERR("read");
    buf[len] = '\0';

    if (close(fd) < 0)
        ERR("close");
    return;
}

void rand_swap(char *buf)
{
    if (NULL == buf || strlen(buf) == 0)
        return;

    srand(time(NULL) * getpid());
    int rand1 = rand() % strlen(buf);
    int rand2 = rand() % strlen(buf);
    char temp = buf[rand1];
    buf[rand1] = buf[rand2];
    buf[rand2] = temp;
}