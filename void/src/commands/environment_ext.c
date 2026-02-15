#include "environment_ext.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

int cmd_mimic(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: mimic <command|alias>\n" COLOR_RESET);
        return 1;
    }
    
    const char *cmd = argv[1];
    
    /* Check if it's an alias */
    if (ctx != NULL) {
        for (int i = 0; i < ctx->alias_count && i < MAX_ALIASES; i++) {
            if (ctx->aliases[i] != NULL && strcmp(ctx->aliases[i]->name, cmd) == 0) {
                printf(COLOR_WHITE "Alias: " COLOR_BLUE "%s\n" COLOR_RESET, ctx->aliases[i]->name);
                printf(COLOR_WHITE "Value: " COLOR_BLUE "%s\n" COLOR_RESET, ctx->aliases[i]->value);
                return 0;
            }
        }
    }
    
    /* Check if it's a built-in command */
    printf(COLOR_WHITE "Command: " COLOR_BLUE "%s\n" COLOR_RESET, cmd);
    
    /* Search PATH for command */
    const char *path = getenv("PATH");
    if (path == NULL) {
        printf(COLOR_GREY "PATH not set.\n" COLOR_RESET);
        return 1;
    }
    
    char *path_copy = string_dup(path);
    char *token = strtok(path_copy, ":");
    bool found = false;
    
    while (token != NULL) {
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", token, cmd);
        
        if (access(full_path, X_OK) == 0) {
            char real_path[PATH_MAX];
            if (realpath(full_path, real_path) != NULL) {
                printf(COLOR_WHITE "Path: " COLOR_BLUE "%s\n" COLOR_RESET, real_path);
                found = true;
                break;
            }
        }
        
        token = strtok(NULL, ":");
    }
    
    free(path_copy);
    
    if (!found) {
        printf(COLOR_GREY "Command not found in PATH.\n" COLOR_RESET);
        return 1;
    }
    
    return 0;
}

