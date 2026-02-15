#ifndef GIT_H
#define GIT_H

#include "../../include/void.h"

/* Git command handlers */
int cmd_tether(int argc, char **argv, shell_context_t *ctx);
int cmd_anchor(int argc, char **argv, shell_context_t *ctx);
int cmd_sync(int argc, char **argv, shell_context_t *ctx);
int cmd_transmit(int argc, char **argv, shell_context_t *ctx);
int cmd_commit(int argc, char **argv, shell_context_t *ctx);
int cmd_status(int argc, char **argv, shell_context_t *ctx);

/* Git utility functions */
bool is_git_repo(const char *path);
char *get_git_branch_name(const char *path);
bool is_git_dirty_state(const char *path);
int execute_git_command(int argc, char **argv);

#endif /* GIT_H */

