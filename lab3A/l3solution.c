#include "l3solution.h"

int main(int argc, char **argv)
{
    int T;
    read_args(argc, argv, &T);

    sigset_t new_mask, old_mask;
    block_sig(SIGINT, &new_mask, &old_mask);

    pthread_t judge_tid;
    if (pthread_create(&judge_tid, NULL, judge_work, &T) != 0)
        ERR("ptread_create");

    if (pthread_join(judge_tid, NULL) != 0)
        ERR("pthread_join");

    printf("EXIT\n");
    pthread_sigmask(SIG_SETMASK, &old_mask, NULL);
    return EXIT_SUCCESS;
}

void *judge_work(void *arg_ptr)
{
    int *T_ptr = (int *)arg_ptr;
    unsigned short xsubi[3] = {time(NULL)};
    pthread_t competitor_tid[NUM_OF_COMPETITORS];
    int L = 0;
    pthread_mutex_t mxL = PTHREAD_MUTEX_INITIALIZER;

    competitor_args args[NUM_OF_COMPETITORS];
    for (int i = 0; i < NUM_OF_COMPETITORS; i++)
    {
        args[i].id = i + 1;
        args[i].xsubi = xsubi;
        args[i].L_ptr = &L;
        args[i].mxL_ptr = &mxL;
    }

    for (int i = 0; i < NUM_OF_COMPETITORS; i++)
        if (pthread_create(competitor_tid + i, NULL, competitor_work, args + i) != 0)
            ERR("pthread_create");

    // /* PART 1 */
    // msleep(nrand48(xsubi) % 1001 + 200);
    // printf("I am judge\n");
    // for (int i = 0; i < NUM_OF_COMPETITORS; i++)
    //     if (pthread_join(competitor_tid[i], NULL) != 0)
    //         ERR("pthread_join");

    judge(&L, &mxL, T_ptr);

    for (int i = 0; i < NUM_OF_COMPETITORS; i++)
    {
        if (pthread_cancel(competitor_tid[i]) != 0)
            ERR("pthread_cancel");
        if (pthread_join(competitor_tid[i], NULL) != 0)
            ERR("pthread_join");
    }
    printf("Game over\n");

    return NULL;
}

void *competitor_work(void *arg_ptr)
{
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    competitor_args *arg = (competitor_args *)arg_ptr;

    // /* PART 1 */
    // msleep(nrand48(arg->xsubi) % 1001 + 200);
    // printf("I am competitor %d\n", arg->id);

    while (1)
    {
        msleep(PULL_INTERVAL);
        int pull_val = nrand48(arg->xsubi) % (MAX_PULL + 1);
        pull(pull_val, arg);
    }

    return NULL;
}