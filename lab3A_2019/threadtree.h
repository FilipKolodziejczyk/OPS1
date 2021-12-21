#ifndef THREAD_TREE
#define THREAD_TREE

/**
 * Good, universal header file should not include other header files (if not necessary),
 * but this header file is dedicated solely to this program, thus for convenient use
 * all header files are included here
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>

/* Error handling */
#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

/* Constants */
#define NUM_OF_ARGS 2

/* Stuctures */
typedef struct _args
{
    int child_thread_num;
    int current_depth;
    int max_depth;
    unsigned short *xsubi;
    sigset_t *new_mask;
    int sig_limit;
    pthread_mutex_t *mx_sig_limit;
} args;

/* Funciton declarations */
void usage();
void read_args(const int argc, char **argv, int *n, int *k);
void initialize_args(args *arr, args *this_arg, pthread_mutex_t *mx);
void *worker(void *argp);
void manage_workes(args *thread_args, pthread_t *threads, args *this);
void main_work(pthread_t *threads, args *worker_args, args *this);
void block_sig(int sig_num, sigset_t *new_mask, sigset_t *old_mask);
int draw_thread(args *worker_args, args *this);

#endif