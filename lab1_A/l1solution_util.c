#include "l1solution.h"
#include <stdlib.h>
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>

/** Global variables */
uid_t uid;
off_t largest_file_size;
const char *largest_file_name;

void display_largest_file(const char *path, uid_t user_id)
{
    uid = user_id;
    largest_file_name = NULL;
    largest_file_size = -1;

    if (nftw(path, analysis, MAX_FTW_DEPTH, FTW_PHYS) != 0)
    {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
}

int analysis(const char *fpath, const struct stat *fstat, int ftype, struct FTW *ftwbuf)
{
    if (ftype == FTW_F && fstat->st_uid == uid)
    {
        printf("Analysing %s\n", fpath);

        if (fstat->st_size > largest_file_size)
        {
            largest_file_path = fpath;
            largest_file_size = fstat->st_size;
        }
    }

    return EXIT_SUCCESS;
}