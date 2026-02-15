#ifndef MISC_H
#define MISC_H

#include "../../include/void.h"

/* Category 14: Miscellaneous Utilities (100 commands) */

/* Random & Games */
int cmd_fortune(int argc, char **argv, shell_context_t *ctx);
int cmd_cowsay(int argc, char **argv, shell_context_t *ctx);
int cmd_cowthink(int argc, char **argv, shell_context_t *ctx);
int cmd_figlet(int argc, char **argv, shell_context_t *ctx);
int cmd_toilet(int argc, char **argv, shell_context_t *ctx);
int cmd_banner(int argc, char **argv, shell_context_t *ctx);
int cmd_sl(int argc, char **argv, shell_context_t *ctx);
int cmd_cmatrix(int argc, char **argv, shell_context_t *ctx);
int cmd_hollywood(int argc, char **argv, shell_context_t *ctx);
int cmd_rig(int argc, char **argv, shell_context_t *ctx);
int cmd_random(int argc, char **argv, shell_context_t *ctx);
int cmd_dice(int argc, char **argv, shell_context_t *ctx);
int cmd_coin(int argc, char **argv, shell_context_t *ctx);
int cmd_shuffle_list(int argc, char **argv, shell_context_t *ctx);
int cmd_pick(int argc, char **argv, shell_context_t *ctx);

/* System Fun */
int cmd_yes(int argc, char **argv, shell_context_t *ctx);
int cmd_factor(int argc, char **argv, shell_context_t *ctx);
int cmd_seq(int argc, char **argv, shell_context_t *ctx);
int cmd_jot(int argc, char **argv, shell_context_t *ctx);

/* Information */
int cmd_tldr(int argc, char **argv, shell_context_t *ctx);

#endif /* MISC_H */

