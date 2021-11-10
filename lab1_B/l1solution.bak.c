#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>

#define ERR(source) (fprintf(stderr, "%s\n", source), exit(EXIT_FAILURE))
#define INVALID_ARG ERR("Invalid arguments")

#define MAX_STR_LEN 255
#define EXT_LEN 7

#define MODE_NOT_SET 0
#define GET_MODE 1
#define SET_MODE 2
#define LIST_MODE 3

int main(int argc, char **argv)
{
    char *key = NULL;
    char *val = NULL;
    int mode = MODE_NOT_SET;

    char c;
    while ((c = getopt(argc, argv, "gslk:v:")) != -1)
        switch (c)
        {
        case 'g':
            if (is_mode_set(mode) == true) INVALID_ARG;
            mode = GET_MODE;
            break;

        case 's':
            if (is_mode_set(mode) == true) INVALID_ARG;
            mode = SET_MODE;
            break;

        case 'l':
            if (is_mode_set(mode) == true) INVALID_ARG;
            mode = LIST_MODE;
            break;

        case 'k':
            key = optarg;
            for (int i = 0; i < strlen(key); i++)
            {
                if (!islower(key[i]))
                    ERR("invalid arguments");
            }
            break;

        case 'v':
            val = optarg;
            break;

        case '?':
        default:
            ERR("Invalid arguments");
            break;
        }

    if (mode == MODE_NOT_SET)
        ERR("Invalid arguments");
    if (mode == SET_MODE && (key == NULL || val == NULL))
        ERR("Invalid arguments");
    if (mode == GET_MODE && (key == NULL || val != NULL))
        ERR("invalid arguments");
    if (mode == LIST_MODE && (key != NULL || val != NULL))
        ERR("invalid arguments");

    char *dir = getenv("L1_DATABASE");
    if (dir != NULL)
        chdir(dir);

    if (mode == SET_MODE)
    {
        FILE *entry;
        char filename[MAX_STR_LEN + 1];
        strcpy(filename, key);
        strcat(filename, ".entry");

        // printf("Writing \"%s\" to file \"%s\"\n", val, filename);
        entry = fopen(filename, "w");
        if (entry == NULL)
            ERR("Can't create entry");
        fprintf(entry, "%s", val);
        fclose(entry);
    }
    else if (mode == GET_MODE)
    {
        FILE *entry;
        char filename[MAX_STR_LEN + 1];
        strcpy(filename, key);
        strcat(filename, ".entry");

        entry = fopen(filename, "r");
        if (entry == NULL)
            ERR("The entry doesn't exist");

        char entry_val[MAX_STR_LEN + 1];
        fgets(entry_val, MAX_STR_LEN, entry);
        printf("%s\n", entry_val);
        fclose(entry);
    }
    else if (mode == LIST_MODE)
    {
        char cwd[MAX_STR_LEN + 1];
        getcwd(cwd, MAX_STR_LEN);
        DIR *dirpt = opendir(cwd);
        if (dirpt == NULL)
            ERR("Can't open directory");

        struct dirent *ent = NULL;
        while ((ent = readdir(dirpt)) != NULL)
        {
            char base[MAX_STR_LEN + 1];
            char ext[MAX_STR_LEN + 1];
            ext[0] = '\0';
            sscanf(ent->d_name, "%[^.] %s", base, ext);
            if (strcmp(ext, ".entry") == 0)
                printf("%s\n", base);
        }
    }

    return EXIT_SUCCESS;
}