#include "l2solution.h"

void sethandler(void (*f)(int), int sig_num)
{
    struct sigaction sig_act;
    memset(&sig_act, 0, sizeof(struct sigaction));
    sig_act.sa_handler = f;
    if (-1 == sigaction(sig_num, &sig_act, NULL))
        ERR("sigaction");
}

extern volatile sig_atomic_t last_sig;

void sigusr1_handler(int sig_num)
{
    // /* PART 1 */
    // printf("[%d]: 0\n", getpid());
    last_sig = sig_num;
}

void sigusr2_handler(int sig_num)
{
    // /* PART 1 */
    // printf("[%d]: 1\n", getpid());
    last_sig = sig_num;
}

void sigint_handler(int sig_num)
{
    last_sig = sig_num;
}