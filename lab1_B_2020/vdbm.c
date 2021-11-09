#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_LEN 100
#define MAX_LINE_LEN 64

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Missing arguments!\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "create") == 0)
    {
        FILE *db = fopen(argv[2], "r");
        if (db != NULL)
        {
            fprintf(stderr, "File %s already exists\n", argv[2]);
            exit(EXIT_FAILURE);
        }

        db = fopen(argv[2], "w");
        if (db == NULL)
        {
            fprintf(stderr, "Can't create %s file\n", argv[2]);
            exit(EXIT_FAILURE);
        }

        for (int i = 3; i < argc; i++)
        {
            if (strlen(argv[i]) >= 32)
            {
                fprintf(stderr, "Option %s is too long\n", argv[i]);
                exit(EXIT_FAILURE);
            }
            fprintf(db, "%-32s%-31s\n", argv[i], "0");
        }

        fclose(db);
    }
    else if (strcmp(argv[1], "show") == 0)
    {
        FILE *db = fopen(argv[2], "r");
        if (db == NULL)
        {
            fprintf(stderr, "Can't open file %s\n", argv[2]);
            exit(EXIT_FAILURE);
        }

        char line[MAX_LINE_LEN];
        printf("Database filename: %s\n", argv[2]);
        while (fgets(line, MAX_LINE_LEN + 1, db) != NULL)
            printf("%s", line);
    }
    else if (strcmp(argv[1], "vote") == 0)
    {
        if (argc < 4)
        {
            fprintf(stderr, "Missing arguments!\n");
            exit(EXIT_FAILURE);
        }

        FILE *db = fopen(argv[2], "r+");
        if (db == NULL)
        {
            fprintf(stderr, "Can't open file %s\n", argv[2]);
            exit(EXIT_FAILURE);
        }

        char line[MAX_LINE_LEN];
        char option[MAX_LINE_LEN / 2];
        char num[MAX_LINE_LEN / 2 + 1];
        while (fgets(line, MAX_LINE_LEN + 1, db) != NULL)
        {
            strncpy(option, line, MAX_LINE_LEN / 2);
            printf("%s\n", option);
            option[32] = '\0';
            printf("%d\n", strcmp(option, argv[3]));
            if (strcmp(option, argv[3]) == 0)
            {
                printf("Hello");
                strncpy(num, line + 32, MAX_LINE_LEN / 2);
                printf("%s\n", num);
            }
        }
    }
    else
    {
        fprintf(stderr, "Option %s is not defined\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}