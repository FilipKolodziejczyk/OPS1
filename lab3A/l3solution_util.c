#include "l3solution.h"

void usage()
{
    fprintf(stderr, "USAGE: T\n");
    fprintf(stderr, "T - duration of a single round of the game (2 <= T <= 128)\n");
    exit(EXIT_FAILURE);
}

void read_args(int argc, char **argv, int *T_ptr)
{
    if (argc != NUM_OF_ARGS + 1)
        usage();

    *T_ptr = atol(argv[NUM_OF_ARGS]);
    if (*T_ptr < T_MIN || *T_ptr > T_MAX)
        usage();
}

void msleep(unsigned ms)
{
    struct timespec t = {ms / S_TO_MS, ms % S_TO_MS * MS_TO_NS};
    if (nanosleep(&t, NULL) != 0)
        ERR("nanosleep");
}

void judge(int *L_ptr, pthread_mutex_t *mxL_ptr, int *T_ptr)
{
    sigset_t waiting;
    sigemptyset(&waiting);
    sigaddset(&waiting, SIGINT);

    // /* PART 2 */
    // while (1)
    for (int i = 0; i < *T_ptr; i++)
    {
        msleep(1000);
        reset(&i, &waiting, L_ptr, mxL_ptr);
        pthread_mutex_lock(mxL_ptr);
        printf("L = %d\n", *L_ptr);
        pthread_mutex_unlock(mxL_ptr);
    }
}

void pull(int pull_val, competitor_args *arg)
{
    pthread_mutex_lock(arg->mxL_ptr);
    if (arg->id == 1)
        *arg->L_ptr += pull_val;
    else
        *arg->L_ptr -= pull_val;
    pthread_mutex_unlock(arg->mxL_ptr);
}

void block_sig(int sig_num, sigset_t *new, sigset_t *old)
{
    sigemptyset(new);
    sigaddset(new, sig_num);
    if (pthread_sigmask(SIG_BLOCK, new, old) != 0)
        ERR("pthread_sigmask");
}

void reset(int *i, sigset_t *waiting, int *L_ptr, pthread_mutex_t *mxL_ptr)
{
    static struct timespec t = {0, 0};
    if (sigtimedwait(waiting, NULL, &t) == SIGINT)
    {
        *i = 0;
        pthread_mutex_lock(mxL_ptr);
        *L_ptr = 0;
        pthread_mutex_unlock(mxL_ptr);
    }
    else if (EAGAIN != errno)
        ERR("sigtimedwait");
}