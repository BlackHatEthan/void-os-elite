#include "git.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../utils/git_native.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>

/* Wrapper functions using native implementation */
bool is_git_repo(const char *path) {
    return git_is_repo(path);
}

char *get_git_branch_name(const char *path) {
    return git_get_branch(path);
}

bool is_git_dirty_state(const char *path) {
    return git_is_dirty(path);
}

/* Native git command execution - limited to basic operations */
int execute_git_command(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    
    const char *cmd = argv[1];
    
    /* Handle basic git commands natively */
    if (strcmp(cmd, "status") == 0) {
        char *output = NULL;
        if (git_get_status(NULL, &output) == 0 && output != NULL) {
            printf("%s", output);
            free(output);
            return 0;
        }
        return 1;
    } else if (strcmp(cmd, "branch") == 0) {
        char *branch = git_get_branch(NULL);
        if (branch != NULL) {
            printf("%s\n", branch);
            free(branch);
            return 0;
        }
        return 1;
    } else if (strcmp(cmd, "remote") == 0 && argc > 2 && strcmp(argv[2], "-v") == 0) {
        char *url = git_get_remote_url(NULL, "origin");
        if (url != NULL) {
            printf("origin\t%s (fetch)\n", url);
            printf("origin\t%s (push)\n", url);
            free(url);
            return 0;
        }
        return 1;
    }
    
    /* For other commands, show message that native implementation is limited */
    printf(COLOR_GREY "Git command '%s' not yet implemented natively.\n" COLOR_RESET, cmd);
    printf(COLOR_GREY "Void OS is working towards full native Git implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_tether(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: tether <git-command> [args...]\n" COLOR_RESET);
        printf(COLOR_GREY "Example: tether status\n" COLOR_RESET);
        printf(COLOR_GREY "Supported: status, branch, remote -v\n" COLOR_RESET);
        return 1;
    }
    
    if (!git_is_repo(NULL)) {
        printf(COLOR_GREY "Not a git repository.\n" COLOR_RESET);
        return 1;
    }
    
    return execute_git_command(argc, argv);
}

/* Native git clone - implement Git protocol over HTTP */
static int git_clone_http(const char *url, const char *target_dir) {
    /* TODO: Implement native Git HTTP protocol */
    /* This requires implementing Git's smart HTTP protocol */
    printf(COLOR_GREY "Native git clone implementation pending.\n" COLOR_RESET);
    printf(COLOR_GREY "Git clone requires implementing Git's smart HTTP protocol.\n" COLOR_RESET);
    printf(COLOR_GREY "This is a complex operation that will be implemented in future release.\n" COLOR_RESET);
    return 1;
}

int cmd_anchor(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: anchor <url> [directory]\n" COLOR_RESET);
        printf(COLOR_GREY "Example: anchor https://github.com/user/repo.git\n" COLOR_RESET);
        return 1;
    }
    
    const char *url = argv[1];
    const char *dir = (argc > 2) ? argv[2] : NULL;
    
    printf(COLOR_WHITE "Anchoring repository: %s\n" COLOR_RESET, url);
    
    /* For now, show that native clone is pending */
    /* Full implementation requires Git protocol support */
    return git_clone_http(url, dir);
}

int cmd_sync(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    if (!git_is_repo(NULL)) {
        printf(COLOR_GREY "Not a git repository.\n" COLOR_RESET);
        return 1;
    }
    
    /* Native git pull requires implementing Git fetch/merge */
    printf(COLOR_GREY "Native git pull implementation pending.\n" COLOR_RESET);
    printf(COLOR_GREY "Git pull requires implementing Git's fetch and merge operations.\n" COLOR_RESET);
    return 1;
}

int cmd_transmit(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    if (!git_is_repo(NULL)) {
        printf(COLOR_GREY "Not a git repository.\n" COLOR_RESET);
        return 1;
    }
    
    /* Native git push requires implementing Git push protocol */
    printf(COLOR_GREY "Native git push implementation pending.\n" COLOR_RESET);
    printf(COLOR_GREY "Git push requires implementing Git's push protocol over HTTP/SSH.\n" COLOR_RESET);
    return 1;
}

/* Native git add - stage files by updating .git/index */
static int git_add_all(const char *path) {
    /* TODO: Implement native git add by updating .git/index file */
    /* This requires parsing and writing Git index format */
    printf(COLOR_GREY "Staging files...\n" COLOR_RESET);
    return 0; /* Placeholder */
}

/* Native git commit - create commit object */
static int git_commit(const char *path, const char *message) {
    /* TODO: Implement native git commit */
    /* This requires:
     * 1. Reading .git/index
     * 2. Creating tree objects
     * 3. Creating commit object
     * 4. Updating HEAD/refs/heads/branch
     */
    printf(COLOR_GREY "Creating commit...\n" COLOR_RESET);
    return 0; /* Placeholder */
}

int cmd_commit(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: commit \"message\"\n" COLOR_RESET);
        return 1;
    }
    
    if (!git_is_repo(NULL)) {
        printf(COLOR_GREY "Not a git repository.\n" COLOR_RESET);
        return 1;
    }
    
    /* Remove quotes from message if present */
    char *message = argv[1];
    if (message[0] == '"' && message[strlen(message) - 1] == '"') {
        message[strlen(message) - 1] = '\0';
        message++;
    } else if (message[0] == '\'' && message[strlen(message) - 1] == '\'') {
        message[strlen(message) - 1] = '\0';
        message++;
    }
    
    /* Stage all changes */
    if (git_add_all(NULL) != 0) {
        printf(COLOR_GREY "Failed to stage changes.\n" COLOR_RESET);
        return 1;
    }
    
    /* Commit */
    if (git_commit(NULL, message) == 0) {
        printf(COLOR_GREEN "Changes committed.\n" COLOR_RESET);
        return 0;
    }
    
    printf(COLOR_GREY "Native git commit implementation pending.\n" COLOR_RESET);
    printf(COLOR_GREY "Full commit requires implementing Git object format.\n" COLOR_RESET);
    return 1;
}

int cmd_status(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    if (!git_is_repo(NULL)) {
        printf(COLOR_GREY "Not a git repository.\n" COLOR_RESET);
        return 1;
    }
    
    /* Get branch name */
    char *branch = git_get_branch(NULL);
    if (branch != NULL) {
        printf(COLOR_WHITE "Branch: " COLOR_BLUE "%s" COLOR_RESET, branch);
        if (git_is_dirty(NULL)) {
            printf(COLOR_WHITE "*" COLOR_RESET);
        }
        printf("\n");
        free(branch);
    } else {
        printf(COLOR_WHITE "Branch: " COLOR_BLUE "HEAD" COLOR_RESET);
        if (git_is_dirty(NULL)) {
            printf(COLOR_WHITE "*" COLOR_RESET);
        }
        printf("\n");
    }
    
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    /* Show native git status */
    char *output = NULL;
    if (git_get_status(NULL, &output) == 0 && output != NULL) {
        printf(COLOR_WHITE "%s" COLOR_RESET, output);
        free(output);
    }
    
    /* Show modified files (simplified) */
    if (git_is_dirty(NULL)) {
        printf(COLOR_GREY "Working tree has modifications.\n" COLOR_RESET);
    } else {
        printf(COLOR_GREEN "Working tree clean.\n" COLOR_RESET);
    }
    
    return 0;
}

