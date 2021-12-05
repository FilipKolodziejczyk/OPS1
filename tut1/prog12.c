#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define FILL_PERCENT 10

#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

void usage(char *pname, char *msg);
int valid_size(char *size_str);
int valid_perms(char *perms_str);
void make_file(char *name, ssize_t size, mode_t perms);

int main(int argc, char **argv)
{
    int c;
    char *name;
    ssize_t size = 0;
    mode_t perms = 0700;

    while ((c = getopt(argc, argv, "n:p:s:")) != -1)
    {
        switch (c)
        {
        case 'n':
            name = optarg;
            break;
        case 'p':
            if (!valid_perms(optarg))
                usage(argv[0], "Invalid permissions");
            perms = atol(optarg);
            break;
        case 's':
            if (!valid_size(optarg))
                usage(argv[0], "Invalid size");
            size = atol(optarg);
            break;
        case '?':
        default:
            usage(argv[0], "Missing argument value");
        }
    }

    make_file(name, size, perms);

    return EXIT_SUCCESS;
}

void usage(char *pname, char *msg)
{
    fprintf(stderr, "USAGE(%s): %s\n", pname, msg);
    exit(EXIT_FAILURE);
}

int valid_size(char *size_str)
{
    for (int i = 0; i < strlen(size_str); i++)
    {
        if (!isdigit(size_str[i]))
            return 0;
    }
    return 1;
}

int valid_perms(char *perms_str)
{
    if (strlen(perms_str) != 3)
        return 0;

    for (int i = 0; i < strlen(perms_str); i++)
    {
        if (!isdigit(perms_str[i]) || perms_str[i] == '8' && perms_str[i] == '9')
            return 0;
    }
    return 1;
}

void make_file(char *name, ssize_t size, mode_t perms)
{
    FILE *file;
    umask(~perms & 0777);
}