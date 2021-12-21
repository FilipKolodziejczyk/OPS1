#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

#define NUM_OF_ARGS 1
#define S_TO_MS 1000
#define MS_TO_NS 1000000

int main(int argc, char **argv)
{
    read_args(argc, argv);
    return EXIT_SUCCESS;
}

void read_args(int argc, char **argv, ...)
{
    if (argc != NUM_OF_ARGS + 1)
        usage();
}

void usage()
{
    fprintf(stderr, "USAGE:\n");
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

typedef struct _args
{

} args;
void *work(void *argp);
void threads()
{
    pthread_t tid;
    args targ;
    if (pthread_create(&tid, NULL, work, &targ) != 0)
        ERR("ptread_create");

    if (pthread_cancel(tid) != 0)
        ERR("pthread_cancel");

    if (pthread_join(tid, NULL) != 0)
        ERR("pthread_join");
}

void cleanup(void *);
void *work(void *argp)
{
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    args *arg = (args *)argp;
    double *arr = malloc((sizeof *arr) * 10);
    pthread_cleanup_push(cleanup, arr);
    pthread_cleanup_pop(1);
    free(arr);
    return NULL;
}

unsigned rand()
{
    unsigned short xsubi[3] = {time(NULL)};
    return nrand48(xsubi);
}

void msleep(unsigned ms)
{
    struct timespec t = {ms / S_TO_MS, ms % S_TO_MS * MS_TO_NS};
    if (nanosleep(&t, NULL) != 0)
        ERR("nanosleep");
}

void set_sigmask()
{
    sigset_t new_mask, old_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    if (pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask) != 0)
        ERR("pthread_sigmask");
    if (pthread_sigmask(SIG_SETMASK, &old_mask, NULL) != 0)
        ERR("pthread_sigmask");
}

void check_for_pending_sig(int sig_num)
{
    static struct timespec t = {0, 0};
    sigset_t waiting;
    sigemptyset(&waiting);
    sigaddset(&waiting, sig_num);
    if (sigtimedwait(&waiting, NULL, &t) == sig_num)
    {
    }
    else if (EAGAIN != errno)
        ERR("sigtimedwait");
}

void wait_for_sig(int sig_num)
{
    sigset_t waiting;
    sigemptyset(&waiting);
    sigaddset(&waiting, sig_num);
    int sig;
    if (sigwait(&waiting, &sig) != 0)
        ERR("sigwait");
    switch (sig)
    {
    }
}