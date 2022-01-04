#ifndef PROG21_H
#define PROG21_H

/* Headers */
/**
 * Good, universal header file should not include other header files (if not necessary),
 * but this header file is dedicated solely to this program, thus for convenient use
 * all header files are included here
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <aio.h>
#define __USE_GNU
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>

/* Error handling */
#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

/* Constants */
#define NUM_OF_ARGS 3
#define LEVEL 3

/* Stuctures */
typedef struct _args
{
    int *building;
    pthread_mutex_t *mx;
    unsigned short *xsubi;
} args;

/* Funciton declarations */
void usage();
void read_args(int *n, int *k, char **filename, int argc, char **argv);
off_t filesize(int filedes);
void buf_init(char *buf[], int n, int size);
void aiocbs_init(struct aiocb *aiocbs, char *buf[], int filedes, int n, int size);
void draw_indexes(int *i1, int *i2, int max);
void suspend(struct aiocb *aiocb);
void reverse_buffer(char *buf, int size);
void read_off(struct aiocb *aiocb, off_t offset);
void write_off(struct aiocb *aiocb, off_t offset);
void data_sync(struct aiocb *aiocb);
void process_file(struct aiocb *aiocbs, int nblock, int times, off_t block_size);
void sethandler(void (*handler)(int), int signo);
void sigint_handler(int signo);
void cancel(int filedes, struct aiocb *aiocbs);

#endif