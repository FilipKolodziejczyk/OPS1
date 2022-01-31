#ifndef TASK4_H
#define TASK4_H

#include <stdio.h>
#include <stdlib.h>
#include <aio.h>
#include <signal.h>

/// Data types

typedef struct attrib
{
    struct aiocb *aiocb;
    pthread_t main_tid;
    int *aio_progress;
    pthread_mutex_t *aio_progress_mx;
} attrib;

/// Error handling

#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

/// Utility functions

void usage();
void open_files(char *dirname, int fd[], int *files_opened, int max_fd, int max_size);
void aiocb_init(struct aiocb *aiocb, char *buf, int filedes, size_t bufsize, off_t offset, void *fun(union sigval));
void attrib_init(attrib *attr ,struct aiocb *aiocb, pthread_t main_tid, int *aio_progress, pthread_mutex_t *aio_progress_mx);
int is_vowel(char c);
void sethandler(void (*f)(int), int sig_num);

#endif //TASK4_H