#include "l3solution.h"

int main(int argc, char **argv)
{
    int N, T;
    read_args(argc, argv, &N, &T);

    int *city = calloc(N, sizeof *city);
    if (NULL == city)
        ERR("calloc");

    pthread_mutex_t *mx = malloc((sizeof *mx) * N);
    if (NULL == mx)
        ERR("malloc");

    pthread_t *threads = malloc((sizeof *threads) * N);
    if (NULL == threads)
        ERR("malloc");

    args *targs = malloc((sizeof *targs) * N);
    if (NULL == targs)
        ERR("malloc");

    unsigned short xsubi[3] = {time(NULL)};
    initialize_args(targs, city, mx, xsubi, N);

    set_sigmask(SIGINT);

    king_kong(city, mx, N, T, threads, targs);

    free(targs);
    free(threads);
    free(mx);
    free(city);
    return EXIT_SUCCESS;
}

void *work(void *argp)
{
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    args *arg = (args *)argp;
    printf("Created\n");

    while (1)
    {
        unsigned ms = nrand48(arg->xsubi) % RAND_MOD + RAND_PLUS;
        msleep(ms);
        pthread_mutex_lock(arg->mx);
        (*arg->building)++;
        pthread_mutex_unlock(arg->mx);
    }

    return NULL;
}

void king_kong(int *city, pthread_mutex_t *mx, int N, int T, pthread_t *threads, args *targs)
{
    int pos = 0;
    printf("Starting position: %d\n", pos + 1);
    print_city(city, mx, N);

    for (int i = 0; i < N; i++)
        if (pthread_create(threads + i, NULL, work, targs + i) != 0)
            ERR("ptread_create");

    for (int i = 0; i < T; i++)
    {
        msleep(S_TO_MS);
        if(check_for_pending_sig(SIGINT))
            reset(city, mx, N);

        pthread_mutex_lock(mx + pos);
        pthread_mutex_lock(mx + pos + 1);
        if (city[pos] < city[pos + 1])
        {
            pthread_mutex_unlock(mx + pos + 1);
            pthread_mutex_unlock(mx + pos);

            pos++;
            printf("Position: %d\n", pos + 1);
            print_city(city, mx, N);
            if (N == pos + 1)
                break;
        }
        else
        {
            pthread_mutex_unlock(mx + pos + 1);
            pthread_mutex_unlock(mx + pos);
        }
    }

    for (int i = 0; i < N; i++)
        if (pthread_cancel(threads[i]) != 0)
            ERR("pthread_cancel");

    for (int i = 0; i < N; i++)
        if (pthread_join(threads[i], NULL) != 0)
            ERR("pthread_join");

    printf("Final position: %d\n", pos + 1);
    print_city(city, mx, N);
}