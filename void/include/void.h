#ifndef VOID_H
#define VOID_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pwd.h>
#include <limits.h>
#include <stdbool.h>

/* ANSI Color Codes - Monochrome Noir Theme */
#define COLOR_RESET   "\033[0m"
#define COLOR_WHITE   "\033[0;37m"
#define COLOR_GREY    "\033[0;90m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_DIM     "\033[2m"

/* Void OS Version */
#define VOID_VERSION "0.1.0"

/* Configuration Paths */
#define VOID_RC_PATH "~/.voidrc"
#define VOID_LOG_PATH "~/.void_logs"
#define VOID_PLUGIN_DIR "~/.void/plugins"

/* Maximum lengths */
#define MAX_INPUT_LEN 4096
#define MAX_PATH_LEN PATH_MAX
#define MAX_ARGS 256
#define MAX_ALIASES 128
#define MAX_JOBS 64

/* Error Messages - Void Style */
#define ERR_FILE_NOT_FOUND "Target lost in the Void."
#define ERR_PERMISSION_DENIED "Access denied by the Veil."
#define ERR_COMMAND_NOT_FOUND "Unknown signal detected."
#define ERR_PROCESS_ERROR "Process destabilized."
#define ERR_TOOL_NOT_FOUND "Required tool not found in this reality."

/* Job structure for background processes */
typedef struct {
    pid_t pid;
    char *command;
    int job_id;
    bool active;
} job_t;

/* Alias structure */
typedef struct {
    char *name;
    char *value;
} alias_t;

/* Shell context structure */
typedef struct {
    char *username;
    char *cwd;
    char *home_dir;
    job_t *jobs[MAX_JOBS];
    int job_count;
    alias_t *aliases[MAX_ALIASES];
    int alias_count;
    bool should_exit;
    int last_exit_code;
} shell_context_t;

/* Command function pointer type */
typedef int (*command_func_t)(int argc, char **argv, shell_context_t *ctx);

/* Command entry structure */
typedef struct {
    char *name;
    command_func_t func;
} command_entry_t;

#endif /* VOID_H */

