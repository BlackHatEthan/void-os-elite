#ifndef SYSTEM_H
#define SYSTEM_H

#include "../../include/void.h"

/* System monitoring command handlers */
int cmd_entropy(int argc, char **argv, shell_context_t *ctx);
int cmd_pulse_graph(int argc, char **argv, shell_context_t *ctx);
int cmd_synapse(int argc, char **argv, shell_context_t *ctx);
int cmd_organelle(int argc, char **argv, shell_context_t *ctx);
int cmd_vitals(int argc, char **argv, shell_context_t *ctx);
int cmd_tempest(int argc, char **argv, shell_context_t *ctx);
int cmd_neural_map(int argc, char **argv, shell_context_t *ctx);
int cmd_overclock_view(int argc, char **argv, shell_context_t *ctx);
int cmd_voltage(int argc, char **argv, shell_context_t *ctx);

#endif /* SYSTEM_H */

