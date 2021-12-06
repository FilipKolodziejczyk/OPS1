#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "l2solution.h"

volatile sig_atomic_t last_signal = 0;

int main(int argc, char **argv)
{
    if (argc != 3)
        usage("Invalid number of arguments");

    int n = atol(argv[1]);
    int s = atol(argv[2]);
    if (n <= 0 || s <= 0)
        usage("Invalid arguments");

    create_children(n, s);
    parent_work();

    return EXIT_SUCCESS;
}

void create_children(int n, int s)
{
    pid_t pid;

    for (int i = 0; i < n + 1; i++)
    {
        if ((pid = fork()) == -1)
            ERR("fork:");

        if (i == n)
            set_mask(SIGALRM, SIGUSR1, SIGUSR2);

        if (!pid)
        {
            if (i == n)
                stdin_work(s);
            else
                child_work(s);
            exit(EXIT_SUCCESS);
        }
    }
}

void stdin_work(int s)
{
    // PART 1
    // printf("stdin ready\n");
    // sleep(TIME);

    char *buf = malloc(s);
    if (buf == NULL)
        ERR("malloc");
    buf[0] = '\0';

    int fd = open("stdin.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd < 0)
        ERR("open");

    struct timespec time = {0, 10 * MILI_TO_NANO};
    printf("stdin ready\n");
    while (stdin_getline(buf, s) != EOF)
    {
        if (write(fd, buf, strlen(buf)) < 0)
            ERR("write");
        if (kill(0, SIGALRM) < 0)
            ERR("kill");
        nanosleep(&time, NULL);
        printf("stdin ready\n");
    }

    if (kill(0, SIGALRM) < 0)
        ERR("kill");
    nanosleep(&time, NULL);
    printf("stdin exits\n");
    kill(0, SIGUSR2);

    if (close(fd) < 0)
        ERR("close");

    free(buf);
}

void child_work(int s)
{
    // PART 1
    // printf("%d.txt\n", getpid());
    // sleep(TIME);

    sethandler(sigalrm_handler, SIGALRM);
    sethandler(sigusr1_handler, SIGUSR1);
    sethandler(sigusr2_handler, SIGUSR2);

    char path[MAX_PATH];
    if (sprintf(path, "%d.txt", getpid()) < 0)
        ERR("sprintf");

    char *buf = malloc(s);
    if (buf == NULL)
        ERR("malloc");
    buf[0] = '\0';

    while (1)
    {
        wait_for_sig();
        if (SIGALRM == last_signal)
        {
            rewrite(path, buf);
            buf_update(buf, s);
        }
        else if (SIGUSR1 == last_signal)
            rand_swap(buf);
        else if (SIGUSR2 == last_signal)
            break;
    }
    rewrite(path, buf);
    free(buf);
    printf("<%d> exits\n", getpid());
}

void parent_work()
{
    struct timespec time = {1, 0};
    pid_t pid = 0;
    while ((pid = waitpid(0, NULL, WNOHANG)) >= 0)
    {
        if (pid == 0)
        {
            nanosleep(&time, NULL);
        kill(0, SIGUSR1);
        }
    }
}
