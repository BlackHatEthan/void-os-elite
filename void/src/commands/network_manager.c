#include "network_manager.h"
#include "network_ext.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../utils/http_client.h"
#include "../utils/ping_native.h"
#include "../utils/wifi_native.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>

/* WiFi Management */

int cmd_wifi_scan(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "Scanning for WiFi networks...\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────\n" COLOR_RESET);
    
    /* Prefer native scan (no host tools): uses kernel nl80211 on Linux */
    if (wifi_native_available() && wifi_native_scan(NULL) == 0) {
        return 0;
    }
    
    /* Fallback: host tools (iw/iwlist/nmcli) only if native failed or unavailable */
    pid_t pid = fork();
    if (pid == 0) {
        if (execlp("iw", "iw", "dev", "wlan0", "scan", NULL) != 0) {
            if (execlp("iwlist", "iwlist", "wlan0", "scan", NULL) != 0) {
                execlp("nmcli", "nmcli", "-t", "-f", "SSID,SIGNAL,SECURITY", "device", "wifi", "list", NULL);
                exit(1);
            }
        }
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return 0;
        }
    }
    
    printf(COLOR_GREY "WiFi scanning requires a WiFi interface (wlan0/wlp*) and kernel nl80211 support.\n" COLOR_RESET);
    printf(COLOR_GREY "Or install: wireless-tools, iw, or network-manager.\n" COLOR_RESET);
    return 1;
}

int cmd_wifi_connect(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    
    if (argc < 2) {
        printf(COLOR_GREY "Usage: wifi-connect <SSID> [password]\n" COLOR_RESET);
        printf(COLOR_GREY "Example: wifi-connect MyNetwork mypassword\n" COLOR_RESET);
        return 1;
    }
    
    const char *ssid = argv[1];
    const char *password = argc > 2 ? argv[2] : NULL;
    
    printf(COLOR_WHITE "Connecting to WiFi: %s\n" COLOR_RESET, ssid);
    
    /* Try using nmcli (NetworkManager) */
    pid_t pid = fork();
    if (pid == 0) {
        if (password != NULL) {
            char cmd[512];
            snprintf(cmd, sizeof(cmd), "nmcli device wifi connect '%s' password '%s'", ssid, password);
            execl("/bin/sh", "sh", "-c", cmd, NULL);
        } else {
            char cmd[256];
            snprintf(cmd, sizeof(cmd), "nmcli device wifi connect '%s'", ssid);
            execl("/bin/sh", "sh", "-c", cmd, NULL);
        }
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf(COLOR_GREEN "Connected to %s\n" COLOR_RESET, ssid);
            return 0;
        }
    }
    
    /* Try wpa_supplicant method */
    printf(COLOR_GREY "Attempting connection via wpa_supplicant...\n" COLOR_RESET);
    printf(COLOR_GREY "WiFi connection requires NetworkManager or wpa_supplicant.\n" COLOR_RESET);
    printf(COLOR_GREY "Install with: sudo apt install network-manager\n" COLOR_RESET);
    return 1;
}

int cmd_wifi_disconnect(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "Disconnecting from WiFi...\n" COLOR_RESET);
    
    pid_t pid = fork();
    if (pid == 0) {
        execlp("nmcli", "nmcli", "device", "disconnect", "wlan0", NULL);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf(COLOR_GREEN "Disconnected\n" COLOR_RESET);
            return 0;
        }
    }
    
    printf(COLOR_GREY "WiFi disconnection requires NetworkManager.\n" COLOR_RESET);
    return 1;
}

int cmd_wifi_status(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "WiFi Status:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────\n" COLOR_RESET);
    
    /* Check interface status */
    struct ifaddrs *ifaddr, *ifa;
    bool found_wifi = false;
    
    if (getifaddrs(&ifaddr) == 0) {
        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL) continue;
            
            /* Check for wireless interfaces (wlan, wlp, etc.) */
            if (strstr(ifa->ifa_name, "wlan") != NULL || 
                strstr(ifa->ifa_name, "wlp") != NULL ||
                strstr(ifa->ifa_name, "wifi") != NULL) {
                found_wifi = true;
                printf(COLOR_WHITE "Interface: %s\n" COLOR_RESET, ifa->ifa_name);
                
                if (ifa->ifa_addr->sa_family == AF_INET) {
                    struct sockaddr_in *sin = (struct sockaddr_in *)ifa->ifa_addr;
                    char ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &sin->sin_addr, ip, INET_ADDRSTRLEN);
                    printf(COLOR_GREY "  IP Address: %s\n" COLOR_RESET, ip);
                }
                
                /* Try to get SSID via iw/nmcli */
                pid_t pid = fork();
                if (pid == 0) {
                    char cmd[256];
                    snprintf(cmd, sizeof(cmd), "iw dev %s link | grep SSID", ifa->ifa_name);
                    execl("/bin/sh", "sh", "-c", cmd, NULL);
                    exit(1);
                } else if (pid > 0) {
                    waitpid(pid, NULL, 0);
                }
            }
        }
        freeifaddrs(ifaddr);
    }
    
    if (!found_wifi) {
        printf(COLOR_GREY "No WiFi interfaces found.\n" COLOR_RESET);
    }
    
    return 0;
}

int cmd_wifi_list(int argc, char **argv, shell_context_t *ctx) {
    return cmd_wifi_scan(argc, argv, ctx);
}

int cmd_wifi_on(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "Enabling WiFi...\n" COLOR_RESET);
    
    pid_t pid = fork();
    if (pid == 0) {
        execlp("nmcli", "nmcli", "radio", "wifi", "on", NULL);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf(COLOR_GREEN "WiFi enabled\n" COLOR_RESET);
            return 0;
        }
    }
    
    printf(COLOR_GREY "WiFi control requires NetworkManager.\n" COLOR_RESET);
    return 1;
}

int cmd_wifi_off(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "Disabling WiFi...\n" COLOR_RESET);
    
    pid_t pid = fork();
    if (pid == 0) {
        execlp("nmcli", "nmcli", "radio", "wifi", "off", NULL);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf(COLOR_GREEN "WiFi disabled\n" COLOR_RESET);
            return 0;
        }
    }
    
    printf(COLOR_GREY "WiFi control requires NetworkManager.\n" COLOR_RESET);
    return 1;
}

/* Network Interface Management */

int cmd_net_up(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    
    if (argc < 2) {
        printf(COLOR_GREY "Usage: net-up <interface>\n" COLOR_RESET);
        printf(COLOR_GREY "Example: net-up eth0\n" COLOR_RESET);
        return 1;
    }
    
    const char *interface = argv[1];
    printf(COLOR_WHITE "Bringing up interface: %s\n" COLOR_RESET, interface);
    
    pid_t pid = fork();
    if (pid == 0) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "ip link set %s up", interface);
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        /* Fallback to ifconfig */
        snprintf(cmd, sizeof(cmd), "ifconfig %s up", interface);
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf(COLOR_GREEN "Interface %s is up\n" COLOR_RESET, interface);
            return 0;
        }
    }
    
    printf(COLOR_GREY "Failed to bring up interface.\n" COLOR_RESET);
    return 1;
}

int cmd_net_down(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    
    if (argc < 2) {
        printf(COLOR_GREY "Usage: net-down <interface>\n" COLOR_RESET);
        return 1;
    }
    
    const char *interface = argv[1];
    printf(COLOR_WHITE "Bringing down interface: %s\n" COLOR_RESET, interface);
    
    pid_t pid = fork();
    if (pid == 0) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "ip link set %s down", interface);
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        snprintf(cmd, sizeof(cmd), "ifconfig %s down", interface);
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf(COLOR_GREEN "Interface %s is down\n" COLOR_RESET, interface);
            return 0;
        }
    }
    
    return 1;
}

int cmd_net_restart(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    
    if (argc < 2) {
        printf(COLOR_GREY "Usage: net-restart <interface>\n" COLOR_RESET);
        return 1;
    }
    
    const char *interface = argv[1];
    printf(COLOR_WHITE "Restarting interface: %s\n" COLOR_RESET, interface);
    
    cmd_net_down(2, (char*[]){"net-down", interface}, ctx);
    sleep(1);
    return cmd_net_up(2, (char*[]){"net-up", interface}, ctx);
}

int cmd_net_status(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "Network Interfaces Status:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────\n" COLOR_RESET);
    
    return cmd_ifconfig(1, (char*[]){"ifconfig"}, ctx);
}

int cmd_net_dhcp(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    
    if (argc < 2) {
        printf(COLOR_GREY "Usage: net-dhcp <interface>\n" COLOR_RESET);
        printf(COLOR_GREY "Example: net-dhcp eth0\n" COLOR_RESET);
        return 1;
    }
    
    const char *interface = argv[1];
    printf(COLOR_WHITE "Configuring %s for DHCP...\n" COLOR_RESET, interface);
    
    /* Try dhcpcd */
    pid_t pid = fork();
    if (pid == 0) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "dhcpcd %s", interface);
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        /* Fallback to dhclient */
        snprintf(cmd, sizeof(cmd), "dhclient %s", interface);
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf(COLOR_GREEN "DHCP configured for %s\n" COLOR_RESET, interface);
            return 0;
        }
    }
    
    printf(COLOR_GREY "DHCP configuration requires dhcpcd or dhclient.\n" COLOR_RESET);
    return 1;
}

int cmd_net_static(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    
    if (argc < 4) {
        printf(COLOR_GREY "Usage: net-static <interface> <IP> <netmask> [gateway]\n" COLOR_RESET);
        printf(COLOR_GREY "Example: net-static eth0 192.168.1.100 255.255.255.0 192.168.1.1\n" COLOR_RESET);
        return 1;
    }
    
    const char *interface = argv[1];
    const char *ip = argv[2];
    const char *netmask = argv[3];
    const char *gateway = argc > 4 ? argv[4] : NULL;
    
    printf(COLOR_WHITE "Configuring static IP for %s...\n" COLOR_RESET, interface);
    printf(COLOR_GREY "  IP: %s\n" COLOR_RESET, ip);
    printf(COLOR_GREY "  Netmask: %s\n" COLOR_RESET, netmask);
    if (gateway) {
        printf(COLOR_GREY "  Gateway: %s\n" COLOR_RESET, gateway);
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        char cmd[512];
        if (gateway) {
            snprintf(cmd, sizeof(cmd), "ip addr add %s/%s dev %s && ip route add default via %s", 
                     ip, netmask, interface, gateway);
        } else {
            snprintf(cmd, sizeof(cmd), "ip addr add %s/%s dev %s", ip, netmask, interface);
        }
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf(COLOR_GREEN "Static IP configured\n" COLOR_RESET);
            return 0;
        }
    }
    
    printf(COLOR_GREY "Static IP configuration requires ip or ifconfig.\n" COLOR_RESET);
    return 1;
}

/* Connectivity Testing */

int cmd_net_test(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "Testing Internet Connectivity...\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────\n" COLOR_RESET);
    
    /* Test DNS */
    printf(COLOR_GREY "Testing DNS... " COLOR_RESET);
    struct hostent *he = gethostbyname("google.com");
    if (he != NULL) {
        printf(COLOR_GREEN "OK\n" COLOR_RESET);
    } else {
        printf(COLOR_RED "FAILED\n" COLOR_RESET);
    }
    
    /* Test HTTP connectivity */
    printf(COLOR_GREY "Testing HTTP... " COLOR_RESET);
    http_response_t *resp = http_get("http://www.google.com");
    if (resp != NULL && resp->status_code == 200) {
        printf(COLOR_GREEN "OK\n" COLOR_RESET);
        http_response_free(resp);
    } else {
        printf(COLOR_RED "FAILED\n" COLOR_RESET);
        if (resp) {
            http_response_free(resp);
        }
    }
    
    /* Test ping */
    printf(COLOR_GREY "Testing ping... " COLOR_RESET);
    if (ping_host("8.8.8.8", 1, 5000) == 0) {
        printf(COLOR_GREEN "OK\n" COLOR_RESET);
    } else {
        printf(COLOR_RED "FAILED\n" COLOR_RESET);
    }
    
    return 0;
}

int cmd_net_check(int argc, char **argv, shell_context_t *ctx) {
    return cmd_net_test(argc, argv, ctx);
}

/* DNS Management */

int cmd_dns_set(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    
    if (argc < 3) {
        printf(COLOR_GREY "Usage: dns-set <dns1> [dns2]\n" COLOR_RESET);
        printf(COLOR_GREY "Example: dns-set 8.8.8.8 8.8.4.4\n" COLOR_RESET);
        return 1;
    }
    
    const char *dns1 = argv[1];
    const char *dns2 = argc > 2 ? argv[2] : NULL;
    
    printf(COLOR_WHITE "Setting DNS servers...\n" COLOR_RESET);
    printf(COLOR_GREY "  Primary: %s\n" COLOR_RESET, dns1);
    if (dns2) {
        printf(COLOR_GREY "  Secondary: %s\n" COLOR_RESET, dns2);
    }
    
    /* Write to /etc/resolv.conf */
    FILE *fp = fopen("/etc/resolv.conf", "w");
    if (fp != NULL) {
        fprintf(fp, "nameserver %s\n", dns1);
        if (dns2) {
            fprintf(fp, "nameserver %s\n", dns2);
        }
        fclose(fp);
        printf(COLOR_GREEN "DNS servers configured\n" COLOR_RESET);
        return 0;
    } else {
        printf(COLOR_GREY "Cannot write to /etc/resolv.conf (requires root)\n" COLOR_RESET);
        return 1;
    }
}

int cmd_dns_get(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    (void)argc;
    (void)argv;
    
    printf(COLOR_WHITE "Current DNS Servers:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────────\n" COLOR_RESET);
    
    FILE *fp = fopen("/etc/resolv.conf", "r");
    if (fp != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), fp) != NULL) {
            if (strncmp(line, "nameserver", 10) == 0) {
                char *dns = line + 11;
                while (*dns == ' ' || *dns == '\t') dns++;
                char *end = strchr(dns, '\n');
                if (end) *end = '\0';
                printf(COLOR_WHITE "  %s\n" COLOR_RESET, dns);
            }
        }
        fclose(fp);
        return 0;
    } else {
        printf(COLOR_GREY "Cannot read /etc/resolv.conf\n" COLOR_RESET);
        return 1;
    }
}

int cmd_dns_test(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    
    const char *hostname = argc > 1 ? argv[1] : "google.com";
    
    printf(COLOR_WHITE "Testing DNS resolution for: %s\n" COLOR_RESET, hostname);
    
    struct hostent *he = gethostbyname(hostname);
    if (he != NULL) {
        struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;
        printf(COLOR_GREEN "Resolved to: " COLOR_RESET);
        for (int i = 0; addr_list[i] != NULL; i++) {
            printf(COLOR_WHITE "%s " COLOR_RESET, inet_ntoa(*addr_list[i]));
        }
        printf("\n");
        return 0;
    } else {
        printf(COLOR_RED "DNS resolution failed\n" COLOR_RESET);
        return 1;
    }
}

