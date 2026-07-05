#include "prompt.h"
#include "../utils/string.h"
#include "../utils/logger.h"
#include "../utils/git_native.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

char *get_git_branch(void) {
    return git_get_branch(NULL);
}

bool is_git_dirty(void) {
    return git_is_dirty(NULL);
}

char *abbreviate_path(const char *path, int max_len) {
    if (path == NULL) {
        return NULL;
    }
    
    size_t path_len = strlen(path);
    if (max_len < 0 || path_len <= (size_t)max_len) {
        return string_dup(path);
    }
    
    /* Try to abbreviate by replacing middle parts with ... */
    const char *home = getenv("HOME");
    char *abbrev = NULL;
    
    if (home != NULL && starts_with(path, home)) {
        /* Replace $HOME with ~ */
        size_t home_len = strlen(home);
        size_t remaining = path_len - home_len;
        
        if (remaining + 2 <= (size_t)max_len) {
            abbrev = malloc(max_len + 1);
            if (abbrev != NULL) {
                abbrev[0] = '~';
                strcpy(abbrev + 1, path + home_len);
            }
        } else {
            /* Need to truncate further */
            abbrev = malloc(max_len + 1);
            if (abbrev != NULL) {
                abbrev[0] = '~';
                abbrev[1] = '/';
                abbrev[2] = '.';
                abbrev[3] = '.';
                abbrev[4] = '.';
                strncpy(abbrev + 5, path + path_len - (max_len - 5), max_len - 5);
                abbrev[max_len] = '\0';
            }
        }
    } else {
        /* Just truncate from the end */
        abbrev = malloc(max_len + 1);
        if (abbrev != NULL) {
            strncpy(abbrev, path, max_len);
            abbrev[max_len] = '\0';
        }
    }
    
    return abbrev;
}

void display_prompt(shell_context_t *ctx) {
    if (ctx == NULL) {
        return;
    }
    
    /* Get current time */
    time_t now;
    time(&now);
    struct tm *tm_info = localtime(&now);
    char time_str[16];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);
    
    /* Get username */
    const char *username = ctx->username ? ctx->username : getenv("USER");
    if (username == NULL) {
        username = "void";
    }
    
    /* Get current directory */
    char *cwd = ctx->cwd;
    if (cwd == NULL) {
        char cwd_buf[PATH_MAX];
        if (getcwd(cwd_buf, sizeof(cwd_buf)) != NULL) {
            cwd = cwd_buf;
        } else {
            cwd = "?";
        }
    }
    
    /* Abbreviate path if too long */
    char *abbrev_path = abbreviate_path(cwd, 40);
    if (abbrev_path == NULL) {
        abbrev_path = string_dup(cwd ? cwd : "?");
    }
    
    /* Get git branch */
    char *branch = get_git_branch();
    bool dirty = is_git_dirty();
    
    /* Count active jobs */
    int active_jobs = 0;
    for (int i = 0; i < ctx->job_count && i < MAX_JOBS; i++) {
        if (ctx->jobs[i] != NULL && ctx->jobs[i]->active) {
            active_jobs++;
        }
    }
    
    /* Build prompt */
    printf(COLOR_GREY "[%s] " COLOR_RESET, time_str);
    printf(COLOR_WHITE "void@%s:" COLOR_RESET, username);
    printf(COLOR_BLUE "%s" COLOR_RESET, abbrev_path);
    
    if (branch != NULL) {
        printf(COLOR_GREY " [" COLOR_WHITE "%s" COLOR_RESET, branch);
        if (dirty) {
            printf(COLOR_WHITE "*" COLOR_RESET);
        }
        printf(COLOR_GREY "]" COLOR_RESET);
        free(branch);
    }
    
    if (active_jobs > 0) {
        printf(COLOR_GREY "[+%d]" COLOR_RESET, active_jobs);
    }
    
    printf(COLOR_WHITE "$ " COLOR_RESET);
    fflush(stdout);
    
    free(abbrev_path);
}

