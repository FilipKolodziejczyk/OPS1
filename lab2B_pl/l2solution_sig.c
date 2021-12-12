#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "l2solution.h"

void sethandler(void (*f)(int), int sigNo)
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;
    if (-1 == sigaction(sigNo, &act, NULL))
        ERR("sigaction");
}

void set_mask(int signo1, int signo2, int signo3)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, signo1);
    sigaddset(&mask, signo2);
    sigaddset(&mask, signo3);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void wait_for_sig()
{
    sigset_t mask;
    sigprocmask(SIG_UNBLOCK, NULL, &mask);
    sigsuspend(&mask);
}

extern volatile sig_atomic_t last_signal;

void sigalrm_handler()
{
    last_signal = SIGALRM;
}

void sigusr1_handler()
{
    last_signal = SIGUSR1;
}

void sigusr2_handler()
{
    last_signal = SIGUSR2;
}