#ifndef L2SOLUTION_H
#define L2SOLUTION_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

/* PREPARATION */
void usage(const char *msg);
int invalid_filename(const char *filename);

#define ERR(source) (fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     perror(source),                              \
                     kill(0, SIGKILL),                            \
                     exit(EXIT_FAILURE))

#define MAX_FILENAME 15
#define MILI_TO_NANO 1000000
#define MAX_BUF 100

/* PART 1 */
pid_t create_children(int n);
void child_work();

/* PART 2 */
void stdin_work(int n);
int read_line(char *dest, int size);
void clear_stdin();
void sethandler(void (*f)(int), int sig_num);
void sigusr1_handler(int sig_num);
void sigusr2_handler(int sig_num);

/* PART 3 */
void write_last_signal(int fd);

/* PART 4 */
void sigint_handler(int sig_num);
void save(char *filename);

#endif