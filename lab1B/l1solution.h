#ifndef L1_SOLUTION
#define L1_SOLUTION

/** Error handling */
#define ERR(source) (fprintf(stderr, "%s\n", source), exit(EXIT_FAILURE))
#define INVALID_ARG ERR("Invalid arguments")

/** Buffers restrictions */
#define MAX_STR_LEN 255
#define EXT_LEN 7 // = strlen(".entry")

/** Operating modes descriptors */
#define MODE_NOT_SET 0
#define GET_MODE 1
#define SET_MODE 2
#define LIST_MODE 3

/** Utility functions */
int is_mode_set(const int mode);
int is_lowercase(const char *str);
int is_valid_mode(const int mode, const char *key, const char *val);
void set_cwd();
void parse_key(char *dest, const char *key);

/** Core functions */

/** 
 * Parses arguments into proper variables.
 * Properly handles invalid arguments (resulting in program failure).
 * @return Selected mode.
 */
int input_parsing(const int argc, char **const argv, char **key, char **val);

/**
 * Creates database entry "key.entry" with value "val".
 * Results in program failure if cannot create entry.
 */
void set_key(const char *key, const char *val);

/**
 * Displays "key.entry" value.
 * Results in program failure if entry does not exist.
 */
void get_key(const char *key);

/**
 * Displays all entries in cwd (depends on L1_DATABASE enviromental variable).
 * Results in program failure if cannot access cwd.
 */
void list_keys();

#endif