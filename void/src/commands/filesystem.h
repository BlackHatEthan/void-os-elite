#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../../include/void.h"

/* Filesystem command handlers */
int cmd_pulse(int argc, char **argv, shell_context_t *ctx);
int cmd_warp(int argc, char **argv, shell_context_t *ctx);
int cmd_vanish(int argc, char **argv, shell_context_t *ctx);
int cmd_echoes(int argc, char **argv, shell_context_t *ctx);
int cmd_shroud(int argc, char **argv, shell_context_t *ctx);
int cmd_transmute(int argc, char **argv, shell_context_t *ctx);
int cmd_veil(int argc, char **argv, shell_context_t *ctx);
int cmd_seal(int argc, char **argv, shell_context_t *ctx);
int cmd_watch(int argc, char **argv, shell_context_t *ctx);
int cmd_scavenge(int argc, char **argv, shell_context_t *ctx);
int cmd_marrow(int argc, char **argv, shell_context_t *ctx);
int cmd_dissect(int argc, char **argv, shell_context_t *ctx);
int cmd_entropy_check(int argc, char **argv, shell_context_t *ctx);
int cmd_fossil(int argc, char **argv, shell_context_t *ctx);

#endif /* FILESYSTEM_H */

