#include "l1solution.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> /** For true and false */
#include <string.h>  /** For strcmp, strlen and strcpy */
#include <ctype.h>   /** For islower() */
#include <unistd.h>  /** For chdir() */

int is_mode_set(const int mode)
{
    if (mode == MODE_NOT_SET)
        return false;
    else
        return true;
}

int is_lowercase(const char *str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (!islower(str[i]))
            return false;
    }

    return true;
}

int is_valid_mode(const int mode, const char *key, const char *val)
{
    if (mode == MODE_NOT_SET)
        return false;

    if (mode == SET_MODE && (key == NULL || val == NULL))
        return false;

    if (mode == GET_MODE && (key == NULL || val != NULL))
        return false;

    if (mode == LIST_MODE && (key != NULL || val != NULL))
        return false;

    return true;
}

void set_cwd()
{
    char *dir = getenv("L1_DATABASE");
    if (dir != NULL)
        chdir(dir);
}

void parse_key(char *dest, const char *key)
{
    if ((strlen(key) + EXT_LEN) > MAX_STR_LEN) // +7 = ".entry" offset
        ERR("Too long key name");

    strcpy(dest, key);
    strcat(dest, ".entry");
}