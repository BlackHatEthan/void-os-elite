#include "plugin.h"
#include "../plugins/loader.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int cmd_forge(int argc, char **argv, shell_context_t *ctx) {
    if (argc < 2) {
        printf(COLOR_GREY "Usage: forge install <path>\n" COLOR_RESET);
        printf(COLOR_GREY "       forge list\n" COLOR_RESET);
        return 1;
    }
    
    const char *action = argv[1];
    
    if (strcmp(action, "install") == 0) {
        if (argc < 3) {
            printf(COLOR_GREY "Usage: forge install <path>\n" COLOR_RESET);
            return 1;
        }
        
        const char *plugin_path = argv[2];
        
        /* Check if file exists */
        struct stat st;
        if (stat(plugin_path, &st) != 0) {
            LOG_ERROR("Plugin file not found: %s", plugin_path);
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
            return 1;
        }
        
        /* Get plugin directory */
        char *plugin_dir = expand_path("~/.void/plugins");
        if (plugin_dir == NULL) {
            plugin_dir = string_dup("/tmp/.void/plugins");
        }
        
        /* Create directory if needed - native implementation */
        struct stat st_dir;
        if (stat(plugin_dir, &st_dir) != 0) {
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
        
        /* Copy plugin to plugin directory - native file copy */
        char *filename = strrchr(plugin_path, '/');
        if (filename == NULL) {
            filename = (char*)plugin_path;
        } else {
            filename++; /* Skip '/' */
        }
        
        char dest_path[PATH_MAX];
        snprintf(dest_path, sizeof(dest_path), "%s/%s", plugin_dir, filename);
        
        /* Native file copy */
        FILE *src = fopen(plugin_path, "rb");
        if (src == NULL) {
            LOG_ERROR("Failed to open source plugin: %s", plugin_path);
            printf(COLOR_GREY "Failed to open plugin file.\n" COLOR_RESET);
            free(plugin_dir);
            return 1;
        }
        
        FILE *dst = fopen(dest_path, "wb");
        if (dst == NULL) {
            fclose(src);
            LOG_ERROR("Failed to create destination: %s", dest_path);
            printf(COLOR_GREY "Failed to create plugin destination.\n" COLOR_RESET);
            free(plugin_dir);
            return 1;
        }
        
        char buffer[4096];
        size_t n;
        while ((n = fread(buffer, 1, sizeof(buffer), src)) > 0) {
            if (fwrite(buffer, 1, n, dst) != n) {
                fclose(src);
                fclose(dst);
                unlink(dest_path);
                LOG_ERROR("Failed to write plugin data");
                printf(COLOR_GREY "Failed to copy plugin.\n" COLOR_RESET);
                free(plugin_dir);
                return 1;
            }
        }
        
        fclose(src);
        fclose(dst);
        
        /* Set executable permission */
        chmod(dest_path, 0755);
        
        /* Try to load the plugin */
        if (load_plugin_from_path(ctx, dest_path) == 0) {
            printf(COLOR_GREEN "Plugin installed and loaded: %s\n" COLOR_RESET, filename);
        } else {
            printf(COLOR_GREY "Plugin copied but failed to load. Restart Void OS to load.\n" COLOR_RESET);
        }
        
        free(plugin_dir);
        return 0;
        
    } else if (strcmp(action, "list") == 0) {
        char *plugin_dir = expand_path("~/.void/plugins");
        if (plugin_dir == NULL) {
            plugin_dir = string_dup("/tmp/.void/plugins");
        }
        
        DIR *dir = opendir(plugin_dir);
        if (dir == NULL) {
            printf(COLOR_GREY "No plugins directory found.\n" COLOR_RESET);
            free(plugin_dir);
            return 0;
        }
        
        printf(COLOR_WHITE "Installed Plugins:\n" COLOR_RESET);
        printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
        
        struct dirent *entry;
        int count = 0;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.') {
                continue;
            }
            
            #ifdef __APPLE__
            if (strstr(entry->d_name, ".dylib") != NULL || 
                strstr(entry->d_name, ".so") != NULL) {
            #else
            if (strstr(entry->d_name, ".so") != NULL) {
            #endif
                printf(COLOR_WHITE "  %s\n" COLOR_RESET, entry->d_name);
                count++;
            }
        }
        
        closedir(dir);
        free(plugin_dir);
        
        if (count == 0) {
            printf(COLOR_GREY "No plugins installed.\n" COLOR_RESET);
        }
        
        return 0;
    } else {
        printf(COLOR_GREY "Unknown action: %s\n" COLOR_RESET, action);
        printf(COLOR_GREY "Usage: forge install <path> | forge list\n" COLOR_RESET);
        return 1;
    }
}

