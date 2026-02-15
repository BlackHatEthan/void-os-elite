#include "alias.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int add_alias(shell_context_t *ctx, const char *name, const char *value) {
    if (ctx == NULL || name == NULL || value == NULL) {
        return -1;
    }
    
    /* Check if alias already exists */
    for (int i = 0; i < ctx->alias_count && i < MAX_ALIASES; i++) {
        if (ctx->aliases[i] != NULL && strcmp(ctx->aliases[i]->name, name) == 0) {
            /* Update existing alias */
            free(ctx->aliases[i]->value);
            ctx->aliases[i]->value = string_dup(value);
            return 0;
        }
    }
    
    /* Find empty slot */
    for (int i = 0; i < MAX_ALIASES; i++) {
        if (ctx->aliases[i] == NULL) {
            ctx->aliases[i] = malloc(sizeof(alias_t));
            if (ctx->aliases[i] == NULL) {
                return -1;
            }
            
            ctx->aliases[i]->name = string_dup(name);
            ctx->aliases[i]->value = string_dup(value);
            
            if (ctx->alias_count < MAX_ALIASES) {
                ctx->alias_count++;
            }
            
            return 0;
        }
    }
    
    return -1;
}

int remove_alias(shell_context_t *ctx, const char *name) {
    if (ctx == NULL || name == NULL) {
        return -1;
    }
    
    for (int i = 0; i < ctx->alias_count && i < MAX_ALIASES; i++) {
        if (ctx->aliases[i] != NULL && strcmp(ctx->aliases[i]->name, name) == 0) {
            free(ctx->aliases[i]->name);
            free(ctx->aliases[i]->value);
            free(ctx->aliases[i]);
            ctx->aliases[i] = NULL;
            return 0;
        }
    }
    
    return -1;
}

int cmd_alias(int argc, char **argv, shell_context_t *ctx) {
    if (argc < 2) {
        /* List all aliases */
        return cmd_aliases(argc, argv, ctx);
    }
    
    if (ctx == NULL) {
        return 1;
    }
    
    char *arg = argv[1];
    char *eq = strchr(arg, '=');
    
    if (eq == NULL) {
        printf(COLOR_GREY "Usage: alias name=command\n" COLOR_RESET);
        return 1;
    }
    
    *eq = '\0';
    const char *name = arg;
    const char *value = eq + 1;
    
    if (add_alias(ctx, name, value) != 0) {
        LOG_ERROR("Failed to add alias '%s'", name);
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
        *eq = '='; /* Restore */
        return 1;
    }
    
    *eq = '='; /* Restore */
    LOG_INFO("Added alias: %s=%s", name, value);
    return 0;
}

int cmd_unalias(int argc, char **argv, shell_context_t *ctx) {
    if (argc < 2) {
        printf(COLOR_GREY "Usage: unalias <name>\n" COLOR_RESET);
        return 1;
    }
    
    if (ctx == NULL) {
        return 1;
    }
    
    if (remove_alias(ctx, argv[1]) != 0) {
        printf(COLOR_GREY "Alias '%s' not found.\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    LOG_INFO("Removed alias: %s", argv[1]);
    return 0;
}

int cmd_aliases(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    
    if (ctx == NULL) {
        return 1;
    }
    
    bool has_aliases = false;
    for (int i = 0; i < ctx->alias_count && i < MAX_ALIASES; i++) {
        if (ctx->aliases[i] != NULL) {
            if (!has_aliases) {
                printf(COLOR_GREY "Aliases:\n" COLOR_RESET);
                printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
                has_aliases = true;
            }
            printf(COLOR_WHITE "%-20s" COLOR_RESET " = " COLOR_BLUE "%s\n" COLOR_RESET,
                   ctx->aliases[i]->name, ctx->aliases[i]->value);
        }
    }
    
    if (!has_aliases) {
        printf(COLOR_GREY "No aliases defined.\n" COLOR_RESET);
    }
    
    return 0;
}

