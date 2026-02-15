#include "kali_tools.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>

/* Kali Top 10 Tools Catalog */
kali_tool_t kali_tools_catalog[] = {
    {
        "metasploit",
        "https://github.com/rapid7/metasploit-framework",
        (const char*[]){"ruby", "postgresql", "bundler", NULL},
        (const char*[]){"msfconsole", "msfvenom", "msfd", "msfrpc", NULL},
        "bundle install",
        false
    },
    {
        "sqlmap",
        "https://github.com/sqlmapproject/sqlmap",
        (const char*[]){"python3", NULL},
        (const char*[]){"sqlmap", NULL},
        NULL,
        false
    },
    {
        "nikto",
        "https://github.com/sullo/nikto",
        (const char*[]){"perl", NULL},
        (const char*[]){"nikto", NULL},
        NULL,
        false
    },
    {
        "nmap",
        "https://github.com/nmap/nmap",
        NULL,
        (const char*[]){"nmap", "ncat", NULL},
        "./configure && make",
        false
    },
    {
        "wpscan",
        "https://github.com/wpscanteam/wpscan",
        (const char*[]){"ruby", NULL},
        (const char*[]){"wpscan", NULL},
        "bundle install",
        false
    },
    {
        "gobuster",
        "https://github.com/OJ/gobuster",
        (const char*[]){"go", NULL},
        (const char*[]){"gobuster", NULL},
        "go build",
        false
    },
    {
        "feroxbuster",
        "https://github.com/epi052/feroxbuster",
        (const char*[]){"rust", "cargo", NULL},
        (const char*[]){"feroxbuster", NULL},
        "cargo build --release",
        false
    },
    {
        "john",
        "https://github.com/openwall/john",
        (const char*[]){"build-essential", NULL},
        (const char*[]){"john", "unshadow", "unafs", NULL},
        "./configure && make",
        false
    },
    {
        "hashcat",
        "https://github.com/hashcat/hashcat",
        (const char*[]){"build-essential", "opencl-headers", NULL},
        (const char*[]){"hashcat", "hashcat64", NULL},
        "make",
        false
    },
    {
        "hydra",
        "https://github.com/vanhauser-thc/thc-hydra",
        (const char*[]){"build-essential", "libssl-dev", "libssh-dev", NULL},
        (const char*[]){"hydra", "pw-inspector", NULL},
        "./configure && make",
        false
    },
    {NULL, NULL, NULL, NULL, NULL, false}
};

/* Check if a tool is installed */
int check_kali_tool_installed(const char *tool_name) {
    if (tool_name == NULL) {
        return 0;
    }
    
    /* Check in system PATH */
    char *path = getenv("PATH");
    if (path != NULL) {
        char path_copy[4096];
        strncpy(path_copy, path, sizeof(path_copy) - 1);
        path_copy[sizeof(path_copy) - 1] = '\0';
        
        char *token = strtok(path_copy, ":");
        while (token != NULL) {
            char tool_path[PATH_MAX];
            snprintf(tool_path, sizeof(tool_path), "%s/%s", token, tool_name);
            
            if (access(tool_path, F_OK) == 0 && access(tool_path, X_OK) == 0) {
                return 1;
            }
            token = strtok(NULL, ":");
        }
    }
    
    /* Check in ~/.void/packages/bin/ */
    char *home = getenv("HOME");
    if (home != NULL) {
        char void_bin[PATH_MAX];
        snprintf(void_bin, sizeof(void_bin), "%s/.void/packages/bin/%s", home, tool_name);
        
        if (access(void_bin, F_OK) == 0 && access(void_bin, X_OK) == 0) {
            return 1;
        }
    }
    
    /* Check for Python/Ruby/Perl scripts */
    char *home_dir = getenv("HOME");
    if (home_dir != NULL) {
        char tool_dir[PATH_MAX];
        snprintf(tool_dir, sizeof(tool_dir), "%s/.void/packages/cache/%s", home_dir, tool_name);
        
        struct stat st;
        if (stat(tool_dir, &st) == 0 && S_ISDIR(st.st_mode)) {
            /* Check for main script */
            char script_path[PATH_MAX];
            
            /* Check for Python script */
            snprintf(script_path, sizeof(script_path), "%s/%s.py", tool_dir, tool_name);
            if (access(script_path, F_OK) == 0) {
                return 1;
            }
            
            /* Check for Ruby script */
            snprintf(script_path, sizeof(script_path), "%s/%s.rb", tool_dir, tool_name);
            if (access(script_path, F_OK) == 0) {
                return 1;
            }
            
            /* Check for Perl script */
            snprintf(script_path, sizeof(script_path), "%s/%s.pl", tool_dir, tool_name);
            if (access(script_path, F_OK) == 0) {
                return 1;
            }
            
            /* Check for executable in bin/ subdirectory */
            snprintf(script_path, sizeof(script_path), "%s/bin/%s", tool_dir, tool_name);
            if (access(script_path, F_OK) == 0 && access(script_path, X_OK) == 0) {
                return 1;
            }
        }
    }
    
    return 0;
}

/* Find tool binary path */
char *find_tool_binary(const char *tool_name) {
    if (tool_name == NULL) {
        return NULL;
    }
    
    static char tool_path[PATH_MAX];
    
    /* Check in system PATH */
    char *path = getenv("PATH");
    if (path != NULL) {
        char path_copy[4096];
        strncpy(path_copy, path, sizeof(path_copy) - 1);
        path_copy[sizeof(path_copy) - 1] = '\0';
        
        char *token = strtok(path_copy, ":");
        while (token != NULL) {
            snprintf(tool_path, sizeof(tool_path), "%s/%s", token, tool_name);
            
            if (access(tool_path, F_OK) == 0 && access(tool_path, X_OK) == 0) {
                return tool_path;
            }
            token = strtok(NULL, ":");
        }
    }
    
    /* Check in ~/.void/packages/bin/ */
    char *home = getenv("HOME");
    if (home != NULL) {
        snprintf(tool_path, sizeof(tool_path), "%s/.void/packages/bin/%s", home, tool_name);
        
        if (access(tool_path, F_OK) == 0 && access(tool_path, X_OK) == 0) {
            return tool_path;
        }
    }
    
    /* Check for scripts in cache */
    char *home_dir = getenv("HOME");
    if (home_dir != NULL) {
        char tool_dir[PATH_MAX];
        snprintf(tool_dir, sizeof(tool_dir), "%s/.void/packages/cache/%s", home_dir, tool_name);
        
        /* Check for Python script */
        snprintf(tool_path, sizeof(tool_path), "%s/%s.py", tool_dir, tool_name);
        if (access(tool_path, F_OK) == 0) {
            return tool_path;
        }
        
        /* Check for Ruby script */
        snprintf(tool_path, sizeof(tool_path), "%s/%s.rb", tool_dir, tool_name);
        if (access(tool_path, F_OK) == 0) {
            return tool_path;
        }
        
        /* Check for Perl script */
        snprintf(tool_path, sizeof(tool_path), "%s/%s.pl", tool_dir, tool_name);
        if (access(tool_path, F_OK) == 0) {
            return tool_path;
        }
        
        /* Check for executable in bin/ */
        snprintf(tool_path, sizeof(tool_path), "%s/bin/%s", tool_dir, tool_name);
        if (access(tool_path, F_OK) == 0 && access(tool_path, X_OK) == 0) {
            return tool_path;
        }
        
        /* Check for main script in root */
        snprintf(tool_path, sizeof(tool_path), "%s/%s", tool_dir, tool_name);
        if (access(tool_path, F_OK) == 0 && access(tool_path, X_OK) == 0) {
            return tool_path;
        }
    }
    
    return NULL;
}

/* Execute Kali tool */
int execute_kali_tool(const char *tool_name, int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    
    if (tool_name == NULL) {
        return 1;
    }
    
    char *tool_path = find_tool_binary(tool_name);
    
    if (tool_path == NULL) {
        printf(COLOR_GREY "Tool '%s' not found.\n" COLOR_RESET, tool_name);
        printf(COLOR_GREY "Install with: void-install-kali %s\n" COLOR_RESET, tool_name);
        return 1;
    }
    
    /* Fork and execute */
    pid_t pid = fork();
    if (pid == 0) {
        /* Child process */
        if (strstr(tool_path, ".py") != NULL) {
            /* Python script */
            char *new_argv[argc + 2];
            new_argv[0] = "python3";
            new_argv[1] = tool_path;
            for (int i = 1; i < argc; i++) {
                new_argv[i + 1] = argv[i];
            }
            new_argv[argc + 1] = NULL;
            execvp("python3", new_argv);
        } else if (strstr(tool_path, ".rb") != NULL) {
            /* Ruby script */
            char *new_argv[argc + 2];
            new_argv[0] = "ruby";
            new_argv[1] = tool_path;
            for (int i = 1; i < argc; i++) {
                new_argv[i + 1] = argv[i];
            }
            new_argv[argc + 1] = NULL;
            execvp("ruby", new_argv);
        } else if (strstr(tool_path, ".pl") != NULL) {
            /* Perl script */
            char *new_argv[argc + 2];
            new_argv[0] = "perl";
            new_argv[1] = tool_path;
            for (int i = 1; i < argc; i++) {
                new_argv[i + 1] = argv[i];
            }
            new_argv[argc + 1] = NULL;
            execvp("perl", new_argv);
        } else {
            /* Binary executable */
            execv(tool_path, argv);
        }
        exit(1);
    } else if (pid > 0) {
        /* Parent process */
        int status;
        waitpid(pid, &status, 0);
        return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 0 : 1;
    }
    
    return 1;
}

/* Get tool information */
kali_tool_t *get_kali_tool_info(const char *tool_name) {
    for (int i = 0; kali_tools_catalog[i].name != NULL; i++) {
        if (strcmp(kali_tools_catalog[i].name, tool_name) == 0) {
            return &kali_tools_catalog[i];
        }
    }
    return NULL;
}

/* List all Kali tools */
int list_kali_tools(void) {
    printf(COLOR_WHITE "Kali Top 10 Security Tools:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────\n" COLOR_RESET);
    
    for (int i = 0; kali_tools_catalog[i].name != NULL; i++) {
        kali_tool_t *tool = &kali_tools_catalog[i];
        int installed = check_kali_tool_installed(tool->name);
        
        printf(COLOR_WHITE "%d. %s" COLOR_RESET, i + 1, tool->name);
        if (installed) {
            printf(COLOR_GREEN " [INSTALLED]\n" COLOR_RESET);
        } else {
            printf(COLOR_GREY " [NOT INSTALLED]\n" COLOR_RESET);
        }
        
        printf(COLOR_GREY "   GitHub: %s\n" COLOR_RESET, tool->github_url);
        
        if (tool->dependencies != NULL && tool->dependencies[0] != NULL) {
            printf(COLOR_GREY "   Dependencies: " COLOR_RESET);
            for (int j = 0; tool->dependencies[j] != NULL; j++) {
                printf(COLOR_WHITE "%s " COLOR_RESET, tool->dependencies[j]);
            }
            printf("\n");
        }
        
        if (tool->binaries != NULL && tool->binaries[0] != NULL) {
            printf(COLOR_GREY "   Binaries: " COLOR_RESET);
            for (int j = 0; tool->binaries[j] != NULL; j++) {
                printf(COLOR_WHITE "%s " COLOR_RESET, tool->binaries[j]);
            }
            printf("\n");
        }
        printf("\n");
    }
    
    return 0;
}

/* Install a specific Kali tool */
int install_kali_tool(const char *tool_name, shell_context_t *ctx) {
    if (tool_name == NULL) {
        printf(COLOR_GREY "No tool specified.\n" COLOR_RESET);
        return 1;
    }
    
    kali_tool_t *tool = get_kali_tool_info(tool_name);
    if (tool == NULL) {
        printf(COLOR_GREY "Unknown tool: %s\n" COLOR_RESET, tool_name);
        printf(COLOR_GREY "Available tools: " COLOR_RESET);
        for (int i = 0; kali_tools_catalog[i].name != NULL; i++) {
            printf(COLOR_WHITE "%s " COLOR_RESET, kali_tools_catalog[i].name);
        }
        printf("\n");
        return 1;
    }
    
    if (check_kali_tool_installed(tool_name)) {
        printf(COLOR_GREY "Tool '%s' is already installed.\n" COLOR_RESET, tool_name);
        return 0;
    }
    
    printf(COLOR_WHITE "Installing Kali tool: %s\n" COLOR_RESET, tool_name);
    printf(COLOR_GREY "─────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "GitHub: %s\n" COLOR_RESET, tool->github_url);
    
    /* Create package directories */
    char *home = getenv("HOME");
    if (home == NULL) {
        printf(COLOR_GREY "HOME environment variable not set.\n" COLOR_RESET);
        return 1;
    }
    
    char packages_dir[PATH_MAX];
    snprintf(packages_dir, sizeof(packages_dir), "%s/.void/packages", home);
    
    /* Create directories */
    char cache_dir[PATH_MAX];
    snprintf(cache_dir, sizeof(cache_dir), "%s/cache", packages_dir);
    char bin_dir[PATH_MAX];
    snprintf(bin_dir, sizeof(bin_dir), "%s/bin", packages_dir);
    
    /* Create directories if they don't exist */
    char mkdir_cmd[1024];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s %s", cache_dir, bin_dir);
    system(mkdir_cmd);
    
    /* Clone repository */
    char install_path[PATH_MAX];
    snprintf(install_path, sizeof(install_path), "%s/%s", cache_dir, tool_name);
    
    printf(COLOR_GREY "Cloning repository...\n" COLOR_RESET);
    char clone_cmd[2048];
    snprintf(clone_cmd, sizeof(clone_cmd), "git clone %s %s", tool->github_url, install_path);
    
    int result = system(clone_cmd);
    if (result != 0) {
        printf(COLOR_GREY "Failed to clone repository.\n" COLOR_RESET);
        printf(COLOR_GREY "Make sure 'git' is available in your system.\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREEN "Repository cloned.\n" COLOR_RESET);
    
    /* Install dependencies if needed */
    if (tool->install_script != NULL) {
        printf(COLOR_GREY "Running install script...\n" COLOR_RESET);
        char original_cwd[PATH_MAX];
        if (getcwd(original_cwd, sizeof(original_cwd)) == NULL) {
            return 1;
        }
        
        if (chdir(install_path) != 0) {
            printf(COLOR_GREY "Cannot change to installation directory.\n" COLOR_RESET);
            return 1;
        }
        
        system(tool->install_script);
        chdir(original_cwd);
    }
    
    /* Create symlinks for binaries */
    if (tool->binaries != NULL) {
        printf(COLOR_GREY "Creating binary symlinks...\n" COLOR_RESET);
        for (int i = 0; tool->binaries[i] != NULL; i++) {
            char binary_path[PATH_MAX];
            char link_path[PATH_MAX];
            
            /* Try different possible locations */
            snprintf(binary_path, sizeof(binary_path), "%s/%s", install_path, tool->binaries[i]);
            if (access(binary_path, F_OK) != 0) {
                snprintf(binary_path, sizeof(binary_path), "%s/bin/%s", install_path, tool->binaries[i]);
            }
            if (access(binary_path, F_OK) != 0) {
                snprintf(binary_path, sizeof(binary_path), "%s/%s.py", install_path, tool->binaries[i]);
            }
            if (access(binary_path, F_OK) != 0) {
                snprintf(binary_path, sizeof(binary_path), "%s/%s.rb", install_path, tool->binaries[i]);
            }
            
            if (access(binary_path, F_OK) == 0) {
                snprintf(link_path, sizeof(link_path), "%s/%s", bin_dir, tool->binaries[i]);
                unlink(link_path); /* Remove existing link */
                symlink(binary_path, link_path);
                printf(COLOR_GREY "  Created symlink: %s\n" COLOR_RESET, tool->binaries[i]);
            }
        }
    }
    
    /* Add to PATH if not already there */
    char *current_path = getenv("PATH");
    char new_path[8192];
    snprintf(new_path, sizeof(new_path), "%s:%s", bin_dir, current_path ? current_path : "");
    setenv("PATH", new_path, 1);
    
    printf(COLOR_GREEN "Tool installed: %s\n" COLOR_RESET, tool_name);
    printf(COLOR_GREY "Binaries available in: %s\n" COLOR_RESET, bin_dir);
    printf(COLOR_GREY "Add to your ~/.voidrc: export PATH=\"%s:$PATH\"\n" COLOR_RESET, bin_dir);
    
    return 0;
}

/* Install all Kali tools */
int install_all_kali_tools(shell_context_t *ctx) {
    printf(COLOR_WHITE "Installing all Kali Top 10 tools...\n" COLOR_RESET);
    printf(COLOR_GREY "This may take a while.\n" COLOR_RESET);
    
    int installed = 0;
    int failed = 0;
    
    for (int i = 0; kali_tools_catalog[i].name != NULL; i++) {
        if (check_kali_tool_installed(kali_tools_catalog[i].name)) {
            printf(COLOR_GREY "Skipping %s (already installed)\n" COLOR_RESET, kali_tools_catalog[i].name);
            continue;
        }
        
        printf(COLOR_WHITE "\n[%d/10] Installing %s...\n" COLOR_RESET, i + 1, kali_tools_catalog[i].name);
        
        if (install_kali_tool(kali_tools_catalog[i].name, ctx) == 0) {
            installed++;
        } else {
            failed++;
            printf(COLOR_GREY "Failed to install %s\n" COLOR_RESET, kali_tools_catalog[i].name);
        }
    }
    
    printf(COLOR_WHITE "\nInstallation complete:\n" COLOR_RESET);
    printf(COLOR_GREEN "  Installed: %d\n" COLOR_RESET, installed);
    if (failed > 0) {
        printf(COLOR_GREY "  Failed: %d\n" COLOR_RESET, failed);
    }
    
    return (failed == 0) ? 0 : 1;
}

/* Individual tool installers (wrapper functions) */
int install_metasploit(shell_context_t *ctx) {
    return install_kali_tool("metasploit", ctx);
}

int install_sqlmap(shell_context_t *ctx) {
    return install_kali_tool("sqlmap", ctx);
}

int install_nikto(shell_context_t *ctx) {
    return install_kali_tool("nikto", ctx);
}

int install_nmap(shell_context_t *ctx) {
    return install_kali_tool("nmap", ctx);
}

int install_wpscan(shell_context_t *ctx) {
    return install_kali_tool("wpscan", ctx);
}

int install_gobuster(shell_context_t *ctx) {
    return install_kali_tool("gobuster", ctx);
}

int install_feroxbuster(shell_context_t *ctx) {
    return install_kali_tool("feroxbuster", ctx);
}

int install_john(shell_context_t *ctx) {
    return install_kali_tool("john", ctx);
}

int install_hashcat(shell_context_t *ctx) {
    return install_kali_tool("hashcat", ctx);
}

int install_hydra(shell_context_t *ctx) {
    return install_kali_tool("hydra", ctx);
}

