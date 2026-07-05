#include "boot.h"
#include "../../include/void.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <pwd.h>

/* ASCII VOID Logo */
static const char *void_logo[] = {
    "██╗   ██╗ ██████╗ ██╗██████╗ ",
    "██║   ██║██╔═══██╗██║██╔══██╗",
    "██║   ██║██║   ██║██║██║  ██║",
    "╚██╗ ██╔╝██║   ██║██║██║  ██║",
    " ╚████╔╝ ╚██████╔╝██║██████╔╝",
    "  ╚═══╝   ╚═════╝ ╚═╝╚═════╝ ",
    "",
    "     Operating System Elite",
    ""
};

void display_void_logo(void) {
    printf(COLOR_WHITE);
    for (size_t i = 0; i < sizeof(void_logo) / sizeof(void_logo[0]); i++) {
        printf("%s\n", void_logo[i]);
    }
    printf(COLOR_RESET);
    printf(COLOR_GREY "          Created by " COLOR_WHITE "Ethan Sharma - Wadeson" COLOR_RESET "\n");
    printf("\n");
}

void display_system_info(void) {
    struct utsname sys_info;
    struct passwd *pw = getpwuid(getuid());
    char cwd[PATH_MAX];
    
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        strcpy(cwd, "?");
    }

    printf(COLOR_GREY "═══════════════════════════════════════\n" COLOR_RESET);
    
    if (uname(&sys_info) == 0) {
        printf(COLOR_GREY "Host OS:     " COLOR_WHITE "%s %s\n" COLOR_RESET, 
               sys_info.sysname, sys_info.release);
        printf(COLOR_GREY "Architecture:" COLOR_WHITE " %s\n" COLOR_RESET, sys_info.machine);
    }
    
    if (pw != NULL) {
        printf(COLOR_GREY "User:        " COLOR_WHITE "%s\n" COLOR_RESET, pw->pw_name);
    }
    
    printf(COLOR_GREY "Directory:   " COLOR_WHITE "%s\n" COLOR_RESET, cwd);
    printf(COLOR_GREY "Void OS:     " COLOR_WHITE "v%s\n" COLOR_RESET, VOID_VERSION);
    
    /* Note: Git version check removed - Void OS is standalone */
    /* Git operations are handled natively through .git file reading */
    
    printf(COLOR_GREY "═══════════════════════════════════════\n" COLOR_RESET);
    printf("\n");
}

void display_init_progress(const char *stage, int current, int total) {
    int percent = (current * 100) / total;
    int bar_width = 30;
    int filled = (percent * bar_width) / 100;
    
    printf(COLOR_BLUE "[%3d%%] " COLOR_RESET, percent);
    printf(COLOR_GREY);
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) {
            printf(COLOR_BLUE "█" COLOR_RESET);
        } else {
            printf("░");
        }
    }
    printf(COLOR_RESET);
    printf(COLOR_GREY " %s\n" COLOR_RESET, stage);
    fflush(stdout);
    
    /* Small delay for visual effect */
    usleep(100000); /* 100ms */
}

bool void_boot(void) {
    display_void_logo();
    display_system_info();
    
    /* Void OS is standalone - no external tool dependencies */
    printf(COLOR_BLUE "Void OS Core: " COLOR_WHITE "Standalone Native Implementation\n" COLOR_RESET);
    printf(COLOR_GREY "All operations use native system calls - no external dependencies\n" COLOR_RESET);
    printf("\n");
    
    printf(COLOR_BLUE "Initializing Void Core...\n\n" COLOR_RESET);
    
    const char *stages[] = {
        "Piercing the Veil…",
        "Mounting Reality…",
        "Initializing Process Manager…",
        "Loading Environment Matrix…",
        "Binding Native Tools…",
        "Detecting Security Capabilities…",
        "Initializing Native Git Interface…",
        "Stabilizing Void Core…"
    };
    
    int total_stages = sizeof(stages) / sizeof(stages[0]);
    
    for (int i = 0; i < total_stages; i++) {
        display_init_progress(stages[i], i + 1, total_stages);
    }
    
    printf("\n");
    display_boot_complete();
    
    return true;
}

void display_boot_complete(void) {
    printf(COLOR_GREEN "✓ " COLOR_WHITE "Void OS Elite Initialized." COLOR_RESET "\n");
    printf(COLOR_GREY "   Created by " COLOR_WHITE "Ethan Sharma - Wadeson" COLOR_RESET "\n\n");
}

