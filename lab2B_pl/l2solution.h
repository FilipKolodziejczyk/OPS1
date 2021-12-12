#ifndef L2SOULTION_H
#define L2SOLUTION_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void usage(const char *msg);
void create_children(int n, int s);
void stdin_work(int s);
void child_work(int s);
void parent_work();
int stdin_getline(char *buf, int s);
void sethandler(void (*f)(int), int sigNo);
void set_mask(int signo1, int signo2, int signo3);
void wait_for_sig();
void rewrite(const char* path, const char* buf);
void buf_update(char* buf, int s);
void sigalrm_handler();
void add_mask(int signo);
void sigusr1_handler();
void rand_swap(char *buf);
void sigusr2_handler();

#define ERR(source) (fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     perror(source),                                 \
                     kill(0, SIGKILL),                               \
                     exit(EXIT_FAILURE))

#define TIME 2
#define MAX_PATH 100
#define MILI_TO_NANO 1000000

#endif