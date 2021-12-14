#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#define ERR(source) (fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     perror(source),                                 \
                     kill(0, SIGKILL),                               \
                     exit(EXIT_FAILURE))

volatile sig_atomic_t last_sig = 0;

int usage(const char *msg);
void create_children(int n, int r);
void parent_work(int k, int p, int r);
void child_work(int r);
void process_msg(const char *msg);

void sethandler(void (*f)(int), int sig_num);
void sig_handler(int sig_num);
void sigchld_handler(int sig_num);

int main(int argc, char **argv)
{
    if (argc != 5)
        usage("Invalid number of arguments");

    int n, k, p, r;
    n = atol(argv[1]);
    k = atol(argv[2]);
    p = atol(argv[3]);
    r = atol(argv[4]);
    if (n <= 0 || k <= 0 || p <= 0 || r <= 0)
        usage("Invalid argument");

    sethandler(SIG_IGN, SIGUSR1);
    sethandler(SIG_IGN, SIGUSR2);
    sethandler(SIG_IGN, SIGCHLD);

    create_children(n, r);
    parent_work(k, p, r);

    return EXIT_SUCCESS;
}

int usage(const char *msg)
{
    fprintf(stderr, "USAGE: %s\n", msg);
    exit(EXIT_FAILURE);
}

void create_children(int n, int r)
{
    for (int i = 0; i < n; i++)
    {
        pid_t pid;
        if ((pid = fork()) == -1)
            ERR("fork:");

        if (pid == 0)
        {
            child_work(r);
            exit(EXIT_SUCCESS);
        }
    }
}

void parent_work(int k, int p, int r)
{
    pid_t pid;
    while (1)
    {
        while ((pid = waitpid(0, NULL, WNOHANG)) > 0)
            ;
        if (pid < 0)
        {
            if (errno != ECHILD)
                ERR("waitpid");
            break;
        }
        sleep(k);
        if (kill(0, SIGUSR1) == -1)
            ERR("kill: ");
        sleep(p);
        if (kill(0, SIGUSR2) == -1)
            ERR("kill: ");
    }
    if (errno != ECHILD)
        ERR("waitpid");
    process_msg("Parent terminates");
}

void child_work(int r)
{
    sethandler(sig_handler, SIGUSR1);
    sethandler(sig_handler, SIGUSR2);

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

void sethandler(void (*f)(int), int sug_num)
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;
    if (-1 == sigaction(sug_num, &act, NULL))
        ERR("sigaction");
}

void sig_handler(int sig_num)
{
    last_sig = sig_num;
}