#include "project.h"
#include "../commands/git.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

bool file_exists(const char *path) {
    if (path == NULL) {
        return false;
    }
    
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    
    return S_ISREG(st.st_mode);
}

bool dir_exists(const char *path) {
    if (path == NULL) {
        return false;
    }
    
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    
    return S_ISDIR(st.st_mode);
}

project_info_t *detect_project(const char *path) {
    if (path == NULL) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            return NULL;
        }
        path = cwd;
    }
    
    project_info_t *info = calloc(1, sizeof(project_info_t));
    if (info == NULL) {
        return NULL;
    }
    
    info->type = PROJECT_NONE;
    info->virtual_env = NULL;
    info->git_branch = NULL;
    
    char file_path[PATH_MAX];
    
    /* Check for .git */
    snprintf(file_path, sizeof(file_path), "%s/.git", path);
    info->has_git = dir_exists(file_path);
    
    /* Check for Python files */
    snprintf(file_path, sizeof(file_path), "%s/requirements.txt", path);
    info->has_requirements = file_exists(file_path);
    
    snprintf(file_path, sizeof(file_path), "%s/pyproject.toml", path);
    info->has_pyproject = file_exists(file_path);
    
    /* Check for Node.js */
    snprintf(file_path, sizeof(file_path), "%s/package.json", path);
    info->has_package_json = file_exists(file_path);
    
    /* Check for Docker */
    snprintf(file_path, sizeof(file_path), "%s/Dockerfile", path);
    info->has_dockerfile = file_exists(file_path);
    
    /* Check for .env */
    snprintf(file_path, sizeof(file_path), "%s/.env", path);
    info->has_env_file = file_exists(file_path);
    
    /* Determine project type */
    int type_count = 0;
    if (info->has_git) type_count++;
    if (info->has_requirements || info->has_pyproject) type_count++;
    if (info->has_package_json) type_count++;
    if (info->has_dockerfile) type_count++;
    
    if (type_count > 1) {
        info->type = PROJECT_MIXED;
    } else if (info->has_requirements || info->has_pyproject) {
        info->type = PROJECT_PYTHON;
    } else if (info->has_package_json) {
        info->type = PROJECT_NODE;
    } else if (info->has_dockerfile) {
        info->type = PROJECT_DOCKER;
    } else if (info->has_git) {
        info->type = PROJECT_GIT;
    }
    
    /* Get git branch if git repo */
    if (info->has_git) {
        info->git_branch = get_git_branch_name(path);
        info->git_dirty = is_git_dirty_state(path);
    }
    
    /* Check for Python virtual environment */
    snprintf(file_path, sizeof(file_path), "%s/venv", path);
    if (dir_exists(file_path)) {
        info->virtual_env = string_dup("venv");
    } else {
        snprintf(file_path, sizeof(file_path), "%s/.venv", path);
        if (dir_exists(file_path)) {
            info->virtual_env = string_dup(".venv");
        } else {
            snprintf(file_path, sizeof(file_path), "%s/env", path);
            if (dir_exists(file_path)) {
                info->virtual_env = string_dup("env");
            }
        }
    }
    
    return info;
}

void free_project_info(project_info_t *info) {
    if (info == NULL) {
        return;
    }
    
    free(info->virtual_env);
    free(info->git_branch);
    free(info);
}

