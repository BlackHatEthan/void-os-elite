#include "core.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <regex.h>
#include <errno.h>
#include <ctype.h>

/* History management */
#define MAX_HISTORY 1000
static char *history[MAX_HISTORY];
static int history_count = 0;
static int history_index = 0;

/* Export for use in shell.c */
void add_to_history(const char *cmd);
void add_to_history(const char *cmd) {
    if (cmd == NULL || strlen(cmd) == 0) return;
    
    /* Don't add duplicates of last command */
    if (history_count > 0 && strcmp(history[history_count - 1], cmd) == 0) {
        return;
    }
    
    if (history_count < MAX_HISTORY) {
        history[history_count++] = string_dup(cmd);
    } else {
        /* Rotate history */
        free(history[0]);
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            history[i] = history[i + 1];
        }
        history[MAX_HISTORY - 1] = string_dup(cmd);
    }
}

int cmd_history(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    int limit = history_count;
    
    if (argc > 1) {
        limit = atoi(argv[1]);
        if (limit <= 0 || limit > history_count) {
            limit = history_count;
        }
    }
    
    int start = (history_count > limit) ? history_count - limit : 0;
    for (int i = start; i < history_count; i++) {
        printf(COLOR_GREY "%5d  " COLOR_WHITE "%s\n" COLOR_RESET, i + 1, history[i]);
    }
    
    return 0;
}

int cmd_history_search(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: history-search <pattern>\n" COLOR_RESET);
        return 1;
    }
    
    const char *pattern = argv[1];
    int found = 0;
    
    for (int i = 0; i < history_count; i++) {
        if (strstr(history[i], pattern) != NULL) {
            printf(COLOR_GREY "%5d  " COLOR_WHITE "%s\n" COLOR_RESET, i + 1, history[i]);
            found++;
        }
    }
    
    if (found == 0) {
        printf(COLOR_GREY "No matching history entries.\n" COLOR_RESET);
    }
    
    return 0;
}

int cmd_history_clear(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    for (int i = 0; i < history_count; i++) {
        free(history[i]);
        history[i] = NULL;
    }
    history_count = 0;
    history_index = 0;
    
    printf(COLOR_GREEN "History cleared.\n" COLOR_RESET);
    return 0;
}

int cmd_repeat(int argc, char **argv, shell_context_t *ctx) {
    if (argc < 3) {
        printf(COLOR_GREY "Usage: repeat <n> <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    int n = atoi(argv[1]);
    if (n <= 0) {
        printf(COLOR_GREY "Invalid count: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    /* Build command */
    char **cmd_argv = &argv[2];
    int cmd_argc = argc - 2;
    
    for (int i = 0; i < n; i++) {
        /* Execute command - would need to call executor */
        printf(COLOR_GREY "Repeat %d/%d: " COLOR_WHITE, i + 1, n);
        for (int j = 0; j < cmd_argc; j++) {
            printf("%s ", cmd_argv[j]);
        }
        printf("\n" COLOR_RESET);
        /* TODO: Actually execute the command */
    }
    
    return 0;
}

int cmd_time_cmd(int argc, char **argv, shell_context_t *ctx) {
    if (argc < 2) {
        printf(COLOR_GREY "Usage: time <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    clock_t start = clock();
    
    /* Execute command */
    char **cmd_argv = &argv[1];
    /* TODO: Execute command and measure time */
    
    clock_t end = clock();
    double elapsed = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    
    printf(COLOR_GREY "Time: %.2fms\n" COLOR_RESET, elapsed);
    return 0;
}

int cmd_which_cmd(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: which <command>\n" COLOR_RESET);
        return 1;
    }
    
    const char *cmd = argv[1];
    
    /* Check if it's a built-in */
    /* TODO: Check built-in commands */
    
    /* Check PATH */
    const char *path = getenv("PATH");
    if (path == NULL) {
        printf(COLOR_GREY "Command not found: %s\n" COLOR_RESET, cmd);
        return 1;
    }
    
    char *path_copy = string_dup(path);
    char *dir = strtok(path_copy, ":");
    
    while (dir != NULL) {
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
        
        struct stat st;
        if (stat(full_path, &st) == 0 && (st.st_mode & S_IXUSR)) {
            printf(COLOR_WHITE "%s\n" COLOR_RESET, full_path);
            free(path_copy);
            return 0;
        }
        
        dir = strtok(NULL, ":");
    }
    
    free(path_copy);
    printf(COLOR_GREY "Command not found: %s\n" COLOR_RESET, cmd);
    return 1;
}

int cmd_whereis_cmd(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: whereis <command>\n" COLOR_RESET);
        return 1;
    }
    
    const char *cmd = argv[1];
    bool found = false;
    
    /* Binary */
    const char *path = getenv("PATH");
    if (path != NULL) {
        char *path_copy = string_dup(path);
        char *dir = strtok(path_copy, ":");
        
        while (dir != NULL) {
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
            
            struct stat st;
            if (stat(full_path, &st) == 0 && (st.st_mode & S_IXUSR)) {
                if (!found) {
                    printf(COLOR_WHITE "%s: " COLOR_RESET, cmd);
                    found = true;
                }
                printf(COLOR_BLUE "%s " COLOR_RESET, full_path);
            }
            
            dir = strtok(NULL, ":");
        }
        free(path_copy);
    }
    
    if (found) {
        printf("\n");
    } else {
        printf(COLOR_GREY "%s: not found\n" COLOR_RESET, cmd);
    }
    
    return found ? 0 : 1;
}

int cmd_type_cmd(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: type <command>\n" COLOR_RESET);
        return 1;
    }
    
    const char *cmd = argv[1];
    
    /* Check if built-in */
    printf(COLOR_WHITE "%s is " COLOR_RESET, cmd);
    
    /* TODO: Check built-in commands, aliases, functions */
    printf(COLOR_BLUE "a command\n" COLOR_RESET);
    
    return 0;
}

int cmd_help_cmd(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_WHITE "Void OS Elite - Available Commands\n" COLOR_RESET);
        printf(COLOR_GREY "Use 'help <command>' for specific help\n" COLOR_RESET);
        return 0;
    }
    
    const char *cmd = argv[1];
    printf(COLOR_WHITE "Help for: %s\n" COLOR_RESET, cmd);
    printf(COLOR_GREY "Documentation pending.\n" COLOR_RESET);
    
    return 0;
}

int cmd_man_cmd(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: man <page>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Manual pages not yet implemented.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'help <command>' for command help.\n" COLOR_RESET);
    return 1;
}

int cmd_info_cmd(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: info <topic>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Info pages not yet implemented.\n" COLOR_RESET);
    return 1;
}

int cmd_apropos_cmd(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: apropos <keyword>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Apropos not yet implemented.\n" COLOR_RESET);
    return 1;
}

int cmd_whatis_cmd(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: whatis <command>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Whatis not yet implemented.\n" COLOR_RESET);
    return 1;
}

/* Input/Output Redirection */
int cmd_cat(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: cat <file> [file2...]\n" COLOR_RESET);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, argv[i]);
            continue;
        }
        
        char line[4096];
        while (fgets(line, sizeof(line), fp) != NULL) {
            printf("%s", line);
        }
        fclose(fp);
    }
    
    return 0;
}

int cmd_tac(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: tac <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    /* Read all lines into array */
    char **lines = NULL;
    int line_count = 0;
    char line[4096];
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        lines = realloc(lines, (line_count + 1) * sizeof(char*));
        lines[line_count++] = string_dup(line);
    }
    fclose(fp);
    
    /* Print in reverse */
    for (int i = line_count - 1; i >= 0; i--) {
        printf("%s", lines[i]);
        free(lines[i]);
    }
    free(lines);
    
    return 0;
}

int cmd_head(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    int n = 10;
    int file_start = 1;
    
    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        if (argc < 4) {
            printf(COLOR_GREY "Usage: head -n <num> <file>\n" COLOR_RESET);
            return 1;
        }
        n = atoi(argv[2]);
        file_start = 3;
    }
    
    if (file_start >= argc) {
        printf(COLOR_GREY "Usage: head [-n <num>] <file>\n" COLOR_RESET);
        return 1;
    }
    
    for (int i = file_start; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, argv[i]);
            continue;
        }
        
        if (argc > file_start + 1) {
            printf(COLOR_WHITE "==> %s <==\n" COLOR_RESET, argv[i]);
        }
        
        char line[4096];
        int count = 0;
        while (fgets(line, sizeof(line), fp) != NULL && count < n) {
            printf("%s", line);
            count++;
        }
        fclose(fp);
        
        if (i < argc - 1) {
            printf("\n");
        }
    }
    
    return 0;
}

int cmd_tail(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    int n = 10;
    int file_start = 1;
    
    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        if (argc < 4) {
            printf(COLOR_GREY "Usage: tail -n <num> <file>\n" COLOR_RESET);
            return 1;
        }
        n = atoi(argv[2]);
        file_start = 3;
    }
    
    if (file_start >= argc) {
        printf(COLOR_GREY "Usage: tail [-n <num>] <file>\n" COLOR_RESET);
        return 1;
    }
    
    for (int i = file_start; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, argv[i]);
            continue;
        }
        
        /* Seek to end, then read backwards */
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        
        if (size == 0) {
            fclose(fp);
            continue;
        }
        
        /* Read last n lines */
        char **lines = NULL;
        int line_count = 0;
        long pos = size - 1;
        int newlines = 0;
        
        while (pos >= 0 && newlines < n) {
            fseek(fp, pos, SEEK_SET);
            char c = fgetc(fp);
            if (c == '\n') {
                newlines++;
            }
            pos--;
        }
        
        pos += 2; /* Adjust for newline */
        if (pos < 0) pos = 0;
        
        fseek(fp, pos, SEEK_SET);
        
        if (argc > file_start + 1) {
            printf(COLOR_WHITE "==> %s <==\n" COLOR_RESET, argv[i]);
        }
        
        char line[4096];
        while (fgets(line, sizeof(line), fp) != NULL) {
            printf("%s", line);
        }
        fclose(fp);
        
        if (i < argc - 1) {
            printf("\n");
        }
    }
    
    return 0;
}

int cmd_less(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: less <file>\n" COLOR_RESET);
        return 1;
    }
    
    /* Simple pager - just use cat for now */
    /* Full implementation would require terminal control */
    return cmd_cat(argc, argv, ctx);
}

int cmd_more(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: more <file>\n" COLOR_RESET);
        return 1;
    }
    
    /* Simple pager - just use cat for now */
    return cmd_cat(argc, argv, ctx);
}

int cmd_tee(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: tee <file> [file2...]\n" COLOR_RESET);
        return 1;
    }
    
    FILE **files = malloc((argc - 1) * sizeof(FILE*));
    if (files == NULL) {
        return 1;
    }
    
    int file_count = 0;
    for (int i = 1; i < argc; i++) {
        files[file_count] = fopen(argv[i], "a");
        if (files[file_count] == NULL) {
            printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, argv[i]);
            continue;
        }
        file_count++;
    }
    
    if (file_count == 0) {
        free(files);
        return 1;
    }
    
    char line[4096];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        printf("%s", line);
        for (int i = 0; i < file_count; i++) {
            fprintf(files[i], "%s", line);
        }
    }
    
    for (int i = 0; i < file_count; i++) {
        fclose(files[i]);
    }
    free(files);
    
    return 0;
}

int cmd_sponge(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: sponge <file>\n" COLOR_RESET);
        return 1;
    }
    
    /* Read all input */
    char *buffer = NULL;
    size_t buffer_size = 0;
    size_t total_size = 0;
    char line[4096];
    
    while (fgets(line, sizeof(line), stdin) != NULL) {
        size_t line_len = strlen(line);
        if (total_size + line_len + 1 > buffer_size) {
            buffer_size = (buffer_size == 0) ? 4096 : buffer_size * 2;
            buffer = realloc(buffer, buffer_size);
            if (buffer == NULL) {
                return 1;
            }
        }
        memcpy(buffer + total_size, line, line_len);
        total_size += line_len;
    }
    
    if (buffer != NULL) {
        buffer[total_size] = '\0';
        
        FILE *fp = fopen(argv[1], "w");
        if (fp != NULL) {
            fwrite(buffer, 1, total_size, fp);
            fclose(fp);
        }
        free(buffer);
    }
    
    return 0;
}

/* Text Processing - Basic implementations */
int cmd_grep_core(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: grep <pattern> [file...]\n" COLOR_RESET);
        return 1;
    }
    
    const char *pattern = argv[1];
    regex_t regex;
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret != 0) {
        printf(COLOR_GREY "Invalid regex pattern.\n" COLOR_RESET);
        return 1;
    }
    
    int file_start = 2;
    if (file_start >= argc) {
        /* Read from stdin */
        char line[4096];
        while (fgets(line, sizeof(line), stdin) != NULL) {
            if (regexec(&regex, line, 0, NULL, 0) == 0) {
                printf("%s", line);
            }
        }
    } else {
        for (int i = file_start; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                continue;
            }
            
            char line[4096];
            while (fgets(line, sizeof(line), fp) != NULL) {
                if (regexec(&regex, line, 0, NULL, 0) == 0) {
                    if (argc > file_start + 1) {
                        printf(COLOR_WHITE "%s:" COLOR_RESET, argv[i]);
                    }
                    printf("%s", line);
                }
            }
            fclose(fp);
        }
    }
    
    regfree(&regex);
    return 0;
}

int cmd_egrep(int argc, char **argv, shell_context_t *ctx) {
    return cmd_grep_core(argc, argv, ctx);
}

int cmd_fgrep(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: fgrep <string> [file...]\n" COLOR_RESET);
        return 1;
    }
    
    const char *pattern = argv[1];
    int file_start = 2;
    
    if (file_start >= argc) {
        char line[4096];
        while (fgets(line, sizeof(line), stdin) != NULL) {
            if (strstr(line, pattern) != NULL) {
                printf("%s", line);
            }
        }
    } else {
        for (int i = file_start; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                continue;
            }
            
            char line[4096];
            while (fgets(line, sizeof(line), fp) != NULL) {
                if (strstr(line, pattern) != NULL) {
                    if (argc > file_start + 1) {
                        printf(COLOR_WHITE "%s:" COLOR_RESET, argv[i]);
                    }
                    printf("%s", line);
                }
            }
            fclose(fp);
        }
    }
    
    return 0;
}

int cmd_sed(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: sed <script> [file...]\n" COLOR_RESET);
        printf(COLOR_GREY "Note: Basic sed implementation - supports s/pattern/replace/\n" COLOR_RESET);
        return 1;
    }
    
    /* Simple sed - just s/pattern/replace/ */
    const char *script = argv[1];
    if (strncmp(script, "s/", 2) != 0) {
        printf(COLOR_GREY "Only s/pattern/replace/ supported.\n" COLOR_RESET);
        return 1;
    }
    
    /* Parse s/pattern/replace/ */
    char *pattern = malloc(strlen(script));
    char *replace = malloc(strlen(script));
    if (pattern == NULL || replace == NULL) {
        free(pattern);
        free(replace);
        return 1;
    }
    
    const char *p = script + 2;
    char *pat_end = strchr(p, '/');
    if (pat_end == NULL) {
        free(pattern);
        free(replace);
        return 1;
    }
    
    strncpy(pattern, p, pat_end - p);
    pattern[pat_end - p] = '\0';
    
    p = pat_end + 1;
    char *rep_end = strchr(p, '/');
    if (rep_end == NULL) {
        free(pattern);
        free(replace);
        return 1;
    }
    
    strncpy(replace, p, rep_end - p);
    replace[rep_end - p] = '\0';
    
    int file_start = 2;
    if (file_start >= argc) {
        /* Read from stdin */
        char line[4096];
        while (fgets(line, sizeof(line), stdin) != NULL) {
            char *pos = strstr(line, pattern);
            if (pos != NULL) {
                size_t before = pos - line;
                size_t after = strlen(pos + strlen(pattern));
                char new_line[8192];
                memcpy(new_line, line, before);
                strcpy(new_line + before, replace);
                strcpy(new_line + before + strlen(replace), pos + strlen(pattern));
                printf("%s", new_line);
            } else {
                printf("%s", line);
            }
        }
    } else {
        for (int i = file_start; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                continue;
            }
            
            char line[4096];
            while (fgets(line, sizeof(line), fp) != NULL) {
                char *pos = strstr(line, pattern);
                if (pos != NULL) {
                    size_t before = pos - line;
                    char new_line[8192];
                    memcpy(new_line, line, before);
                    strcpy(new_line + before, replace);
                    strcpy(new_line + before + strlen(replace), pos + strlen(pattern));
                    printf("%s", new_line);
                } else {
                    printf("%s", line);
                }
            }
            fclose(fp);
        }
    }
    
    free(pattern);
    free(replace);
    return 0;
}

int cmd_awk(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "AWK implementation pending.\n" COLOR_RESET);
    printf(COLOR_GREY "Full AWK requires implementing pattern matching and scripting.\n" COLOR_RESET);
    return 1;
}

int cmd_cut(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: cut -d <delim> -f <fields> <file>\n" COLOR_RESET);
        return 1;
    }
    
    char delim = '\t';
    char *fields = NULL;
    const char *file = NULL;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            delim = argv[++i][0];
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            fields = argv[++i];
        } else {
            file = argv[i];
        }
    }
    
    if (fields == NULL || file == NULL) {
        printf(COLOR_GREY "Usage: cut -d <delim> -f <fields> <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, file);
        return 1;
    }
    
    /* Parse field list (e.g., "1,3,5" or "1-3") */
    char line[4096];
    while (fgets(line, sizeof(line), fp) != NULL) {
        /* Remove newline */
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        
        /* Split by delimiter */
        char *token = strtok(line, &delim);
        int field_num = 1;
        bool print_field[256] = {false};
        
        /* Parse field specification */
        char *field_spec = string_dup(fields);
        char *field_token = strtok(field_spec, ",");
        while (field_token != NULL) {
            if (strchr(field_token, '-') != NULL) {
                /* Range */
                int start = atoi(field_token);
                char *dash = strchr(field_token, '-');
                int end = (dash[1] != '\0') ? atoi(dash + 1) : start;
                for (int i = start; i <= end && i < 256; i++) {
                    print_field[i] = true;
                }
            } else {
                int f = atoi(field_token);
                if (f > 0 && f < 256) {
                    print_field[f] = true;
                }
            }
            field_token = strtok(NULL, ",");
        }
        free(field_spec);
        
        /* Print selected fields */
        bool first = true;
        while (token != NULL) {
            if (print_field[field_num]) {
                if (!first) {
                    printf("%c", delim);
                }
                printf("%s", token);
                first = false;
            }
            token = strtok(NULL, &delim);
            field_num++;
        }
        printf("\n");
    }
    
    fclose(fp);
    return 0;
}

int cmd_paste(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: paste <file1> <file2> [file3...]\n" COLOR_RESET);
        return 1;
    }
    
    FILE **files = malloc((argc - 1) * sizeof(FILE*));
    if (files == NULL) {
        return 1;
    }
    
    int file_count = 0;
    for (int i = 1; i < argc; i++) {
        files[file_count] = fopen(argv[i], "r");
        if (files[file_count] == NULL) {
            printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, argv[i]);
            continue;
        }
        file_count++;
    }
    
    if (file_count == 0) {
        free(files);
        return 1;
    }
    
    char **lines = malloc(file_count * sizeof(char*));
    bool more = true;
    
    while (more) {
        more = false;
        for (int i = 0; i < file_count; i++) {
            if (files[i] != NULL) {
                lines[i] = malloc(4096);
                if (fgets(lines[i], 4096, files[i]) != NULL) {
                    char *nl = strchr(lines[i], '\n');
                    if (nl) *nl = '\0';
                    more = true;
                } else {
                    free(lines[i]);
                    lines[i] = NULL;
                    fclose(files[i]);
                    files[i] = NULL;
                }
            }
        }
        
        if (more) {
            for (int i = 0; i < file_count; i++) {
                if (i > 0) printf("\t");
                if (lines[i] != NULL) {
                    printf("%s", lines[i]);
                    free(lines[i]);
                }
            }
            printf("\n");
        }
    }
    
    for (int i = 0; i < file_count; i++) {
        if (files[i] != NULL) {
            fclose(files[i]);
        }
    }
    
    free(lines);
    free(files);
    return 0;
}

int cmd_join(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: join <file1> <file2>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Join implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_sort(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: sort <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    /* Read all lines */
    char **lines = NULL;
    int line_count = 0;
    char line[4096];
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        lines = realloc(lines, (line_count + 1) * sizeof(char*));
        lines[line_count++] = string_dup(line);
    }
    fclose(fp);
    
    /* Sort */
    for (int i = 0; i < line_count - 1; i++) {
        for (int j = i + 1; j < line_count; j++) {
            if (strcmp(lines[i], lines[j]) > 0) {
                char *tmp = lines[i];
                lines[i] = lines[j];
                lines[j] = tmp;
            }
        }
    }
    
    /* Print */
    for (int i = 0; i < line_count; i++) {
        printf("%s", lines[i]);
        free(lines[i]);
    }
    free(lines);
    
    return 0;
}

int cmd_uniq(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: uniq <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    char prev_line[4096] = {0};
    char line[4096];
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strcmp(line, prev_line) != 0) {
            printf("%s", line);
            strcpy(prev_line, line);
        }
    }
    
    fclose(fp);
    return 0;
}

int cmd_wc(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: wc <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    int lines = 0, words = 0, chars = 0;
    char line[4096];
    bool in_word = false;
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        lines++;
        chars += strlen(line);
        
        for (int i = 0; line[i] != '\0'; i++) {
            if (isspace(line[i])) {
                in_word = false;
            } else if (!in_word) {
                words++;
                in_word = true;
            }
        }
        in_word = false;
    }
    
    fclose(fp);
    printf(COLOR_WHITE "%8d %8d %8d %s\n" COLOR_RESET, lines, words, chars, argv[1]);
    
    return 0;
}

int cmd_tr(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: tr <set1> <set2>\n" COLOR_RESET);
        return 1;
    }
    
    const char *set1 = argv[1];
    const char *set2 = argv[2];
    
    char line[4096];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        for (int i = 0; line[i] != '\0'; i++) {
            const char *pos = strchr(set1, line[i]);
            if (pos != NULL) {
                int idx = pos - set1;
                if (idx < (int)strlen(set2)) {
                    line[i] = set2[idx];
                }
            }
        }
        printf("%s", line);
    }
    
    return 0;
}

int cmd_fold(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    int width = 80;
    const char *file = NULL;
    
    if (argc > 1) {
        if (strcmp(argv[1], "-w") == 0 && argc > 2) {
            width = atoi(argv[2]);
            if (argc > 3) {
                file = argv[3];
            }
        } else {
            file = argv[1];
        }
    }
    
    FILE *fp = (file != NULL) ? fopen(file, "r") : stdin;
    if (fp == NULL && file != NULL) {
        printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, file);
        return 1;
    }
    
    char line[4096];
    while (fgets(line, sizeof(line), fp) != NULL) {
        int len = strlen(line);
        for (int i = 0; i < len; i += width) {
            int chunk = (len - i < width) ? len - i : width;
            char chunk_str[width + 1];
            memcpy(chunk_str, line + i, chunk);
            chunk_str[chunk] = '\0';
            printf("%s", chunk_str);
            if (i + chunk < len) {
                printf("\n");
            }
        }
    }
    
    if (file != NULL && fp != NULL) {
        fclose(fp);
    }
    
    return 0;
}

int cmd_fmt(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Fmt implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_nl(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: nl <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    int line_num = 1;
    char line[4096];
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf(COLOR_GREY "%6d  " COLOR_RESET "%s", line_num++, line);
    }
    
    fclose(fp);
    return 0;
}

int cmd_pr(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Pr implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_split(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: split <file> [prefix]\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    const char *prefix = (argc > 2) ? argv[2] : "x";
    int file_num = 0;
    int line_count = 0;
    FILE *out = NULL;
    char line[4096];
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (line_count % 1000 == 0) {
            if (out != NULL) {
                fclose(out);
            }
            char out_name[256];
            snprintf(out_name, sizeof(out_name), "%s%02d", prefix, file_num++);
            out = fopen(out_name, "w");
            if (out == NULL) {
                fclose(fp);
                return 1;
            }
        }
        
        if (out != NULL) {
            fprintf(out, "%s", line);
        }
        line_count++;
    }
    
    if (out != NULL) {
        fclose(out);
    }
    fclose(fp);
    
    printf(COLOR_GREEN "Split into %d files\n" COLOR_RESET, file_num);
    return 0;
}

int cmd_csplit(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Csplit implementation pending.\n" COLOR_RESET);
    return 1;
}

