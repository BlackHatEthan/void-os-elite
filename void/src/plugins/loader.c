#include "loader.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>

#define MAX_PLUGINS 32
static plugin_t plugins[MAX_PLUGINS];
static int plugin_count = 0;

int load_plugin_from_path(shell_context_t *ctx, const char *path) {
    if (path == NULL || ctx == NULL) {
        return -1;
    }
    
    if (plugin_count >= MAX_PLUGINS) {
        LOG_ERROR("Maximum plugin count reached");
        return -1;
    }
    
    void *handle = dlopen(path, RTLD_LAZY);
    if (handle == NULL) {
        LOG_ERROR("Failed to load plugin '%s': %s", path, dlerror());
        return -1;
    }
    
    /* Look for plugin functions */
    plugin_init_func_t init = (plugin_init_func_t)dlsym(handle, "void_plugin_init");
    plugin_register_func_t register_cmd = (plugin_register_func_t)dlsym(handle, "void_plugin_register_command");
    plugin_cleanup_func_t cleanup = (plugin_cleanup_func_t)dlsym(handle, "void_plugin_cleanup");
    
    if (init == NULL || register_cmd == NULL) {
        LOG_ERROR("Plugin '%s' missing required functions", path);
        dlclose(handle);
        return -1;
    }
    
    /* Initialize plugin */
    void *plugin_data = init(ctx);
    if (plugin_data == NULL) {
        LOG_WARN("Plugin '%s' init returned NULL", path);
    }
    
    plugins[plugin_count].handle = handle;
    plugins[plugin_count].path = string_dup(path);
    plugins[plugin_count].init = init;
    plugins[plugin_count].register_cmd = register_cmd;
    plugins[plugin_count].cleanup = cleanup;
    plugins[plugin_count].loaded = true;
    
    plugin_count++;
    
    LOG_INFO("Loaded plugin: %s", path);
    return 0;
}

int load_plugins(shell_context_t *ctx) {
    if (ctx == NULL) {
        return -1;
    }
    
    /* Get plugin directory */
    char *plugin_dir = expand_path("~/.void/plugins");
    if (plugin_dir == NULL) {
        plugin_dir = string_dup("/tmp/.void/plugins");
    }
    
    /* Create directory if it doesn't exist - native implementation */
    struct stat st;
    if (stat(plugin_dir, &st) != 0) {
        /* Create directory recursively */
        char *dir_path = string_dup(plugin_dir);
        char *p = dir_path;
        
        /* Skip leading / */
        if (*p == '/') p++;
        
        while (*p != '\0') {
            if (*p == '/') {
                *p = '\0';
                mkdir(dir_path, 0755);
                *p = '/';
            }
            p++;
        }
        mkdir(dir_path, 0755);
        free(dir_path);
    }
    
    DIR *dir = opendir(plugin_dir);
    if (dir == NULL) {
        LOG_WARN("Plugin directory '%s' not accessible", plugin_dir);
        free(plugin_dir);
        return 0; /* Not an error, just no plugins */
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        
        /* Check for shared library extension */
        #ifdef __APPLE__
        if (strstr(entry->d_name, ".dylib") == NULL && 
            strstr(entry->d_name, ".so") == NULL) {
            continue;
        }
        #else
        if (strstr(entry->d_name, ".so") == NULL) {
            continue;
        }
        #endif
        
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", plugin_dir, entry->d_name);
        
        load_plugin_from_path(ctx, full_path);
    }
    
    closedir(dir);
    free(plugin_dir);
    
    LOG_INFO("Loaded %d plugins", plugin_count);
    return plugin_count;
}

void unload_plugins(shell_context_t *ctx) {
    (void)ctx;
    
    for (int i = 0; i < plugin_count; i++) {
        if (plugins[i].loaded && plugins[i].cleanup != NULL) {
            plugins[i].cleanup(ctx);
        }
        
        if (plugins[i].handle != NULL) {
            dlclose(plugins[i].handle);
        }
        
        free(plugins[i].path);
        plugins[i].loaded = false;
    }
    
    plugin_count = 0;
}

