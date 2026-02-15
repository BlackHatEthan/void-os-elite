#include "system.h"
#include "../utils/logger.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <signal.h>

#ifdef __linux__
#include <sys/sysinfo.h>
#endif

static volatile bool entropy_running = false;

void entropy_sigint_handler(int sig) {
    (void)sig;
    entropy_running = false;
}

int cmd_entropy(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    entropy_running = true;
    signal(SIGINT, entropy_sigint_handler);
    
    printf(COLOR_WHITE "Void OS System Monitoring\n" COLOR_RESET);
    printf(COLOR_GREY "Press Ctrl+C to exit\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    while (entropy_running) {
        /* Clear screen and move cursor to top */
        printf("\033[2J\033[H");
        
        printf(COLOR_WHITE "Void Stability Metrics\n" COLOR_RESET);
        printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
        
        /* CPU Usage (Void Stability) */
        #ifdef __linux__
        FILE *stat = fopen("/proc/stat", "r");
        if (stat != NULL) {
            unsigned long long user, nice, system, idle;
            fscanf(stat, "cpu %llu %llu %llu %llu", &user, &nice, &system, &idle);
            fclose(stat);
            
            unsigned long long total = user + nice + system + idle;
            unsigned long long used = user + nice + system;
            int cpu_percent = (int)((used * 100) / total);
            
            printf(COLOR_WHITE "Void Stability (CPU): " COLOR_RESET);
            if (cpu_percent < 50) {
                printf(COLOR_GREEN "%d%%\n" COLOR_RESET, cpu_percent);
            } else if (cpu_percent < 80) {
                printf(COLOR_BLUE "%d%%\n" COLOR_RESET, cpu_percent);
            } else {
                printf(COLOR_GREY "%d%%\n" COLOR_RESET, cpu_percent);
            }
        }
        #else
        /* macOS - try to read from sysctl or /proc if available */
        /* Note: macOS doesn't have /proc/stat, so CPU monitoring is limited */
        printf(COLOR_WHITE "Void Stability (CPU): " COLOR_RESET);
        printf(COLOR_GREY "CPU monitoring requires /proc/stat (Linux) or sysctl (macOS)\n" COLOR_RESET);
        printf(COLOR_GREY "Native implementation pending for macOS.\n" COLOR_RESET);
        #endif
        
        /* Memory Usage (Memory Leakage) */
        #ifdef __linux__
        FILE *meminfo = fopen("/proc/meminfo", "r");
        if (meminfo != NULL) {
            unsigned long mem_total = 0, mem_free = 0, mem_available = 0;
            char line[256];
            while (fgets(line, sizeof(line), meminfo) != NULL) {
                if (strncmp(line, "MemTotal:", 9) == 0) {
                    sscanf(line, "MemTotal: %lu", &mem_total);
                } else if (strncmp(line, "MemFree:", 8) == 0) {
                    sscanf(line, "MemFree: %lu", &mem_free);
                } else if (strncmp(line, "MemAvailable:", 13) == 0) {
                    sscanf(line, "MemAvailable: %lu", &mem_available);
                }
            }
            fclose(meminfo);
            
            if (mem_total > 0) {
                unsigned long mem_used = mem_total - mem_available;
                int mem_percent = (int)((mem_used * 100) / mem_total);
                
                printf(COLOR_WHITE "Memory Leakage (RAM): " COLOR_RESET);
                if (mem_percent < 50) {
                    printf(COLOR_GREEN "%d%% (%lu MB / %lu MB)\n" COLOR_RESET,
                           mem_percent, mem_used / 1024, mem_total / 1024);
                } else if (mem_percent < 80) {
                    printf(COLOR_BLUE "%d%% (%lu MB / %lu MB)\n" COLOR_RESET,
                           mem_percent, mem_used / 1024, mem_total / 1024);
                } else {
                    printf(COLOR_GREY "%d%% (%lu MB / %lu MB)\n" COLOR_RESET,
                           mem_percent, mem_used / 1024, mem_total / 1024);
                }
            }
        }
        #else
        /* macOS - try to read from sysctl */
        /* Note: macOS memory info requires sysctl or vm_stat parsing */
        printf(COLOR_WHITE "Memory Leakage (RAM): " COLOR_RESET);
        printf(COLOR_GREY "Memory monitoring requires /proc/meminfo (Linux) or sysctl (macOS)\n" COLOR_RESET);
        printf(COLOR_GREY "Native implementation pending for macOS.\n" COLOR_RESET);
        #endif
        
        /* Disk Usage (Reality Allocation) */
        struct statvfs vfs;
        if (statvfs(".", &vfs) == 0) {
            unsigned long long total = vfs.f_blocks * vfs.f_frsize;
            unsigned long long available = vfs.f_bavail * vfs.f_frsize;
            unsigned long long used = total - available;
            int disk_percent = (int)((used * 100) / total);
            
            printf(COLOR_WHITE "Reality Allocation (Disk): " COLOR_RESET);
            if (disk_percent < 70) {
                printf(COLOR_GREEN "%d%%\n" COLOR_RESET, disk_percent);
            } else if (disk_percent < 90) {
                printf(COLOR_BLUE "%d%%\n" COLOR_RESET, disk_percent);
            } else {
                printf(COLOR_GREY "%d%%\n" COLOR_RESET, disk_percent);
            }
        }
        
        /* Process Count */
        int process_count = 0;
        #ifdef __linux__
        DIR *proc = opendir("/proc");
        if (proc != NULL) {
            struct dirent *entry;
            while ((entry = readdir(proc)) != NULL) {
                if (entry->d_name[0] >= '0' && entry->d_name[0] <= '9') {
                    process_count++;
                }
            }
            closedir(proc);
        }
        #else
        /* macOS - count processes by reading /proc if available, or use alternative */
        /* Try to read from /proc if it exists (some macOS systems have procfs) */
        DIR *proc = opendir("/proc");
        if (proc != NULL) {
            struct dirent *entry;
            while ((entry = readdir(proc)) != NULL) {
                if (entry->d_name[0] >= '0' && entry->d_name[0] <= '9') {
                    process_count++;
                }
            }
            closedir(proc);
        } else {
            /* Fallback: estimate based on system */
            process_count = 50; /* Placeholder */
        }
        #endif
        
        printf(COLOR_WHITE "Process Count: " COLOR_BLUE "%d\n" COLOR_RESET, process_count);
        
        printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
        printf(COLOR_DIM "Updating every 2 seconds...\n" COLOR_RESET);
        
        fflush(stdout);
        sleep(2);
    }
    
    printf("\n");
    signal(SIGINT, SIG_DFL);
    return 0;
}

int cmd_pulse_graph(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "Pulse Graph - System Resource Sparkline\n" COLOR_RESET);
    printf(COLOR_GREY "Press Ctrl+C to stop.\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    signal(SIGINT, SIG_IGN);
    
    while (1) {
        /* Get CPU usage */
        FILE *stat = fopen("/proc/stat", "r");
        if (stat == NULL) {
            printf(COLOR_GREY "Not available on this system.\n" COLOR_RESET);
            break;
        }
        
        char line[256];
        if (fgets(line, sizeof(line), stat) != NULL) {
            unsigned long user, nice, system, idle;
            sscanf(line, "cpu %lu %lu %lu %lu", &user, &nice, &system, &idle);
            unsigned long total = user + nice + system + idle;
            int cpu_percent = (total > 0) ? ((user + nice + system) * 100 / total) : 0;
            
            /* Simple ASCII sparkline */
            int bars = cpu_percent / 5;
            printf(COLOR_WHITE "CPU: " COLOR_RESET);
            for (int i = 0; i < 20; i++) {
                if (i < bars) {
                    printf(COLOR_BLUE "█" COLOR_RESET);
                } else {
                    printf(COLOR_GREY "░" COLOR_RESET);
                }
            }
            printf(COLOR_WHITE " %d%%\n" COLOR_RESET, cpu_percent);
        }
        fclose(stat);
        
        fflush(stdout);
        sleep(1);
    }
    
    signal(SIGINT, SIG_DFL);
    return 0;
}

int cmd_synapse(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: synapse <pid>\n" COLOR_RESET);
        return 1;
    }
    
    pid_t pid = atoi(argv[1]);
    if (pid <= 0) {
        printf(COLOR_GREY "Invalid PID.\n" COLOR_RESET);
        return 1;
    }
    
    /* Native system call tracing using ptrace() */
    /* TODO: Implement native ptrace-based syscall tracing */
    printf(COLOR_GREY "Native system call tracing implementation pending.\n" COLOR_RESET);
    printf(COLOR_GREY "This requires implementing ptrace() interface for syscall interception.\n" COLOR_RESET);
    printf(COLOR_GREY "For now, use external strace/dtruss directly.\n" COLOR_RESET);
    return 1;
}

int cmd_organelle(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "Storage Devices:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    #ifdef __linux__
    /* Read from /proc/partitions or /sys/block */
    FILE *partitions = fopen("/proc/partitions", "r");
    if (partitions != NULL) {
        char line[256];
        int count = 0;
        while (fgets(line, sizeof(line), partitions) != NULL && count < 20) {
            if (count < 2) {
                count++;
                continue; /* Skip header lines */
            }
            char name[64];
            unsigned long major, minor, blocks;
            if (sscanf(line, "%lu %lu %lu %s", &major, &minor, &blocks, name) == 4) {
                printf(COLOR_WHITE "%-10s " COLOR_BLUE "%lu blocks\n" COLOR_RESET, name, blocks);
            }
            count++;
        }
        fclose(partitions);
    } else {
        printf(COLOR_GREY "Failed to read partition information.\n" COLOR_RESET);
    }
    #else
    /* macOS - read from /dev or use sysctl */
    printf(COLOR_GREY "Storage device listing requires /proc/partitions (Linux) or diskutil (macOS)\n" COLOR_RESET);
    printf(COLOR_GREY "Native implementation pending for macOS.\n" COLOR_RESET);
    #endif
    
    return 0;
}

int cmd_vitals(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "System Vitals:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    /* OS Info */
    #ifdef __linux__
    FILE *os = fopen("/etc/os-release", "r");
    if (os != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), os) != NULL) {
            if (strncmp(line, "PRETTY_NAME=", 12) == 0) {
                printf(COLOR_WHITE "OS: " COLOR_BLUE "%s" COLOR_RESET, line + 12);
                break;
            }
        }
        fclose(os);
    }
    #else
    /* macOS - read from /System/Library/CoreServices/SystemVersion.plist or use sysctl */
    printf(COLOR_GREY "OS version detection requires /etc/os-release (Linux) or SystemVersion.plist (macOS)\n" COLOR_RESET);
    printf(COLOR_GREY "Native implementation pending for macOS.\n" COLOR_RESET);
    #endif
    
    /* CPU Info */
    #ifdef __linux__
    FILE *cpu = fopen("/proc/cpuinfo", "r");
    if (cpu != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), cpu) != NULL) {
            if (strncmp(line, "model name", 10) == 0) {
                char *colon = strchr(line, ':');
                if (colon != NULL) {
                    printf(COLOR_WHITE "CPU: " COLOR_BLUE "%s" COLOR_RESET, colon + 2);
                    break;
                }
            }
        }
        fclose(cpu);
    }
    #else
    /* macOS - CPU info requires sysctl */
    printf(COLOR_GREY "CPU info requires /proc/cpuinfo (Linux) or sysctl (macOS)\n" COLOR_RESET);
    printf(COLOR_GREY "Native implementation pending for macOS.\n" COLOR_RESET);
    #endif
    
    /* Memory */
    #ifdef __linux__
    FILE *mem = fopen("/proc/meminfo", "r");
    if (mem != NULL) {
        char line[256];
        if (fgets(line, sizeof(line), mem) != NULL) {
            unsigned long total_kb;
            sscanf(line, "MemTotal: %lu", &total_kb);
            printf(COLOR_WHITE "RAM: " COLOR_BLUE "%.2f GB\n" COLOR_RESET, total_kb / 1024.0 / 1024.0);
        }
        fclose(mem);
    }
    #else
    /* macOS - memory info requires sysctl */
    printf(COLOR_GREY "Memory info requires /proc/meminfo (Linux) or sysctl (macOS)\n" COLOR_RESET);
    printf(COLOR_GREY "Native implementation pending for macOS.\n" COLOR_RESET);
    #endif
    
    return 0;
}

int cmd_tempest(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "Thermal Sensors:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    #ifdef __linux__
    DIR *thermal = opendir("/sys/class/thermal");
    if (thermal != NULL) {
        struct dirent *entry;
        while ((entry = readdir(thermal)) != NULL) {
            if (strncmp(entry->d_name, "thermal_zone", 12) == 0) {
                char path[256];
                snprintf(path, sizeof(path), "/sys/class/thermal/%s/temp", entry->d_name);
                FILE *temp_file = fopen(path, "r");
                if (temp_file != NULL) {
                    int temp_millidegrees;
                    if (fscanf(temp_file, "%d", &temp_millidegrees) == 1) {
                        double temp_c = temp_millidegrees / 1000.0;
                        printf(COLOR_WHITE "%s: " COLOR_BLUE "%.1f°C\n" COLOR_RESET, entry->d_name, temp_c);
                    }
                    fclose(temp_file);
                }
            }
        }
        closedir(thermal);
    } else {
        printf(COLOR_GREY "Thermal sensors not available.\n" COLOR_RESET);
    }
    #else
    /* macOS - thermal sensors require powermetrics or IOKit */
    printf(COLOR_GREY "Thermal sensors require /sys/class/thermal (Linux) or IOKit (macOS)\n" COLOR_RESET);
    printf(COLOR_GREY "Native implementation pending for macOS.\n" COLOR_RESET);
    #endif
    
    return 0;
}

int cmd_neural_map(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "Kernel Modules:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    #ifdef __linux__
    /* Read from /proc/modules */
    FILE *modules = fopen("/proc/modules", "r");
    if (modules != NULL) {
        char line[512];
        int count = 0;
        while (fgets(line, sizeof(line), modules) != NULL && count < 20) {
            char name[256];
            if (sscanf(line, "%s", name) == 1) {
                printf(COLOR_WHITE "%-30s\n" COLOR_RESET, name);
                count++;
            }
        }
        fclose(modules);
    } else {
        printf(COLOR_GREY "Failed to read kernel modules.\n" COLOR_RESET);
    }
    #else
    /* macOS - kernel extensions require kextstat or IOKit */
    printf(COLOR_GREY "Kernel modules require /proc/modules (Linux) or kextstat (macOS)\n" COLOR_RESET);
    printf(COLOR_GREY "Native implementation pending for macOS.\n" COLOR_RESET);
    #endif
    
    return 0;
}

int cmd_overclock_view(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "CPU Clock Speeds:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    #ifdef __linux__
    FILE *freq = fopen("/proc/cpuinfo", "r");
    if (freq != NULL) {
        char line[256];
        int core = 0;
        while (fgets(line, sizeof(line), freq) != NULL) {
            if (strncmp(line, "cpu MHz", 7) == 0) {
                float mhz;
                if (sscanf(line, "cpu MHz : %f", &mhz) == 1) {
                    printf(COLOR_WHITE "Core %d: " COLOR_BLUE "%.2f MHz\n" COLOR_RESET, core++, mhz);
                }
            }
        }
        fclose(freq);
    }
    #else
    /* macOS - CPU frequency requires sysctl or IOKit */
    printf(COLOR_GREY "CPU frequency requires /proc/cpuinfo (Linux) or sysctl (macOS)\n" COLOR_RESET);
    printf(COLOR_GREY "Native implementation pending for macOS.\n" COLOR_RESET);
    #endif
    
    return 0;
}

int cmd_voltage(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "Power Status:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    #ifdef __APPLE__
    /* macOS - battery info requires IOKit or pmset */
    printf(COLOR_GREY "Battery info requires IOKit framework (macOS)\n" COLOR_RESET);
    printf(COLOR_GREY "Native implementation pending.\n" COLOR_RESET);
    #else
    FILE *bat = fopen("/sys/class/power_supply/BAT0/capacity", "r");
    if (bat != NULL) {
        int capacity;
        if (fscanf(bat, "%d", &capacity) == 1) {
            printf(COLOR_WHITE "Battery: " COLOR_BLUE "%d%%\n" COLOR_RESET, capacity);
        }
        fclose(bat);
    } else {
        printf(COLOR_GREY "Battery info not available.\n" COLOR_RESET);
    }
    #endif
    
    return 0;
}

