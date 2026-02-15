#ifndef GIT_NATIVE_H
#define GIT_NATIVE_H

#include <stdbool.h>

/* Native Git operations - read .git files directly */

/* Check if directory is a git repository */
bool git_is_repo(const char *path);

/* Get current branch name by reading .git/HEAD */
char *git_get_branch(const char *path);

/* Check if repository has uncommitted changes */
bool git_is_dirty(const char *path);

/* Get remote URL from .git/config */
char *git_get_remote_url(const char *path, const char *remote_name);

/* Read .git/config file */
char *git_read_config(const char *path);

/* Parse git status by reading .git/index and comparing with working tree */
int git_get_status(const char *path, char **output);

#endif /* GIT_NATIVE_H */

