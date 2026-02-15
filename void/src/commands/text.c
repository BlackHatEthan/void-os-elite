#include "text.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>

/* Text Editors */
int cmd_vi(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: vi <file>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Vi requires interactive text editor implementation.\n" COLOR_RESET);
    printf(COLOR_GREY "Void OS does not include a native text editor.\n" COLOR_RESET);
    return 1;
}

int cmd_vim(int argc, char **argv, shell_context_t *ctx) {
    return cmd_vi(argc, argv, ctx);
}

int cmd_nano(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: nano <file>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Nano requires interactive text editor implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_emacs(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: emacs <file>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Emacs requires interactive text editor implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_ed(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: ed <file>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Ed requires line editor implementation.\n" COLOR_RESET);
    return 1;
}

/* Text Manipulation */
int cmd_expand(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: expand <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open file: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    int c;
    int spaces = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\t') {
            /* Expand tab to 8 spaces */
            for (int i = 0; i < 8; i++) {
                putchar(' ');
            }
            spaces = 0;
        } else {
            putchar(c);
            if (c == '\n') {
                spaces = 0;
            } else {
                spaces++;
            }
        }
    }
    
    fclose(fp);
    return 0;
}

int cmd_unexpand(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: unexpand <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open file: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    int c;
    int spaces = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (c == ' ') {
            spaces++;
            if (spaces == 8) {
                putchar('\t');
                spaces = 0;
            }
        } else {
            while (spaces > 0) {
                putchar(' ');
                spaces--;
            }
            putchar(c);
            if (c == '\n') {
                spaces = 0;
            }
        }
    }
    
    fclose(fp);
    return 0;
}

int cmd_col(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    FILE *fp = (argc > 1) ? fopen(argv[1], "r") : stdin;
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open file: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    int c;
    while ((c = fgetc(fp)) != EOF) {
        /* Filter reverse line feeds and other control characters */
        if (c == '\r') {
            continue; /* Skip carriage return */
        }
        if (c == '\033') { /* ESC sequence */
            /* Skip ANSI escape sequences */
            int next = fgetc(fp);
            if (next == '[') {
                while ((next = fgetc(fp)) != EOF && next != 'm' && next != 'H' && next != 'J') {
                    /* Skip escape sequence */
                }
            }
            continue;
        }
        putchar(c);
    }
    
    if (fp != stdin) {
        fclose(fp);
    }
    return 0;
}

int cmd_colrm(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: colrm <start> <end>\n" COLOR_RESET);
        return 1;
    }
    
    int start = atoi(argv[1]);
    int end = (argc > 2) ? atoi(argv[2]) : 0;
    
    if (start < 1) start = 1;
    if (end > 0 && end < start) {
        printf(COLOR_GREY "End column must be >= start column\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = (argc > 3) ? fopen(argv[3], "r") : stdin;
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open file: %s\n" COLOR_RESET, argv[3]);
        return 1;
    }
    
    char line[4096];
    while (fgets(line, sizeof(line), fp) != NULL) {
        int len = strlen(line);
        if (line[len - 1] == '\n') len--;
        
        for (int i = 0; i < len; i++) {
            int col = i + 1;
            if (col < start || (end > 0 && col > end)) {
                putchar(line[i]);
            }
        }
        putchar('\n');
    }
    
    if (fp != stdin) {
        fclose(fp);
    }
    return 0;
}

int cmd_column(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    FILE *fp = (argc > 1) ? fopen(argv[1], "r") : stdin;
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open file: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    char line[4096];
    int max_width = 0;
    char *lines[1000];
    int line_count = 0;
    
    /* Read all lines */
    while (fgets(line, sizeof(line), fp) != NULL && line_count < 1000) {
        lines[line_count] = string_dup(line);
        int len = strlen(lines[line_count]);
        if (len > max_width) max_width = len;
        line_count++;
    }
    
    if (fp != stdin) {
        fclose(fp);
    }
    
    /* Print in columns */
    int cols = 80 / (max_width + 2);
    if (cols < 1) cols = 1;
    
    for (int i = 0; i < line_count; i += cols) {
        for (int j = 0; j < cols && (i + j) < line_count; j++) {
            printf("%-*s", max_width + 2, lines[i + j]);
        }
        printf("\n");
    }
    
    /* Free lines */
    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    
    return 0;
}

int cmd_rev(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    FILE *fp = (argc > 1) ? fopen(argv[1], "r") : stdin;
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open file: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    char line[4096];
    while (fgets(line, sizeof(line), fp) != NULL) {
        int len = strlen(line);
        if (line[len - 1] == '\n') {
            len--;
        }
        
        /* Reverse the line */
        for (int i = len - 1; i >= 0; i--) {
            putchar(line[i]);
        }
        putchar('\n');
    }
    
    if (fp != stdin) {
        fclose(fp);
    }
    return 0;
}

int cmd_shuf(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    FILE *fp = (argc > 1) ? fopen(argv[1], "r") : stdin;
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open file: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    char *lines[10000];
    int line_count = 0;
    char line[4096];
    
    /* Read all lines */
    while (fgets(line, sizeof(line), fp) != NULL && line_count < 10000) {
        lines[line_count++] = string_dup(line);
    }
    
    if (fp != stdin) {
        fclose(fp);
    }
    
    /* Fisher-Yates shuffle */
    srand(time(NULL));
    for (int i = line_count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char *temp = lines[i];
        lines[i] = lines[j];
        lines[j] = temp;
    }
    
    /* Print shuffled lines */
    for (int i = 0; i < line_count; i++) {
        printf("%s", lines[i]);
        free(lines[i]);
    }
    
    return 0;
}

int cmd_most(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: most <file>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Most requires interactive pager implementation.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'less' or 'more' for basic paging.\n" COLOR_RESET);
    return 1;
}

/* Text Search */
int cmd_ag(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: ag <pattern> [file...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Ag (Silver Searcher) requires fast text search implementation.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'grep' for basic text search.\n" COLOR_RESET);
    return 1;
}

int cmd_rg(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: rg <pattern> [file...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Rg (Ripgrep) requires fast text search implementation.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'grep' for basic text search.\n" COLOR_RESET);
    return 1;
}

int cmd_ack(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: ack <pattern> [file...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Ack requires code-aware search implementation.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'grep' for basic text search.\n" COLOR_RESET);
    return 1;
}

int cmd_ugrep(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: ugrep <pattern> [file...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Ugrep requires ultra-fast grep implementation.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'grep' for basic text search.\n" COLOR_RESET);
    return 1;
}

int cmd_sift(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: sift <pattern> [file...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Sift requires fast text search implementation.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'grep' for basic text search.\n" COLOR_RESET);
    return 1;
}

int cmd_pt(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: pt <pattern> [file...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Pt (Platinum Searcher) requires fast text search implementation.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'grep' for basic text search.\n" COLOR_RESET);
    return 1;
}

/* Text Statistics */
int cmd_wc_l(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    FILE *fp = (argc > 1) ? fopen(argv[1], "r") : stdin;
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open file: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    int lines = 0;
    char line[4096];
    while (fgets(line, sizeof(line), fp) != NULL) {
        lines++;
    }
    
    printf(COLOR_WHITE "%d\n" COLOR_RESET, lines);
    
    if (fp != stdin) {
        fclose(fp);
    }
    return 0;
}

int cmd_wc_w(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    FILE *fp = (argc > 1) ? fopen(argv[1], "r") : stdin;
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open file: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    int words = 0;
    int c;
    bool in_word = false;
    
    while ((c = fgetc(fp)) != EOF) {
        if (isspace(c)) {
            in_word = false;
        } else if (!in_word) {
            in_word = true;
            words++;
        }
    }
    
    printf(COLOR_WHITE "%d\n" COLOR_RESET, words);
    
    if (fp != stdin) {
        fclose(fp);
    }
    return 0;
}

int cmd_wc_c(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    FILE *fp = (argc > 1) ? fopen(argv[1], "r") : stdin;
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open file: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    int chars = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        chars++;
    }
    
    printf(COLOR_WHITE "%d\n" COLOR_RESET, chars);
    
    if (fp != stdin) {
        fclose(fp);
    }
    return 0;
}

int cmd_wc_m(int argc, char **argv, shell_context_t *ctx) {
    /* Same as wc-c for character count */
    return cmd_wc_c(argc, argv, ctx);
}

int cmd_wc_L(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    FILE *fp = (argc > 1) ? fopen(argv[1], "r") : stdin;
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot open file: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    int max_len = 0;
    int current_len = 0;
    int c;
    
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            if (current_len > max_len) {
                max_len = current_len;
            }
            current_len = 0;
        } else {
            current_len++;
        }
    }
    
    if (current_len > max_len) {
        max_len = current_len;
    }
    
    printf(COLOR_WHITE "%d\n" COLOR_RESET, max_len);
    
    if (fp != stdin) {
        fclose(fp);
    }
    return 0;
}

