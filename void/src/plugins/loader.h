#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include "../../include/void.h"

/* Plugin function types */
typedef void* (*plugin_init_func_t)(void* shell_context);
typedef void (*plugin_register_func_t)(void* shell_context, const char* name, command_func_t func);
typedef void (*plugin_cleanup_func_t)(void* shell_context);

/* Plugin structure */
typedef struct {
    void *handle;
    char *path;
    plugin_init_func_t init;
    plugin_register_func_t register_cmd;
    plugin_cleanup_func_t cleanup;
    bool loaded;
} plugin_t;

/* Plugin loader functions */
int load_plugins(shell_context_t *ctx);
void unload_plugins(shell_context_t *ctx);
int load_plugin_from_path(shell_context_t *ctx, const char *path);

#endif /* PLUGIN_LOADER_H */

