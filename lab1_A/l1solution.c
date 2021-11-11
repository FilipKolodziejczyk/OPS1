#include "l1solution.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc < 2)
        INVALID_ARGUMENTS;

    uid_t user_id = getuid();

    for (int i = 1; i < argc; i++)
    {
        display_largest_file(argv[i], user_id);
    }

    return EXIT_SUCCESS;
}