#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    printf("A");
    fork();
    printf("B");
    wait(NULL);
    return EXIT_SUCCESS;
}
