#include "threadtree.h"

int main(int argc, char **argv)
{
    int n, k;
    read_args(argc, argv, &n, &k);

    pthread_t *threads = malloc((sizeof *threads) * k);
    if (NULL == threads)
        ERR("malloc");

    pthread_mutex_t *mx = malloc((sizeof *mx) * k);
    if (NULL == mx)
        ERR("malloc");

    args *worker_args = malloc((sizeof *worker_args) * k);
    if (NULL == worker_args)
        ERR("malloc");

    unsigned short xsubi[3] = {time(NULL)};
    sigset_t new_mask, old_mask;
    block_sig(SIGUSR1, &new_mask, &old_mask);

    args this = {k, 0, n, xsubi, &new_mask, pow(k, n), NULL};
    initialize_args(worker_args, &this, mx);

    main_work(threads, worker_args, &this);

    free(worker_args);
    free(mx);
    free(threads);
    // /* PART 1 */
    // if (sleep(1) != 0)
    //     ERR("sleep");
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) != 0)
        ERR("sigprocmask");
    printf("[Main] End (depth = 0)\n");
    return EXIT_SUCCESS;
}

void main_work(pthread_t *threads, args *worker_args, args *this)
{
    for (int i = 0; i < this->child_thread_num; i++)
        if (pthread_create(threads + i, NULL, worker, worker_args + i) != 0)
            ERR("ptread_create");

    char c;
    printf("[Main] Sig limit = %d\n", this->sig_limit);
    for (int i = 0; i < this->sig_limit;)
    {
        c = fgetc(stdin);
        if (EOF == c)
            ERR("fgetc");

        if ('k' == c)
        {
            int target = draw_thread(worker_args, this);

            printf("Signal to [%ld]\n", threads[target]);
            if (pthread_kill(threads[target], SIGUSR1) != 0)
                ERR("pthread_kill");

            i++;
        }
        else if ('\n' != c)
            printf("Wrong character (%c)\n", c);
    }

    for (int i = 0; i < this->child_thread_num; i++)
        if (pthread_join(threads[i], NULL) != 0)
            ERR("pthread_join");

    // /* PART 1 */
    // for (int i = 0; i < this->child_thread_num; i++)
    //     if (pthread_join(threads[i], NULL) != 0)
    //         ERR("pthread_join");
}

void *worker(void *argp)
{
    args *this = (args *)argp;
    printf("[%ld] Start (depth = %d, sig_limit = %d)\n", pthread_self(), this->current_depth, this->sig_limit);

    if (this->current_depth < this->max_depth)
    {

        pthread_t *threads = malloc((sizeof *threads) * this->child_thread_num);
        if (NULL == threads)
            ERR("malloc");

        pthread_mutex_t *mx = malloc((sizeof *mx) * this->child_thread_num);
        if (NULL == mx)
            ERR("malloc");

        args *worker_args = malloc((sizeof *worker_args) * this->child_thread_num);
        if (NULL == worker_args)
            ERR("malloc");
        initialize_args(worker_args, this, mx);

        manage_workes(worker_args, threads, this);

        for (int i = 0; i < this->child_thread_num; i++)
            if (pthread_join(threads[i], NULL) != 0)
                ERR("pthread_join");

        free(worker_args);
        free(mx);
        free(threads);
    }

    int sig_num;
    if (sigwait(this->new_mask, &sig_num) != 0 || sig_num != SIGUSR1)
        ERR("sigwait");

    pthread_mutex_lock(this->mx_sig_limit);
    this->sig_limit--;
    pthread_mutex_unlock(this->mx_sig_limit);

    // /* PART 1 */
    // if (sleep(1) != 0)
    //     ERR("sleep");

    printf("[%ld] End (depth = %d, sig_limit = %d)\n", pthread_self(), this->current_depth, this->sig_limit);
    return NULL;
}