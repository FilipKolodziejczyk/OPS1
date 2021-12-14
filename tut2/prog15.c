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

#define NANO_TO_MICRO 1000
volatile sig_atomic_t last_sig = 0;

int usage(const char *msg);
void parent_work(sigset_t old_mask);
void child_work(int n, int m);

void sethandler(void (*f)(int), int sig_num);
void sig_handler(int sig_num);

int main(int argc, char **argv)
{
    if (argc != 3)
        usage("Invalid number of arguments");

    int n, m;
    n = atol(argv[1]);
    m = atol(argv[2]);
    if (n <= 0 || m <= 0)
        usage("Invalid argument");

    sethandler(sig_handler, SIGINT);
    sethandler(sig_handler, SIGUSR1);
    sethandler(sig_handler, SIGUSR2);

    sigset_t mask, old_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, &old_mask);

    pid_t pid = fork();
    if (pid < 0)
        ERR("fork");
    else if (pid == 0)
        child_work(n, m);
    else
    {
        parent_work(old_mask);
        if (wait(NULL) < 0 && errno != ECHILD)
            ERR("wait");
    }

    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    return EXIT_SUCCESS;
}

int usage(const char *msg)
{
    fprintf(stderr, "USAGE: %s\n", msg);
    exit(EXIT_FAILURE);
}

void parent_work(sigset_t old_mask)
{
    int count = 0;
    while (last_sig != SIGINT)
    {
        sigsuspend(&old_mask);
        if (last_sig == SIGUSR2)
            count++;
    }

    printf("[PARENT] received %d SIGUSR2\n", count);
}

void child_work(int n, int m)
{
    int count = 1;
    struct timespec time = {0, m * NANO_TO_MICRO};
    pid_t ppid = getppid();

    while (last_sig != SIGINT)
    {
        nanosleep(&time, NULL);
        if (count % m == 0)
            kill(ppid, SIGUSR2);
        else
            kill(ppid, SIGUSR1);
        count++;
    }

    printf("[CHILD] sent %d SIGUSR2\n", count / m);
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