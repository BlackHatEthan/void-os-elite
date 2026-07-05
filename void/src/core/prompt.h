#ifndef PROMPT_H
#define PROMPT_H

#include "../../include/void.h"

/* Generate and display the Void OS prompt */
void display_prompt(shell_context_t *ctx);

/* Get current git branch (if in git repo) */
char *get_git_branch(void);

/* Check if git repo is dirty (has uncommitted changes) */
bool is_git_dirty(void);

/* Abbreviate path for display */
char *abbreviate_path(const char *path, int max_len);

#endif /* PROMPT_H */

