#ifndef ALIAS_H
#define ALIAS_H

#include "../../include/void.h"

/* Alias command handlers */
int cmd_alias(int argc, char **argv, shell_context_t *ctx);
int cmd_unalias(int argc, char **argv, shell_context_t *ctx);
int cmd_aliases(int argc, char **argv, shell_context_t *ctx);

/* Alias management functions */
int add_alias(shell_context_t *ctx, const char *name, const char *value);
int remove_alias(shell_context_t *ctx, const char *name);

#endif /* ALIAS_H */

