#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#define __USE_XOPEN_EXTENDED
#include <ftw.h>

#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

#define MAX_FD 20
int dirs, files, links, others;

int count_elements(const char *path, const struct stat *el_stat, int el_type, struct FTW *ftw);

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        dirs = files = links = others = 0;
        if (nftw(argv[i], count_elements, MAX_FD, FTW_PHYS) != 0)
        {
            printf("Can't access %s.\n", argv[i]);
            continue;
        }

        printf("Dirs: %d\n", dirs);
        printf("Files: %d\n", files);
        printf("Links: %d\n", links);
        printf("Others: %d\n", others);
    }

    return EXIT_SUCCESS;
}

int count_elements(const char *path, const struct stat *el_stat, int el_type, struct FTW *ftw)
{
    switch (el_type)
    {
    case FTW_D:
    case FTW_DNR:
        dirs++;
        break;
    case FTW_SL:
        links++;
        break;
    case FTW_F:
        files++;
        break;
    default:
        others++;
    }

    return 0;
}