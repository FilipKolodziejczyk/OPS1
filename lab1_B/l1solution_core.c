#include "l1solution.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> /** For true and false */
#include <string.h>  /** For strcmp, strlen and strcpy */
#ifndef __USE_POSIX2
#define __USE_POSIX2 /** For VS CODE - w/o it marks oprtarg as not defined */
#endif
#include <unistd.h>  /** For getopt */
#include <dirent.h>  /** For DIR, dirent and readdir() */

int input_parsing(const int argc, char **const argv, char **key, char **val)
{
    char c;
    int mode = MODE_NOT_SET;

    while ((c = getopt(argc, argv, "gslk:v:")) != -1)
    {
        switch (c)
        {
        case 'g':
            if (is_mode_set(mode) == true)
                INVALID_ARG;
            mode = GET_MODE;
            break;

        case 's':
            if (is_mode_set(mode) == true)
                INVALID_ARG;
            mode = SET_MODE;
            break;

        case 'l':
            if (is_mode_set(mode) == true)
                INVALID_ARG;
            mode = LIST_MODE;
            break;

        case 'k':
            *key = optarg;
            if (is_lowercase(*key) == false)
                INVALID_ARG;
            
            break;

        case 'v':
            *val = optarg;
            break;

        case '?':
        default:
            INVALID_ARG;
            break;
        }
    }

    if (is_valid_mode(mode, *key, *val) == false)
        INVALID_ARG;
    
    return mode;
}

void set_key(const char *key, const char *val)
{
    char entry_key[MAX_STR_LEN + 1];
    parse_key(entry_key, key);

    // printf("Writing \"%s\" to file \"%s\"\n", val, filename);
    FILE *entry;
    entry = fopen(entry_key, "w");
    if (entry == NULL)
        ERR("Cannot create entry");
    fprintf(entry, "%s", val);
    fclose(entry);
}

void get_key(const char *key)
{
    char entry_key[MAX_STR_LEN + 1];
    parse_key(entry_key, key);

    FILE *entry;
    entry = fopen(entry_key, "r");
    if (entry == NULL)
        ERR("The entry does not exist");

    char c = fgetc(entry);
    while (c != EOF)
    {
        printf("%c", c);
        c = fgetc(entry);
    }
    printf("\n");
    fclose(entry);
}

void list_keys()
{
    char cwd[MAX_STR_LEN + 1];
    getcwd(cwd, MAX_STR_LEN);
    if (cwd == NULL)
        ERR("Cwd could not be determined or path too long (max 255 characters)");

    DIR *dir = opendir(cwd);
    if (dir == NULL)
        ERR("Cannot open directory");

    struct dirent *entry = NULL;
    while ((entry = readdir(dir)) != NULL)
    {
        char key[MAX_STR_LEN + 1];
        key[0] = '\0';
        key[255] = '\0';
        char ext[EXT_LEN + 1];
        ext[0] = '\0';

        sscanf(entry->d_name, "%255[^.] %7s", key, ext);
        if (key[255] != '\0')
            ERR("Buffer overflow (file name)");
        if (strcmp(ext, ".entry") == 0)
            printf("%s\n", key);
    }
}