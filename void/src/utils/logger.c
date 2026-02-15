#include "logger.h"
#include "../../include/void.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <pwd.h>

static FILE *log_file = NULL;
static bool initialized = false;

bool logger_init(void) {
    if (initialized) {
        return true;
    }

    /* Expand ~ to home directory */
    const char *log_path = "~/.void_logs";
    char expanded_path[PATH_MAX];
    
    if (log_path[0] == '~') {
        struct passwd *pw = getpwuid(getuid());
        if (pw == NULL) {
            return false;
        }
        snprintf(expanded_path, sizeof(expanded_path), "%s%s", 
                 pw->pw_dir, log_path + 1);
    } else {
        strncpy(expanded_path, log_path, sizeof(expanded_path) - 1);
        expanded_path[sizeof(expanded_path) - 1] = '\0';
    }

    log_file = fopen(expanded_path, "a");
    if (log_file == NULL) {
        return false;
    }

    initialized = true;
    LOG_INFO("Void OS Elite logger initialized");
    return true;
}

void logger_log(const char *level, const char *format, ...) {
    if (!initialized || log_file == NULL) {
        return;
    }

    time_t now;
    time(&now);
    struct tm *tm_info = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    va_list args;
    va_start(args, format);

    fprintf(log_file, "[%s] [%s] ", timestamp, level);
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");
    fflush(log_file);

    va_end(args);
}

void logger_cleanup(void) {
    if (log_file != NULL) {
        LOG_INFO("Void OS Elite logger shutting down");
        fclose(log_file);
        log_file = NULL;
    }
    initialized = false;
}

