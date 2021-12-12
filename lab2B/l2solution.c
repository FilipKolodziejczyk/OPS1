#include "l2solution.h"

int main(int argc, char **argv)
{
    if (argc != 3)
        usage("Invalid number of arguments");

    int n = atol(argv[1]);
    if (n < 2 || n > 10)
        usage("Invalid 1st argument");

    char *filename = argv[2];
    if (invalid_filename(filename))
        usage("Invalid 2nd argument");

    sethandler(SIG_IGN, SIGUSR1);
    sethandler(SIG_IGN, SIGUSR2);

    pid_t stdin_pid = create_children(n);

    // /* PART 1 */
    // while (wait(NULL) > 0)
    //     ;

    // /* PART 2 */
    // wait(NULL);
    // kill(0, SIGTERM);

    /* PART 4 */
    sethandler(SIG_IGN, SIGINT);
    if (waitpid(stdin_pid, NULL, 0) == -1)
        ERR("waitpid");
    kill(0, SIGINT);
    save(filename);

    return EXIT_SUCCESS;
}

pid_t create_children(int n)
{
    pid_t pid;
    for (int i = 0; i <= n; i++)
    {
        pid = fork();
        if (pid < 0)
            ERR("fork");

        /* PART 1 */
        // if (pid == 0)
        // {
        //     child_work();
        //     exit(EXIT_SUCCESS);
        // }

        if (pid == 0)
        {
            if (i == n)
            {
                stdin_work(n);
            }
            else
                child_work();
            exit(EXIT_SUCCESS);
        }
    }
    return pid;
}

void stdin_work(int n)
{
    char *buf = malloc(n + 2);
    if (NULL == buf)
        ERR("malloc");

    struct timespec time = {0, 500 * MILI_TO_NANO};
    while (read_line(buf, n) != EOF)
    {
        for (size_t i = 0; i + 1 < strlen(buf); i++)
        {
            if (buf[i] == '0')
                kill(0, SIGUSR1);
            else
                kill(0, SIGUSR2);
            nanosleep(&time, NULL);
        }
    }
    free(buf);
}

volatile sig_atomic_t last_sig = 0;

void child_work()
{
    /* PART 1 */
    // srand(time(NULL) * getpid());
    // unsigned time = rand() % 3 + 1;
    // sleep(time);
    // printf("%d\n", getpid());

    sethandler(sigusr1_handler, SIGUSR1);
    sethandler(sigusr2_handler, SIGUSR2);
    sethandler(sigint_handler, SIGINT);

    // /* PART 2 */
    // while(1)
    //     ;

    char filename[MAX_FILENAME];
    if (snprintf(filename, MAX_FILENAME, "%d.txt", getpid()) == -1)
        ERR("snprintf");

    int fd = open(filename, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
    if (fd < 0)
        ERR("open");

    srand(time(NULL) * getpid());
    while (1)
    {
        int ms = rand() % 2701 + 300;
        struct timespec time = {ms / 1000, ms % 1000 * MILI_TO_NANO};
        nanosleep(&time, NULL);
        write_last_signal(fd);
    }
}