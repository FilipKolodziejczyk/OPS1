#ifndef L4_SOLUTION
#define L4_SOLUTION

/**
 * Good, universal header file should not include other header files (if not necessary),
 * but this header file is dedicated solely to this program, thus for convenient use
 * all header files are included here
 */
#include <stdio.h>
#include <stdlib.h>
#include <aio.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/* Error handling */
#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

/* Constants */
#define NUM_OF_ARGS 5

/* Stuctures */


/* Funciton declarations */
void usage();
void read_args(int argc, char **argv, char **ofn1, char **ofn2, char **ofn3, char **ifn, int *bs);
int open_file(const char *filename, int flags, int mask);
off_t filelen(int fd);
void aiocb_init(struct aiocb *aiocb, char *buf, int filedes, size_t bufsize, off_t offset);
void suspend(struct aiocb *aiocb);

#endif