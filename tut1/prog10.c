#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

#define MAX_PATH 100

void scan_cwd();

int main(int argc, char **argv)
{
    char path[MAX_PATH];
    if (getcwd(path, MAX_PATH) == NULL)
        ERR("getcwd");

    for (int i = 1; i < argc; i++)
    {
        if (chdir(argv[i]) == -1)
        {
            printf("Can't open %s.\n", argv[i]);
            continue;
        }

        printf("%s:\n", argv[i]);
        scan_cwd();
    }

    if (chdir(path) == -1)
            ERR("chdir");

    return EXIT_SUCCESS;
}

void scan_cwd()
{
    DIR *dirp;
    if (NULL == (dirp = opendir(".")))
        ERR("opendir");

    struct dirent *dp;
    struct stat fstat;
    int dirs, files, links, others;
    dirs = files = links = others = 0;

    do
    {
        errno = 0;
        if ((dp = readdir(dirp)) != NULL)
        {
            if (lstat(dp->d_name, &fstat))
                ERR("lstat");

            if (S_ISDIR(fstat.st_mode))
                dirs++;
            else if (S_ISLNK(fstat.st_mode))
                links++;
            else if (S_ISREG(fstat.st_mode))
                files++;
            else
                others++;
        }
    } while (dp != NULL);

    if (errno != 0)
        ERR("readdir");
    if (closedir(dirp))
        ERR("closedir");

    printf("Dirs: %d\n", dirs);
    printf("Files: %d\n", files);
    printf("Links: %d\n", links);
    printf("Others: %d\n", others);
}