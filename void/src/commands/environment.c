#include "environment.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

int cmd_env(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    extern char **environ;
    
    printf(COLOR_GREY "Environment Variables:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    for (char **env = environ; *env != NULL; env++) {
        char *eq = strchr(*env, '=');
        if (eq != NULL) {
            *eq = '\0';
            printf(COLOR_WHITE "%s" COLOR_RESET "=" COLOR_BLUE "%s\n" COLOR_RESET, *env, eq + 1);
            *eq = '='; /* Restore */
        }
    }
    
    return 0;
}

int cmd_export(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: export KEY=VALUE\n" COLOR_RESET);
        return 1;
    }
    
    char *arg = argv[1];
    char *eq = strchr(arg, '=');
    
    if (eq == NULL) {
        printf(COLOR_GREY "Usage: export KEY=VALUE\n" COLOR_RESET);
        return 1;
    }
    
    *eq = '\0';
    const char *key = arg;
    const char *value = eq + 1;
    
    if (setenv(key, value, 1) != 0) {
        LOG_ERROR("Failed to set environment variable '%s': %s", key, strerror(errno));
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
        *eq = '='; /* Restore */
        return 1;
    }
    
    *eq = '='; /* Restore */
    LOG_INFO("Set environment variable: %s=%s", key, value);
    return 0;
}

int cmd_unset(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: unset KEY\n" COLOR_RESET);
        return 1;
    }
    
    if (unsetenv(argv[1]) != 0) {
        LOG_ERROR("Failed to unset environment variable '%s': %s", argv[1], strerror(errno));
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
        return 1;
    }
    
    LOG_INFO("Unset environment variable: %s", argv[1]);
    return 0;
}

int load_config(shell_context_t *ctx) {
    if (ctx == NULL) {
        return 1;
    }
    
    char *rc_path = expand_path("~/.voidrc");
    if (rc_path == NULL) {
        return 1;
    }
    
    FILE *fp = fopen(rc_path, "r");
    if (fp == NULL) {
        /* Config file doesn't exist - that's okay */
        free(rc_path);
        return 0;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), fp) != NULL) {
        /* Remove newline */
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        
        /* Skip empty lines and comments */
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }
        
        /* Parse export commands */
        if (strncmp(line, "export ", 7) == 0) {
            char *export_line = line + 7;
            char *eq = strchr(export_line, '=');
            if (eq != NULL) {
                *eq = '\0';
                setenv(export_line, eq + 1, 1);
                *eq = '=';
            }
        }
    }
    
    fclose(fp);
    free(rc_path);
    return 0;
}

int save_config(shell_context_t *ctx) {
    (void)ctx;
    /* For now, just return success. Full implementation would save aliases, etc. */
    return 0;
}

int cmd_reload(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    
    if (load_config(ctx) == 0) {
        printf(COLOR_GREEN "Configuration reloaded.\n" COLOR_RESET);
        return 0;
    } else {
        printf(COLOR_GREY "Failed to reload configuration.\n" COLOR_RESET);
        return 1;
    }
}

