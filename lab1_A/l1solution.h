#ifndef L1_SOLUTION
#define L1_SOLUTION

#define _XOPEN_SOURCE 500
#include <ftw.h>

/** Error handling */
#define ERROR(source) (fprintf(stderr, "%s\n", source), exit(EXIT_FAILURE))
#define INVALID_ARGUMENTS ERROR("Invalid arguments")

/** Program restrictions */
#define MAX_FTW_DEPTH 20

/** Utility function */
void display_largest_file(const char *path, uid_t user_id);
int analysis(const char *fpath, const struct stat *fstat, int ftype, struct FTW *ftwbuf);

#endif