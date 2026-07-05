#include "system_ext.h"
#include "system.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#ifdef __linux__
#include <sys/sysinfo.h>
#endif
#include <sys/resource.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#ifdef __linux__
#include <utmp.h>
#endif
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#if defined(__linux__) || defined(__APPLE__)
#include <sys/sysctl.h>
#endif
#ifdef __APPLE__
#include <sys/types.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#endif

/* System Information */
int cmd_uname_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    struct utsname info;
    
    if (uname(&info) != 0) {
        printf(COLOR_GREY "Cannot get system information\n" COLOR_RESET);
        return 1;
    }
    
    bool show_all = false;
    bool show_kernel = false;
    bool show_nodename = false;
    bool show_release = false;
    bool show_version = false;
    bool show_machine = false;
    
    if (argc == 1) {
        show_all = true;
    } else {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) {
                show_all = true;
            } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--kernel-name") == 0) {
                show_kernel = true;
            } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--nodename") == 0) {
                show_nodename = true;
            } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--kernel-release") == 0) {
                show_release = true;
            } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--kernel-version") == 0) {
                show_version = true;
            } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--machine") == 0) {
                show_machine = true;
            }
        }
    }
    
    if (show_all || show_kernel) printf("%s ", info.sysname);
    if (show_all || show_nodename) printf("%s ", info.nodename);
    if (show_all || show_release) printf("%s ", info.release);
    if (show_all || show_version) printf("%s ", info.version);
    if (show_all || show_machine) printf("%s", info.machine);
    printf("\n");
    
    return 0;
}

int cmd_arch(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    struct utsname info;
    if (uname(&info) == 0) {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, info.machine);
        return 0;
    }
    
    return 1;
}

int cmd_uptime(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    #ifdef __linux__
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        unsigned long uptime_sec = info.uptime;
        unsigned long days = uptime_sec / 86400;
        unsigned long hours = (uptime_sec % 86400) / 3600;
        unsigned long minutes = (uptime_sec % 3600) / 60;
        
        printf(COLOR_WHITE "up %lu day%s, %lu:%02lu\n" COLOR_RESET,
               days, (days != 1) ? "s" : "", hours, minutes);
        return 0;
    }
    #elif defined(__APPLE__)
    struct timeval boottime;
    size_t len = sizeof(boottime);
    if (sysctlbyname("kern.boottime", &boottime, &len, NULL, 0) == 0) {
        time_t now = time(NULL);
        unsigned long uptime_sec = (unsigned long)(now - boottime.tv_sec);
        unsigned long days = uptime_sec / 86400;
        unsigned long hours = (uptime_sec % 86400) / 3600;
        unsigned long minutes = (uptime_sec % 3600) / 60;
        printf(COLOR_WHITE "up %lu day%s, %lu:%02lu\n" COLOR_RESET,
               days, (days != 1) ? "s" : "", hours, minutes);
        return 0;
    }
    #else
    printf(COLOR_GREY "Uptime not available on this system.\n" COLOR_RESET);
    #endif
    
    return 1;
}

int cmd_w_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_WHITE "%-8s %-12s %-20s %s\n" COLOR_RESET,
           "USER", "TTY", "FROM", "LOGIN@");
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    
    /* Read from /var/run/utmp or utmp.h */
    #ifdef __linux__
    FILE *fp = fopen("/var/run/utmp", "r");
    if (fp != NULL) {
        struct utmp entry;
        while (fread(&entry, sizeof(entry), 1, fp) == 1) {
            if (entry.ut_type == USER_PROCESS) {
                printf(COLOR_WHITE "%-8s %-12s %-20s " COLOR_RESET,
                       entry.ut_user, entry.ut_line, entry.ut_host);
                
                time_t login_time = entry.ut_tv.tv_sec;
                struct tm *tm_info = localtime(&login_time);
                char time_str[32];
                strftime(time_str, sizeof(time_str), "%H:%M", tm_info);
                printf("%s\n", time_str);
            }
        }
        fclose(fp);
        return 0;
    }
    #endif
    
    /* Fallback: show current user */
    const char *user = getenv("USER");
    if (user != NULL) {
        printf(COLOR_WHITE "%-8s %-12s %-20s %s\n" COLOR_RESET,
               user, "pts/0", "-", "now");
    }
    
    return 0;
}

int cmd_who_ext(int argc, char **argv, shell_context_t *ctx) {
    return cmd_w_ext(argc, argv, ctx);
}

int cmd_id_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    uid_t uid;
    gid_t gid;
    const char *username = NULL;
    
    if (argc > 1) {
        username = argv[1];
        struct passwd *pw = getpwnam(username);
        if (pw == NULL) {
            printf(COLOR_GREY "User '%s' not found\n" COLOR_RESET, username);
            return 1;
        }
        uid = pw->pw_uid;
        gid = pw->pw_gid;
    } else {
        uid = getuid();
        gid = getgid();
    }
    
    struct passwd *pw = getpwuid(uid);
    struct group *gr = getgrgid(gid);
    
    printf(COLOR_WHITE "uid=%d(%s) gid=%d(%s)",
           uid, pw ? pw->pw_name : "unknown",
           gid, gr ? gr->gr_name : "unknown");
    
    /* Get groups */
    gid_t groups[32];
    int ngroups = 32;
    if (getgrouplist(pw ? pw->pw_name : "", gid, groups, &ngroups) == 0) {
        printf(" groups=");
        for (int i = 0; i < ngroups; i++) {
            struct group *g = getgrgid(groups[i]);
            printf("%d(%s)%s", groups[i], g ? g->gr_name : "unknown",
                   (i < ngroups - 1) ? "," : "");
        }
    }
    printf("\n");
    
    return 0;
}

int cmd_groups_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    uid_t uid;
    const char *username = NULL;
    
    if (argc > 1) {
        username = argv[1];
        struct passwd *pw = getpwnam(username);
        if (pw == NULL) {
            printf(COLOR_GREY "User '%s' not found\n" COLOR_RESET, username);
            return 1;
        }
        uid = pw->pw_uid;
    } else {
        uid = getuid();
    }
    
    struct passwd *pw = getpwuid(uid);
    if (pw == NULL) {
        return 1;
    }
    
    gid_t groups[32];
    int ngroups = 32;
    if (getgrouplist(pw->pw_name, pw->pw_gid, groups, &ngroups) == 0) {
        for (int i = 0; i < ngroups; i++) {
            struct group *g = getgrgid(groups[i]);
            printf(COLOR_WHITE "%s " COLOR_RESET, g ? g->gr_name : "unknown");
        }
        printf("\n");
        return 0;
    }
    
    return 1;
}

int cmd_last(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *user = (argc > 1) ? argv[1] : NULL;
    
    printf(COLOR_GREY "Last login information requires /var/log/wtmp.\n" COLOR_RESET);
    printf(COLOR_GREY "Implementation pending.\n" COLOR_RESET);
    return 1;
}

int cmd_lastlog(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *user = (argc > 1) ? argv[1] : NULL;
    
    printf(COLOR_GREY "Lastlog requires /var/log/lastlog.\n" COLOR_RESET);
    return 1;
}

int cmd_finger(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: finger <user>\n" COLOR_RESET);
        return 1;
    }
    
    const char *username = argv[1];
    struct passwd *pw = getpwnam(username);
    if (pw == NULL) {
        printf(COLOR_GREY "User '%s' not found\n" COLOR_RESET, username);
        return 1;
    }
    
    printf(COLOR_WHITE "Login: %-8s" COLOR_RESET, pw->pw_name);
    printf(COLOR_WHITE " Name: %s\n" COLOR_RESET, pw->pw_gecos ? pw->pw_gecos : "");
    printf(COLOR_WHITE "Directory: %s" COLOR_RESET, pw->pw_dir);
    printf(COLOR_WHITE " Shell: %s\n" COLOR_RESET, pw->pw_shell);
    
    return 0;
}

int cmd_users(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    /* Show logged in users */
    const char *user = getenv("USER");
    if (user != NULL) {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, user);
    }
    
    return 0;
}

int cmd_logname(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    const char *user = getlogin();
    if (user != NULL) {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, user);
        return 0;
    }
    
    return 1;
}

/* System Monitoring */
int cmd_vmstat(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    int interval = (argc > 1) ? atoi(argv[1]) : 1;
    
    printf(COLOR_WHITE "procs -----------memory---------- ---swap-- -----io---- -system-- ----cpu----\n" COLOR_RESET);
    printf(COLOR_WHITE " r  b   swpd   free   buff  cache   si   so    bi    bo   in   cs us sy id wa\n" COLOR_RESET);
    
    #ifdef __linux__
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp != NULL) {
        char line[256];
        unsigned long mem_total = 0, mem_free = 0, mem_buffers = 0, mem_cached = 0;
        
        while (fgets(line, sizeof(line), fp) != NULL) {
            if (sscanf(line, "MemTotal: %lu kB", &mem_total) == 1) continue;
            if (sscanf(line, "MemFree: %lu kB", &mem_free) == 1) continue;
            if (sscanf(line, "Buffers: %lu kB", &mem_buffers) == 1) continue;
            if (sscanf(line, "Cached: %lu kB", &mem_cached) == 1) continue;
        }
        fclose(fp);
        
        printf(COLOR_WHITE " 0  0      0 %6lu     0 %6lu    0    0     0     0   0   0  0  0 100  0\n" COLOR_RESET,
               mem_free / 1024, mem_cached / 1024);
        return 0;
    }
    #elif defined(__APPLE__)
    {
        uint64_t memsize = 0;
        size_t len = sizeof(memsize);
        if (sysctlbyname("hw.memsize", &memsize, &len, NULL, 0) == 0) {
            mach_port_t host_port = mach_host_self();
            vm_statistics64_data_t vm_stat;
            mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
            if (host_statistics64(host_port, HOST_VM_INFO64, (host_info64_t)&vm_stat, &count) == 0) {
                unsigned long free_m = (unsigned long)((vm_stat.free_count * (uint64_t)vm_page_size) / 1024);
                unsigned long inactive_m = (unsigned long)((vm_stat.inactive_count * (uint64_t)vm_page_size) / 1024);
                printf(COLOR_WHITE " 0  0      0 %6lu     0 %6lu    0    0     0     0   0   0  0  0 100  0\n" COLOR_RESET, free_m, inactive_m);
                return 0;
            }
        }
    }
    #endif
    
    printf(COLOR_GREY "VM statistics not available on this system.\n" COLOR_RESET);
    return 1;
}

int cmd_iostat(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    int interval = (argc > 1) ? atoi(argv[1]) : 1;
    
    printf(COLOR_GREY "I/O statistics require kernel I/O accounting.\n" COLOR_RESET);
    return 1;
}

int cmd_mpstat(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    int interval = (argc > 1) ? atoi(argv[1]) : 1;
    
    #ifdef __linux__
    FILE *fp = fopen("/proc/stat", "r");
    if (fp != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), fp) != NULL) {
            if (strncmp(line, "cpu", 3) == 0 && line[3] != ' ') {
                /* Per-CPU stats */
                unsigned long user, nice, system, idle;
                if (sscanf(line, "cpu%*d %lu %lu %lu %lu", &user, &nice, &system, &idle) == 4) {
                    unsigned long total = user + nice + system + idle;
                    int cpu_percent = (total > 0) ? ((user + nice + system) * 100 / total) : 0;
                    printf(COLOR_WHITE "CPU: %d%%\n" COLOR_RESET, cpu_percent);
                }
            }
        }
        fclose(fp);
        return 0;
    }
    #elif defined(__APPLE__)
    {
        size_t len;
        int ncpu;
        len = sizeof(ncpu);
        if (sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0) == 0) {
            printf(COLOR_WHITE "CPU: (use Activity Monitor for per-CPU on macOS)\n" COLOR_RESET);
            return 0;
        }
    }
    #endif
    
    printf(COLOR_GREY "CPU statistics not available.\n" COLOR_RESET);
    return 1;
}

int cmd_sar(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "SAR requires system activity reporter daemon.\n" COLOR_RESET);
    return 1;
}

int cmd_free(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    #ifdef __linux__
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp != NULL) {
        char line[256];
        unsigned long mem_total = 0, mem_free = 0, mem_available = 0, mem_buffers = 0, mem_cached = 0;
        unsigned long swap_total = 0, swap_free = 0;
        
        while (fgets(line, sizeof(line), fp) != NULL) {
            sscanf(line, "MemTotal: %lu kB", &mem_total);
            sscanf(line, "MemFree: %lu kB", &mem_free);
            sscanf(line, "MemAvailable: %lu kB", &mem_available);
            sscanf(line, "Buffers: %lu kB", &mem_buffers);
            sscanf(line, "Cached: %lu kB", &mem_cached);
            sscanf(line, "SwapTotal: %lu kB", &swap_total);
            sscanf(line, "SwapFree: %lu kB", &swap_free);
        }
        fclose(fp);
        
        unsigned long mem_used = mem_total - mem_free;
        unsigned long swap_used = swap_total - swap_free;
        
        printf(COLOR_WHITE "              total        used        free      shared  buff/cache   available\n" COLOR_RESET);
        printf(COLOR_WHITE "Mem:   %12lu %12lu %12lu %12lu %12lu %12lu\n" COLOR_RESET,
               mem_total, mem_used, mem_free, 0UL, mem_buffers + mem_cached, mem_available);
        printf(COLOR_WHITE "Swap:  %12lu %12lu %12lu\n" COLOR_RESET,
               swap_total, swap_used, swap_free);
        return 0;
    }
    #elif defined(__APPLE__)
    {
        uint64_t memsize = 0;
        size_t len = sizeof(memsize);
        if (sysctlbyname("hw.memsize", &memsize, &len, NULL, 0) == 0) {
            mach_port_t host_port = mach_host_self();
            vm_statistics64_data_t vm_stat;
            mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
            if (host_statistics64(host_port, HOST_VM_INFO64, (host_info64_t)&vm_stat, &count) == 0) {
                uint64_t total_m = memsize / (1024 * 1024);
                uint64_t free_m = (vm_stat.free_count * (uint64_t)vm_page_size) / (1024 * 1024);
                uint64_t used_m = total_m - free_m;
                uint64_t inactive_m = (vm_stat.inactive_count * (uint64_t)vm_page_size) / (1024 * 1024);
                printf(COLOR_WHITE "              total        used        free      shared  buff/cache   available\n" COLOR_RESET);
                printf(COLOR_WHITE "Mem:   %12llu %12llu %12llu %12u %12llu %12llu\n" COLOR_RESET,
                       (unsigned long long)total_m, (unsigned long long)used_m, (unsigned long long)free_m,
                       0u, (unsigned long long)inactive_m, (unsigned long long)(free_m + inactive_m));
                printf(COLOR_WHITE "Swap:  (swap not shown on macOS)\n" COLOR_RESET);
                return 0;
            }
        }
    }
    #endif
    
    printf(COLOR_GREY "Memory information not available on this system.\n" COLOR_RESET);
    return 1;
}

int cmd_lscpu(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    #ifdef __linux__
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp != NULL) {
        char line[256];
        int cpu_count = 0;
        char model[256] = "";
        char architecture[64] = "";
        
        while (fgets(line, sizeof(line), fp) != NULL) {
            if (strncmp(line, "processor", 9) == 0) {
                cpu_count++;
            } else if (strncmp(line, "model name", 10) == 0) {
                char *colon = strchr(line, ':');
                if (colon != NULL) {
                    strncpy(model, colon + 2, sizeof(model) - 1);
                    char *nl = strchr(model, '\n');
                    if (nl) *nl = '\0';
                }
            } else if (strncmp(line, "Architecture", 12) == 0) {
                char *colon = strchr(line, ':');
                if (colon != NULL) {
                    strncpy(architecture, colon + 2, sizeof(architecture) - 1);
                    char *nl = strchr(architecture, '\n');
                    if (nl) *nl = '\0';
                }
            }
        }
        fclose(fp);
        
        printf(COLOR_WHITE "Architecture:        %s\n" COLOR_RESET, architecture);
        printf(COLOR_WHITE "CPU op-mode(s):      32-bit, 64-bit\n" COLOR_RESET);
        printf(COLOR_WHITE "CPU(s):              %d\n" COLOR_RESET, cpu_count);
        printf(COLOR_WHITE "Model name:          %s\n" COLOR_RESET, model);
        return 0;
    }
    #elif defined(__APPLE__)
    {
        int ncpu = 0;
        size_t len = sizeof(ncpu);
        char model[256] = "";
        size_t model_len = sizeof(model);
        if (sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0) == 0) {
            sysctlbyname("hw.model", model, &model_len, NULL, 0);
            printf(COLOR_WHITE "Architecture:        %s\n" COLOR_RESET, sizeof(void*) == 8 ? "aarch64/x86_64" : "armv7/i386");
            printf(COLOR_WHITE "CPU(s):              %d\n" COLOR_RESET, ncpu);
            printf(COLOR_WHITE "Model name:          %s\n" COLOR_RESET, model[0] ? model : "Apple");
            return 0;
        }
    }
    #endif
    
    printf(COLOR_GREY "CPU information not available.\n" COLOR_RESET);
    return 1;
}

int cmd_lspci(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    #ifdef __linux__
    FILE *fp = fopen("/proc/bus/pci/devices", "r");
    if (fp != NULL) {
        char line[256];
        printf(COLOR_WHITE "PCI devices:\n" COLOR_RESET);
        while (fgets(line, sizeof(line), fp) != NULL) {
            printf("%s", line);
        }
        fclose(fp);
        return 0;
    }
    #elif defined(__APPLE__)
    {
        FILE *fp = popen("system_profiler SPPCIDataType 2>/dev/null", "r");
        if (fp != NULL) {
            char line[256];
            while (fgets(line, sizeof(line), fp) != NULL) printf("%s", line);
            pclose(fp);
            return 0;
        }
    }
    #endif
    
    printf(COLOR_GREY "PCI device listing not available.\n" COLOR_RESET);
    return 1;
}

int cmd_lsusb(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    #ifdef __linux__
    FILE *fp = fopen("/proc/bus/usb/devices", "r");
    if (fp != NULL) {
        char line[256];
        printf(COLOR_WHITE "USB devices:\n" COLOR_RESET);
        while (fgets(line, sizeof(line), fp) != NULL) {
            printf("%s", line);
        }
        fclose(fp);
        return 0;
    }
    #elif defined(__APPLE__)
    {
        FILE *fp = popen("system_profiler SPUSBDataType 2>/dev/null", "r");
        if (fp != NULL) {
            char line[256];
            while (fgets(line, sizeof(line), fp) != NULL) printf("%s", line);
            pclose(fp);
            return 0;
        }
    }
    #endif
    
    printf(COLOR_GREY "USB device listing not available.\n" COLOR_RESET);
    return 1;
}

int cmd_lsblk(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    #ifdef __linux__
    DIR *d = opendir("/sys/block");
    if (d != NULL) {
        struct dirent *entry;
        printf(COLOR_WHITE "NAME   MAJ:MIN RM   SIZE RO TYPE MOUNTPOINT\n" COLOR_RESET);
        while ((entry = readdir(d)) != NULL) {
            if (entry->d_name[0] == '.') continue;
            printf(COLOR_WHITE "%-6s " COLOR_RESET, entry->d_name);
            printf(COLOR_GREY "8:0   1   100G  0 disk\n" COLOR_RESET);
        }
        closedir(d);
        return 0;
    }
    #endif
    
    printf(COLOR_GREY "Block device listing not available.\n" COLOR_RESET);
    return 1;
}

int cmd_blkid(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Blkid requires libblkid.\n" COLOR_RESET);
    return 1;
}

int cmd_fdisk(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Fdisk requires interactive partition editor.\n" COLOR_RESET);
    return 1;
}

int cmd_parted(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Parted requires partition editor library.\n" COLOR_RESET);
    return 1;
}

int cmd_gparted(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Gparted requires GUI partition editor.\n" COLOR_RESET);
    return 1;
}

int cmd_lsmod(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    #ifdef __linux__
    FILE *fp = fopen("/proc/modules", "r");
    if (fp != NULL) {
        char line[256];
        printf(COLOR_WHITE "Module                  Size  Used by\n" COLOR_RESET);
        while (fgets(line, sizeof(line), fp) != NULL) {
            char module[64];
            unsigned long size;
            int used;
            if (sscanf(line, "%s %lu %d", module, &size, &used) == 3) {
                printf(COLOR_WHITE "%-22s %8lu %d\n" COLOR_RESET, module, size, used);
            }
        }
        fclose(fp);
        return 0;
    }
    #endif
    
    printf(COLOR_GREY "Module listing not available.\n" COLOR_RESET);
    return 1;
}

int cmd_modinfo(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: modinfo <module>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Modinfo requires module information.\n" COLOR_RESET);
    return 1;
}

int cmd_modprobe(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: modprobe <module>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Modprobe requires kernel module loader.\n" COLOR_RESET);
    return 1;
}

int cmd_rmmod(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: rmmod <module>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Rmmod requires kernel module unloader.\n" COLOR_RESET);
    return 1;
}

int cmd_insmod(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: insmod <module>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Insmod requires kernel module loader.\n" COLOR_RESET);
    return 1;
}

int cmd_depmod(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Depmod requires module dependency generator.\n" COLOR_RESET);
    return 1;
}

int cmd_dmesg(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    #ifdef __linux__
    FILE *fp = fopen("/var/log/dmesg", "r");
    if (fp == NULL) {
        fp = fopen("/proc/kmsg", "r");
    }
    if (fp != NULL) {
        char line[4096];
        while (fgets(line, sizeof(line), fp) != NULL) {
            printf("%s", line);
        }
        fclose(fp);
        return 0;
    }
    #endif
    
    printf(COLOR_GREY "Kernel messages not available.\n" COLOR_RESET);
    return 1;
}

int cmd_journalctl(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Journalctl requires systemd journal.\n" COLOR_RESET);
    return 1;
}

/* System Control */
int cmd_shutdown(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Shutdown requires root privileges and init system.\n" COLOR_RESET);
    printf(COLOR_GREY "Void OS is a shell environment - use host OS shutdown.\n" COLOR_RESET);
    return 1;
}

int cmd_reboot(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Reboot requires root privileges and init system.\n" COLOR_RESET);
    return 1;
}

int cmd_halt(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Halt requires root privileges and init system.\n" COLOR_RESET);
    return 1;
}

int cmd_poweroff(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Poweroff requires root privileges and init system.\n" COLOR_RESET);
    return 1;
}

int cmd_init_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: init <level>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Init requires root privileges and init system.\n" COLOR_RESET);
    return 1;
}

int cmd_service(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: service <name> <command>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Service requires service management system.\n" COLOR_RESET);
    return 1;
}

int cmd_chkconfig(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Chkconfig requires service configuration system.\n" COLOR_RESET);
    return 1;
}

int cmd_update_rc_d(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Update-rc.d requires runlevel system.\n" COLOR_RESET);
    return 1;
}

int cmd_systemd_analyze(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Systemd-analyze requires systemd.\n" COLOR_RESET);
    return 1;
}

int cmd_systemd_cgtop(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Systemd-cgtop requires systemd.\n" COLOR_RESET);
    return 1;
}

int cmd_systemd_cgls(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Systemd-cgls requires systemd.\n" COLOR_RESET);
    return 1;
}

/* User & Group Management */
int cmd_useradd(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: useradd <user>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Useradd requires root privileges and user management system.\n" COLOR_RESET);
    return 1;
}

int cmd_userdel(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: userdel <user>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Userdel requires root privileges.\n" COLOR_RESET);
    return 1;
}

int cmd_usermod(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: usermod <user> [options]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Usermod requires root privileges.\n" COLOR_RESET);
    return 1;
}

int cmd_passwd(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Passwd requires password management system.\n" COLOR_RESET);
    return 1;
}

int cmd_chage(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: chage <user>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Chage requires root privileges.\n" COLOR_RESET);
    return 1;
}

int cmd_chfn(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: chfn <user>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Chfn requires finger information system.\n" COLOR_RESET);
    return 1;
}

int cmd_chsh(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: chsh <user>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Chsh requires shell change system.\n" COLOR_RESET);
    return 1;
}

int cmd_groupadd(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: groupadd <group>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Groupadd requires root privileges.\n" COLOR_RESET);
    return 1;
}

int cmd_groupdel(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: groupdel <group>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Groupdel requires root privileges.\n" COLOR_RESET);
    return 1;
}

int cmd_groupmod(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: groupmod <group> [options]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Groupmod requires root privileges.\n" COLOR_RESET);
    return 1;
}

int cmd_gpasswd(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: gpasswd <group>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Gpasswd requires group password system.\n" COLOR_RESET);
    return 1;
}

int cmd_newgrp(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: newgrp <group>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Newgrp requires group switching.\n" COLOR_RESET);
    return 1;
}

int cmd_su(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *user = (argc > 1) ? argv[1] : "root";
    
    printf(COLOR_GREY "Su requires user switching system.\n" COLOR_RESET);
    return 1;
}

int cmd_sudo(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: sudo <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Sudo requires sudoers configuration.\n" COLOR_RESET);
    return 1;
}

int cmd_visudo(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Visudo requires sudoers editor.\n" COLOR_RESET);
    return 1;
}

/* Package Management */
int cmd_apt(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: apt <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "APT requires Debian package management system.\n" COLOR_RESET);
    return 1;
}

int cmd_apt_get(int argc, char **argv, shell_context_t *ctx) {
    return cmd_apt(argc, argv, ctx);
}

int cmd_apt_cache(int argc, char **argv, shell_context_t *ctx) {
    return cmd_apt(argc, argv, ctx);
}

int cmd_dpkg(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Dpkg requires Debian package system.\n" COLOR_RESET);
    return 1;
}

int cmd_rpm(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "RPM requires RPM package system.\n" COLOR_RESET);
    return 1;
}

int cmd_yum(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "YUM requires YUM package manager.\n" COLOR_RESET);
    return 1;
}

int cmd_dnf(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "DNF requires DNF package manager.\n" COLOR_RESET);
    return 1;
}

int cmd_pacman(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Pacman requires Arch package manager.\n" COLOR_RESET);
    return 1;
}

int cmd_zypper(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Zypper requires SUSE package manager.\n" COLOR_RESET);
    return 1;
}

int cmd_emerge(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Emerge requires Gentoo package manager.\n" COLOR_RESET);
    return 1;
}

int cmd_brew(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Brew requires Homebrew package manager.\n" COLOR_RESET);
    return 1;
}

int cmd_snap(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Snap requires Snap package system.\n" COLOR_RESET);
    return 1;
}

int cmd_flatpak(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Flatpak requires Flatpak package system.\n" COLOR_RESET);
    return 1;
}

int cmd_pip(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Pip requires Python package manager.\n" COLOR_RESET);
    return 1;
}

int cmd_npm(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "NPM requires Node.js package manager.\n" COLOR_RESET);
    return 1;
}

int cmd_cargo(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Cargo requires Rust package manager.\n" COLOR_RESET);
    return 1;
}

int cmd_go_pkg(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Go requires Go package manager.\n" COLOR_RESET);
    return 1;
}

/* System Logs */
int cmd_logwatch(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Logwatch requires log analysis system.\n" COLOR_RESET);
    return 1;
}

int cmd_logrotate(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: logrotate <config>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Logrotate requires log rotation system.\n" COLOR_RESET);
    return 1;
}

int cmd_rsyslog(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Rsyslog requires syslog daemon.\n" COLOR_RESET);
    return 1;
}

int cmd_syslog_ng(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Syslog-ng requires syslog-ng daemon.\n" COLOR_RESET);
    return 1;
}

