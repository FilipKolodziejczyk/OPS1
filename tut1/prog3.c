#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXL 20
#define MAXL2 21
#define S_(x) #x
#define S(x) S_(x)
#define ERR(source) (perror(source),                                 \
                     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                     exit(EXIT_FAILURE))

int main(int argc, char **argv)
{
    char name[MAXL + 2];
    scanf("%"S(MAXL2)"s", name);
    if(strlen(name) > MAXL)
        ERR("Name too long");
    printf("Hello %s\n", name);

    return EXIT_SUCCESS;
}