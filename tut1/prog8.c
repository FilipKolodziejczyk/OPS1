#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

#define MAX_STR 20

int main(int argc, char **argv)
{
    char *times_env = getenv("TIMES");
    int times = 1;
    if (times_env != NULL)
        times = atoi(times_env);

    char name[MAX_STR + 2];

    while (fgets(name, MAX_STR + 2, stdin) != NULL)
    {
        for (int i = 0; i < times; i++)
            printf("Hello %s\n", name);
    }

    if (setenv("RESULT", "DONE", 1) != 0)
    {
        fprintf(stderr, "putenv failed");
        return EXIT_FAILURE;
    }

    printf("%s\n", getenv("RESULT"));
    if (system("env | grep RESULT") != 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}