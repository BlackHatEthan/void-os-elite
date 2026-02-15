#include "process_ext.h"
#include "process.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"

/* Forward declaration for cmd_ps */
extern int cmd_ps(int argc, char **argv, shell_context_t *ctx);
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>

/* Process Control */
int cmd_kill_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: kill <pid> [signal]\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[1]);
    int sig = SIGTERM;
    
    if (argc > 2) {
        if (argv[2][0] == '-') {
            sig = atoi(argv[2] + 1);
        } else {
            sig = atoi(argv[2]);
        }
    }
    
    if (kill(pid, sig) != 0) {
        printf(COLOR_GREY "Cannot kill process %d: %s\n" COLOR_RESET, pid, strerror(errno));
        return 1;
    }
    
    printf(COLOR_GREEN "Sent signal %d to process %d\n" COLOR_RESET, sig, pid);
    return 0;
}

/* Find PIDs by name pattern */
static int find_pids_by_name(const char *pattern, pid_t *pids, int max_pids) {
    int count = 0;
    DIR *proc = opendir("/proc");
    if (proc == NULL) {
        return 0;
    }
    
    struct dirent *entry;
    while ((entry = readdir(proc)) != NULL && count < max_pids) {
        if (!isdigit(entry->d_name[0])) {
            continue;
        }
        
        pid_t pid = (pid_t)atoi(entry->d_name);
        char path[256];
        snprintf(path, sizeof(path), "/proc/%d/comm", pid);
        
        FILE *fp = fopen(path, "r");
        if (fp != NULL) {
            char comm[256];
            if (fgets(comm, sizeof(comm), fp) != NULL) {
                char *nl = strchr(comm, '\n');
                if (nl) *nl = '\0';
                
                if (strstr(comm, pattern) != NULL) {
                    pids[count++] = pid;
                }
            }
            fclose(fp);
        }
    }
    
    closedir(proc);
    return count;
}

int cmd_killall(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: killall <name> [signal]\n" COLOR_RESET);
        return 1;
    }
    
    const char *name = argv[1];
    int sig = SIGTERM;
    
    if (argc > 2) {
        if (argv[2][0] == '-') {
            sig = atoi(argv[2] + 1);
        } else {
            sig = atoi(argv[2]);
        }
    }
    
    pid_t pids[256];
    int count = find_pids_by_name(name, pids, 256);
    
    if (count == 0) {
        printf(COLOR_GREY "No processes found matching: %s\n" COLOR_RESET, name);
        return 1;
    }
    
    int killed = 0;
    for (int i = 0; i < count; i++) {
        if (kill(pids[i], sig) == 0) {
            killed++;
        }
    }
    
    printf(COLOR_GREEN "Killed %d/%d processes matching '%s'\n" COLOR_RESET, killed, count, name);
    return (killed == count) ? 0 : 1;
}

int cmd_pkill(int argc, char **argv, shell_context_t *ctx) {
    return cmd_killall(argc, argv, ctx);
}

int cmd_xkill(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Xkill requires X11 support.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'killall <name>' or 'kill <pid>' instead.\n" COLOR_RESET);
    return 1;
}

int cmd_killpg_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: killpg <pgid> [signal]\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pgid = (pid_t)atoi(argv[1]);
    int sig = SIGTERM;
    
    if (argc > 2) {
        if (argv[2][0] == '-') {
            sig = atoi(argv[2] + 1);
        } else {
            sig = atoi(argv[2]);
        }
    }
    
    /* killpg may not be available on all systems */
    if (kill(-pgid, sig) != 0) {
        printf(COLOR_GREY "Cannot kill process group %d: %s\n" COLOR_RESET, pgid, strerror(errno));
        return 1;
    }
    
    printf(COLOR_GREEN "Sent signal %d to process group %d\n" COLOR_RESET, sig, pgid);
    return 0;
}

int cmd_pkillall(int argc, char **argv, shell_context_t *ctx) {
    return cmd_killall(argc, argv, ctx);
}

int cmd_skill(int argc, char **argv, shell_context_t *ctx) {
    return cmd_kill_ext(argc, argv, ctx);
}

int cmd_snice(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: snice <pid> <nice>\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[1]);
    int nice_val = atoi(argv[2]);
    
    if (setpriority(PRIO_PROCESS, pid, nice_val) != 0) {
        printf(COLOR_GREY "Cannot set nice value: %s\n" COLOR_RESET, strerror(errno));
        return 1;
    }
    
    printf(COLOR_GREEN "Set nice value to %d for process %d\n" COLOR_RESET, nice_val, pid);
    return 0;
}

int cmd_renice(int argc, char **argv, shell_context_t *ctx) {
    return cmd_snice(argc, argv, ctx);
}

int cmd_nice_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: nice <nice> <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    int nice_val = atoi(argv[1]);
    setpriority(PRIO_PROCESS, 0, nice_val);
    
    /* Execute command */
    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv[2], &argv[2]);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }
    
    return 1;
}

int cmd_nohup(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: nohup <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    signal(SIGHUP, SIG_IGN);
    
    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv[1], &argv[1]);
        exit(1);
    } else if (pid > 0) {
        printf(COLOR_GREEN "Process %d running in background (nohup)\n" COLOR_RESET, pid);
        return 0;
    }
    
    return 1;
}

int cmd_setsid_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: setsid <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        setsid();
        execvp(argv[1], &argv[1]);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }
    
    return 1;
}

int cmd_disown(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: disown <job_id>\n" COLOR_RESET);
        return 1;
    }
    
    int job_id = atoi(argv[1]);
    
    if (ctx != NULL) {
        for (int i = 0; i < MAX_JOBS; i++) {
            if (ctx->jobs[i] != NULL && ctx->jobs[i]->job_id == job_id) {
                ctx->jobs[i]->active = false;
                printf(COLOR_GREEN "Disowned job %d\n" COLOR_RESET, job_id);
                return 0;
            }
        }
    }
    
    printf(COLOR_GREY "Job %d not found\n" COLOR_RESET, job_id);
    return 1;
}

int cmd_fg_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: fg <job_id>\n" COLOR_RESET);
        return 1;
    }
    
    int job_id = atoi(argv[1]);
    
    if (ctx != NULL) {
        for (int i = 0; i < MAX_JOBS; i++) {
            if (ctx->jobs[i] != NULL && ctx->jobs[i]->job_id == job_id) {
                pid_t pid = ctx->jobs[i]->pid;
                if (kill(pid, SIGCONT) == 0) {
                    tcsetpgrp(STDIN_FILENO, getpgid(pid));
                    int status;
                    waitpid(pid, &status, 0);
                    tcsetpgrp(STDIN_FILENO, getpgrp());
                    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
                }
            }
        }
    }
    
    printf(COLOR_GREY "Job %d not found\n" COLOR_RESET, job_id);
    return 1;
}

int cmd_bg_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: bg <job_id>\n" COLOR_RESET);
        return 1;
    }
    
    int job_id = atoi(argv[1]);
    
    if (ctx != NULL) {
        for (int i = 0; i < MAX_JOBS; i++) {
            if (ctx->jobs[i] != NULL && ctx->jobs[i]->job_id == job_id) {
                pid_t pid = ctx->jobs[i]->pid;
                if (kill(pid, SIGCONT) == 0) {
                    printf(COLOR_GREEN "Resumed job %d in background\n" COLOR_RESET, job_id);
                    return 0;
                }
            }
        }
    }
    
    printf(COLOR_GREY "Job %d not found\n" COLOR_RESET, job_id);
    return 1;
}

int cmd_wait_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: wait <pid>\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[1]);
    int status;
    
    if (waitpid(pid, &status, 0) == -1) {
        printf(COLOR_GREY "Cannot wait for process %d: %s\n" COLOR_RESET, pid, strerror(errno));
        return 1;
    }
    
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

int cmd_waitpid_ext(int argc, char **argv, shell_context_t *ctx) {
    return cmd_wait_ext(argc, argv, ctx);
}

/* Process Information */
static void read_process_tree(pid_t pid, int depth, int max_depth) {
    if (depth > max_depth) return;
    
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/comm", pid);
    
    FILE *fp = fopen(path, "r");
    if (fp == NULL) return;
    
    char comm[256];
    if (fgets(comm, sizeof(comm), fp) != NULL) {
        char *nl = strchr(comm, '\n');
        if (nl) *nl = '\0';
        
        for (int i = 0; i < depth; i++) {
            printf("  ");
        }
        printf(COLOR_WHITE "%d: %s\n" COLOR_RESET, pid, comm);
    }
    fclose(fp);
    
    /* Find children */
    snprintf(path, sizeof(path), "/proc/%d/task/%d/children", pid, pid);
    fp = fopen(path, "r");
    if (fp != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), fp) != NULL) {
            pid_t child = (pid_t)atoi(line);
            read_process_tree(child, depth + 1, max_depth);
        }
        fclose(fp);
    }
}

int cmd_pstree(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    pid_t root_pid = 1; /* init */
    
    if (argc > 1) {
        root_pid = (pid_t)atoi(argv[1]);
    }
    
    read_process_tree(root_pid, 0, 10);
    return 0;
}

int cmd_pgrep(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: pgrep <pattern>\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pids[256];
    int count = find_pids_by_name(argv[1], pids, 256);
    
    for (int i = 0; i < count; i++) {
        printf(COLOR_WHITE "%d\n" COLOR_RESET, pids[i]);
    }
    
    return (count > 0) ? 0 : 1;
}

int cmd_pidof(int argc, char **argv, shell_context_t *ctx) {
    return cmd_pgrep(argc, argv, ctx);
}

int cmd_fuser(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: fuser <file>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Fuser requires /proc/<pid>/fd scanning.\n" COLOR_RESET);
    printf(COLOR_GREY "Implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_lsof_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: lsof <file>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Lsof requires /proc/<pid>/fd scanning.\n" COLOR_RESET);
    printf(COLOR_GREY "Implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_pmap(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: pmap <pid>\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[1]);
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot read memory map for process %d\n" COLOR_RESET, pid);
        return 1;
    }
    
    char line[4096];
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }
    fclose(fp);
    
    return 0;
}

int cmd_procinfo(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: procinfo <pid>\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[1]);
    char path[256];
    
    printf(COLOR_WHITE "Process Information for PID %d:\n" COLOR_RESET, pid);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    /* Read /proc/pid/status */
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *fp = fopen(path, "r");
    if (fp != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), fp) != NULL) {
            printf("%s", line);
        }
        fclose(fp);
    }
    
    return 0;
}

int cmd_pidstat(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: pidstat <pid>\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[1]);
    char path[256];
    
    /* Read /proc/pid/stat */
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot read statistics for process %d\n" COLOR_RESET, pid);
        return 1;
    }
    
    char line[4096];
    if (fgets(line, sizeof(line), fp) != NULL) {
        printf(COLOR_WHITE "Statistics for PID %d:\n" COLOR_RESET, pid);
        printf(COLOR_GREY "%s\n" COLOR_RESET, line);
    }
    fclose(fp);
    
    return 0;
}

int cmd_top_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Top requires interactive terminal UI.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'ps' for process listing.\n" COLOR_RESET);
    return cmd_ps(1, (char*[]){"ps"}, ctx);
}

int cmd_htop(int argc, char **argv, shell_context_t *ctx) {
    return cmd_top_ext(argc, argv, ctx);
}

int cmd_btop(int argc, char **argv, shell_context_t *ctx) {
    return cmd_top_ext(argc, argv, ctx);
}

int cmd_glances(int argc, char **argv, shell_context_t *ctx) {
    return cmd_top_ext(argc, argv, ctx);
}

int cmd_nmon(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Nmon requires interactive terminal UI.\n" COLOR_RESET);
    return 1;
}

int cmd_iotop(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Iotop requires I/O statistics from kernel.\n" COLOR_RESET);
    return 1;
}

int cmd_powertop(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Powertop requires power management statistics.\n" COLOR_RESET);
    return 1;
}

int cmd_nethogs_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Nethogs requires per-process network statistics.\n" COLOR_RESET);
    return 1;
}

/* Process Tracing & Debugging */
int cmd_strace_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: strace <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Strace requires ptrace() implementation.\n" COLOR_RESET);
    printf(COLOR_GREY "Full syscall tracing implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_ltrace(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: ltrace <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Ltrace requires library call tracing.\n" COLOR_RESET);
    return 1;
}

int cmd_ptrace_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: ptrace <pid>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Ptrace requires kernel support and permissions.\n" COLOR_RESET);
    return 1;
}

int cmd_gdb_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "GDB requires debugger implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_lldb_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "LLDB requires debugger implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_valgrind(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Valgrind requires memory debugging framework.\n" COLOR_RESET);
    return 1;
}

int cmd_perf(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Perf requires Linux perf_events support.\n" COLOR_RESET);
    return 1;
}

int cmd_timeout(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: timeout <seconds> <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    int seconds = atoi(argv[1]);
    
    pid_t pid = fork();
    if (pid == 0) {
        alarm(seconds);
        execvp(argv[2], &argv[2]);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM) {
            printf(COLOR_GREY "Command timed out after %d seconds\n" COLOR_RESET, seconds);
            return 124; /* Standard timeout exit code */
        }
        
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }
    
    return 1;
}

int cmd_watch_process(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: watch <seconds> <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    int interval = atoi(argv[1]);
    
    printf(COLOR_GREY "Watching command every %d seconds (Ctrl+C to stop)\n" COLOR_RESET, interval);
    
    while (1) {
        printf(COLOR_GREY "\033[2J\033[H" COLOR_RESET); /* Clear screen */
        printf(COLOR_WHITE "Every %ds: " COLOR_RESET, interval);
        for (int i = 2; i < argc; i++) {
            printf("%s ", argv[i]);
        }
        printf("\n");
        printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
        
        pid_t pid = fork();
        if (pid == 0) {
            execvp(argv[2], &argv[2]);
            exit(1);
        } else if (pid > 0) {
            waitpid(pid, NULL, 0);
        }
        
        sleep(interval);
    }
    
    return 0;
}

/* Process Scheduling */
int cmd_at(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: at <time> <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "At requires job scheduling daemon.\n" COLOR_RESET);
    printf(COLOR_GREY "Implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_atq(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Atq requires job scheduling daemon.\n" COLOR_RESET);
    return 1;
}

int cmd_atrm(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: atrm <job_id>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Atrm requires job scheduling daemon.\n" COLOR_RESET);
    return 1;
}

int cmd_batch(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: batch <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Batch requires job scheduling daemon.\n" COLOR_RESET);
    return 1;
}

int cmd_cron(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Cron daemon requires system service implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_crontab(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: crontab <file>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Crontab requires cron daemon.\n" COLOR_RESET);
    return 1;
}

int cmd_anacron(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Anacron requires system service implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_systemd_run(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: systemd-run <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Systemd-run requires systemd support.\n" COLOR_RESET);
    return 1;
}

int cmd_systemctl(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: systemctl <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Systemctl requires systemd support.\n" COLOR_RESET);
    return 1;
}

/* Process Limits */
int cmd_ulimit_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        /* Show all limits */
        struct rlimit rlim;
        
        printf(COLOR_WHITE "Resource Limits:\n" COLOR_RESET);
        printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
        
        if (getrlimit(RLIMIT_CPU, &rlim) == 0) {
            printf(COLOR_WHITE "CPU time: " COLOR_RESET);
            if (rlim.rlim_max == RLIM_INFINITY) {
                printf("unlimited\n");
            } else {
                printf("%lu seconds\n", (unsigned long)rlim.rlim_max);
            }
        }
        
        if (getrlimit(RLIMIT_FSIZE, &rlim) == 0) {
            printf(COLOR_WHITE "File size: " COLOR_RESET);
            if (rlim.rlim_max == RLIM_INFINITY) {
                printf("unlimited\n");
            } else {
                printf("%lu bytes\n", (unsigned long)rlim.rlim_max);
            }
        }
        
        if (getrlimit(RLIMIT_DATA, &rlim) == 0) {
            printf(COLOR_WHITE "Data segment: " COLOR_RESET);
            if (rlim.rlim_max == RLIM_INFINITY) {
                printf("unlimited\n");
            } else {
                printf("%lu bytes\n", (unsigned long)rlim.rlim_max);
            }
        }
        
        if (getrlimit(RLIMIT_STACK, &rlim) == 0) {
            printf(COLOR_WHITE "Stack size: " COLOR_RESET);
            if (rlim.rlim_max == RLIM_INFINITY) {
                printf("unlimited\n");
            } else {
                printf("%lu bytes\n", (unsigned long)rlim.rlim_max);
            }
        }
        
        if (getrlimit(RLIMIT_CORE, &rlim) == 0) {
            printf(COLOR_WHITE "Core file size: " COLOR_RESET);
            if (rlim.rlim_max == RLIM_INFINITY) {
                printf("unlimited\n");
            } else {
                printf("%lu bytes\n", (unsigned long)rlim.rlim_max);
            }
        }
        
        if (getrlimit(RLIMIT_NOFILE, &rlim) == 0) {
            printf(COLOR_WHITE "Open files: " COLOR_RESET);
            if (rlim.rlim_max == RLIM_INFINITY) {
                printf("unlimited\n");
            } else {
                printf("%lu\n", (unsigned long)rlim.rlim_max);
            }
        }
        
        return 0;
    }
    
    printf(COLOR_GREY "Setting limits requires specific resource type.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'ulimit -a' to view all limits.\n" COLOR_RESET);
    return 1;
}

int cmd_prlimit(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: prlimit <pid> [resource] [limit]\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = (pid_t)atoi(argv[1]);
    struct rlimit rlim;
    
    if (getrlimit(RLIMIT_CPU, &rlim) == 0) {
        printf(COLOR_WHITE "Process %d limits:\n" COLOR_RESET, pid);
        printf(COLOR_GREY "CPU time: " COLOR_RESET);
        if (rlim.rlim_max == RLIM_INFINITY) {
            printf("unlimited\n");
        } else {
            printf("%lu seconds\n", (unsigned long)rlim.rlim_max);
        }
    }
    
    return 0;
}

int cmd_limit_ext(int argc, char **argv, shell_context_t *ctx) {
    return cmd_ulimit_ext(argc, argv, ctx);
}

int cmd_quota(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Quota requires filesystem quota support.\n" COLOR_RESET);
    return 1;
}

int cmd_edquota(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Edquota requires filesystem quota support.\n" COLOR_RESET);
    return 1;
}

int cmd_repquota(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Repquota requires filesystem quota support.\n" COLOR_RESET);
    return 1;
}

