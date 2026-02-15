#ifndef NETWORK_H
#define NETWORK_H

#include "../../include/void.h"

/* Networking command handlers */
int cmd_intercept(int argc, char **argv, shell_context_t *ctx);
int cmd_ping(int argc, char **argv, shell_context_t *ctx);
int cmd_ports(int argc, char **argv, shell_context_t *ctx);
int cmd_scan(int argc, char **argv, shell_context_t *ctx);
int cmd_serve(int argc, char **argv, shell_context_t *ctx);
int cmd_tunnel_info(int argc, char **argv, shell_context_t *ctx);
int cmd_throb(int argc, char **argv, shell_context_t *ctx);
int cmd_rift(int argc, char **argv, shell_context_t *ctx);
int cmd_wormhole(int argc, char **argv, shell_context_t *ctx);
int cmd_beacon(int argc, char **argv, shell_context_t *ctx);
int cmd_trace_route(int argc, char **argv, shell_context_t *ctx);
int cmd_ghost_ping(int argc, char **argv, shell_context_t *ctx);

#endif /* NETWORK_H */

