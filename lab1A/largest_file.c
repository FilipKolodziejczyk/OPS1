/**
 * This is a simple program that finds the largest file in a given directories.
 * Direcories are given as agruments.
 * It searches recursively through the directory trees.
 * It ommits every non-regular file and files not owned by the user.
 * Files may by additionaly skipped considering its content.
 * Using option -I [string] will skip files containing the string at the beginning.
 * Option -I may be used multiple times.
 * If environment variable L1_LOGFILE is set, information about all analyzed files will be written there.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ftw.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/* Constants */
#define MAX_PATH_LENGTH 1024
#define MAX_NFTW_FD 10
#define MAX_FILTERS 10
#define LOGFILE_ENV "L1_LOGFILE"

/* Global variables */
off_t largest_file_size;
char largest_file_name[MAX_PATH_LENGTH];
FILE *logfile = NULL;
char *filter_list[MAX_FILTERS];
int filter_list_length = 0;

#define ERR(source) ((perror(source),                                 \
                      fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                      exit(EXIT_FAILURE)))

int process_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int string_file_equality(FILE *file, char *string, off_t file_offset);

int main(int argc, char **argv)
{
    char *logfile_path = getenv(LOGFILE_ENV);
    if (logfile_path != NULL)
    {
        printf("Logfile: %s\n", logfile_path);
        logfile = fopen(logfile_path, "w");
        if (logfile == NULL)
            ERR("fopen");
    }

    int option;
    while ((option = getopt(argc, argv, "I:")) != -1)
    {
        switch (option)
        {
        case 'I':
            filter_list_length++;
            if (filter_list_length > MAX_FILTERS)
            {
                fprintf(stderr, "Too many filters\n");
                exit(EXIT_FAILURE);
            }

            filter_list[filter_list_length - 1] = optarg;
            break;

        default:
            fprintf(stderr, "Usage: %s [-I filtered_string] list_of_paths\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++)
    {
        largest_file_size = -1;

        fprintf(logfile, "Scanned dir: %s\n", argv[i]);

        if (nftw(argv[i], process_file, MAX_NFTW_FD, FTW_PHYS) == -1)
            ERR("nftw");

        if (largest_file_size == -1)
            printf("No files found in %s\n", argv[i]);
        else
            printf("Largest file in %s is %s [%ld]\n", argv[i], largest_file_name, largest_file_size);

        fprintf(logfile, "\n");
    }

    if (logfile != NULL)
        if (fclose(logfile) == EOF)
            ERR("fclose");

    return EXIT_SUCCESS;
}

int process_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    bool is_file = (typeflag == FTW_F);
    bool is_user_file = (sb->st_uid == getuid());
    bool is_largest_file = (sb->st_size > largest_file_size);

    if (is_file && is_user_file)
    {
        if (filter_list_length != 0)
        {
            bool is_filtered = false;
            FILE *current_file = fopen(fpath, "r");
            if (current_file == NULL)
            {
                // Skipping files which cannot be read instead of throwing error
                return EXIT_SUCCESS;
            }

            for (int i = 0; i < filter_list_length; i++)
            {
                if (string_file_equality(current_file, filter_list[i], 0) == true)
                {
                    is_filtered = true;
                    break;
                }
            }

            if (fclose(current_file) == EOF)
                ERR("fclose");

            if (is_filtered)
                return EXIT_SUCCESS;
        }

        if (logfile != NULL)
            fprintf(logfile, "%s[%ld]\n", fpath, sb->st_size);

        if (is_largest_file)
        {
            largest_file_size = sb->st_size;
            if (strlen(fpath) > MAX_PATH_LENGTH)
            {
                fprintf(stderr, "Path too long: %s\n", fpath);
                exit(EXIT_FAILURE);
            }
            strncpy(largest_file_name, fpath, MAX_PATH_LENGTH);
        }
    }

    return EXIT_SUCCESS;
}

// Skipping files which cannot be read instead of throwing error
int string_file_equality(FILE *file, char *string, off_t file_offset)
{
    if (fseek(file, file_offset, SEEK_SET) == -1)
        return false;

    char c;
    for (int i = 0; i < strlen(string); i++)
    {
        c = fgetc(file);
        if (c == EOF || c != string[i])
        {
            return false;
        }
    }

    return true;
}