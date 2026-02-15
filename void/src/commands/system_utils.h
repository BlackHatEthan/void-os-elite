#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

#include "../../include/void.h"

/* Category 10: System Utilities (100 commands) */

/* Date & Time */
int cmd_date_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_cal(int argc, char **argv, shell_context_t *ctx);
int cmd_ncal(int argc, char **argv, shell_context_t *ctx);
int cmd_hwclock(int argc, char **argv, shell_context_t *ctx);
int cmd_timedatectl(int argc, char **argv, shell_context_t *ctx);
int cmd_tzselect(int argc, char **argv, shell_context_t *ctx);
int cmd_zdump(int argc, char **argv, shell_context_t *ctx);

#endif /* SYSTEM_UTILS_H */

