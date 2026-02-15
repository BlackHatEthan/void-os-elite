#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../../include/void.h"

/* Environment command handlers */
int cmd_env(int argc, char **argv, shell_context_t *ctx);
int cmd_export(int argc, char **argv, shell_context_t *ctx);
int cmd_unset(int argc, char **argv, shell_context_t *ctx);
int cmd_reload(int argc, char **argv, shell_context_t *ctx);

/* Configuration file functions */
int load_config(shell_context_t *ctx);
int save_config(shell_context_t *ctx);

#endif /* ENVIRONMENT_H */

