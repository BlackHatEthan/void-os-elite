#include "process.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>

int add_job(shell_context_t *ctx, pid_t pid, const char *command) {
    if (ctx == NULL || command == NULL) {
        return -1;
    }
    
    /* Find empty slot */
    for (int i = 0; i < MAX_JOBS; i++) {
        if (ctx->jobs[i] == NULL || !ctx->jobs[i]->active) {
            if (ctx->jobs[i] == NULL) {
                ctx->jobs[i] = malloc(sizeof(job_t));
                if (ctx->jobs[i] == NULL) {
                    return -1;
                }
            }
            
            ctx->jobs[i]->pid = pid;
            ctx->jobs[i]->command = string_dup(command);
            ctx->jobs[i]->job_id = ctx->job_count + 1;
            ctx->jobs[i]->active = true;
            
            if (ctx->job_count < MAX_JOBS) {
                ctx->job_count++;
            }
            
            return ctx->jobs[i]->job_id;
        }
    }
    
    return -1;
}

void remove_job(shell_context_t *ctx, pid_t pid) {
    if (ctx == NULL) {
        return;
    }
    
    for (int i = 0; i < MAX_JOBS; i++) {
        if (ctx->jobs[i] != NULL && ctx->jobs[i]->pid == pid) {
            ctx->jobs[i]->active = false;
            free(ctx->jobs[i]->command);
            ctx->jobs[i]->command = NULL;
            break;
        }
    }
}

void update_jobs(shell_context_t *ctx) {
    if (ctx == NULL) {
        return;
    }
    
    /* Check for completed jobs */
    for (int i = 0; i < MAX_JOBS; i++) {
        if (ctx->jobs[i] != NULL && ctx->jobs[i]->active) {
            pid_t pid = ctx->jobs[i]->pid;
            int status;
            pid_t result = waitpid(pid, &status, WNOHANG);
            
            if (result > 0) {
                /* Job completed */
                ctx->jobs[i]->active = false;
            } else if (result == 0) {
                /* Job still running */
            } else {
                /* Error or process doesn't exist */
                ctx->jobs[i]->active = false;
            }
        }
    }
}

int cmd_spawn(int argc, char **argv, shell_context_t *ctx) {
    if (argc < 2) {
        printf(COLOR_GREY "Usage: spawn <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        LOG_ERROR("Failed to fork process: %s", strerror(errno));
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
        return 1;
    }
    
    if (pid == 0) {
        /* Child process - run in background */
        setpgid(0, 0); /* New process group */
        
        /* Note: execvp is used here for user binaries, not external tools */
        /* This allows Void OS to execute user-compiled programs */
        execvp(argv[1], &argv[1]);
        
        /* If execvp returns, it failed */
        LOG_ERROR("Failed to execute '%s': %s", argv[1], strerror(errno));
        printf(COLOR_GREY "Failed to execute: %s\n" COLOR_RESET, argv[1]);
        exit(1);
    } else {
        /* Parent process */
        /* Build command string for display */
        char cmd_str[1024] = {0};
        for (int i = 1; i < argc; i++) {
            if (i > 1) strcat(cmd_str, " ");
            strncat(cmd_str, argv[i], sizeof(cmd_str) - strlen(cmd_str) - 1);
        }
        
        int job_id = add_job(ctx, pid, cmd_str);
        if (job_id > 0) {
            printf(COLOR_GREY "[%d] %d\n" COLOR_RESET, job_id, pid);
        }
    }
    
    return 0;
}

int cmd_jobs(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    
    if (ctx == NULL) {
        return 1;
    }
    
    update_jobs(ctx);
    
    bool has_jobs = false;
    for (int i = 0; i < MAX_JOBS; i++) {
        if (ctx->jobs[i] != NULL && ctx->jobs[i]->active) {
            if (!has_jobs) {
                printf(COLOR_GREY "%-5s %-8s %s\n" COLOR_RESET, "ID", "PID", "COMMAND");
                printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
                has_jobs = true;
            }
            
            /* Check if process is still running */
            int status;
            pid_t result = waitpid(ctx->jobs[i]->pid, &status, WNOHANG);
            const char *state = (result == 0) ? "Running" : "Done";
            
            printf(COLOR_WHITE "[%d]" COLOR_RESET " %-8d %s %s\n",
                   ctx->jobs[i]->job_id,
                   ctx->jobs[i]->pid,
                   ctx->jobs[i]->command ? ctx->jobs[i]->command : "?",
                   state);
        }
    }
    
    if (!has_jobs) {
        printf(COLOR_GREY "No background jobs.\n" COLOR_RESET);
    }
    
    return 0;
}

int cmd_terminate(int argc, char **argv, shell_context_t *ctx) {
    if (argc < 2) {
        printf(COLOR_GREY "Usage: terminate <pid>\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[1]);
    if (pid <= 0) {
        printf(COLOR_GREY "Invalid PID: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    if (kill(pid, SIGTERM) != 0) {
        LOG_ERROR("Failed to kill process %d: %s", pid, strerror(errno));
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
        return 1;
    }
    
    remove_job(ctx, pid);
    return 0;
}

int cmd_foreground(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: foreground <pid>\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[1]);
    if (pid <= 0) {
        printf(COLOR_GREY "Invalid PID: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    /* Bring process group to foreground */
    if (tcsetpgrp(STDIN_FILENO, pid) != 0) {
        LOG_ERROR("Failed to bring process %d to foreground: %s", pid, strerror(errno));
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
        return 1;
    }
    
    /* Wait for process */
    int status;
    waitpid(pid, &status, 0);
    
    /* Restore terminal control */
    tcsetpgrp(STDIN_FILENO, getpgrp());
    
    return 0;
}

int cmd_ps(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    /* Simple ps - list processes from /proc (Linux) or use ps command */
    #ifdef __linux__
    DIR *proc = opendir("/proc");
    if (proc != NULL) {
        printf(COLOR_GREY "%-8s %s\n" COLOR_RESET, "PID", "COMMAND");
        printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
        
        struct dirent *entry;
        while ((entry = readdir(proc)) != NULL) {
            if (entry->d_name[0] >= '0' && entry->d_name[0] <= '9') {
                pid_t pid = (pid_t)atoi(entry->d_name);
                char cmdline_path[256];
                snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%s/cmdline", entry->d_name);
                
                FILE *fp = fopen(cmdline_path, "r");
                if (fp != NULL) {
                    char cmdline[256];
                    if (fgets(cmdline, sizeof(cmdline), fp) != NULL) {
                        /* Replace null bytes with spaces */
                        for (int i = 0; cmdline[i] != '\0'; i++) {
                            if (cmdline[i] == '\0' && i > 0) {
                                cmdline[i] = ' ';
                            }
                        }
                        printf(COLOR_WHITE "%-8d %s\n" COLOR_RESET, pid, cmdline);
                    }
                    fclose(fp);
                }
            }
        }
        closedir(proc);
    }
    #else
    /* macOS/BSD - try /proc if available, otherwise show message */
    DIR *proc = opendir("/proc");
    if (proc != NULL) {
        printf(COLOR_GREY "%-8s %s\n" COLOR_RESET, "PID", "COMMAND");
        printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
        
        struct dirent *entry;
        int count = 0;
        while ((entry = readdir(proc)) != NULL && count < 20) {
            if (entry->d_name[0] >= '0' && entry->d_name[0] <= '9') {
                pid_t pid = (pid_t)atoi(entry->d_name);
                char cmdline_path[256];
                snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%s/cmdline", entry->d_name);
                
                FILE *fp = fopen(cmdline_path, "r");
                if (fp != NULL) {
                    char cmdline[256];
                    if (fgets(cmdline, sizeof(cmdline), fp) != NULL) {
                        for (int i = 0; cmdline[i] != '\0' && i < sizeof(cmdline) - 1; i++) {
                            if (cmdline[i] == '\0' && i > 0) {
                                cmdline[i] = ' ';
                            }
                        }
                        printf(COLOR_WHITE "%-8d %s\n" COLOR_RESET, pid, cmdline);
                        count++;
                    }
                    fclose(fp);
                }
            }
        }
        closedir(proc);
    } else {
        printf(COLOR_GREY "Process listing requires /proc filesystem (Linux)\n" COLOR_RESET);
        printf(COLOR_GREY "Native implementation pending for macOS.\n" COLOR_RESET);
    }
    #endif
    
    return 0;
}

int cmd_silence(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: silence <pid>\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[1]);
    if (pid <= 0) {
        printf(COLOR_GREY "Invalid PID: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    if (kill(pid, SIGSTOP) != 0) {
        LOG_ERROR("Failed to stop process %d: %s", pid, strerror(errno));
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
        return 1;
    }
    
    printf(COLOR_GREEN "Process %d frozen (SIGSTOP)\n" COLOR_RESET, pid);
    return 0;
}

int cmd_awaken(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: awaken <pid>\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[1]);
    if (pid <= 0) {
        printf(COLOR_GREY "Invalid PID: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    if (kill(pid, SIGCONT) != 0) {
        LOG_ERROR("Failed to resume process %d: %s", pid, strerror(errno));
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
        return 1;
    }
    
    printf(COLOR_GREEN "Process %d resumed (SIGCONT)\n" COLOR_RESET, pid);
    return 0;
}

