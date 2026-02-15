#include "filesystem.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <termios.h>
#include <math.h>
#include <ctype.h>
#include <math.h>
#include <ctype.h>

/* Format file size for display */
static char *format_size(off_t size) {
    static char buf[32];
    if (size < 1024) {
        snprintf(buf, sizeof(buf), "%ldB", (long)size);
    } else if (size < 1024 * 1024) {
        snprintf(buf, sizeof(buf), "%.1fK", size / 1024.0);
    } else if (size < 1024 * 1024 * 1024) {
        snprintf(buf, sizeof(buf), "%.1fM", size / (1024.0 * 1024.0));
    } else {
        snprintf(buf, sizeof(buf), "%.1fG", size / (1024.0 * 1024.0 * 1024.0));
    }
    return buf;
}

/* Get file type character */
static char get_file_type(mode_t mode) {
    if (S_ISREG(mode)) return '-';
    if (S_ISDIR(mode)) return 'd';
    if (S_ISLNK(mode)) return 'l';
    if (S_ISCHR(mode)) return 'c';
    if (S_ISBLK(mode)) return 'b';
    if (S_ISFIFO(mode)) return 'p';
    if (S_ISSOCK(mode)) return 's';
    return '?';
}

/* Format permissions string */
static void format_perms(mode_t mode, char *buf) {
    buf[0] = (mode & S_IRUSR) ? 'r' : '-';
    buf[1] = (mode & S_IWUSR) ? 'w' : '-';
    buf[2] = (mode & S_IXUSR) ? 'x' : '-';
    buf[3] = (mode & S_IRGRP) ? 'r' : '-';
    buf[4] = (mode & S_IWGRP) ? 'w' : '-';
    buf[5] = (mode & S_IXGRP) ? 'x' : '-';
    buf[6] = (mode & S_IROTH) ? 'r' : '-';
    buf[7] = (mode & S_IWOTH) ? 'w' : '-';
    buf[8] = (mode & S_IXOTH) ? 'x' : '-';
    buf[9] = '\0';
}

/* Format date string */
static char *format_date(time_t mtime) {
    static char buf[32];
    struct tm *tm_info = localtime(&mtime);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", tm_info);
    return buf;
}

int cmd_pulse(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *dir = (argc > 1) ? argv[1] : ".";
    
    DIR *d = opendir(dir);
    if (d == NULL) {
        LOG_ERROR("Failed to open directory '%s': %s", dir, strerror(errno));
        if (errno == ENOENT) {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
        } else {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PERMISSION_DENIED);
        }
        return 1;
    }
    
    printf(COLOR_GREY "%-10s %-12s %-19s %-10s %s\n" COLOR_RESET,
           "TYPE", "SIZE", "DATE", "PERMS", "NAME");
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.' && argc <= 1) {
            /* Skip hidden files unless explicitly requested */
            continue;
        }
        
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) != 0) {
            continue;
        }
        
        char type = get_file_type(st.st_mode);
        char perms[10];
        format_perms(st.st_mode, perms);
        
        printf(COLOR_WHITE "%c" COLOR_RESET, type);
        printf(COLOR_GREY " %-10s" COLOR_RESET, format_size(st.st_size));
        printf(COLOR_GREY " %-19s" COLOR_RESET, format_date(st.st_mtime));
        printf(COLOR_GREY " %-10s" COLOR_RESET, perms);
        
        if (type == 'd') {
            printf(COLOR_BLUE " %s/\n" COLOR_RESET, entry->d_name);
        } else if (type == 'l') {
            printf(COLOR_GREY " %s -> " COLOR_RESET, entry->d_name);
            char link_target[PATH_MAX];
            ssize_t len = readlink(full_path, link_target, sizeof(link_target) - 1);
            if (len != -1) {
                link_target[len] = '\0';
                printf(COLOR_GREY "%s\n" COLOR_RESET, link_target);
            } else {
                printf(COLOR_GREY "?\n" COLOR_RESET);
            }
        } else {
            printf(COLOR_WHITE " %s\n" COLOR_RESET, entry->d_name);
        }
    }
    
    closedir(d);
    return 0;
}

int cmd_warp(int argc, char **argv, shell_context_t *ctx) {
    /* warp is just an alias for cd, but we'll implement it for consistency */
    const char *dir = (argc > 1) ? argv[1] : getenv("HOME");
    
    if (dir == NULL) {
        dir = "~";
    }
    
    char *expanded = expand_path(dir);
    if (expanded == NULL) {
        expanded = string_dup(dir);
    }
    
    if (chdir(expanded) != 0) {
        LOG_ERROR("Failed to change directory to '%s': %s", expanded, strerror(errno));
        if (errno == ENOENT) {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
        } else if (errno == EACCES) {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PERMISSION_DENIED);
        } else {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
        }
        free(expanded);
        return 1;
    }
    
    if (ctx != NULL) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            free(ctx->cwd);
            ctx->cwd = string_dup(cwd);
        }
    }
    
    free(expanded);
    return 0;
}

int cmd_vanish(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    /* Clear terminal using ANSI escape codes */
    printf("\033[2J\033[H");
    fflush(stdout);
    return 0;
}

int cmd_echoes(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: echoes <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        LOG_ERROR("Failed to open file '%s': %s", argv[1], strerror(errno));
        if (errno == ENOENT) {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
        } else {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PERMISSION_DENIED);
        }
        return 1;
    }
    
    char line[4096];
    int line_num = 1;
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf(COLOR_GREY "%4d: " COLOR_WHITE "%s" COLOR_RESET, line_num, line);
        line_num++;
    }
    
    fclose(fp);
    return 0;
}

int cmd_shroud(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: shroud <name> [directory]\n" COLOR_RESET);
        return 1;
    }
    
    const char *name = argv[1];
    bool is_dir = (argc > 2 && strcmp(argv[2], "dir") == 0);
    
    char *expanded = expand_path(name);
    if (expanded == NULL) {
        expanded = string_dup(name);
    }
    
    if (is_dir) {
        if (mkdir(expanded, 0755) != 0) {
            LOG_ERROR("Failed to create directory '%s': %s", expanded, strerror(errno));
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
            free(expanded);
            return 1;
        }
    } else {
        FILE *fp = fopen(expanded, "w");
        if (fp == NULL) {
            LOG_ERROR("Failed to create file '%s': %s", expanded, strerror(errno));
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
            free(expanded);
            return 1;
        }
        fclose(fp);
    }
    
    free(expanded);
    return 0;
}

int cmd_transmute(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: transmute <old> <new>\n" COLOR_RESET);
        return 1;
    }
    
    char *old_expanded = expand_path(argv[1]);
    char *new_expanded = expand_path(argv[2]);
    
    if (old_expanded == NULL) old_expanded = string_dup(argv[1]);
    if (new_expanded == NULL) new_expanded = string_dup(argv[2]);
    
    if (rename(old_expanded, new_expanded) != 0) {
        LOG_ERROR("Failed to rename '%s' to '%s': %s", old_expanded, new_expanded, strerror(errno));
        if (errno == ENOENT) {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
        } else {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
        }
        free(old_expanded);
        free(new_expanded);
        return 1;
    }
    
    free(old_expanded);
    free(new_expanded);
    return 0;
}

int cmd_veil(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: veil <file>\n" COLOR_RESET);
        return 1;
    }
    
    struct stat st;
    if (stat(argv[1], &st) != 0) {
        LOG_ERROR("Failed to stat '%s': %s", argv[1], strerror(errno));
        if (errno == ENOENT) {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
        } else {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PERMISSION_DENIED);
        }
        return 1;
    }
    
    char perms[10];
    format_perms(st.st_mode, perms);
    char type = get_file_type(st.st_mode);
    
    printf(COLOR_WHITE "File: " COLOR_RESET "%s\n", argv[1]);
    printf(COLOR_WHITE "Type: " COLOR_RESET "%c\n", type);
    printf(COLOR_WHITE "Permissions: " COLOR_RESET "%s (%04o)\n", perms, st.st_mode & 0777);
    printf(COLOR_WHITE "Size: " COLOR_RESET "%s\n", format_size(st.st_size));
    printf(COLOR_WHITE "Modified: " COLOR_RESET "%s\n", format_date(st.st_mtime));
    
    return 0;
}

int cmd_seal(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: seal <file> <mode>\n" COLOR_RESET);
        printf(COLOR_GREY "Mode can be octal (e.g., 755) or symbolic (e.g., u+x)\n" COLOR_RESET);
        return 1;
    }
    
    char *expanded = expand_path(argv[1]);
    if (expanded == NULL) {
        expanded = string_dup(argv[1]);
    }
    
    mode_t mode;
    if (argv[2][0] >= '0' && argv[2][0] <= '7') {
        /* Octal mode */
        mode = (mode_t)strtol(argv[2], NULL, 8);
    } else {
        /* For now, only support octal. Symbolic mode would require more parsing */
        printf(COLOR_GREY "Only octal mode supported (e.g., 755)\n" COLOR_RESET);
        free(expanded);
        return 1;
    }
    
    if (chmod(expanded, mode) != 0) {
        LOG_ERROR("Failed to chmod '%s': %s", expanded, strerror(errno));
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PERMISSION_DENIED);
        free(expanded);
        return 1;
    }
    
    free(expanded);
    return 0;
}

int cmd_watch(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *dir = (argc > 1) ? argv[1] : ".";
    
    char *expanded = expand_path(dir);
    if (expanded == NULL) {
        expanded = string_dup(dir);
    }
    
    printf(COLOR_WHITE "Watching directory: %s\n" COLOR_RESET, expanded);
    printf(COLOR_GREY "Press Ctrl+C to stop.\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    /* Simple directory watching using stat and polling */
    struct stat last_stat;
    if (stat(expanded, &last_stat) != 0) {
        LOG_ERROR("Failed to stat directory '%s': %s", expanded, strerror(errno));
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
        free(expanded);
        return 1;
    }
    
    time_t last_mtime = last_stat.st_mtime;
    
    while (1) {
        sleep(2); /* Poll every 2 seconds */
        
        struct stat current_stat;
        if (stat(expanded, &current_stat) != 0) {
            break; /* Directory removed or error */
        }
        
        if (current_stat.st_mtime != last_mtime) {
            time_t now = time(NULL);
            struct tm *tm_info = localtime(&now);
            char time_str[16];
            strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);
            
            printf(COLOR_BLUE "[%s] " COLOR_RESET "Directory changed: %s\n", time_str, expanded);
            last_mtime = current_stat.st_mtime;
        }
    }
    
    free(expanded);
    return 0;
}

