#include "filesystem_ext.h"
#include "filesystem.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../utils/hash_native.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <libgen.h>
#include <limits.h>
#include <sys/statvfs.h>

/* Basic File Operations */
int cmd_touch(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: touch <file> [file2...]\n" COLOR_RESET);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "a");
        if (fp == NULL) {
            printf(COLOR_GREY "Cannot touch: %s\n" COLOR_RESET, argv[i]);
            continue;
        }
        fclose(fp);
        
        /* Update mtime */
        struct timespec times[2];
        times[0].tv_sec = time(NULL);
        times[0].tv_nsec = 0;
        times[1] = times[0];
        utimensat(AT_FDCWD, argv[i], times, 0);
    }
    
    return 0;
}

int cmd_mkdir_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: mkdir <dir> [dir2...]\n" COLOR_RESET);
        return 1;
    }
    
    bool parents = false;
    mode_t mode = 0755;
    int start = 1;
    
    if (argc > 1 && strcmp(argv[1], "-p") == 0) {
        parents = true;
        start = 2;
    }
    
    for (int i = start; i < argc; i++) {
        if (parents) {
            /* Create parent directories */
            char *path = string_dup(argv[i]);
            char *p = path;
            
            if (*p == '/') p++;
            
            while (*p != '\0') {
                if (*p == '/') {
                    *p = '\0';
                    mkdir(path, mode);
                    *p = '/';
                }
                p++;
            }
            mkdir(path, mode);
            free(path);
        } else {
            if (mkdir(argv[i], mode) != 0 && errno != EEXIST) {
                printf(COLOR_GREY "Cannot create directory: %s\n" COLOR_RESET, argv[i]);
            }
        }
    }
    
    return 0;
}

int cmd_rmdir_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: rmdir <dir> [dir2...]\n" COLOR_RESET);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        if (rmdir(argv[i]) != 0) {
            printf(COLOR_GREY "Cannot remove directory: %s\n" COLOR_RESET, argv[i]);
        }
    }
    
    return 0;
}

int cmd_rm(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: rm <file> [file2...]\n" COLOR_RESET);
        return 1;
    }
    
    bool recursive = false;
    bool force = false;
    int start = 1;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "-R") == 0) {
            recursive = true;
            start = i + 1;
        } else if (strcmp(argv[i], "-f") == 0) {
            force = true;
            if (start == i) start = i + 1;
        } else if (strcmp(argv[i], "-rf") == 0 || strcmp(argv[i], "-fr") == 0) {
            recursive = true;
            force = true;
            start = i + 1;
        }
    }
    
    for (int i = start; i < argc; i++) {
        struct stat st;
        if (stat(argv[i], &st) != 0) {
            if (!force) {
                printf(COLOR_GREY "Cannot remove: %s\n" COLOR_RESET, argv[i]);
            }
            continue;
        }
        
        if (S_ISDIR(st.st_mode)) {
            if (recursive) {
                /* Recursive remove */
                DIR *d = opendir(argv[i]);
                if (d != NULL) {
                    struct dirent *entry;
                    while ((entry = readdir(d)) != NULL) {
                        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                            continue;
                        }
                        char full_path[PATH_MAX];
                        snprintf(full_path, sizeof(full_path), "%s/%s", argv[i], entry->d_name);
                        char *rm_argv[] = {"rm", "-rf", full_path, NULL};
                        cmd_rm(3, rm_argv, ctx);
                    }
                    closedir(d);
                }
                rmdir(argv[i]);
            } else {
                printf(COLOR_GREY "Cannot remove directory (use -r): %s\n" COLOR_RESET, argv[i]);
            }
        } else {
            if (unlink(argv[i]) != 0 && !force) {
                printf(COLOR_GREY "Cannot remove: %s\n" COLOR_RESET, argv[i]);
            }
        }
    }
    
    return 0;
}

/* Native file copy */
static int copy_file(const char *src, const char *dst) {
    FILE *src_fp = fopen(src, "rb");
    if (src_fp == NULL) {
        return -1;
    }
    
    FILE *dst_fp = fopen(dst, "wb");
    if (dst_fp == NULL) {
        fclose(src_fp);
        return -1;
    }
    
    char buffer[8192];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), src_fp)) > 0) {
        if (fwrite(buffer, 1, n, dst_fp) != n) {
            fclose(src_fp);
            fclose(dst_fp);
            unlink(dst);
            return -1;
        }
    }
    
    fclose(src_fp);
    fclose(dst_fp);
    
    /* Copy permissions */
    struct stat st;
    if (stat(src, &st) == 0) {
        chmod(dst, st.st_mode);
    }
    
    return 0;
}

int cmd_cp(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: cp <src> <dst> [src2...]\n" COLOR_RESET);
        return 1;
    }
    
    bool recursive = false;
    int start = 1;
    
    if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "-R") == 0) {
        recursive = true;
        start = 2;
    }
    
    if (argc - start < 2) {
        printf(COLOR_GREY "Usage: cp [-r] <src> <dst> [src2...]\n" COLOR_RESET);
        return 1;
    }
    
    const char *dst = argv[argc - 1];
    struct stat dst_st;
    bool dst_is_dir = (stat(dst, &dst_st) == 0 && S_ISDIR(dst_st.st_mode));
    
    for (int i = start; i < argc - 1; i++) {
        struct stat src_st;
        if (stat(argv[i], &src_st) != 0) {
            printf(COLOR_GREY "Cannot access: %s\n" COLOR_RESET, argv[i]);
            continue;
        }
        
        char target[PATH_MAX];
        if (dst_is_dir) {
            snprintf(target, sizeof(target), "%s/%s", dst, basename((char*)argv[i]));
        } else {
            strncpy(target, dst, sizeof(target) - 1);
            target[sizeof(target) - 1] = '\0';
        }
        
        if (S_ISDIR(src_st.st_mode)) {
            if (recursive) {
                /* Recursive copy */
                mkdir(target, src_st.st_mode);
                DIR *d = opendir(argv[i]);
                if (d != NULL) {
                    struct dirent *entry;
                    while ((entry = readdir(d)) != NULL) {
                        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                            continue;
                        }
                        char src_path[PATH_MAX];
                        char dst_path[PATH_MAX];
                        snprintf(src_path, sizeof(src_path), "%s/%s", argv[i], entry->d_name);
                        snprintf(dst_path, sizeof(dst_path), "%s/%s", target, entry->d_name);
                        char *cp_argv[] = {"cp", "-r", src_path, dst_path, NULL};
                        cmd_cp(4, cp_argv, ctx);
                    }
                    closedir(d);
                }
            } else {
                printf(COLOR_GREY "Cannot copy directory (use -r): %s\n" COLOR_RESET, argv[i]);
            }
        } else {
            if (copy_file(argv[i], target) != 0) {
                printf(COLOR_GREY "Cannot copy: %s\n" COLOR_RESET, argv[i]);
            }
        }
    }
    
    return 0;
}

int cmd_mv(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: mv <src> <dst> [src2...]\n" COLOR_RESET);
        return 1;
    }
    
    const char *dst = argv[argc - 1];
    struct stat dst_st;
    bool dst_is_dir = (stat(dst, &dst_st) == 0 && S_ISDIR(dst_st.st_mode));
    
    for (int i = 1; i < argc - 1; i++) {
        char target[PATH_MAX];
        if (dst_is_dir) {
            snprintf(target, sizeof(target), "%s/%s", dst, basename((char*)argv[i]));
        } else {
            strncpy(target, dst, sizeof(target) - 1);
            target[sizeof(target) - 1] = '\0';
        }
        
        if (rename(argv[i], target) != 0) {
            printf(COLOR_GREY "Cannot move: %s\n" COLOR_RESET, argv[i]);
        }
    }
    
    return 0;
}

int cmd_ln(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: ln [-s] <target> <link>\n" COLOR_RESET);
        return 1;
    }
    
    bool symbolic = false;
    int target_idx = 1;
    
    if (strcmp(argv[1], "-s") == 0) {
        symbolic = true;
        target_idx = 2;
    }
    
    if (argc - target_idx < 2) {
        printf(COLOR_GREY "Usage: ln [-s] <target> <link>\n" COLOR_RESET);
        return 1;
    }
    
    const char *target = argv[target_idx];
    const char *link = argv[target_idx + 1];
    
    if (symbolic) {
        if (symlink(target, link) != 0) {
            printf(COLOR_GREY "Cannot create symlink: %s\n" COLOR_RESET, link);
            return 1;
        }
    } else {
        if (link(target, link) != 0) {
            printf(COLOR_GREY "Cannot create hard link: %s\n" COLOR_RESET, link);
            return 1;
        }
    }
    
    return 0;
}

int cmd_symlink_ext(int argc, char **argv, shell_context_t *ctx) {
    return cmd_ln(argc, argv, ctx);
}

int cmd_readlink_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: readlink <link>\n" COLOR_RESET);
        return 1;
    }
    
    char target[PATH_MAX];
    ssize_t len = readlink(argv[1], target, sizeof(target) - 1);
    if (len == -1) {
        printf(COLOR_GREY "Cannot read link: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    target[len] = '\0';
    printf(COLOR_WHITE "%s\n" COLOR_RESET, target);
    return 0;
}

int cmd_stat_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: stat <file>\n" COLOR_RESET);
        return 1;
    }
    
    struct stat st;
    if (stat(argv[1], &st) != 0) {
        printf(COLOR_GREY "Cannot stat: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    printf(COLOR_WHITE "File: " COLOR_RESET "%s\n", argv[1]);
    printf(COLOR_WHITE "Size: " COLOR_RESET "%ld bytes\n", (long)st.st_size);
    printf(COLOR_WHITE "Blocks: " COLOR_RESET "%ld\n", (long)st.st_blocks);
    printf(COLOR_WHITE "IO Block: " COLOR_RESET "%ld\n", (long)st.st_blksize);
    printf(COLOR_WHITE "Device: " COLOR_RESET "%lu/%lu\n", 
           (unsigned long)major(st.st_dev), (unsigned long)minor(st.st_dev));
    printf(COLOR_WHITE "Inode: " COLOR_RESET "%lu\n", (unsigned long)st.st_ino);
    printf(COLOR_WHITE "Links: " COLOR_RESET "%lu\n", (unsigned long)st.st_nlink);
    printf(COLOR_WHITE "Access: " COLOR_RESET "(%04o/%s)\n", 
           st.st_mode & 0777, ctime(&st.st_atime));
    printf(COLOR_WHITE "Modify: " COLOR_RESET "%s", ctime(&st.st_mtime));
    printf(COLOR_WHITE "Change: " COLOR_RESET "%s", ctime(&st.st_ctime));
    
    return 0;
}

int cmd_file_type(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: file <file>\n" COLOR_RESET);
        return 1;
    }
    
    struct stat st;
    if (stat(argv[1], &st) != 0) {
        printf(COLOR_GREY "Cannot access: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    const char *type;
    if (S_ISREG(st.st_mode)) {
        /* Check file signature */
        FILE *fp = fopen(argv[1], "r");
        if (fp != NULL) {
            unsigned char magic[4];
            if (fread(magic, 1, 4, fp) == 4) {
                if (magic[0] == 0x7f && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F') {
                    type = "ELF executable";
                } else if (magic[0] == '#' && magic[1] == '!') {
                    type = "script";
                } else if (magic[0] == 0x89 && magic[1] == 'P' && magic[2] == 'N' && magic[3] == 'G') {
                    type = "PNG image";
                } else {
                    type = "regular file";
                }
            } else {
                type = "regular file";
            }
            fclose(fp);
        } else {
            type = "regular file";
        }
    } else if (S_ISDIR(st.st_mode)) {
        type = "directory";
    } else if (S_ISLNK(st.st_mode)) {
        type = "symbolic link";
    } else if (S_ISCHR(st.st_mode)) {
        type = "character device";
    } else if (S_ISBLK(st.st_mode)) {
        type = "block device";
    } else if (S_ISFIFO(st.st_mode)) {
        type = "FIFO";
    } else if (S_ISSOCK(st.st_mode)) {
        type = "socket";
    } else {
        type = "unknown";
    }
    
    printf(COLOR_WHITE "%s: %s\n" COLOR_RESET, argv[1], type);
    return 0;
}

int cmd_mime(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: mime <file>\n" COLOR_RESET);
        return 1;
    }
    
    const char *ext = strrchr(argv[1], '.');
    if (ext == NULL) {
        printf(COLOR_GREY "application/octet-stream\n" COLOR_RESET);
        return 0;
    }
    
    ext++; /* Skip '.' */
    
    if (strcmp(ext, "txt") == 0 || strcmp(ext, "md") == 0) {
        printf(COLOR_WHITE "text/plain\n" COLOR_RESET);
    } else if (strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0) {
        printf(COLOR_WHITE "text/html\n" COLOR_RESET);
    } else if (strcmp(ext, "css") == 0) {
        printf(COLOR_WHITE "text/css\n" COLOR_RESET);
    } else if (strcmp(ext, "js") == 0) {
        printf(COLOR_WHITE "application/javascript\n" COLOR_RESET);
    } else if (strcmp(ext, "json") == 0) {
        printf(COLOR_WHITE "application/json\n" COLOR_RESET);
    } else if (strcmp(ext, "png") == 0) {
        printf(COLOR_WHITE "image/png\n" COLOR_RESET);
    } else if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) {
        printf(COLOR_WHITE "image/jpeg\n" COLOR_RESET);
    } else if (strcmp(ext, "gif") == 0) {
        printf(COLOR_WHITE "image/gif\n" COLOR_RESET);
    } else {
        printf(COLOR_GREY "application/octet-stream\n" COLOR_RESET);
    }
    
    return 0;
}

int cmd_size(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: size <file>\n" COLOR_RESET);
        return 1;
    }
    
    struct stat st;
    if (stat(argv[1], &st) != 0) {
        printf(COLOR_GREY "Cannot access: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    printf(COLOR_WHITE "%ld bytes\n" COLOR_RESET, (long)st.st_size);
    return 0;
}

/* Recursive directory size calculation */
static off_t calculate_dir_size(const char *path) {
    off_t total = 0;
    DIR *d = opendir(path);
    if (d == NULL) {
        return 0;
    }
    
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                total += calculate_dir_size(full_path);
            } else {
                total += st.st_size;
            }
        }
    }
    
    closedir(d);
    return total;
}

int cmd_du(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *dir = (argc > 1) ? argv[1] : ".";
    bool human = false;
    
    if (argc > 1 && strcmp(argv[1], "-h") == 0) {
        human = true;
        dir = (argc > 2) ? argv[2] : ".";
    }
    
    off_t size = calculate_dir_size(dir);
    
    if (human) {
        if (size < 1024) {
            printf(COLOR_WHITE "%.0f\t%s\n" COLOR_RESET, (double)size, dir);
        } else if (size < 1024 * 1024) {
            printf(COLOR_WHITE "%.1fK\t%s\n" COLOR_RESET, size / 1024.0, dir);
        } else if (size < 1024 * 1024 * 1024) {
            printf(COLOR_WHITE "%.1fM\t%s\n" COLOR_RESET, size / (1024.0 * 1024.0), dir);
        } else {
            printf(COLOR_WHITE "%.1fG\t%s\n" COLOR_RESET, size / (1024.0 * 1024.0 * 1024.0), dir);
        }
    } else {
        printf(COLOR_WHITE "%ld\t%s\n" COLOR_RESET, (long)(size / 1024), dir);
    }
    
    return 0;
}

int cmd_df(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *path = (argc > 1) ? argv[1] : ".";
    
    struct statvfs vfs;
    if (statvfs(path, &vfs) != 0) {
        printf(COLOR_GREY "Cannot get filesystem info: %s\n" COLOR_RESET, path);
        return 1;
    }
    
    unsigned long long total = vfs.f_blocks * vfs.f_frsize;
    unsigned long long available = vfs.f_bavail * vfs.f_frsize;
    unsigned long long used = total - (vfs.f_blocks - vfs.f_bfree) * vfs.f_frsize;
    int percent = (total > 0) ? ((used * 100) / total) : 0;
    
    printf(COLOR_GREY "%-15s %10s %10s %10s %5s %s\n" COLOR_RESET,
           "Filesystem", "1K-blocks", "Used", "Available", "Use%", "Mounted on");
    printf(COLOR_WHITE "%-15s %10llu %10llu %10llu %4d%% %s\n" COLOR_RESET,
           "filesystem", total / 1024, used / 1024, available / 1024, percent, path);
    
    return 0;
}

/* Recursive file find */
static void find_files(const char *path, const char *pattern, bool name_only) {
    DIR *d = opendir(path);
    if (d == NULL) {
        return;
    }
    
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) != 0) {
            continue;
        }
        
        bool matches = false;
        if (name_only) {
            matches = (strstr(entry->d_name, pattern) != NULL);
        } else {
            matches = (strstr(full_path, pattern) != NULL);
        }
        
        if (matches) {
            printf(COLOR_WHITE "%s\n" COLOR_RESET, full_path);
        }
        
        if (S_ISDIR(st.st_mode)) {
            find_files(full_path, pattern, name_only);
        }
    }
    
    closedir(d);
}

int cmd_find_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: find <path> [pattern]\n" COLOR_RESET);
        return 1;
    }
    
    const char *path = argv[1];
    const char *pattern = (argc > 2) ? argv[2] : "";
    bool name_only = true;
    
    find_files(path, pattern, name_only);
    return 0;
}

int cmd_locate(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: locate <pattern>\n" COLOR_RESET);
        printf(COLOR_GREY "Note: Requires updatedb to build index first.\n" COLOR_RESET);
        return 1;
    }
    
    /* Simple locate - search from root */
    printf(COLOR_GREY "Locate database not yet implemented.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'find / <pattern>' for now.\n" COLOR_RESET);
    return 1;
}

int cmd_updatedb(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Locate database implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_where(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: where <file>\n" COLOR_RESET);
        return 1;
    }
    
    const char *file = argv[1];
    const char *path = getenv("PATH");
    
    if (path == NULL) {
        return 1;
    }
    
    char *path_copy = string_dup(path);
    char *dir = strtok(path_copy, ":");
    
    while (dir != NULL) {
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, file);
        
        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode)) {
            printf(COLOR_WHITE "%s\n" COLOR_RESET, full_path);
        }
        
        dir = strtok(NULL, ":");
    }
    
    free(path_copy);
    return 0;
}

int cmd_realpath_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: realpath <path>\n" COLOR_RESET);
        return 1;
    }
    
    char resolved[PATH_MAX];
    if (realpath(argv[1], resolved) != NULL) {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, resolved);
        return 0;
    }
    
    printf(COLOR_GREY "Cannot resolve path: %s\n" COLOR_RESET, argv[1]);
    return 1;
}

int cmd_basename_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: basename <path>\n" COLOR_RESET);
        return 1;
    }
    
    char *path_copy = string_dup(argv[1]);
    char *base = basename(path_copy);
    printf(COLOR_WHITE "%s\n" COLOR_RESET, base);
    free(path_copy);
    return 0;
}

int cmd_dirname_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: dirname <path>\n" COLOR_RESET);
        return 1;
    }
    
    char *path_copy = string_dup(argv[1]);
    char *dir = dirname(path_copy);
    printf(COLOR_WHITE "%s\n" COLOR_RESET, dir);
    free(path_copy);
    return 0;
}

int cmd_pathjoin(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: pathjoin <path1> <path2>\n" COLOR_RESET);
        return 1;
    }
    
    char result[PATH_MAX];
    snprintf(result, sizeof(result), "%s/%s", argv[1], argv[2]);
    
    /* Normalize */
    char resolved[PATH_MAX];
    if (realpath(result, resolved) != NULL) {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, resolved);
    } else {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, result);
    }
    
    return 0;
}

int cmd_pathsplit(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: pathsplit <path>\n" COLOR_RESET);
        return 1;
    }
    
    char *path_copy = string_dup(argv[1]);
    char *dir = dirname(path_copy);
    char *base = basename((char*)argv[1]);
    
    printf(COLOR_WHITE "Directory: %s\n" COLOR_RESET, dir);
    printf(COLOR_WHITE "Basename: %s\n" COLOR_RESET, base);
    
    free(path_copy);
    return 0;
}

int cmd_pathnorm(int argc, char **argv, shell_context_t *ctx) {
    return cmd_realpath_ext(argc, argv, ctx);
}

int cmd_pathabs(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: pathabs <path>\n" COLOR_RESET);
        return 1;
    }
    
    if (argv[1][0] == '/') {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, argv[1]);
        return 0;
    }
    
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        return 1;
    }
    
    char abs[PATH_MAX];
    snprintf(abs, sizeof(abs), "%s/%s", cwd, argv[1]);
    
    char resolved[PATH_MAX];
    if (realpath(abs, resolved) != NULL) {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, resolved);
    } else {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, abs);
    }
    
    return 0;
}

int cmd_pathrel(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: pathrel <path> <base>\n" COLOR_RESET);
        return 1;
    }
    
    char path_resolved[PATH_MAX];
    char base_resolved[PATH_MAX];
    
    if (realpath(argv[1], path_resolved) == NULL) {
        strncpy(path_resolved, argv[1], sizeof(path_resolved) - 1);
    }
    if (realpath(argv[2], base_resolved) == NULL) {
        strncpy(base_resolved, argv[2], sizeof(base_resolved) - 1);
    }
    
    /* Simple relative path calculation */
    if (strncmp(path_resolved, base_resolved, strlen(base_resolved)) == 0) {
        const char *rel = path_resolved + strlen(base_resolved);
        if (*rel == '/') rel++;
        printf(COLOR_WHITE "%s\n" COLOR_RESET, rel);
    } else {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, path_resolved);
    }
    
    return 0;
}

/* File Attributes */
int cmd_chmod_ext(int argc, char **argv, shell_context_t *ctx) {
    return cmd_seal(argc, argv, ctx); /* seal is chmod */
}

int cmd_chown_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: chown <owner>[:group] <file>\n" COLOR_RESET);
        return 1;
    }
    
    char *owner_spec = string_dup(argv[1]);
    char *owner = owner_spec;
    char *group = NULL;
    
    char *colon = strchr(owner_spec, ':');
    if (colon != NULL) {
        *colon = '\0';
        group = colon + 1;
    }
    
    uid_t uid = -1;
    gid_t gid = -1;
    
    if (owner != NULL && strlen(owner) > 0) {
        struct passwd *pw = getpwnam(owner);
        if (pw != NULL) {
            uid = pw->pw_uid;
        } else {
            uid = (uid_t)atoi(owner);
        }
    }
    
    if (group != NULL && strlen(group) > 0) {
        struct group *gr = getgrnam(group);
        if (gr != NULL) {
            gid = gr->gr_gid;
        } else {
            gid = (gid_t)atoi(group);
        }
    }
    
    for (int i = 2; i < argc; i++) {
        if (chown(argv[i], uid, gid) != 0) {
            printf(COLOR_GREY "Cannot chown: %s\n" COLOR_RESET, argv[i]);
        }
    }
    
    free(owner_spec);
    return 0;
}

int cmd_chgrp_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: chgrp <group> <file>\n" COLOR_RESET);
        return 1;
    }
    
    struct group *gr = getgrnam(argv[1]);
    gid_t gid = (gr != NULL) ? gr->gr_gid : (gid_t)atoi(argv[1]);
    
    for (int i = 2; i < argc; i++) {
        if (chown(argv[i], -1, gid) != 0) {
            printf(COLOR_GREY "Cannot chgrp: %s\n" COLOR_RESET, argv[i]);
        }
    }
    
    return 0;
}

int cmd_umask_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        mode_t current = umask(0);
        umask(current);
        printf(COLOR_WHITE "%04o\n" COLOR_RESET, current);
        return 0;
    }
    
    mode_t mask = (mode_t)strtol(argv[1], NULL, 8);
    umask(mask);
    return 0;
}

int cmd_getfacl(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: getfacl <file>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "ACL support requires extended attributes.\n" COLOR_RESET);
    printf(COLOR_GREY "Basic permissions:\n" COLOR_RESET);
    return cmd_veil(argc, argv, ctx);
}

int cmd_setfacl(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "ACL support requires extended attributes.\n" COLOR_RESET);
    return 1;
}

int cmd_lsattr(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: lsattr <file>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Extended attributes not yet implemented.\n" COLOR_RESET);
    return 1;
}

int cmd_chattr(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Extended attributes not yet implemented.\n" COLOR_RESET);
    return 1;
}

int cmd_getfattr(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Extended attributes not yet implemented.\n" COLOR_RESET);
    return 1;
}

int cmd_setfattr(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Extended attributes not yet implemented.\n" COLOR_RESET);
    return 1;
}

/* File Comparison */
int cmd_diff(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: diff <file1> <file2>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp1 = fopen(argv[1], "r");
    FILE *fp2 = fopen(argv[2], "r");
    
    if (fp1 == NULL || fp2 == NULL) {
        if (fp1) fclose(fp1);
        if (fp2) fclose(fp2);
        printf(COLOR_GREY "Cannot open files for comparison.\n" COLOR_RESET);
        return 1;
    }
    
    char line1[4096], line2[4096];
    int line_num = 1;
    bool file1_eof = false, file2_eof = false;
    
    while (!file1_eof || !file2_eof) {
        bool got_line1 = (fgets(line1, sizeof(line1), fp1) != NULL);
        bool got_line2 = (fgets(line2, sizeof(line2), fp2) != NULL);
        
        if (!got_line1) file1_eof = true;
        if (!got_line2) file2_eof = true;
        
        if (file1_eof && file2_eof) break;
        
        if (file1_eof) {
            printf(COLOR_BLUE ">%d: %s" COLOR_RESET, line_num, line2);
            line_num++;
        } else if (file2_eof) {
            printf(COLOR_RED "<%d: %s" COLOR_RESET, line_num, line1);
            line_num++;
        } else if (strcmp(line1, line2) != 0) {
            printf(COLOR_RED "<%d: %s" COLOR_RESET, line_num, line1);
            printf(COLOR_BLUE ">%d: %s" COLOR_RESET, line_num, line2);
            line_num++;
        } else {
            line_num++;
        }
    }
    
    fclose(fp1);
    fclose(fp2);
    return 0;
}

int cmd_diff3(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "3-way diff not yet implemented.\n" COLOR_RESET);
    return 1;
}

int cmd_sdiff(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: sdiff <file1> <file2>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp1 = fopen(argv[1], "r");
    FILE *fp2 = fopen(argv[2], "r");
    
    if (fp1 == NULL || fp2 == NULL) {
        if (fp1) fclose(fp1);
        if (fp2) fclose(fp2);
        return 1;
    }
    
    char line1[4096], line2[4096];
    while (fgets(line1, sizeof(line1), fp1) != NULL || fgets(line2, sizeof(line2), fp2) != NULL) {
        if (fgets(line1, sizeof(line1), fp1) == NULL) {
            strcpy(line1, "");
        }
        if (fgets(line2, sizeof(line2), fp2) == NULL) {
            strcpy(line2, "");
        }
        
        char *nl1 = strchr(line1, '\n');
        if (nl1) *nl1 = '\0';
        char *nl2 = strchr(line2, '\n');
        if (nl2) *nl2 = '\0';
        
        if (strcmp(line1, line2) == 0) {
            printf(COLOR_WHITE "%-40s | %s\n" COLOR_RESET, line1, line2);
        } else {
            printf(COLOR_RED "%-40s < " COLOR_RESET, line1);
            printf(COLOR_BLUE "%s\n" COLOR_RESET, line2);
        }
    }
    
    fclose(fp1);
    fclose(fp2);
    return 0;
}

int cmd_cmp(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: cmp <file1> <file2>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp1 = fopen(argv[1], "rb");
    FILE *fp2 = fopen(argv[2], "rb");
    
    if (fp1 == NULL || fp2 == NULL) {
        if (fp1) fclose(fp1);
        if (fp2) fclose(fp2);
        return 1;
    }
    
    int byte1, byte2;
    long pos = 0;
    while ((byte1 = fgetc(fp1)) != EOF && (byte2 = fgetc(fp2)) != EOF) {
        if (byte1 != byte2) {
            printf(COLOR_GREY "%s %s differ: byte %ld\n" COLOR_RESET, argv[1], argv[2], pos);
            fclose(fp1);
            fclose(fp2);
            return 1;
        }
        pos++;
    }
    
    if (byte1 != byte2) {
        printf(COLOR_GREY "%s %s differ: byte %ld\n" COLOR_RESET, argv[1], argv[2], pos);
        fclose(fp1);
        fclose(fp2);
        return 1;
    }
    
    fclose(fp1);
    fclose(fp2);
    return 0;
}

int cmd_comm(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: comm <file1> <file2>\n" COLOR_RESET);
        return 1;
    }
    
    /* Requires sorted files - simple implementation */
    printf(COLOR_GREY "Comm requires sorted input files.\n" COLOR_RESET);
    return cmd_diff(argc, argv, ctx);
}

int cmd_patch(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Patch implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_diffstat(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Diffstat implementation pending.\n" COLOR_RESET);
    return 1;
}

/* File Compression & Archives - Placeholders */
int cmd_gzip(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Gzip compression requires zlib library.\n" COLOR_RESET);
    printf(COLOR_GREY "Native compression implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_gunzip(int argc, char **argv, shell_context_t *ctx) {
    return cmd_gzip(argc, argv, ctx);
}

int cmd_bzip2(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Bzip2 compression requires bzip2 library.\n" COLOR_RESET);
    return 1;
}

int cmd_bunzip2(int argc, char **argv, shell_context_t *ctx) {
    return cmd_bzip2(argc, argv, ctx);
}

int cmd_xz(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "XZ compression requires xz library.\n" COLOR_RESET);
    return 1;
}

int cmd_unxz(int argc, char **argv, shell_context_t *ctx) {
    return cmd_xz(argc, argv, ctx);
}

int cmd_zip(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Zip archive creation requires zlib library.\n" COLOR_RESET);
    return 1;
}

int cmd_unzip(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Zip extraction requires zlib library.\n" COLOR_RESET);
    return 1;
}

int cmd_tar(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Tar archive operations require tar format implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_zcat(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Zcat requires gzip decompression.\n" COLOR_RESET);
    return 1;
}

int cmd_zless(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Zless requires gzip decompression.\n" COLOR_RESET);
    return 1;
}

int cmd_zmore(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Zmore requires gzip decompression.\n" COLOR_RESET);
    return 1;
}

int cmd_zcmp(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Zcmp requires gzip decompression.\n" COLOR_RESET);
    return 1;
}

int cmd_zdiff(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Zdiff requires gzip decompression.\n" COLOR_RESET);
    return 1;
}

/* File Integrity & Checksums */
static char *read_file_for_hash(const char *file, size_t *len) {
    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        return NULL;
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *data = malloc(size);
    if (data == NULL) {
        fclose(fp);
        return NULL;
    }
    
    fread(data, 1, size, fp);
    fclose(fp);
    
    *len = size;
    return data;
}

int cmd_md5sum(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: md5sum <file>\n" COLOR_RESET);
        return 1;
    }
    
    size_t len;
    unsigned char *data = (unsigned char *)read_file_for_hash(argv[1], &len);
    if (data == NULL) {
        printf(COLOR_GREY "Cannot read file: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    char *hash = hash_md5_native(data, len);
    if (hash != NULL) {
        printf(COLOR_WHITE "%s  %s\n" COLOR_RESET, hash, argv[1]);
        free(hash);
    }
    free(data);
    
    return 0;
}

int cmd_sha1sum(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "SHA1 implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_sha256sum(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: sha256sum <file>\n" COLOR_RESET);
        return 1;
    }
    
    size_t len;
    unsigned char *data = (unsigned char *)read_file_for_hash(argv[1], &len);
    if (data == NULL) {
        printf(COLOR_GREY "Cannot read file: %s\n" COLOR_RESET, argv[1]);
        return 1;
    }
    
    char *hash = hash_sha256_native(data, len);
    if (hash != NULL) {
        printf(COLOR_WHITE "%s  %s\n" COLOR_RESET, hash, argv[1]);
        free(hash);
    }
    free(data);
    
    return 0;
}

int cmd_sha512sum(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "SHA512 implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_cksum(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "CRC checksum implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_sum(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "BSD sum implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_b2sum(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "BLAKE2 implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_verify(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: verify <file> <checksum>\n" COLOR_RESET);
        return 1;
    }
    
    /* Verify MD5 or SHA256 */
    size_t len;
    unsigned char *data = (unsigned char *)read_file_for_hash(argv[1], &len);
    if (data == NULL) {
        return 1;
    }
    
    char *hash = hash_sha256_native(data, len);
    if (hash != NULL) {
        if (strcmp(hash, argv[2]) == 0) {
            printf(COLOR_GREEN "OK: %s\n" COLOR_RESET, argv[1]);
            free(hash);
            free(data);
            return 0;
        } else {
            printf(COLOR_RED "FAILED: %s\n" COLOR_RESET, argv[1]);
            free(hash);
            free(data);
            return 1;
        }
    }
    
    free(data);
    return 1;
}

/* File Monitoring */
int cmd_inotifywait(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: inotifywait <dir>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Inotify requires Linux kernel support.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'watch' command for basic monitoring.\n" COLOR_RESET);
    return 1;
}

int cmd_inotifywatch(int argc, char **argv, shell_context_t *ctx) {
    return cmd_inotifywait(argc, argv, ctx);
}

int cmd_fswatch(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: fswatch <dir>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Fswatch requires platform-specific implementation.\n" COLOR_RESET);
    return cmd_watch(argc, argv, ctx);
}

int cmd_entr(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: entr <command> <file>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Entr requires file watching implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_watch_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: watch <command>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Watch command execution pending.\n" COLOR_RESET);
    return 1;
}

/* File Conversion */
int cmd_dos2unix(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: dos2unix <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        return 1;
    }
    
    /* Read and convert */
    char *content = NULL;
    size_t size = 0;
    char buffer[4096];
    
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t len = strlen(buffer);
        content = realloc(content, size + len);
        memcpy(content + size, buffer, len);
        size += len;
    }
    fclose(fp);
    
    /* Remove \r */
    char *new_content = malloc(size);
    size_t new_size = 0;
    for (size_t i = 0; i < size; i++) {
        if (content[i] != '\r') {
            new_content[new_size++] = content[i];
        }
    }
    free(content);
    
    /* Write back */
    fp = fopen(argv[1], "wb");
    if (fp != NULL) {
        fwrite(new_content, 1, new_size, fp);
        fclose(fp);
    }
    free(new_content);
    
    return 0;
}

int cmd_unix2dos(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: unix2dos <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        return 1;
    }
    
    char *content = NULL;
    size_t size = 0;
    char buffer[4096];
    
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t len = strlen(buffer);
        content = realloc(content, size + len + 1);
        memcpy(content + size, buffer, len);
        size += len;
        if (len > 0 && buffer[len - 1] == '\n') {
            content[size - 1] = '\r';
            content[size++] = '\n';
        }
    }
    fclose(fp);
    
    fp = fopen(argv[1], "wb");
    if (fp != NULL) {
        fwrite(content, 1, size, fp);
        fclose(fp);
    }
    free(content);
    
    return 0;
}

int cmd_mac2unix(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Mac line ending conversion pending.\n" COLOR_RESET);
    return 1;
}

int cmd_unix2mac(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Unix to Mac line ending conversion pending.\n" COLOR_RESET);
    return 1;
}

int cmd_iconv_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Iconv requires character encoding library.\n" COLOR_RESET);
    return 1;
}

int cmd_uuencode(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "UUencode implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_uudecode(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "UUdecode implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_hexdump(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: hexdump <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        return 1;
    }
    
    unsigned char buffer[16];
    long offset = 0;
    size_t n;
    
    while ((n = fread(buffer, 1, 16, fp)) > 0) {
        printf(COLOR_GREY "%08lx  " COLOR_RESET, offset);
        
        for (size_t i = 0; i < 16; i++) {
            if (i < n) {
                printf(COLOR_WHITE "%02x " COLOR_RESET, buffer[i]);
            } else {
                printf("   ");
            }
            if (i == 7) printf(" ");
        }
        
        printf(" |");
        for (size_t i = 0; i < n; i++) {
            printf(COLOR_GREY "%c" COLOR_RESET, isprint(buffer[i]) ? buffer[i] : '.');
        }
        printf("|\n");
        
        offset += n;
    }
    
    fclose(fp);
    return 0;
}

int cmd_od(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: od <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        return 1;
    }
    
    unsigned char buffer[16];
    long offset = 0;
    size_t n;
    
    while ((n = fread(buffer, 1, 16, fp)) > 0) {
        printf(COLOR_GREY "%07lo  " COLOR_RESET, offset);
        
        for (size_t i = 0; i < n; i++) {
            printf(COLOR_WHITE "%03o " COLOR_RESET, buffer[i]);
        }
        printf("\n");
        
        offset += n;
    }
    
    fclose(fp);
    return 0;
}

int cmd_xxd(int argc, char **argv, shell_context_t *ctx) {
    return cmd_hexdump(argc, argv, ctx);
}

int cmd_strings_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: strings <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        return 1;
    }
    
    unsigned char buffer[4096];
    size_t n;
    int in_string = 0;
    int string_len = 0;
    
    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        for (size_t i = 0; i < n; i++) {
            if (isprint(buffer[i])) {
                if (!in_string) {
                    in_string = 1;
                    string_len = 0;
                }
                if (string_len < 80) {
                    printf("%c", buffer[i]);
                    string_len++;
                }
            } else {
                if (in_string && string_len >= 4) {
                    printf("\n");
                }
                in_string = 0;
                string_len = 0;
            }
        }
    }
    
    if (in_string && string_len >= 4) {
        printf("\n");
    }
    
    fclose(fp);
    return 0;
}

int cmd_bin2hex(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: bin2hex <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        return 1;
    }
    
    unsigned char byte;
    while (fread(&byte, 1, 1, fp) == 1) {
        printf(COLOR_WHITE "%02x" COLOR_RESET, byte);
    }
    printf("\n");
    
    fclose(fp);
    return 0;
}

int cmd_hex2bin(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: hex2bin <hexfile> <binfile>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *in = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "wb");
    
    if (in == NULL || out == NULL) {
        if (in) fclose(in);
        if (out) fclose(out);
        return 1;
    }
    
    char hex[3] = {0};
    int byte;
    
    while (fread(hex, 1, 2, in) == 2) {
        byte = strtol(hex, NULL, 16);
        fwrite(&byte, 1, 1, out);
    }
    
    fclose(in);
    fclose(out);
    return 0;
}

/* Special File Operations */
int cmd_shred(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: shred <file>\n" COLOR_RESET);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r+b");
    if (fp == NULL) {
        return 1;
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    /* Overwrite with random data 3 times */
    unsigned char *random_data = malloc(size);
    if (random_data == NULL) {
        fclose(fp);
        return 1;
    }
    
    for (int pass = 0; pass < 3; pass++) {
        for (long i = 0; i < size; i++) {
            random_data[i] = rand() % 256;
        }
        fseek(fp, 0, SEEK_SET);
        fwrite(random_data, 1, size, fp);
        fflush(fp);
    }
    
    free(random_data);
    fclose(fp);
    
    /* Delete file */
    unlink(argv[1]);
    
    printf(COLOR_GREEN "Shredded: %s\n" COLOR_RESET, argv[1]);
    return 0;
}

int cmd_wipe(int argc, char **argv, shell_context_t *ctx) {
    return cmd_shred(argc, argv, ctx);
}

int cmd_srm(int argc, char **argv, shell_context_t *ctx) {
    return cmd_shred(argc, argv, ctx);
}

int cmd_secure_delete(int argc, char **argv, shell_context_t *ctx) {
    return cmd_shred(argc, argv, ctx);
}

int cmd_truncate_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: truncate <file> <size>\n" COLOR_RESET);
        return 1;
    }
    
    long size = atol(argv[2]);
    FILE *fp = fopen(argv[1], "r+b");
    if (fp == NULL) {
        fp = fopen(argv[1], "wb");
        if (fp == NULL) {
            return 1;
        }
    }
    
    ftruncate(fileno(fp), size);
    fclose(fp);
    
    return 0;
}

int cmd_fallocate(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: fallocate <file> <size>\n" COLOR_RESET);
        return 1;
    }
    
    long size = atol(argv[2]);
    int fd = open(argv[1], O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
        return 1;
    }
    
    /* Allocate space by writing zeros */
    lseek(fd, size - 1, SEEK_SET);
    write(fd, "", 1);
    close(fd);
    
    return 0;
}

int cmd_mktemp(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *template = (argc > 1) ? argv[1] : "/tmp/void.XXXXXX";
    
    char *tmp = string_dup(template);
    int fd = mkstemp(tmp);
    if (fd == -1) {
        free(tmp);
        return 1;
    }
    
    close(fd);
    printf(COLOR_WHITE "%s\n" COLOR_RESET, tmp);
    free(tmp);
    
    return 0;
}

int cmd_tempfile(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    char template[] = "/tmp/void.XXXXXX";
    int fd = mkstemp(template);
    if (fd == -1) {
        return 1;
    }
    
    close(fd);
    printf(COLOR_WHITE "%s\n" COLOR_RESET, template);
    return 0;
}

