#ifndef SECURITY_H
#define SECURITY_H

#include "../../include/void.h"

/* Security command handlers */
int cmd_startup_check(int argc, char **argv, shell_context_t *ctx);
int cmd_perm_audit(int argc, char **argv, shell_context_t *ctx);
int cmd_net_watch(int argc, char **argv, shell_context_t *ctx);
int cmd_incantation(int argc, char **argv, shell_context_t *ctx);
int cmd_black_hole(int argc, char **argv, shell_context_t *ctx);
int cmd_echo_chamber(int argc, char **argv, shell_context_t *ctx);
int cmd_stasis(int argc, char **argv, shell_context_t *ctx);
int cmd_black_box(int argc, char **argv, shell_context_t *ctx);
int cmd_signal_lost(int argc, char **argv, shell_context_t *ctx);
int cmd_chronos(int argc, char **argv, shell_context_t *ctx);
int cmd_singularity(int argc, char **argv, shell_context_t *ctx);
int cmd_glitch(int argc, char **argv, shell_context_t *ctx);

#endif /* SECURITY_H */

