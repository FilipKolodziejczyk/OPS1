#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#define ERR(source) (fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     perror(source),                                 \
                     kill(0, SIGKILL),                               \
                     exit(EXIT_FAILURE))

int usage(char *pname, char *msg);
void child_work();
void create_n_child(int n);
void wait_n_child(int n);

int main(int argc, char **argv)
{
    if (argc != 2)
        usage(argv[0], "Invalid number of arguments");

    int n = atol(argv[1]);
    if (n <= 0)
        usage(argv[0], "Invalid argument");

    create_n_child(n);
    wait_n_child(n);

    return EXIT_SUCCESS;
}

int usage(char *pname, char *msg)
{
    fprintf(stderr, "USAGE(%s): %s\n", pname, msg);
    exit(EXIT_FAILURE);
}

void create_n_child(int n)
{
    for (int i = 0; i < n; i++)
    {
        pid_t pid;
        if ((pid = fork()) == -1)
            ERR("fork:");

        if (!pid)
        {
            child_work();
            exit(EXIT_SUCCESS);
        }
    }
}

void wait_n_child(int n)
{
    while (n > 0)
    {
        sleep(3);
        pid_t pid;
        while (1)
        {
            pid = waitpid(0, NULL, WNOHANG);
            if (pid > 0)
                n--;
            else
            {
                if (0 == pid || ECHILD == errno)
                    break;
                ERR("waitpid: ");
            }
        }
        printf("Parent: %d processes remain\n", n);
    }
}

void child_work()
{
    srand(time(NULL) * getpid());
    int time = rand() % 6 + 5;
    sleep(time);
    printf("Process with pid %d terminates\n", getpid());
}