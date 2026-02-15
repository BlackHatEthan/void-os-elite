#include "context.h"
#include "../utils/project.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>

static const char *project_type_name(project_type_t type) {
    switch (type) {
        case PROJECT_NONE: return "None";
        case PROJECT_GIT: return "Git Repository";
        case PROJECT_PYTHON: return "Python Project";
        case PROJECT_NODE: return "Node.js Project";
        case PROJECT_DOCKER: return "Docker Project";
        case PROJECT_MIXED: return "Mixed Project";
        default: return "Unknown";
    }
}

int cmd_context(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf(COLOR_GREY "Failed to get current directory.\n" COLOR_RESET);
        return 1;
    }
    
    project_info_t *info = detect_project(cwd);
    if (info == NULL) {
        printf(COLOR_GREY "Failed to detect project.\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_WHITE "Project Context: " COLOR_BLUE "%s\n" COLOR_RESET, cwd);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    printf(COLOR_WHITE "Type: " COLOR_RESET "%s\n", project_type_name(info->type));
    
    if (info->has_git) {
        printf(COLOR_WHITE "Git: " COLOR_RESET COLOR_GREEN "✓" COLOR_RESET);
        if (info->git_branch != NULL) {
            printf(COLOR_WHITE " Branch: " COLOR_BLUE "%s" COLOR_RESET, info->git_branch);
            if (info->git_dirty) {
                printf(COLOR_WHITE "*" COLOR_RESET);
            }
        }
        printf("\n");
    } else {
        printf(COLOR_WHITE "Git: " COLOR_RESET COLOR_GREY "✗\n" COLOR_RESET);
    }
    
    if (info->type == PROJECT_PYTHON || info->type == PROJECT_MIXED) {
        printf(COLOR_WHITE "Python: " COLOR_RESET COLOR_GREEN "✓" COLOR_RESET);
        if (info->has_requirements) {
            printf(COLOR_GREY " (requirements.txt)" COLOR_RESET);
        }
        if (info->has_pyproject) {
            printf(COLOR_GREY " (pyproject.toml)" COLOR_RESET);
        }
        printf("\n");
        
        if (info->virtual_env != NULL) {
            printf(COLOR_WHITE "Virtual Env: " COLOR_BLUE "%s\n" COLOR_RESET, info->virtual_env);
        } else {
            printf(COLOR_WHITE "Virtual Env: " COLOR_GREY "None\n" COLOR_RESET);
        }
    }
    
    if (info->type == PROJECT_NODE || info->type == PROJECT_MIXED) {
        if (info->has_package_json) {
            printf(COLOR_WHITE "Node.js: " COLOR_RESET COLOR_GREEN "✓" COLOR_GREY " (package.json)\n" COLOR_RESET);
        }
    }
    
    if (info->has_dockerfile) {
        printf(COLOR_WHITE "Docker: " COLOR_RESET COLOR_GREEN "✓" COLOR_GREY " (Dockerfile)\n" COLOR_RESET);
    }
    
    if (info->has_env_file) {
        printf(COLOR_WHITE ".env: " COLOR_RESET COLOR_GREY "⚠ Found\n" COLOR_RESET);
    }
    
    /* Dependency hints */
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_WHITE "Dependencies:\n" COLOR_RESET);
    
    if (info->has_requirements) {
        printf(COLOR_GREY "  - Python: Check requirements.txt\n" COLOR_RESET);
    }
    if (info->has_pyproject) {
        printf(COLOR_GREY "  - Python: Check pyproject.toml\n" COLOR_RESET);
    }
    if (info->has_package_json) {
        printf(COLOR_GREY "  - Node.js: Run 'npm install'\n" COLOR_RESET);
    }
    if (info->has_dockerfile) {
        printf(COLOR_GREY "  - Docker: Run 'docker build .'\n" COLOR_RESET);
    }
    
    free_project_info(info);
    return 0;
}

