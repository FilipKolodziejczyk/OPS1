#include <stdio.h>
#include <aio.h>
#include <errno.h>
#include <stdlib.h>

#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

void decryptor(int nb, int sb, char **msg_files, char **key_files, int nfiles);

void usage(char *filename) {
    printf("Usage: %s nb sb msg1 key1 [msg2 key2 ...]\n", filename);
    printf("nb - number of aio blocks per each file\n");
    printf("sb - size of buffers in aio control blocks\n");
    printf("msg? - name/path of the file containing encrypted message\n");
    printf("key? - name/path of the file containing key used to encrypt the relevant (same number) message\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc < 5) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int nb = atoi(argv[1]);
    if (0 == nb)
        usage(argv[0]);

    int sb = atoi(argv[2]);
    if (0 == sb)
        usage(argv[0]);

    int nfiles = (argc - 3) / 2;
    if ((argc - 3) % 2 != 0)
        usage(argv[0]);

    char **msg_files = (char **)malloc(nfiles * sizeof(char *));
    if (!msg_files)
        ERR("malloc");

    char **key_files = (char **)malloc(nfiles * sizeof(char *));
    if (!key_files)
        ERR("malloc");
    
    for (int i = 0; i < nfiles; i++) {
        msg_files[i] = argv[3 + 2 * i];
        key_files[i] = argv[4 + 2 * i];
    }

    decryptor(nb, sb, msg_files, key_files, nfiles);

    free(msg_files);
    free(key_files);

    return EXIT_SUCCESS;
}

void decryptor(int nb, int sb, char **msg_files, char **key_files, int nfiles) {

}