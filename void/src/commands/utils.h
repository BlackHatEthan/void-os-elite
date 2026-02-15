#ifndef UTILS_H
#define UTILS_H

#include "../../include/void.h"

/* Developer utility command handlers */
int cmd_summon(int argc, char **argv, shell_context_t *ctx);
int cmd_forgepy(int argc, char **argv, shell_context_t *ctx);
int cmd_multitool(int argc, char **argv, shell_context_t *ctx);
int cmd_fabricate(int argc, char **argv, shell_context_t *ctx);
int cmd_refactor(int argc, char **argv, shell_context_t *ctx);
int cmd_minify(int argc, char **argv, shell_context_t *ctx);
int cmd_blueprint(int argc, char **argv, shell_context_t *ctx);
int cmd_debug_vision(int argc, char **argv, shell_context_t *ctx);

/* Multitool sub-commands */
int multitool_hash(int argc, char **argv);
int multitool_base64(int argc, char **argv);
int multitool_json(int argc, char **argv);
int multitool_uuid(int argc, char **argv);
int multitool_timestamp(int argc, char **argv);

#endif /* UTILS_H */

