#include "string.h"
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <limits.h>
#include "void.h"

char *expand_path(const char *path) {
    if (path == NULL) {
        return NULL;
    }

    if (path[0] == '~') {
        struct passwd *pw = getpwuid(getuid());
        if (pw == NULL) {
            return string_dup(path);
        }
        
        size_t home_len = strlen(pw->pw_dir);
        size_t path_len = strlen(path);
        char *expanded = malloc(home_len + path_len);
        if (expanded == NULL) {
            return NULL;
        }
        
        strcpy(expanded, pw->pw_dir);
        strcat(expanded, path + 1);
        return expanded;
    }
    
    return string_dup(path);
}

char *trim_whitespace(char *str) {
    if (str == NULL) {
        return NULL;
    }

    /* Trim leading whitespace */
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
        str++;
    }

    if (*str == '\0') {
        return str;
    }

    /* Trim trailing whitespace */
    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }
    end[1] = '\0';

    return str;
}

char **split_string(const char *str, const char *delim, int *count) {
    if (str == NULL || delim == NULL || count == NULL) {
        return NULL;
    }

    /* Count tokens */
    char *str_copy = string_dup(str);
    if (str_copy == NULL) {
        return NULL;
    }

    int token_count = 0;
    char *token = strtok(str_copy, delim);
    while (token != NULL) {
        token_count++;
        token = strtok(NULL, delim);
    }
    free(str_copy);

    if (token_count == 0) {
        *count = 0;
        return NULL;
    }

    /* Allocate array */
    char **tokens = malloc((token_count + 1) * sizeof(char*));
    if (tokens == NULL) {
        *count = 0;
        return NULL;
    }

    /* Split again and store tokens */
    str_copy = string_dup(str);
    if (str_copy == NULL) {
        free(tokens);
        *count = 0;
        return NULL;
    }

    token = strtok(str_copy, delim);
    int i = 0;
    while (token != NULL && i < token_count) {
        tokens[i] = string_dup(token);
        if (tokens[i] == NULL) {
            /* Free what we've allocated so far */
            for (int j = 0; j < i; j++) {
                free(tokens[j]);
            }
            free(tokens);
            free(str_copy);
            *count = 0;
            return NULL;
        }
        i++;
        token = strtok(NULL, delim);
    }
    tokens[i] = NULL; /* Null terminate */
    *count = token_count;

    free(str_copy);
    return tokens;
}

void free_string_array(char **arr, int count) {
    if (arr == NULL) {
        return;
    }

    for (int i = 0; i < count; i++) {
        free(arr[i]);
    }
    free(arr);
}

bool starts_with(const char *str, const char *prefix) {
    if (str == NULL || prefix == NULL) {
        return false;
    }
    size_t prefix_len = strlen(prefix);
    return strncmp(str, prefix, prefix_len) == 0;
}

bool ends_with(const char *str, const char *suffix) {
    if (str == NULL || suffix == NULL) {
        return false;
    }
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str_len) {
        return false;
    }
    return strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}

char *string_dup(const char *str) {
    if (str == NULL) {
        return NULL;
    }
    size_t len = strlen(str) + 1;
    char *dup = malloc(len);
    if (dup == NULL) {
        return NULL;
    }
    memcpy(dup, str, len);
    return dup;
}

