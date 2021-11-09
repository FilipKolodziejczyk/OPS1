#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
/** Define below is needed to use FTW_PHYS flag. Check ftw.h file */
#define __USE_XOPEN_EXTENDED
#include <ftw.h>

#define MAX_FILENAME_LEN 255
#define MAX_IGNORE_STRING_LEN 255
#define MAX_NO_OPENED_FOLDERS 20
const char *ignore_arg = "-I";

/** Global variables because we can't pass to or return with function "analyze" this stuff */
off_t largest_file_size;
char largest_file_name[MAX_FILENAME_LEN];
uid_t uid;
FILE *logfile = NULL;
const char *ignore_string = NULL;

/** Declaration according to nftw declaration */
int analyze(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

int main(int argc, char **argv)
{
    /** If -I option we need at least 4 args, because argv[1] is "-I" and argv[2] is ignore_string */
    if (argc < 2 || (strcmp(argv[1], ignore_arg) == 0 && argc < 4))
    {
        fprintf(stderr, "Missing arguments!\n");
        exit(EXIT_FAILURE);
    }

    uid = getuid();

    char *logfile_name = getenv("L1_LOGFILE");
    if (logfile_name != NULL && strlen(logfile_name) > 0)
    {
        /** logfile_name exists and not empty */
        logfile = fopen(logfile_name, "w");
    }

    if (strcmp(argv[1], ignore_arg) == 0)
    {
        /** Arg -I passed */
        if (strlen(argv[2]) >= MAX_IGNORE_STRING_LEN)
        {
            fprintf(stderr, "Ignore string too long (max lenght is 254 characters)!\n");
            exit(EXIT_FAILURE);
        }

        ignore_string = argv[2];
    }
    /** We start from i = 3 if arg "-I" used */
    for (int i = (ignore_string != NULL ? 3 : 1); i < argc; i++)
    {
        largest_file_size = 0;
        largest_file_name[0] = '\0';

        if (nftw(argv[i], analyze, MAX_NO_OPENED_FOLDERS, FTW_PHYS) != 0)
        {
            perror("NFTW error");
            exit(EXIT_FAILURE);
        }

        if (strlen(largest_file_name) == 0)
        {
            printf("No files found in %s\n", argv[i]);
        }
        else
        {
            printf("Largest file in %s: %s (%ld)\n", argv[i], largest_file_name, largest_file_size);
        }
    }

    if (logfile != NULL)
    {
        fclose(logfile);
    }

    return EXIT_SUCCESS;
}

int analyze(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F && sb->st_uid == uid)
    {
        /** fpath if file (FTW_F) and its owner is uid */

        if (ignore_string != NULL && sb->st_size > strlen(ignore_string))
        {
            /** ignore_string exists and file fpath is not shorter than ignore_string */
            FILE *temp_file = fopen(fpath, "r");
            char temp_str[MAX_IGNORE_STRING_LEN];
            fgets(temp_str, strlen(ignore_string) + 1, temp_file);
            if (strcmp(temp_str, ignore_string) == 0)
            {
                /** File contains ignore_string at the beginning - ignoring */
                return 0;
            }
        }

        fprintf(logfile != NULL ? logfile : stdout, "Analyzing %s\n", fpath);

        if (strlen(largest_file_name) == 0 || sb->st_size > largest_file_size)
        {
            if (strlen(fpath) >= MAX_FILENAME_LEN)
            {
                fprintf(stderr, "Too long filename\n");
                exit(EXIT_FAILURE);
            }
            strcpy(largest_file_name, fpath);
            largest_file_size = sb->st_size;
        }
    }

    return 0;
}