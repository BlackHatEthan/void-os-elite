#include "security.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

int cmd_startup_check(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_WHITE "Void OS Security Awareness Check\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    int warnings = 0;
    
    /* Check Git global config email - read from ~/.gitconfig directly */
    struct passwd *pw_git = getpwuid(getuid());
    if (pw_git != NULL) {
        char gitconfig_path[PATH_MAX];
        snprintf(gitconfig_path, sizeof(gitconfig_path), "%s/.gitconfig", pw_git->pw_dir);
        
        FILE *gitconfig = fopen(gitconfig_path, "r");
        if (gitconfig != NULL) {
            char line[512];
            char email[256] = {0};
            bool in_user_section = false;
            
            while (fgets(line, sizeof(line), gitconfig) != NULL) {
                if (strstr(line, "[user]") != NULL) {
                    in_user_section = true;
                    continue;
                }
                if (line[0] == '[') {
                    in_user_section = false;
                    continue;
                }
                if (in_user_section && strncmp(line, "email = ", 8) == 0) {
                    sscanf(line, "email = %255[^\n]", email);
                    break;
                }
            }
            fclose(gitconfig);
            
            if (strlen(email) > 0) {
                printf(COLOR_WHITE "Git Email: " COLOR_BLUE "%s\n" COLOR_RESET, email);
                
                if (strstr(email, "example") != NULL || 
                    strstr(email, "test") != NULL ||
                    strstr(email, "noreply") != NULL) {
                    printf(COLOR_GREY "  ⚠ Warning: Generic email detected\n" COLOR_RESET);
                    warnings++;
                }
            } else {
                printf(COLOR_WHITE "Git Email: " COLOR_GREY "Not configured\n" COLOR_RESET);
                printf(COLOR_GREY "  ⚠ Warning: Git email not set\n" COLOR_RESET);
                warnings++;
            }
        } else {
            printf(COLOR_WHITE "Git Email: " COLOR_GREY "Not configured\n" COLOR_RESET);
            printf(COLOR_GREY "  ⚠ Warning: Git email not set\n" COLOR_RESET);
            warnings++;
        }
    }
    
    /* Check SSH keys */
    struct passwd *pw = getpwuid(getuid());
    if (pw != NULL) {
        char ssh_dir[PATH_MAX];
        snprintf(ssh_dir, sizeof(ssh_dir), "%s/.ssh", pw->pw_dir);
        
        struct stat st;
        if (stat(ssh_dir, &st) == 0 && S_ISDIR(st.st_mode)) {
            printf(COLOR_WHITE "SSH Keys: " COLOR_GREEN "✓ Directory exists\n" COLOR_RESET);
            
            /* Check for public keys */
            DIR *dir = opendir(ssh_dir);
            if (dir != NULL) {
                struct dirent *entry;
                int key_count = 0;
                while ((entry = readdir(dir)) != NULL) {
                    if (strstr(entry->d_name, ".pub") != NULL) {
                        key_count++;
                    }
                }
                closedir(dir);
                
                if (key_count > 0) {
                    printf(COLOR_WHITE "  Public Keys: " COLOR_BLUE "%d found\n" COLOR_RESET, key_count);
                } else {
                    printf(COLOR_GREY "  ⚠ Warning: No public keys found\n" COLOR_RESET);
                    warnings++;
                }
            }
        } else {
            printf(COLOR_WHITE "SSH Keys: " COLOR_GREY "✗ No .ssh directory\n" COLOR_RESET);
            printf(COLOR_GREY "  ⚠ Warning: SSH keys not configured\n" COLOR_RESET);
            warnings++;
        }
    }
    
    /* Check for .env files in current directory */
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char env_path[PATH_MAX];
        snprintf(env_path, sizeof(env_path), "%s/.env", cwd);
        
        struct stat st;
        if (stat(env_path, &st) == 0 && S_ISREG(st.st_mode)) {
            printf(COLOR_WHITE ".env File: " COLOR_GREY "⚠ Found in current directory\n" COLOR_RESET);
            printf(COLOR_GREY "  Warning: .env files may contain secrets\n" COLOR_RESET);
            printf(COLOR_GREY "  Ensure .env is in .gitignore\n" COLOR_RESET);
            warnings++;
        } else {
            printf(COLOR_WHITE ".env File: " COLOR_GREEN "✓ Not found\n" COLOR_RESET);
        }
    }
    
    /* Check for exposed secrets risk */
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    if (warnings > 0) {
        printf(COLOR_GREY "Total Warnings: " COLOR_WHITE "%d\n" COLOR_RESET, warnings);
        printf(COLOR_GREY "Review the above items for security concerns.\n" COLOR_RESET);
    } else {
        printf(COLOR_GREEN "✓ No security warnings detected.\n" COLOR_RESET);
    }
    
    return 0;
}

int cmd_perm_audit(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *dir = (argc > 1) ? argv[1] : ".";
    
    printf(COLOR_WHITE "Permission Audit: %s\n" COLOR_RESET, dir);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    int issues = 0;
    
    DIR *d = opendir(dir);
    if (d == NULL) {
        LOG_ERROR("Failed to open directory '%s': %s", dir, strerror(errno));
        if (errno == ENOENT) {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
        } else {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PERMISSION_DENIED);
        }
        return 1;
    }
    
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue; /* Skip hidden files */
        }
        
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) != 0) {
            continue;
        }
        
        mode_t mode = st.st_mode;
        
        /* Check for world-writable files */
        if (S_ISREG(mode) && (mode & S_IWOTH)) {
            printf(COLOR_GREY "⚠ World-writable: " COLOR_WHITE "%s\n" COLOR_RESET, full_path);
            issues++;
        }
        
        /* Check for executables in unexpected locations */
        if (S_ISREG(mode) && (mode & S_IXUSR) && 
            strstr(full_path, "/tmp") == NULL &&
            strstr(full_path, "/bin") == NULL &&
            strstr(full_path, "/usr/bin") == NULL &&
            strstr(full_path, "/usr/local/bin") == NULL) {
            
            /* Check if it's a script or binary */
            FILE *fp = fopen(full_path, "r");
            if (fp != NULL) {
                char first_bytes[4];
                if (fread(first_bytes, 1, 4, fp) == 4) {
                    /* Check for shebang or ELF magic */
                    if (first_bytes[0] == '#' || 
                        (first_bytes[0] == 0x7f && first_bytes[1] == 'E' && 
                         first_bytes[2] == 'L' && first_bytes[3] == 'F')) {
                        printf(COLOR_GREY "⚠ Executable in non-standard location: " COLOR_WHITE "%s\n" COLOR_RESET, full_path);
                        issues++;
                    }
                }
                fclose(fp);
            }
        }
    }
    
    closedir(d);
    
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    if (issues > 0) {
        printf(COLOR_GREY "Total Issues: " COLOR_WHITE "%d\n" COLOR_RESET, issues);
    } else {
        printf(COLOR_GREEN "✓ No permission issues found.\n" COLOR_RESET);
    }
    
    return 0;
}

int cmd_net_watch(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_WHITE "Active Network Connections\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    #ifdef __linux__
    /* Read from /proc/net/tcp and /proc/net/udp */
    FILE *tcp = fopen("/proc/net/tcp", "r");
    if (tcp != NULL) {
        char line[256];
        int count = 0;
        printf(COLOR_WHITE "TCP Connections:\n" COLOR_RESET);
        while (fgets(line, sizeof(line), tcp) != NULL && count < 20) {
            if (count == 0) {
                count++;
                continue; /* Skip header */
            }
            
            unsigned int local_addr, local_port, remote_addr, remote_port;
            char state[16];
            if (sscanf(line, "%*d: %x:%x %x:%x %s", 
                      &local_addr, &local_port, &remote_addr, &remote_port, state) == 5) {
                struct in_addr local, remote;
                local.s_addr = htonl(local_addr);
                remote.s_addr = htonl(remote_addr);
                
                printf(COLOR_BLUE "%s:%d" COLOR_RESET " -> " COLOR_BLUE "%s:%d" COLOR_RESET " [%s]\n",
                       inet_ntoa(local), ntohs(local_port),
                       inet_ntoa(remote), ntohs(remote_port), state);
                count++;
            }
        }
        fclose(tcp);
    }
    
    FILE *udp = fopen("/proc/net/udp", "r");
    if (udp != NULL) {
        char line[256];
        int count = 0;
        printf(COLOR_WHITE "\nUDP Connections:\n" COLOR_RESET);
        while (fgets(line, sizeof(line), udp) != NULL && count < 10) {
            if (count == 0) {
                count++;
                continue; /* Skip header */
            }
            
            unsigned int local_addr, local_port;
            if (sscanf(line, "%*d: %x:%x", &local_addr, &local_port) == 2) {
                struct in_addr local;
                local.s_addr = htonl(local_addr);
                
                printf(COLOR_BLUE "%s:%d" COLOR_RESET " (UDP)\n",
                       inet_ntoa(local), ntohs(local_port));
                count++;
            }
        }
        fclose(udp);
    }
    #else
    /* macOS/BSD - try /proc/net/tcp if available, otherwise show message */
    FILE *tcp = fopen("/proc/net/tcp", "r");
    if (tcp != NULL) {
        char line[256];
        int count = 0;
        printf(COLOR_WHITE "TCP Connections:\n" COLOR_RESET);
        while (fgets(line, sizeof(line), tcp) != NULL && count < 20) {
            if (count == 0) {
                count++;
                continue;
            }
            unsigned int local_addr, local_port, remote_addr, remote_port;
            char state[16];
            if (sscanf(line, "%*d: %x:%x %x:%x %s", 
                      &local_addr, &local_port, &remote_addr, &remote_port, state) == 5) {
                struct in_addr local, remote;
                local.s_addr = htonl(local_addr);
                remote.s_addr = htonl(remote_addr);
                printf(COLOR_BLUE "%s:%d" COLOR_RESET " -> " COLOR_BLUE "%s:%d" COLOR_RESET " [%s]\n",
                       inet_ntoa(local), ntohs(local_port),
                       inet_ntoa(remote), ntohs(remote_port), state);
                count++;
            }
        }
        fclose(tcp);
    } else {
        printf(COLOR_GREY "Network connection monitoring requires /proc/net/tcp (Linux)\n" COLOR_RESET);
        printf(COLOR_GREY "Native implementation pending for macOS.\n" COLOR_RESET);
    }
    #endif
    
    return 0;
}

