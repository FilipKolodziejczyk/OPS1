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

int usage(const char *pname, const char *msg);
void create_children(int n, int r);
void parent_work(int k, int p, int r);
void child_work(int r);
void process_msg(const char *msg);

void sethandler;

volatile last_sig = 0;

int main(int argc, char **argv)
{
    if (argc != 5)
        usage(argv[0], "Invalid number of arguments");

    int n, k, p, r;
    n = atol(argv[1]);
    k = atol(argv[2]);
    p = atol(argv[3]);
    r = atol(argv[4]);
    if (n <= 0 || k <= 0 || p <= 0 || r <= 0)
        usage(argv[0], "Invalid argument");

    create_children(n, r);
    parent_work(k, p, r);

    return EXIT_SUCCESS;
}

int usage(const char *pname, const char *msg)
{
    fprintf(stderr, "USAGE(%s): %s\n", pname, msg);
    exit(EXIT_FAILURE);
}

void create_children(int n, int r)
{
    for (int i = 0; i < n; i++)
    {
        pid_t pid;
        if ((pid = fork()) == -1)
            ERR("fork:");

        if (!pid)
        {
            sethan
            child_work(r);
            exit(EXIT_SUCCESS);
        }


    }
}

void parent_work(int k, int p, int r)
{
    while (waitpid(0, NULL, WNOHANG) >= 0)
    {
        sleep(k);
        if (kill(0, SIGUSR1) == -1)
            ERR("kill: ");
        sleep(p);
        if (kill(0, SIGUSR2) == -1)
            ERR("kill: ");
    }
    process_msg("Parent terminates");
}

void child_work(int r)
{
    srand(time(NULL) * getpid());
    int start_time = rand() % 6 + 5;

    for (int i = 0; i < r; i++)
    {
        for (int time = start_time; time > 0; time = sleep(time))
            ;
        if (last_sig == SIGUSR1)
            process_msg("Succes");
        else if (last_sig == SIGUSR2)
            process_msg("Failure");
    }

    process_msg("Child terminates");
}

void process_msg(const char *msg)
{
    printf("[%d]%s\n", getpid(), msg);
}