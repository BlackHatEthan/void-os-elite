#include "parser.h"
#include "../utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int parse_command(const char *input, char **argv, int max_args) {
    if (input == NULL || argv == NULL || max_args <= 0) {
        return 0;
    }
    
    /* Skip leading whitespace */
    while (isspace(*input)) {
        input++;
    }
    
    if (*input == '\0') {
        return 0;
    }
    
    /* Simple tokenizer - handles quoted strings */
    int argc = 0;
    const char *p = input;
    bool in_quotes = false;
    char quote_char = '\0';
    
    while (*p != '\0' && argc < max_args - 1) {
        /* Skip whitespace (unless in quotes) */
        if (!in_quotes && isspace(*p)) {
            p++;
            continue;
        }
        
        /* Handle quotes */
        if ((*p == '"' || *p == '\'') && !in_quotes) {
            in_quotes = true;
            quote_char = *p;
            p++;
            continue;
        }
        
        if (in_quotes && *p == quote_char) {
            in_quotes = false;
            p++;
            continue;
        }
        
        /* Start of token */
        const char *start = p;
        
        /* Find end of token */
        while (*p != '\0') {
            if (in_quotes) {
                if (*p == quote_char) {
                    break;
                }
                p++;
            } else {
                if (isspace(*p)) {
                    break;
                }
                p++;
            }
        }
        
        /* Allocate and copy token */
        size_t token_len = p - start;
        argv[argc] = malloc(token_len + 1);
        if (argv[argc] == NULL) {
            /* Free what we've allocated */
            for (int i = 0; i < argc; i++) {
                free(argv[i]);
            }
            return 0;
        }
        
        strncpy(argv[argc], start, token_len);
        argv[argc][token_len] = '\0';
        argc++;
    }
    
    argv[argc] = NULL; /* Null terminate */
    return argc;
}

void free_parsed_args(char **argv, int argc) {
    if (argv == NULL) {
        return;
    }
    
    for (int i = 0; i < argc; i++) {
        free(argv[i]);
    }
}

char *expand_env_vars(const char *str) {
    if (str == NULL) {
        return NULL;
    }
    
    /* Simple expansion - find $VAR or ${VAR} */
    size_t len = strlen(str);
    char *result = malloc(len * 2); /* Worst case expansion */
    if (result == NULL) {
        return NULL;
    }
    
    const char *p = str;
    char *out = result;
    
    while (*p != '\0') {
        if (*p == '$' && (isalnum(p[1]) || p[1] == '{' || p[1] == '_')) {
            p++; /* Skip $ */
            
            char var_name[256];
            char *var_ptr = var_name;
            bool brace = false;
            
            if (*p == '{') {
                brace = true;
                p++;
            }
            
            while (*p != '\0' && (isalnum(*p) || *p == '_')) {
                *var_ptr++ = *p++;
                if (var_ptr - var_name >= sizeof(var_name) - 1) {
                    break;
                }
            }
            *var_ptr = '\0';
            
            if (brace && *p == '}') {
                p++;
            }
            
            /* Get environment variable */
            const char *value = getenv(var_name);
            if (value != NULL) {
                strcpy(out, value);
                out += strlen(value);
            }
        } else {
            *out++ = *p++;
        }
    }
    
    *out = '\0';
    
    /* Reallocate to actual size */
    size_t actual_len = strlen(result) + 1;
    result = realloc(result, actual_len);
    
    return result;
}

