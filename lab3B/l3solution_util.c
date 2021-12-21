#include "l3solution.h"

void usage()
{
    fprintf(stderr, "USAGE: N T\n");
    fprintf(stderr, "N - No of buildings in the New York city (N > 2)\n");
    fprintf(stderr, "T - the time of the simulation given in seconds (T >= 5)\n");
    exit(EXIT_FAILURE);
}

void read_args(int argc, char **argv, int *N_ptr, int *T_ptr)
{
    if (argc != NUM_OF_ARGS + 1)
        usage();

    *N_ptr = atol(argv[1]);
    if (*N_ptr <= 2)
        usage();

    *T_ptr = atol(argv[2]);
    if (*T_ptr < 5)
        usage();
}

void print_city(int *city, pthread_mutex_t *mx, int size)
{
    for (int i = 0; i < size; i++)
    {
        pthread_mutex_lock(mx + i);
        printf("%d ", city[i]);
        pthread_mutex_unlock(mx + i);
    }
    printf("\n");
}

void initialize_args(args *arr, int *city, pthread_mutex_t *mx, unsigned short *xsubi, int N)
{
    for (int i = 0; i < N; i++)
    {
        arr[i].building = city + i;
        arr[i].mx = mx + i;
        if (pthread_mutex_init(arr[i].mx, NULL) != 0)
            ERR("pthread_mutex_init");
        arr[i].xsubi = xsubi;
    }
}

void msleep(unsigned ms)
{
    struct timespec t = {ms / S_TO_MS, ms % S_TO_MS * MS_TO_NS};
    if (nanosleep(&t, NULL) != 0)
        ERR("nanosleep");
}

void set_sigmask(int sig_num)
{
    sigset_t new_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, sig_num);
    if (pthread_sigmask(SIG_BLOCK, &new_mask, NULL) != 0)
        ERR("pthread_sigmask");
}

int check_for_pending_sig(int sig_num)
{
    static struct timespec t = {0, 0};
    sigset_t waiting;
    sigemptyset(&waiting);
    sigaddset(&waiting, sig_num);
    if (sigtimedwait(&waiting, NULL, &t) == sig_num)
        return 1;
    else if (EAGAIN != errno)
        ERR("sigtimedwait");
    return 0;
}

void reset(int *city, pthread_mutex_t *mx, int size)
{
    for (int i = 0; i < size; i++)
    {
        pthread_mutex_lock(mx + i);
        city[i] = 0;
        pthread_mutex_unlock(mx + i);
    }
}