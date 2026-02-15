#include "misc.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

/* Random & Games */
int cmd_fortune(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    const char *fortunes[] = {
        "The Void awaits your command.",
        "In the depths of the terminal, power resides.",
        "Code flows like data through the system.",
        "Every command is a step into the unknown.",
        "The shell is your gateway to the machine.",
        "Void OS Elite - Where developers thrive.",
        "In the silence of the CLI, clarity emerges.",
        "Master the commands, master the system."
    };
    
    srand(time(NULL));
    int index = rand() % (sizeof(fortunes) / sizeof(fortunes[0]));
    printf(COLOR_WHITE "%s\n" COLOR_RESET, fortunes[index]);
    return 0;
}

int cmd_cowsay(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *text = (argc > 1) ? argv[1] : "Moo!";
    
    int len = strlen(text);
    printf(COLOR_WHITE " ");
    for (int i = 0; i < len + 2; i++) printf("_");
    printf("\n");
    printf("< %s >\n", text);
    printf(" ");
    for (int i = 0; i < len + 2; i++) printf("-");
    printf("\n");
    printf(COLOR_GREY "        \\   ^__^\n" COLOR_RESET);
    printf(COLOR_GREY "         \\  (oo)\\_______\n" COLOR_RESET);
    printf(COLOR_GREY "            (__)\\       )\\/\\\n" COLOR_RESET);
    printf(COLOR_GREY "                ||----w |\n" COLOR_RESET);
    printf(COLOR_GREY "                ||     ||\n" COLOR_RESET);
    return 0;
}

int cmd_cowthink(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *text = (argc > 1) ? argv[1] : "Hmm...";
    
    int len = strlen(text);
    printf(COLOR_WHITE " ");
    for (int i = 0; i < len + 2; i++) printf("_");
    printf("\n");
    printf("( %s )\n", text);
    printf(" ");
    for (int i = 0; i < len + 2; i++) printf("-");
    printf("\n");
    printf(COLOR_GREY "        o   ^__^\n" COLOR_RESET);
    printf(COLOR_GREY "         o  (oo)\\_______\n" COLOR_RESET);
    printf(COLOR_GREY "            (__)\\       )\\/\\\n" COLOR_RESET);
    printf(COLOR_GREY "                ||----w |\n" COLOR_RESET);
    printf(COLOR_GREY "                ||     ||\n" COLOR_RESET);
    return 0;
}

int cmd_figlet(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: figlet <text>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Figlet requires ASCII art font rendering.\n" COLOR_RESET);
    printf(COLOR_WHITE "%s\n" COLOR_RESET, argv[1]);
    return 0;
}

int cmd_toilet(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: toilet <text>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Toilet requires ASCII art font rendering.\n" COLOR_RESET);
    printf(COLOR_WHITE "%s\n" COLOR_RESET, argv[1]);
    return 0;
}

int cmd_banner(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: banner <text>\n" COLOR_RESET);
        return 1;
    }
    
    const char *text = argv[1];
    int len = strlen(text);
    
    printf(COLOR_WHITE "#");
    for (int i = 0; i < len + 2; i++) printf("=");
    printf("#\n");
    printf("# %s #\n", text);
    printf("#");
    for (int i = 0; i < len + 2; i++) printf("=");
    printf("#\n" COLOR_RESET);
    return 0;
}

int cmd_sl(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Steam locomotive animation requires implementation.\n" COLOR_RESET);
    printf(COLOR_WHITE "    ___\n" COLOR_RESET);
    printf(COLOR_WHITE "  _|___|_\n" COLOR_RESET);
    printf(COLOR_WHITE "  |     |\n" COLOR_RESET);
    printf(COLOR_WHITE "  |     |\n" COLOR_RESET);
    printf(COLOR_WHITE "  |_____|\n" COLOR_RESET);
    printf(COLOR_WHITE "   o   o\n" COLOR_RESET);
    return 0;
}

int cmd_cmatrix(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Matrix effect requires terminal animation.\n" COLOR_RESET);
    printf(COLOR_GREEN "01010101\n" COLOR_RESET);
    printf(COLOR_GREEN "10101010\n" COLOR_RESET);
    printf(COLOR_GREEN "01010101\n" COLOR_RESET);
    return 0;
}

int cmd_hollywood(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Hollywood effect requires terminal multiplexer.\n" COLOR_RESET);
    return 1;
}

int cmd_rig(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    const char *first[] = {"John", "Jane", "Bob", "Alice", "Charlie"};
    const char *last[] = {"Smith", "Doe", "Johnson", "Williams", "Brown"};
    
    srand(time(NULL));
    printf(COLOR_WHITE "%s %s\n" COLOR_RESET, 
           first[rand() % (sizeof(first) / sizeof(first[0]))],
           last[rand() % (sizeof(last) / sizeof(last[0]))]);
    return 0;
}

int cmd_random(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    int max = (argc > 1) ? atoi(argv[1]) : 100;
    
    if (max <= 0) {
        printf(COLOR_GREY "Invalid maximum value.\n" COLOR_RESET);
        return 1;
    }
    
    srand(time(NULL));
    printf(COLOR_WHITE "%d\n" COLOR_RESET, rand() % max);
    return 0;
}

int cmd_dice(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    int sides = (argc > 1) ? atoi(argv[1]) : 6;
    
    if (sides < 2) {
        printf(COLOR_GREY "Invalid number of sides (minimum 2).\n" COLOR_RESET);
        return 1;
    }
    
    srand(time(NULL));
    printf(COLOR_WHITE "%d\n" COLOR_RESET, (rand() % sides) + 1);
    return 0;
}

int cmd_coin(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    srand(time(NULL));
    const char *result = (rand() % 2 == 0) ? "Heads" : "Tails";
    printf(COLOR_WHITE "%s\n" COLOR_RESET, result);
    return 0;
}

int cmd_shuffle_list(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: shuffle <item1> <item2> ...\n" COLOR_RESET);
        return 1;
    }
    
    /* Fisher-Yates shuffle */
    srand(time(NULL));
    for (int i = argc - 1; i > 1; i--) {
        int j = (rand() % (i - 1)) + 1;
        char *temp = argv[i];
        argv[i] = argv[j];
        argv[j] = temp;
    }
    
    for (int i = 1; i < argc; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, argv[i]);
    }
    printf("\n");
    return 0;
}

int cmd_pick(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: pick <item1> <item2> ...\n" COLOR_RESET);
        return 1;
    }
    
    srand(time(NULL));
    int index = (rand() % (argc - 1)) + 1;
    printf(COLOR_WHITE "%s\n" COLOR_RESET, argv[index]);
    return 0;
}

/* System Fun */
int cmd_yes(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *text = (argc > 1) ? argv[1] : "y";
    
    while (1) {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, text);
    }
    
    return 0;
}

int cmd_factor(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: factor <number>\n" COLOR_RESET);
        return 1;
    }
    
    long num = atol(argv[1]);
    if (num < 2) {
        printf(COLOR_GREY "Invalid number (must be >= 2).\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_WHITE "%ld:" COLOR_RESET, num);
    
    long n = num;
    for (long i = 2; i * i <= n; i++) {
        while (n % i == 0) {
            printf(COLOR_WHITE " %ld" COLOR_RESET, i);
            n /= i;
        }
    }
    
    if (n > 1) {
        printf(COLOR_WHITE " %ld" COLOR_RESET, n);
    }
    printf("\n");
    return 0;
}

int cmd_seq(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: seq [start] [end] [step]\n" COLOR_RESET);
        return 1;
    }
    
    long start = 1, end, step = 1;
    
    if (argc == 2) {
        end = atol(argv[1]);
    } else if (argc == 3) {
        start = atol(argv[1]);
        end = atol(argv[2]);
    } else {
        start = atol(argv[1]);
        end = atol(argv[2]);
        step = atol(argv[3]);
    }
    
    if (step == 0) {
        printf(COLOR_GREY "Step cannot be zero.\n" COLOR_RESET);
        return 1;
    }
    
    if ((step > 0 && start > end) || (step < 0 && start < end)) {
        return 0; /* Empty sequence */
    }
    
    for (long i = start; (step > 0) ? (i <= end) : (i >= end); i += step) {
        printf(COLOR_WHITE "%ld\n" COLOR_RESET, i);
    }
    
    return 0;
}

int cmd_jot(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: jot <count> [start] [end]\n" COLOR_RESET);
        return 1;
    }
    
    int count = atoi(argv[1]);
    double start = (argc > 2) ? atof(argv[2]) : 1.0;
    double end = (argc > 3) ? atof(argv[3]) : (double)count;
    
    if (count < 1) {
        printf(COLOR_GREY "Count must be >= 1.\n" COLOR_RESET);
        return 1;
    }
    
    double step = (count > 1) ? (end - start) / (count - 1) : 0.0;
    
    for (int i = 0; i < count; i++) {
        double value = start + (step * i);
        printf(COLOR_WHITE "%.6g\n" COLOR_RESET, value);
    }
    
    return 0;
}

/* Information */
int cmd_tldr(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: tldr <command>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "TLDR pages require TLDR database.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'help %s' for basic help.\n" COLOR_RESET, argv[1]);
    return 1;
}

