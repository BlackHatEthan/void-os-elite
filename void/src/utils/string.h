#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdbool.h>

/* Expand ~ to home directory */
char *expand_path(const char *path);

/* Trim whitespace from string */
char *trim_whitespace(char *str);

/* Split string by delimiter, returns array of strings (caller must free) */
char **split_string(const char *str, const char *delim, int *count);

/* Free string array allocated by split_string */
void free_string_array(char **arr, int count);

/* Check if string starts with prefix */
bool starts_with(const char *str, const char *prefix);

/* Check if string ends with suffix */
bool ends_with(const char *str, const char *suffix);

/* Duplicate string (wrapper for strdup with null check) */
char *string_dup(const char *str);

#endif /* STRING_H */

