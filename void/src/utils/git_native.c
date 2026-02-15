#include "git_native.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

bool git_is_repo(const char *path) {
    if (path == NULL) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            return false;
        }
        path = cwd;
    }
    
    char git_path[PATH_MAX];
    snprintf(git_path, sizeof(git_path), "%s/.git", path);
    
    struct stat st;
    return (stat(git_path, &st) == 0 && S_ISDIR(st.st_mode));
}

char *git_get_branch(const char *path) {
    if (path == NULL) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            return NULL;
        }
        path = cwd;
    }
    
    if (!git_is_repo(path)) {
        return NULL;
    }
    
    /* Read .git/HEAD file */
    char head_path[PATH_MAX];
    snprintf(head_path, sizeof(head_path), "%s/.git/HEAD", path);
    
    FILE *fp = fopen(head_path, "r");
    if (fp == NULL) {
        return NULL;
    }
    
    char line[256];
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    
    /* Remove newline */
    char *nl = strchr(line, '\n');
    if (nl) *nl = '\0';
    
    /* Parse ref: refs/heads/branch-name */
    if (strncmp(line, "ref: refs/heads/", 16) == 0) {
        return string_dup(line + 16);
    }
    
    /* If it's a detached HEAD (SHA), return NULL or "HEAD" */
    if (strlen(line) == 40) { /* SHA-1 hash */
        return string_dup("HEAD");
    }
    
    return NULL;
}

bool git_is_dirty(const char *path) {
    if (path == NULL) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            return false;
        }
        path = cwd;
    }
    
    if (!git_is_repo(path)) {
        return false;
    }
    
    /* Check for uncommitted changes by comparing .git/index with working tree */
    /* This is a simplified check - full implementation would need to parse index */
    
    /* Check if .git/index exists and is newer than some files */
    char index_path[PATH_MAX];
    snprintf(index_path, sizeof(index_path), "%s/.git/index", path);
    
    struct stat index_st;
    if (stat(index_path, &index_st) != 0) {
        return true; /* No index means dirty */
    }
    
    /* Simple heuristic: check if common files are newer than index */
    const char *check_files[] = {
        "README.md",
        "Makefile",
        "*.c",
        "*.h",
        NULL
    };
    
    /* For now, do a simple directory scan */
    DIR *d = opendir(path);
    if (d == NULL) {
        return false;
    }
    
    struct dirent *entry;
    bool dirty = false;
    
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        
        char file_path[PATH_MAX];
        snprintf(file_path, sizeof(file_path), "%s/%s", path, entry->d_name);
        
        struct stat file_st;
        if (stat(file_path, &file_st) == 0 && S_ISREG(file_st.st_mode)) {
            /* If file is newer than index, likely dirty */
            if (file_st.st_mtime > index_st.st_mtime) {
                dirty = true;
                break;
            }
        }
    }
    
    closedir(d);
    return dirty;
}

char *git_get_remote_url(const char *path, const char *remote_name) {
    if (path == NULL) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            return NULL;
        }
        path = cwd;
    }
    
    if (remote_name == NULL) {
        remote_name = "origin";
    }
    
    if (!git_is_repo(path)) {
        return NULL;
    }
    
    char config_path[PATH_MAX];
    snprintf(config_path, sizeof(config_path), "%s/.git/config", path);
    
    FILE *fp = fopen(config_path, "r");
    if (fp == NULL) {
        return NULL;
    }
    
    char line[512];
    bool in_remote_section = false;
    char *url = NULL;
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        /* Remove newline */
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        
        /* Check for remote section */
        char remote_section[256];
        snprintf(remote_section, sizeof(remote_section), "[remote \"%s\"]", remote_name);
        
        if (strcmp(line, remote_section) == 0) {
            in_remote_section = true;
            continue;
        }
        
        if (line[0] == '[') {
            in_remote_section = false;
            continue;
        }
        
        if (in_remote_section && strncmp(line, "url = ", 6) == 0) {
            url = string_dup(line + 6);
            break;
        }
    }
    
    fclose(fp);
    return url;
}

char *git_read_config(const char *path) {
    if (path == NULL) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            return NULL;
        }
        path = cwd;
    }
    
    if (!git_is_repo(path)) {
        return NULL;
    }
    
    char config_path[PATH_MAX];
    snprintf(config_path, sizeof(config_path), "%s/.git/config", path);
    
    FILE *fp = fopen(config_path, "r");
    if (fp == NULL) {
        return NULL;
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *content = malloc(size + 1);
    if (content == NULL) {
        fclose(fp);
        return NULL;
    }
    
    fread(content, 1, size, fp);
    content[size] = '\0';
    
    fclose(fp);
    return content;
}

int git_get_status(const char *path, char **output) {
    if (path == NULL) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            return -1;
        }
        path = cwd;
    }
    
    if (!git_is_repo(path)) {
        return -1;
    }
    
    /* Simplified status - just show branch and dirty state */
    char *branch = git_get_branch(path);
    bool dirty = git_is_dirty(path);
    
    size_t len = 256;
    *output = malloc(len);
    if (*output == NULL) {
        free(branch);
        return -1;
    }
    
    if (branch != NULL) {
        snprintf(*output, len, "On branch %s%s\n", branch, dirty ? " (modified)" : "");
        free(branch);
    } else {
        snprintf(*output, len, "Not on any branch%s\n", dirty ? " (modified)" : "");
    }
    
    return 0;
}

