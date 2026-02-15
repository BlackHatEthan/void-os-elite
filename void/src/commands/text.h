#ifndef TEXT_H
#define TEXT_H

#include "../../include/void.h"

/* Category 6: Text Processing & Editing (100 commands) */

/* Text Editors */
int cmd_vi(int argc, char **argv, shell_context_t *ctx);
int cmd_vim(int argc, char **argv, shell_context_t *ctx);
int cmd_nano(int argc, char **argv, shell_context_t *ctx);
int cmd_emacs(int argc, char **argv, shell_context_t *ctx);
int cmd_ed(int argc, char **argv, shell_context_t *ctx);

/* Text Manipulation */
int cmd_expand(int argc, char **argv, shell_context_t *ctx);
int cmd_unexpand(int argc, char **argv, shell_context_t *ctx);
int cmd_col(int argc, char **argv, shell_context_t *ctx);
int cmd_colrm(int argc, char **argv, shell_context_t *ctx);
int cmd_column(int argc, char **argv, shell_context_t *ctx);
int cmd_rev(int argc, char **argv, shell_context_t *ctx);
int cmd_shuf(int argc, char **argv, shell_context_t *ctx);

/* Text Search */
int cmd_ag(int argc, char **argv, shell_context_t *ctx);
int cmd_rg(int argc, char **argv, shell_context_t *ctx);
int cmd_ack(int argc, char **argv, shell_context_t *ctx);
int cmd_ugrep(int argc, char **argv, shell_context_t *ctx);
int cmd_sift(int argc, char **argv, shell_context_t *ctx);
int cmd_pt(int argc, char **argv, shell_context_t *ctx);

/* Text Statistics */
int cmd_wc_l(int argc, char **argv, shell_context_t *ctx);
int cmd_wc_w(int argc, char **argv, shell_context_t *ctx);
int cmd_wc_c(int argc, char **argv, shell_context_t *ctx);
int cmd_wc_m(int argc, char **argv, shell_context_t *ctx);
int cmd_wc_L(int argc, char **argv, shell_context_t *ctx);

#endif /* TEXT_H */

