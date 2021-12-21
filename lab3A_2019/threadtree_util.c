#include "threadtree.h"

void usage()
{
    fprintf(stderr, "USAGE: n k\n");
    fprintf(stderr, "n - tree depth (n > 0)\n");
    fprintf(stderr, "k - number of childer per each node (k > 0)\n");
    exit(EXIT_FAILURE);
}

void read_args(int argc, char **argv, int *n, int *k)
{
    if (argc != NUM_OF_ARGS + 1)
        usage();

    *n = atol(argv[1]);
    *k = atol(argv[2]);
    if (*n < 1 || *k < 1)
        usage();
}

void initialize_args(args *arr, args *this, pthread_mutex_t *mx)
{
    for (int i = 0; i < this->child_thread_num; i++)
    {
        arr[i].current_depth = this->current_depth + 1;
        arr[i].max_depth = this->max_depth;
        arr[i].xsubi = this->xsubi;
        arr[i].new_mask = this->new_mask;
        if (arr[i].current_depth == this->max_depth)
        {
            arr[i].sig_limit = 0;
            arr[i].child_thread_num = 0;
        }
        else
        {
            arr[i].sig_limit = pow(this->child_thread_num, this->max_depth - arr[i].current_depth);          
            arr[i].child_thread_num = this->child_thread_num;

        }
        arr[i].mx_sig_limit = mx + i;
        if (pthread_mutex_init(arr[i].mx_sig_limit, NULL) != 0)
            ERR("pthread_mutex_init");
    }
}

void manage_workes(args *thread_args, pthread_t *threads, args *this)
{
    for (int i = 0; i < this->child_thread_num; i++)
        if (pthread_create(threads + i, NULL, worker, thread_args + i) != 0)
            ERR("ptread_create");

    int sig_num;
    while (sigwait(this->new_mask, &sig_num) != 0 || sig_num == SIGUSR1)
    {
        int target = draw_thread(thread_args, this);
        if (pthread_kill(threads[target], SIGUSR1) != 0)
            ERR("pthread_kill");

        pthread_mutex_lock(this->mx_sig_limit);
        this->sig_limit--;
        if (this->sig_limit < 0)
        {
            pthread_mutex_unlock(this->mx_sig_limit);
            return;
        }
        pthread_mutex_unlock(this->mx_sig_limit);

        // for (int i = 0; i < this->child_thread_num; i++)
        //     if (pthread_join(threads[i], NULL) != 0)
        //         ERR("pthread_join");
    }
    ERR("sigwait");
}

void block_sig(int sig_num, sigset_t *new_mask, sigset_t *old_mask)
{
    sigemptyset(new_mask);
    sigaddset(new_mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, new_mask, old_mask) != 0)
        ERR("sigprocmask");
}

int draw_thread(args *worker_args, args *this)
{
    int result;
    while (1)
    {
        result = nrand48(this->xsubi) % this->child_thread_num;
        pthread_mutex_lock(worker_args[result].mx_sig_limit);
        if (worker_args[result].sig_limit >= 0)
        {
            pthread_mutex_unlock(worker_args[result].mx_sig_limit);
            return result;
        }
        pthread_mutex_unlock(worker_args[result].mx_sig_limit);
    }
}