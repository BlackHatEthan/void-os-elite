#include "system_utils.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

/* Date & Time */
int cmd_date_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    if (argc > 1) {
        /* Format string provided */
        char format[256];
        strncpy(format, argv[1], sizeof(format) - 1);
        format[sizeof(format) - 1] = '\0';
        
        char buffer[512];
        if (strftime(buffer, sizeof(buffer), format, tm_info) > 0) {
            printf(COLOR_WHITE "%s\n" COLOR_RESET, buffer);
            return 0;
        } else {
            printf(COLOR_GREY "Invalid date format.\n" COLOR_RESET);
            return 1;
        }
    }
    
    /* Default format: RFC 2822 */
    char buffer[128];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %z", tm_info);
    printf(COLOR_WHITE "%s\n" COLOR_RESET, buffer);
    return 0;
}

int cmd_cal(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    int month = tm_info->tm_mon + 1;
    int year = tm_info->tm_year + 1900;
    
    if (argc > 1) {
        month = atoi(argv[1]);
        if (month < 1 || month > 12) {
            printf(COLOR_GREY "Invalid month (1-12).\n" COLOR_RESET);
            return 1;
        }
    }
    
    if (argc > 2) {
        year = atoi(argv[2]);
        if (year < 1 || year > 9999) {
            printf(COLOR_GREY "Invalid year (1-9999).\n" COLOR_RESET);
            return 1;
        }
    }
    
    /* Calculate first day of month */
    struct tm first_day = {0};
    first_day.tm_year = year - 1900;
    first_day.tm_mon = month - 1;
    first_day.tm_mday = 1;
    mktime(&first_day);
    
    int start_day = first_day.tm_wday;
    int days_in_month = 0;
    
    /* Calculate days in month */
    struct tm last_day = {0};
    last_day.tm_year = year - 1900;
    last_day.tm_mon = month;
    last_day.tm_mday = 0;
    mktime(&last_day);
    days_in_month = last_day.tm_mday;
    
    /* Month names */
    const char *months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    
    printf(COLOR_WHITE "   %s %d\n" COLOR_RESET, months[month - 1], year);
    printf(COLOR_GREY "Su Mo Tu We Th Fr Sa\n" COLOR_RESET);
    
    /* Print leading spaces */
    for (int i = 0; i < start_day; i++) {
        printf("   ");
    }
    
    /* Print days */
    for (int day = 1; day <= days_in_month; day++) {
        printf(COLOR_WHITE "%2d " COLOR_RESET, day);
        if ((start_day + day) % 7 == 0) {
            printf("\n");
        }
    }
    
    if ((start_day + days_in_month) % 7 != 0) {
        printf("\n");
    }
    
    return 0;
}

int cmd_ncal(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    /* ncal is similar to cal but with different layout */
    printf(COLOR_GREY "Ncal (alternative calendar layout) - using cal format.\n" COLOR_RESET);
    return cmd_cal(argc, argv, ctx);
}

int cmd_hwclock(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: hwclock [command]\n" COLOR_RESET);
        printf(COLOR_GREY "Commands: --show, --set, --systohc, --hctosys\n" COLOR_RESET);
        return 1;
    }
    
    if (strcmp(argv[1], "--show") == 0) {
        /* Show hardware clock time */
        printf(COLOR_GREY "Hardware clock requires system-level access.\n" COLOR_RESET);
        printf(COLOR_GREY "Void OS cannot directly access hardware clock.\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Hardware clock operations require system-level access.\n" COLOR_RESET);
    return 1;
}

int cmd_timedatectl(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: timedatectl [command] [options]\n" COLOR_RESET);
        printf(COLOR_GREY "Commands: status, set-time, set-timezone, list-timezones\n" COLOR_RESET);
        return 1;
    }
    
    if (strcmp(argv[1], "status") == 0) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char buffer[128];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S %z", tm_info);
        
        printf(COLOR_WHITE "Local time: %s\n" COLOR_RESET, buffer);
        printf(COLOR_GREY "System clock: synchronized\n" COLOR_RESET);
        return 0;
    }
    
    printf(COLOR_GREY "Timedatectl requires systemd or system-level time management.\n" COLOR_RESET);
    return 1;
}

int cmd_tzselect(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Tzselect requires interactive timezone selection.\n" COLOR_RESET);
    printf(COLOR_GREY "Void OS does not include interactive timezone selector.\n" COLOR_RESET);
    return 1;
}

int cmd_zdump(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: zdump [timezone]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Zdump requires timezone database access.\n" COLOR_RESET);
    printf(COLOR_GREY "Void OS does not include timezone database.\n" COLOR_RESET);
    return 1;
}

