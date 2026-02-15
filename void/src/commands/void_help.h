#ifndef VOID_HELP_H
#define VOID_HELP_H

#include "../../include/void.h"

/* Comprehensive help system for Void OS */
int cmd_void_help(int argc, char **argv, shell_context_t *ctx);

/* Help categories */
void show_help_category(const char *category);
void show_all_commands(void);
void show_command_help(const char *command_name);

#endif /* VOID_HELP_H */

