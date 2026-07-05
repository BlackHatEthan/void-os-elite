#ifndef SHELL_H
#define SHELL_H

#include "../../include/void.h"

/* Initialize shell context */
shell_context_t *shell_init(void);

/* Cleanup shell context */
void shell_cleanup(shell_context_t *ctx);

/* Main shell loop */
void shell_loop(shell_context_t *ctx);

/* Read input line */
char *read_input(void);

#endif /* SHELL_H */

