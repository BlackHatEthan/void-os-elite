#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "../../include/void.h"

/* Execute a command */
int execute_command(int argc, char **argv, shell_context_t *ctx);

/* Execute external command via PATH */
int execute_external(const char *command, char **argv, shell_context_t *ctx);

/* Resolve alias */
char *resolve_alias(const char *name, shell_context_t *ctx);

#endif /* EXECUTOR_H */

