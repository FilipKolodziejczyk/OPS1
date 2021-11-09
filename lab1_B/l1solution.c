#include "l1solution.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    char *key = NULL;
    char *val = NULL;
    int mode = input_parsing(argc, argv, &key, &val);

    set_cwd();

    if (mode == SET_MODE)
    {
        set_key(key, val);
    }
    else if (mode == GET_MODE)
    {
        get_key(key);
    }
    else if (mode == LIST_MODE)
    {
        list_keys();
    }

    return EXIT_SUCCESS;
}