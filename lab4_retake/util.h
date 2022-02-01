#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <aio.h>

/// Data types


/// Error handling

#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

/// Utility functions

void usage();
void aiocb_init(struct aiocb *aiocb, char *buf, int filedes, size_t bufsize, off_t offset, void *fun(union sigval));

#endif // UTIL_H_