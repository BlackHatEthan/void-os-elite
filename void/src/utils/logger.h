#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdbool.h>

/* Initialize logging system */
bool logger_init(void);

/* Log a message with timestamp */
void logger_log(const char *level, const char *format, ...);

/* Convenience macros */
#define LOG_ERROR(fmt, ...) logger_log("ERROR", fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  logger_log("WARN",  fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  logger_log("INFO",  fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) logger_log("DEBUG", fmt, ##__VA_ARGS__)

/* Cleanup logging system */
void logger_cleanup(void);

#endif /* LOGGER_H */

