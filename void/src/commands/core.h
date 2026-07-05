#ifndef CORE_H
#define CORE_H

#include "../../include/void.h"

/* History management */
void add_to_history(const char *cmd);

/* Core shell commands - Category 1 from COMMAND_EXPANSION.md */

/* Navigation & Basic Operations */
int cmd_history(int argc, char **argv, shell_context_t *ctx);
int cmd_history_search(int argc, char **argv, shell_context_t *ctx);
int cmd_history_clear(int argc, char **argv, shell_context_t *ctx);
int cmd_repeat(int argc, char **argv, shell_context_t *ctx);
int cmd_time_cmd(int argc, char **argv, shell_context_t *ctx);
int cmd_which_cmd(int argc, char **argv, shell_context_t *ctx);
int cmd_whereis_cmd(int argc, char **argv, shell_context_t *ctx);
int cmd_type_cmd(int argc, char **argv, shell_context_t *ctx);
int cmd_help_cmd(int argc, char **argv, shell_context_t *ctx);
int cmd_man_cmd(int argc, char **argv, shell_context_t *ctx);
int cmd_info_cmd(int argc, char **argv, shell_context_t *ctx);
int cmd_apropos_cmd(int argc, char **argv, shell_context_t *ctx);
int cmd_whatis_cmd(int argc, char **argv, shell_context_t *ctx);

/* Input/Output Redirection */
int cmd_cat(int argc, char **argv, shell_context_t *ctx);
int cmd_tac(int argc, char **argv, shell_context_t *ctx);
int cmd_head(int argc, char **argv, shell_context_t *ctx);
int cmd_tail(int argc, char **argv, shell_context_t *ctx);
int cmd_less(int argc, char **argv, shell_context_t *ctx);
int cmd_more(int argc, char **argv, shell_context_t *ctx);
int cmd_tee(int argc, char **argv, shell_context_t *ctx);
int cmd_sponge(int argc, char **argv, shell_context_t *ctx);

/* Text Processing */
int cmd_grep_core(int argc, char **argv, shell_context_t *ctx);
int cmd_egrep(int argc, char **argv, shell_context_t *ctx);
int cmd_fgrep(int argc, char **argv, shell_context_t *ctx);
int cmd_sed(int argc, char **argv, shell_context_t *ctx);
int cmd_awk(int argc, char **argv, shell_context_t *ctx);
int cmd_cut(int argc, char **argv, shell_context_t *ctx);
int cmd_paste(int argc, char **argv, shell_context_t *ctx);
int cmd_join(int argc, char **argv, shell_context_t *ctx);
int cmd_sort(int argc, char **argv, shell_context_t *ctx);
int cmd_uniq(int argc, char **argv, shell_context_t *ctx);
int cmd_wc(int argc, char **argv, shell_context_t *ctx);
int cmd_tr(int argc, char **argv, shell_context_t *ctx);
int cmd_fold(int argc, char **argv, shell_context_t *ctx);
int cmd_fmt(int argc, char **argv, shell_context_t *ctx);
int cmd_nl(int argc, char **argv, shell_context_t *ctx);
int cmd_pr(int argc, char **argv, shell_context_t *ctx);
int cmd_split(int argc, char **argv, shell_context_t *ctx);
int cmd_csplit(int argc, char **argv, shell_context_t *ctx);

#endif /* CORE_H */

