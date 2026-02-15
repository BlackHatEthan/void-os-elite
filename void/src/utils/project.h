#ifndef PROJECT_H
#define PROJECT_H

#include <stdbool.h>

/* Project type enumeration */
typedef enum {
    PROJECT_NONE,
    PROJECT_GIT,
    PROJECT_PYTHON,
    PROJECT_NODE,
    PROJECT_DOCKER,
    PROJECT_MIXED
} project_type_t;

/* Project information structure */
typedef struct {
    project_type_t type;
    bool has_git;
    bool has_requirements;
    bool has_pyproject;
    bool has_package_json;
    bool has_dockerfile;
    bool has_env_file;
    char *virtual_env;
    char *git_branch;
    bool git_dirty;
} project_info_t;

/* Detect project type in current directory */
project_info_t *detect_project(const char *path);

/* Free project info */
void free_project_info(project_info_t *info);

/* Check if file exists */
bool file_exists(const char *path);

/* Check if directory exists */
bool dir_exists(const char *path);

#endif /* PROJECT_H */

