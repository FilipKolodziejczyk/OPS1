#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#define __USE_XOPEN_EXTENDED
#include <ftw.h>

#define MAX_FILE_PATH 255
#define MAX_NO_OPENED_DIRS 20
#define MAX_FILE_EXT 50

int no_txt_files;
int no_c_files;
int no_h_files;

// int no_files = 0;

int fun(const char *fpath, const struct stat *fstat, int ftype, struct FTW *ftw_struct);

int main(int argc, char **argv)
{
    // char cwd[MAX_FILE_PATH];
    // getcwd(cwd, MAX_FILE_PATH);

    for (int i = 1; i < argc; i++)
    {
        no_c_files = no_h_files = no_txt_files = 0;

        if (nftw(argv[i], fun, MAX_NO_OPENED_DIRS, 0) == -1)
        {
            perror("nftw");
            exit(EXIT_FAILURE);
        }

        printf("Directory: %s\n", argv[i]);
        // printf("All: %d\n", no_files);
        printf("txt: %d\n", no_txt_files);
        printf("c: %d\n", no_c_files);
        printf("h: %d\n", no_h_files);
    }

    exit(EXIT_SUCCESS);
}

int fun(const char *fpath, const struct stat *fstat, int ftype, struct FTW *ftw_struct)
{
    if (ftype == FTW_F)
    {
        //no_files++;
        char ext[4];
        char fname[1000];
        strcpy(fname, fpath);
        sscanf(basename(fname), "%*[^.] . %3s", ext);

        if (strcmp(ext, "txt") == 0)
            no_txt_files++;
        else if (strcmp(ext, "c") == 0)
            no_c_files++;
        else if (strcmp(ext, "h") == 0)
            no_h_files++;
    }

    return 0;
}