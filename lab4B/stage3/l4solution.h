#ifndef L4_SOLUTION_H_
#define L4_SOLUTION_H_

/**
 * Good, universal header file should not include other header files (if not necessary),
 * but this header file is dedicated solely to this program, thus for convenient use
 * all header files are included here (instead of per each file separately).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

/// Structures

typedef struct suspend_result
{
    int n;
    ssize_t retval;
} suspend_result;

/// Constants

extern const int num_of_args;
extern const int num_of_files;
extern const int bufsize;

/// Error handling

#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

/// Core funciton declarations

void process_files(struct aiocb aiocbs[], int n, int bytes, int bufsize);
void sort(struct aiocb aiocbs[], int n, int nbytes);

/// Utility funciton declarations

void usage();
void read_args(int argc, char **argv, char *filename[num_of_args], int *level);
int open_file(const char *filename, int flags, int mask);
off_t filelen(int fd);
void aiocb_init(struct aiocb *aiocb, int fd, off_t offset, volatile void *buf, size_t nbytes, int lio_code);
void aiolist_init(struct aiocb *aiolist[], struct aiocb aiocbs[], int n);
suspend_result suspend(struct aiocb *aiolist[], int n);
void insertion_sort(char c[], int n);

#endif // L4_SOLUTION_H_