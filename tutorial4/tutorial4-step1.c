#include <stdio.h>
#include <stdlib.h>

#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

void decryptor(int nb, int sb, char **msg_files, char **key_files, char **out_files, int nfiles);

void usage(char *filename) {
    printf("Usage: %s nb sb msg1 key1 out1 [msg2 key2 out2 ...]\n", filename);
    printf("nb - number of aio blocks per each file\n");
    printf("sb - size of buffers in aio control blocks\n");
    printf("msg? - name/path of the file containing encrypted message\n");
    printf("key? - name/path of the file containing key used to decrypt message\n");
    printf("out? - name/path of the file to which decrypted message will be written\n");
}

int main(int argc, char **argv) {
    if (argc < 6) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int nb = atoi(argv[1]);
    if (0 == nb)
        usage(argv[0]);

    int sb = atoi(argv[2]);
    if (0 == sb)
        usage(argv[0]);

    int nfiles = (argc - 3) / 3;
    if ((argc - 3) % 3 != 0)
        usage(argv[0]);

    char **msg_files = (char **)malloc(nfiles * sizeof(char *));
    if (!msg_files)
        ERR("malloc");

    char **key_files = (char **)malloc(nfiles * sizeof(char *));
    if (!key_files)
        ERR("malloc");

    char **out_files = (char **)malloc(nfiles * sizeof(char *));
    if (!out_files)
        ERR("malloc");
    
    for (int i = 0; i < nfiles; i++) {
        msg_files[i] = argv[3 + 3 * i];
        key_files[i] = argv[4 + 3 * i];
        out_files[i] = argv[5 + 3 * i];
    }

    decryptor(nb, sb, msg_files, key_files, out_files, nfiles);

    free(msg_files);
    free(key_files);
    free(out_files);

    return EXIT_SUCCESS;
}

void decryptor(int nb, int sb, char **msg_files, char **key_files, char **out_files, int nfiles) {

}