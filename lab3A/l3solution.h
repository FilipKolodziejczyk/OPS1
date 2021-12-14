#ifndef L3_SOLUTION
#define L3_SOLUTION

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

/* Error handling */
#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

/* Constants */
#define NUM_OF_ARGS 1
#define T_MIN 2
#define T_MAX 128
#define S_TO_MS 1000
#define MS_TO_NS 1000000
#define NUM_OF_COMPETITORS 2
#define PULL_INTERVAL 100
#define MAX_PULL 6

/* Stuctures */
typedef struct _competitor_args
{
    unsigned short *xsubi;
    int id;
    int *L_ptr;
    pthread_mutex_t *mxL_ptr;
} competitor_args;

/* Funciton declarations */
void usage();
void read_args(const int argc, char **argv, int *T_ptr);
void *judge_work(void *arg_ptr);
void *competitor_work(void *arg_ptr);
void msleep(unsigned ms);
void judge(int *L_ptr, pthread_mutex_t *mxL_ptr, int *T_ptr);
void pull(int pull_val, competitor_args *arg);
void block_sig(int sig_num, sigset_t *new, sigset_t *old);
void reset(int *i, sigset_t *waiting, int *L_ptr, pthread_mutex_t *mxL_ptr);

#endif