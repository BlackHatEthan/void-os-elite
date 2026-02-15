#ifndef PROCESS_H
#define PROCESS_H

#include "../../include/void.h"

/* Process management command handlers */
int cmd_spawn(int argc, char **argv, shell_context_t *ctx);
int cmd_jobs(int argc, char **argv, shell_context_t *ctx);
int cmd_terminate(int argc, char **argv, shell_context_t *ctx);
int cmd_foreground(int argc, char **argv, shell_context_t *ctx);
int cmd_ps(int argc, char **argv, shell_context_t *ctx);
int cmd_silence(int argc, char **argv, shell_context_t *ctx);
int cmd_awaken(int argc, char **argv, shell_context_t *ctx);

/* Job management functions */
int add_job(shell_context_t *ctx, pid_t pid, const char *command);
void remove_job(shell_context_t *ctx, pid_t pid);
void update_jobs(shell_context_t *ctx);

#endif /* PROCESS_H */

