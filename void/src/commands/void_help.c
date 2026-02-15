#include "void_help.h"
#include "../utils/logger.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Command categories with descriptions */
typedef struct {
    const char *name;
    const char *description;
    const char **commands;
    int count;
} command_category_t;

/* Core Shell Commands */
static const char *core_commands[] = {
    "exit", "echo", "pwd", "cd", "whoami", "identity",
    "history", "history-search", "history-clear", "repeat", "time",
    "which", "whereis", "type", "help", "man", "info", "apropos", "whatis",
    "cat", "tac", "head", "tail", "less", "more", "tee", "sponge",
    "grep", "egrep", "fgrep", "sed", "awk", "cut", "paste", "join",
    "sort", "uniq", "wc", "tr", "fold", "fmt", "nl", "pr", "split", "csplit",
    NULL
};

/* Void OS Styled Commands */
static const char *void_commands[] = {
    "pulse", "warp", "vanish", "echoes", "shroud", "transmute",
    "veil", "seal", "watch-dir", "scavenge", "marrow", "dissect",
    "entropy-check", "fossil", "spawn", "jobs", "ps", "terminate",
    "foreground", "silence", "awaken", "env", "export", "unset",
    "reload", "alias", "unalias", "aliases", "mimic",
    "tether", "anchor", "sync", "transmit", "commit", "status",
    "context", "summon", "forgepy", "multitool", "fabricate",
    "refactor", "minify", "blueprint", "debug-vision",
    "intercept", "ping", "ports", "scan", "serve", "tunnel-info",
    "throb", "rift", "wormhole", "beacon", "trace-route", "ghost-ping",
    "entropy", "pulse-graph", "synapse", "organelle", "vitals",
    "tempest", "neural-map", "overclock-view", "voltage",
    "startup-check", "perm-audit", "net-watch", "incantation",
    "black-hole", "echo-chamber", "stasis", "black-box",
    "signal-lost", "chronos", "singularity", "glitch", "forge",
    NULL
};

/* Filesystem Operations */
static const char *filesystem_commands[] = {
    "touch", "mkdir", "rmdir", "rm", "cp", "mv", "ln", "symlink",
    "readlink", "stat", "file", "mime", "size", "du", "df", "find",
    "locate", "updatedb", "where", "realpath", "basename", "dirname",
    "pathjoin", "pathsplit", "pathnorm", "pathabs", "pathrel",
    "chmod", "chown", "chgrp", "umask", "getfacl", "setfacl",
    "lsattr", "chattr", "getfattr", "setfattr", "diff", "diff3",
    "sdiff", "cmp", "comm", "patch", "diffstat", "gzip", "gunzip",
    "bzip2", "bunzip2", "xz", "unxz", "zip", "unzip", "tar",
    "zcat", "zless", "zmore", "zcmp", "zdiff", "md5sum", "sha1sum",
    "sha256sum", "sha512sum", "cksum", "sum", "b2sum", "verify",
    "inotifywait", "inotifywatch", "fswatch", "entr", "dos2unix",
    "unix2dos", "mac2unix", "unix2mac", "iconv", "uuencode", "uudecode",
    "base64", "hexdump", "od", "xxd", "strings", "bin2hex", "hex2bin",
    "shred", "wipe", "srm", "secure-delete", "truncate", "fallocate",
    "mktemp", "tempfile",
    NULL
};

/* Process Management */
static const char *process_commands[] = {
    "kill", "killall", "pkill", "xkill", "killpg", "pkillall",
    "skill", "snice", "renice", "nice", "nohup", "setsid", "disown",
    "fg", "bg", "wait", "waitpid", "pstree", "pgrep", "pidof",
    "fuser", "lsof", "pmap", "procinfo", "pidstat", "top", "htop",
    "btop", "glances", "nmon", "iotop", "powertop", "nethogs",
    "strace", "ltrace", "ptrace", "gdb", "lldb", "valgrind", "perf",
    "timeout", "watch", "at", "atq", "atrm", "batch", "cron",
    "crontab", "anacron", "systemd-run", "systemctl", "ulimit",
    "prlimit", "limit", "quota", "edquota", "repquota",
    NULL
};

/* Networking */
static const char *network_commands[] = {
    "ping", "ping6", "fping", "hping", "traceroute", "tracepath",
    "mtr", "pathping", "ifconfig", "ip", "route", "netstat", "ss",
    "iwconfig", "iwlist", "nmcli", "nmtui", "wpa-cli", "hostname",
    "domainname", "dnsdomainname", "nmap", "masscan", "zmap",
    "arp-scan", "netdiscover", "nbtscan", "smbclient", "enum4linux",
    "tcpdump", "wireshark", "tshark", "ngrep", "iftop", "bmon",
    "vnstat", "iptraf", "nload", "slurm", "speedometer", "nethogs",
    "curl", "wget", "aria2c", "axel", "httpie", "http", "httpx",
    "feroxbuster", "gobuster", "dirb", "nikto", "sqlmap", "wpscan",
    "nslookup", "dig", "host", "getent", "resolveip", "dnsenum",
    "dnsrecon", "fierce", "sublist3r", "amass", "telnet", "nc",
    "ncat", "socat", "openssl", "ssh", "scp", "sftp", "rsync",
    "rsh", "rlogin", "ftp", "lftp", "wput", "iptables", "ip6tables",
    "ufw", "firewalld", "fail2ban-client", "tcpwrappers",
    NULL
};

/* WiFi & Network Management */
static const char *wifi_commands[] = {
    "wifi-scan", "wifi-connect", "wifi-disconnect", "wifi-status",
    "wifi-list", "wifi-on", "wifi-off", "net-up", "net-down",
    "net-restart", "net-status", "net-dhcp", "net-static",
    "net-test", "net-check", "dns-set", "dns-get", "dns-test",
    NULL
};

/* System Administration */
static const char *system_commands[] = {
    "uname", "arch", "uptime", "w", "who", "id", "groups", "last",
    "lastlog", "finger", "users", "logname", "vmstat", "iostat",
    "mpstat", "sar", "free", "lscpu", "lspci", "lsusb", "lsblk",
    "blkid", "fdisk", "parted", "gparted", "lsmod", "modinfo",
    "modprobe", "rmmod", "insmod", "depmod", "dmesg", "journalctl",
    "shutdown", "reboot", "halt", "poweroff", "init", "service",
    "chkconfig", "update-rc.d", "systemd-analyze", "systemd-cgtop",
    "systemd-cgls", "useradd", "userdel", "usermod", "passwd",
    "chage", "chfn", "chsh", "groupadd", "groupdel", "groupmod",
    "gpasswd", "newgrp", "su", "sudo", "visudo", "apt", "apt-get",
    "apt-cache", "dpkg", "rpm", "yum", "dnf", "pacman", "zypper",
    "emerge", "brew", "snap", "flatpak", "pip", "npm", "cargo", "go",
    "logwatch", "logrotate", "rsyslog", "syslog-ng",
    NULL
};

/* Text Processing */
static const char *text_commands[] = {
    "vi", "vim", "nano", "emacs", "ed", "expand", "unexpand",
    "col", "colrm", "column", "rev", "shuf", "most", "ag", "rg",
    "ack", "ugrep", "sift", "pt", "wc-l", "wc-w", "wc-c", "wc-m", "wc-L",
    NULL
};

/* Development Tools */
static const char *development_commands[] = {
    "svn", "hg", "bzr", "cvs", "fossil", "make", "cmake", "autoconf",
    "automake", "configure", "ninja", "bazel", "buck", "gradle",
    "maven", "ant", "scons", "waf", "gcc", "g++", "clang", "clang++",
    "rustc", "javac", "python", "python3", "ruby", "node", "perl",
    "php", "lua", "tclsh", "cppcheck", "clang-tidy", "pylint",
    "flake8", "mypy", "eslint", "jshint", "rubocop", "golint",
    "gometalinter", "staticcheck", "shellcheck", "clang-format",
    "indent", "astyle", "uncrustify", "black", "autopep8", "yapf",
    "prettier", "js-beautify", "gofmt", "rustfmt", "pytest",
    "unittest", "nose", "mocha", "jest", "rspec", "go-test",
    "cargo-test", "ctest",
    NULL
};

/* Database Operations */
static const char *database_commands[] = {
    "mysql", "mysqladmin", "mysqldump", "psql", "pg_dump", "pg_restore",
    "sqlite3", "sqlcmd", "mongo", "mongosh", "redis-cli",
    "cassandra-cli", "cqlsh", "hbase", "neo4j", "mongodump", "redis-dump",
    NULL
};

/* Security & Encryption */
static const char *security_commands[] = {
    "gpg", "gpg2", "pgp", "sha1sum", "sha512sum", "b2sum", "cksum",
    "sum", "burpsuite", "metasploit", "aircrack-ng", "reaver",
    "hashcat", "john", "hydra", "sqlmap", "nikto", "wpscan",
    "gobuster", "feroxbuster", "void-install-kali", "hosts.allow",
    "hosts.deny", "ssh-keygen", "ssh-add", "ssh-agent",
    "ssh-copy-id", "pass", "keepassxc-cli",
    NULL
};

/* System Utilities */
static const char *utility_commands[] = {
    "date", "cal", "ncal", "hwclock", "timedatectl", "tzselect", "zdump",
    NULL
};

/* Void OS Specific */
static const char *void_os_commands[] = {
    "void-info", "void-status", "void-health", "void-update",
    "void-backup", "void-restore", "void-config", "void-theme",
    "void-plugins", "void-install-plugin", "void-remove-plugin",
    "void-enable-plugin", "void-disable-plugin", "void-reload",
    "void-version", "void-license", "void-credits", "void-docs",
    "void-examples", "void-tutorial", "void-cheatsheet",
    "void-shortcuts", "void-aliases", "void-history", "void-stats",
    "void-benchmark", "void-test", "void-debug", "void-trace",
    "void-profile", "void-optimize", "void-clean", "void-verify",
    "void-repair", "void-reset", "void-export-config",
    "void-import-config", "void-migrate", "void-rollback",
    "void-snapshot", "void-restore-snapshot", "void-list-snapshots",
    "void-delete-snapshot", "void-compare-snapshots",
    "void-merge-snapshots", "void-clone", "void-sync",
    "void-backup-remote", "void-restore-remote", "void-help",
    NULL
};

/* Multimedia */
static const char *multimedia_commands[] = {
    "convert", "identify", "mogrify", "composite", "montage",
    "ffmpeg", "ffplay", "ffprobe", "imagemagick", "gimp", "inkscape",
    "sox", "mpg123", "mpg321", "ogg123", "flac", "lame", "opusenc",
    "opusdec", "vlc", "mplayer", "mpv",
    NULL
};

/* Archive & Compression */
static const char *archive_commands[] = {
    "lzma", "unlzma", "zstd", "unzstd", "lz4", "unlz4", "brotli",
    "unbrotli", "ar", "cpio", "pax",
    NULL
};

/* Miscellaneous */
static const char *misc_commands[] = {
    "fortune", "cowsay", "cowthink", "figlet", "toilet", "banner",
    "sl", "cmatrix", "hollywood", "rig", "random", "dice", "coin",
    "shuffle", "pick", "yes", "factor", "seq", "jot", "tldr",
    NULL
};

/* Count commands in array */
static int count_commands(const char **commands) {
    int count = 0;
    while (commands[count] != NULL) {
        count++;
    }
    return count;
}

/* Show help for a specific category */
void show_help_category(const char *category) {
    const char **commands = NULL;
    const char *description = "";
    int count = 0;
    
    if (strcmp(category, "core") == 0 || strcmp(category, "1") == 0) {
        commands = core_commands;
        description = "Core Shell Commands - Basic operations, navigation, text processing";
        count = count_commands(commands);
    } else if (strcmp(category, "void") == 0 || strcmp(category, "void-os") == 0) {
        commands = void_commands;
        description = "Void OS Styled Commands - Void-themed command names";
        count = count_commands(commands);
    } else if (strcmp(category, "filesystem") == 0 || strcmp(category, "fs") == 0 || strcmp(category, "2") == 0) {
        commands = filesystem_commands;
        description = "Filesystem Operations - File and directory management";
        count = count_commands(commands);
    } else if (strcmp(category, "process") == 0 || strcmp(category, "proc") == 0 || strcmp(category, "3") == 0) {
        commands = process_commands;
        description = "Process Management - Process control and monitoring";
        count = count_commands(commands);
    } else if (strcmp(category, "network") == 0 || strcmp(category, "net") == 0 || strcmp(category, "4") == 0) {
        commands = network_commands;
        description = "Networking - Network tools and utilities";
        count = count_commands(commands);
    } else if (strcmp(category, "wifi") == 0 || strcmp(category, "wireless") == 0) {
        commands = wifi_commands;
        description = "WiFi & Network Management - WiFi and network configuration";
        count = count_commands(commands);
    } else if (strcmp(category, "system") == 0 || strcmp(category, "sys") == 0 || strcmp(category, "5") == 0) {
        commands = system_commands;
        description = "System Administration - System management and monitoring";
        count = count_commands(commands);
    } else if (strcmp(category, "text") == 0 || strcmp(category, "6") == 0) {
        commands = text_commands;
        description = "Text Processing & Editing - Text manipulation and editors";
        count = count_commands(commands);
    } else if (strcmp(category, "development") == 0 || strcmp(category, "dev") == 0 || strcmp(category, "7") == 0) {
        commands = development_commands;
        description = "Development Tools - Compilers, build systems, testing";
        count = count_commands(commands);
    } else if (strcmp(category, "database") == 0 || strcmp(category, "db") == 0 || strcmp(category, "8") == 0) {
        commands = database_commands;
        description = "Database Operations - Database clients and tools";
        count = count_commands(commands);
    } else if (strcmp(category, "security") == 0 || strcmp(category, "sec") == 0 || strcmp(category, "9") == 0) {
        commands = security_commands;
        description = "Security & Encryption - Security tools and encryption";
        count = count_commands(commands);
    } else if (strcmp(category, "utility") == 0 || strcmp(category, "utils") == 0 || strcmp(category, "10") == 0) {
        commands = utility_commands;
        description = "System Utilities - Date, time, calendar utilities";
        count = count_commands(commands);
    } else if (strcmp(category, "void-os") == 0 || strcmp(category, "voidos") == 0 || strcmp(category, "11") == 0) {
        commands = void_os_commands;
        description = "Void OS Specific - Void OS management commands";
        count = count_commands(commands);
    } else if (strcmp(category, "multimedia") == 0 || strcmp(category, "media") == 0 || strcmp(category, "12") == 0) {
        commands = multimedia_commands;
        description = "Multimedia & Media - Image, audio, video tools";
        count = count_commands(commands);
    } else if (strcmp(category, "archive") == 0 || strcmp(category, "13") == 0) {
        commands = archive_commands;
        description = "Archive & Compression - Compression and archiving tools";
        count = count_commands(commands);
    } else if (strcmp(category, "misc") == 0 || strcmp(category, "miscellaneous") == 0 || strcmp(category, "14") == 0) {
        commands = misc_commands;
        description = "Miscellaneous Utilities - Fun and utility commands";
        count = count_commands(commands);
    } else {
        printf(COLOR_RED "Unknown category: %s\n" COLOR_RESET, category);
        printf(COLOR_GREY "Use 'void-help' to see all categories.\n" COLOR_RESET);
        return;
    }
    
    printf(COLOR_WHITE "%s\n" COLOR_RESET, description);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Total commands: %d\n\n" COLOR_RESET, count);
    
    int col = 0;
    for (int i = 0; commands[i] != NULL; i++) {
        printf(COLOR_WHITE "%-20s" COLOR_RESET, commands[i]);
        col++;
        if (col >= 4) {
            printf("\n");
            col = 0;
        }
    }
    if (col > 0) {
        printf("\n");
    }
    printf("\n");
}

/* Show all commands organized by category */
void show_all_commands(void) {
    printf(COLOR_WHITE "Void OS Elite - Complete Command Reference\n" COLOR_RESET);
    printf(COLOR_GREY "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf("\n");
    
    printf(COLOR_WHITE "CATEGORY 1: Core Shell Commands\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Basic operations, navigation, text processing\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; core_commands[i] != NULL; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, core_commands[i]);
    }
    printf("\n\n");
    
    printf(COLOR_WHITE "VOID OS STYLED COMMANDS\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Void-themed command names with unique aesthetics\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; void_commands[i] != NULL; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, void_commands[i]);
    }
    printf("\n\n");
    
    printf(COLOR_WHITE "CATEGORY 2: Filesystem Operations\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "File and directory management, compression, checksums\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    int fs_count = 0;
    for (int i = 0; filesystem_commands[i] != NULL && fs_count < 20; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, filesystem_commands[i]);
        fs_count++;
    }
    printf(COLOR_GREY "... (%d total)\n" COLOR_RESET, count_commands(filesystem_commands));
    printf("\n");
    
    printf(COLOR_WHITE "CATEGORY 3: Process Management\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Process control, monitoring, scheduling\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; process_commands[i] != NULL && i < 20; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, process_commands[i]);
    }
    printf(COLOR_GREY "... (%d total)\n" COLOR_RESET, count_commands(process_commands));
    printf("\n");
    
    printf(COLOR_WHITE "CATEGORY 4: Networking\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Network tools, scanning, protocols\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; network_commands[i] != NULL && i < 20; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, network_commands[i]);
    }
    printf(COLOR_GREY "... (%d total)\n" COLOR_RESET, count_commands(network_commands));
    printf("\n");
    
    printf(COLOR_WHITE "WiFi & Network Management\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "WiFi scanning, connection, network configuration\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; wifi_commands[i] != NULL; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, wifi_commands[i]);
    }
    printf("\n\n");
    
    printf(COLOR_WHITE "CATEGORY 5: System Administration\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "System management, user management, package management\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; system_commands[i] != NULL && i < 20; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, system_commands[i]);
    }
    printf(COLOR_GREY "... (%d total)\n" COLOR_RESET, count_commands(system_commands));
    printf("\n");
    
    printf(COLOR_WHITE "CATEGORY 6: Text Processing & Editing\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Text editors and text manipulation tools\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; text_commands[i] != NULL; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, text_commands[i]);
    }
    printf("\n\n");
    
    printf(COLOR_WHITE "CATEGORY 7: Development Tools\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Compilers, build systems, code analysis, testing\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; development_commands[i] != NULL && i < 20; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, development_commands[i]);
    }
    printf(COLOR_GREY "... (%d total)\n" COLOR_RESET, count_commands(development_commands));
    printf("\n");
    
    printf(COLOR_WHITE "CATEGORY 8: Database Operations\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Database clients and management tools\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; database_commands[i] != NULL; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, database_commands[i]);
    }
    printf("\n\n");
    
    printf(COLOR_WHITE "CATEGORY 9: Security & Encryption\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Security tools, encryption, Kali Linux tools\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; security_commands[i] != NULL; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, security_commands[i]);
    }
    printf("\n\n");
    
    printf(COLOR_WHITE "CATEGORY 10: System Utilities\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Date, time, calendar utilities\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; utility_commands[i] != NULL; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, utility_commands[i]);
    }
    printf("\n\n");
    
    printf(COLOR_WHITE "CATEGORY 11: Void OS Specific\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Void OS management and configuration\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; void_os_commands[i] != NULL && i < 20; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, void_os_commands[i]);
    }
    printf(COLOR_GREY "... (%d total)\n" COLOR_RESET, count_commands(void_os_commands));
    printf("\n");
    
    printf(COLOR_WHITE "CATEGORY 12: Multimedia & Media\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Image, audio, video processing tools\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; multimedia_commands[i] != NULL; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, multimedia_commands[i]);
    }
    printf("\n\n");
    
    printf(COLOR_WHITE "CATEGORY 13: Archive & Compression\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Compression and archiving tools\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; archive_commands[i] != NULL; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, archive_commands[i]);
    }
    printf("\n\n");
    
    printf(COLOR_WHITE "CATEGORY 14: Miscellaneous Utilities\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Fun and utility commands\n" COLOR_RESET);
    printf(COLOR_GREY "Commands: " COLOR_RESET);
    for (int i = 0; misc_commands[i] != NULL; i++) {
        printf(COLOR_WHITE "%s " COLOR_RESET, misc_commands[i]);
    }
    printf("\n\n");
    
    printf(COLOR_GREY "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf(COLOR_WHITE "Total Commands: 1,000+\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'void-help <category>' to see commands in a specific category.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'void-help <command>' for help on a specific command.\n" COLOR_RESET);
}

/* Main help command */
int cmd_void_help(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    
    if (argc < 2) {
        /* Show main help menu */
        printf(COLOR_WHITE "Void OS Elite - Help System\n" COLOR_RESET);
        printf(COLOR_GREY "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
        printf("\n");
        printf(COLOR_WHITE "Usage:\n" COLOR_RESET);
        printf(COLOR_GREY "  void-help                    - Show this help menu\n" COLOR_RESET);
        printf(COLOR_GREY "  void-help all                - Show all commands by category\n" COLOR_RESET);
        printf(COLOR_GREY "  void-help <category>         - Show commands in a category\n" COLOR_RESET);
        printf(COLOR_GREY "  void-help <command>          - Show help for a specific command\n" COLOR_RESET);
        printf("\n");
        printf(COLOR_WHITE "Categories:\n" COLOR_RESET);
        printf(COLOR_GREY "  1, core                     - Core Shell Commands\n" COLOR_RESET);
        printf(COLOR_GREY "  void, void-os               - Void OS Styled Commands\n" COLOR_RESET);
        printf(COLOR_GREY "  2, filesystem, fs           - Filesystem Operations\n" COLOR_RESET);
        printf(COLOR_GREY "  3, process, proc            - Process Management\n" COLOR_RESET);
        printf(COLOR_GREY "  4, network, net             - Networking\n" COLOR_RESET);
        printf(COLOR_GREY "  wifi, wireless              - WiFi & Network Management\n" COLOR_RESET);
        printf(COLOR_GREY "  5, system, sys              - System Administration\n" COLOR_RESET);
        printf(COLOR_GREY "  6, text                     - Text Processing & Editing\n" COLOR_RESET);
        printf(COLOR_GREY "  7, development, dev         - Development Tools\n" COLOR_RESET);
        printf(COLOR_GREY "  8, database, db             - Database Operations\n" COLOR_RESET);
        printf(COLOR_GREY "  9, security, sec            - Security & Encryption\n" COLOR_RESET);
        printf(COLOR_GREY "  10, utility, utils          - System Utilities\n" COLOR_RESET);
        printf(COLOR_GREY "  11, void-os, voidos         - Void OS Specific\n" COLOR_RESET);
        printf(COLOR_GREY "  12, multimedia, media       - Multimedia & Media\n" COLOR_RESET);
        printf(COLOR_GREY "  13, archive                 - Archive & Compression\n" COLOR_RESET);
        printf(COLOR_GREY "  14, misc, miscellaneous     - Miscellaneous Utilities\n" COLOR_RESET);
        printf("\n");
        printf(COLOR_WHITE "Examples:\n" COLOR_RESET);
        printf(COLOR_GREY "  void-help all\n" COLOR_RESET);
        printf(COLOR_GREY "  void-help network\n" COLOR_RESET);
        printf(COLOR_GREY "  void-help wifi\n" COLOR_RESET);
        printf(COLOR_GREY "  void-help sqlmap\n" COLOR_RESET);
        printf("\n");
        return 0;
    }
    
    const char *arg = argv[1];
    
    if (strcmp(arg, "all") == 0) {
        show_all_commands();
        return 0;
    }
    
    /* Check if it's a category */
    if (strcmp(arg, "core") == 0 || strcmp(arg, "1") == 0 ||
        strcmp(arg, "void") == 0 || strcmp(arg, "void-os") == 0 ||
        strcmp(arg, "filesystem") == 0 || strcmp(arg, "fs") == 0 || strcmp(arg, "2") == 0 ||
        strcmp(arg, "process") == 0 || strcmp(arg, "proc") == 0 || strcmp(arg, "3") == 0 ||
        strcmp(arg, "network") == 0 || strcmp(arg, "net") == 0 || strcmp(arg, "4") == 0 ||
        strcmp(arg, "wifi") == 0 || strcmp(arg, "wireless") == 0 ||
        strcmp(arg, "system") == 0 || strcmp(arg, "sys") == 0 || strcmp(arg, "5") == 0 ||
        strcmp(arg, "text") == 0 || strcmp(arg, "6") == 0 ||
        strcmp(arg, "development") == 0 || strcmp(arg, "dev") == 0 || strcmp(arg, "7") == 0 ||
        strcmp(arg, "database") == 0 || strcmp(arg, "db") == 0 || strcmp(arg, "8") == 0 ||
        strcmp(arg, "security") == 0 || strcmp(arg, "sec") == 0 || strcmp(arg, "9") == 0 ||
        strcmp(arg, "utility") == 0 || strcmp(arg, "utils") == 0 || strcmp(arg, "10") == 0 ||
        strcmp(arg, "void-os") == 0 || strcmp(arg, "voidos") == 0 || strcmp(arg, "11") == 0 ||
        strcmp(arg, "multimedia") == 0 || strcmp(arg, "media") == 0 || strcmp(arg, "12") == 0 ||
        strcmp(arg, "archive") == 0 || strcmp(arg, "13") == 0 ||
        strcmp(arg, "misc") == 0 || strcmp(arg, "miscellaneous") == 0 || strcmp(arg, "14") == 0) {
        show_help_category(arg);
        return 0;
    }
    
    /* Otherwise, show help for specific command */
    printf(COLOR_WHITE "Help for command: %s\n" COLOR_RESET, arg);
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_GREY "Command-specific help is being developed.\n" COLOR_RESET);
    printf(COLOR_GREY "For now, try running the command with no arguments or use 'man %s'.\n" COLOR_RESET, arg);
    return 0;
}

