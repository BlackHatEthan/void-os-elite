#include "shell.h"
#include "prompt.h"
#include "parser.h"
#include "executor.h"
#include "../commands/core.h"
#include "../commands/environment.h"
#include "../plugins/loader.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

static shell_context_t *g_ctx = NULL;

/* Signal handler for SIGINT (Ctrl+C) */
static void sigint_handler(int sig) {
    (void)sig;
    printf("\n");
    if (g_ctx != NULL) {
        display_prompt(g_ctx);
    }
}

/* Signal handler for SIGCHLD (child process terminated) */
static void sigchld_handler(int sig) {
    (void)sig;
    /* Reap zombie processes */
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        /* Process reaped */
    }
}

shell_context_t *shell_init(void) {
    shell_context_t *ctx = calloc(1, sizeof(shell_context_t));
    if (ctx == NULL) {
        LOG_ERROR("Failed to allocate shell context");
        return NULL;
    }
    
    /* Get username */
    const char *user = getenv("USER");
    if (user != NULL) {
        ctx->username = string_dup(user);
    } else {
        struct passwd *pw = getpwuid(getuid());
        if (pw != NULL) {
            ctx->username = string_dup(pw->pw_name);
        } else {
            ctx->username = string_dup("void");
        }
    }
    
    /* Get current working directory */
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        ctx->cwd = string_dup(cwd);
    } else {
        ctx->cwd = string_dup("/");
    }
    
    /* Get home directory */
    const char *home = getenv("HOME");
    if (home != NULL) {
        ctx->home_dir = string_dup(home);
    } else {
        struct passwd *pw = getpwuid(getuid());
        if (pw != NULL) {
            ctx->home_dir = string_dup(pw->pw_dir);
        } else {
            ctx->home_dir = string_dup("/");
        }
    }
    
    ctx->should_exit = false;
    ctx->last_exit_code = 0;
    ctx->job_count = 0;
    ctx->alias_count = 0;
    
    /* Initialize jobs array */
    for (int i = 0; i < MAX_JOBS; i++) {
        ctx->jobs[i] = NULL;
    }
    
    /* Initialize aliases array */
    for (int i = 0; i < MAX_ALIASES; i++) {
        ctx->aliases[i] = NULL;
    }
    
    /* Register signal handlers */
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);
    
    /* Load configuration */
    load_config(ctx);
    
    /* Load plugins */
    load_plugins(ctx);
    
    g_ctx = ctx;
    
    LOG_INFO("Shell context initialized for user: %s", ctx->username);
    
    return ctx;
}

void shell_cleanup(shell_context_t *ctx) {
    if (ctx == NULL) {
        return;
    }
    
    /* Free jobs */
    for (int i = 0; i < ctx->job_count && i < MAX_JOBS; i++) {
        if (ctx->jobs[i] != NULL) {
            free(ctx->jobs[i]->command);
            free(ctx->jobs[i]);
        }
    }
    
    /* Free aliases */
    for (int i = 0; i < ctx->alias_count && i < MAX_ALIASES; i++) {
        if (ctx->aliases[i] != NULL) {
            free(ctx->aliases[i]->name);
            free(ctx->aliases[i]->value);
            free(ctx->aliases[i]);
        }
    }
    
    /* Unload plugins */
    unload_plugins(ctx);
    
    free(ctx->username);
    free(ctx->cwd);
    free(ctx->home_dir);
    free(ctx);
    
    g_ctx = NULL;
}

char *read_input(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    
    nread = getline(&line, &len, stdin);
    
    if (nread == -1) {
        if (feof(stdin)) {
            /* EOF - user pressed Ctrl+D */
            free(line);
            return NULL;
        }
        /* Error */
        free(line);
        return NULL;
    }
    
    /* Remove newline */
    if (nread > 0 && line[nread - 1] == '\n') {
        line[nread - 1] = '\0';
    }
    
    return line;
}

void shell_loop(shell_context_t *ctx) {
    if (ctx == NULL) {
        return;
    }
    
    char *input = NULL;
    char **argv = NULL;
    
    while (!ctx->should_exit) {
        /* Update current directory */
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            free(ctx->cwd);
            ctx->cwd = string_dup(cwd);
        }
        
        /* Display prompt */
        display_prompt(ctx);
        
        /* Read input */
        input = read_input();
        if (input == NULL) {
            /* EOF or error */
            printf("\n");
            break;
        }
        
        /* Skip empty lines */
        char *trimmed = trim_whitespace(input);
        if (trimmed == NULL || strlen(trimmed) == 0) {
            free(input);
            continue;
        }
        
        /* Parse command */
        argv = malloc(MAX_ARGS * sizeof(char*));
        if (argv == NULL) {
            LOG_ERROR("Failed to allocate argv");
            free(input);
            continue;
        }
        
        int argc = parse_command(trimmed, argv, MAX_ARGS);
        
        if (argc > 0) {
            /* Add to history */
            add_to_history(trimmed);
            
            /* Execute command */
            ctx->last_exit_code = execute_command(argc, argv, ctx);
        }
        
        /* Cleanup */
        free_parsed_args(argv, argc);
        free(argv);
        free(input);
    }
}

