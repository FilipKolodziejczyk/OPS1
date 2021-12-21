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
#define NUM_OF_ARGS 2
#define S_TO_MS 1000
#define MS_TO_NS 1000000
#define RAND_MOD 401
#define RAND_PLUS 100

/* Stuctures */
typedef struct _args
{
    int *building;
    pthread_mutex_t *mx;
    unsigned short *xsubi;
} args;

/* Funciton declarations */
void usage();
void read_args(int argc, char **argv, int *N_ptr, int *T_ptr);
void print_city(int *city, pthread_mutex_t *mx, int size);
void *work(void *argp);
void initialize_args(args *arr, int *city, pthread_mutex_t *mx, unsigned short *xsubi, int N);
void msleep(unsigned ms);
void king_kong(int *city, pthread_mutex_t *mx, int N, int T, pthread_t *threads, args *targs);
void set_sigmask(int sig_num);
int check_for_pending_sig(int sig_num);
void reset(int *city, pthread_mutex_t *mx, int size);

#endif