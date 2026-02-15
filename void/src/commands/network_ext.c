#include "network_ext.h"
#include "network.h"
#include "kali_tools.h"
#include "network_manager.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../utils/http_client.h"
#include "../utils/ping_native.h"

/* Forward declarations */
extern int cmd_trace_route(int argc, char **argv, shell_context_t *ctx);
extern int cmd_rift(int argc, char **argv, shell_context_t *ctx);
extern int cmd_wormhole(int argc, char **argv, shell_context_t *ctx);
extern int cmd_scan(int argc, char **argv, shell_context_t *ctx);
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
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/select.h>

/* Basic Network Operations */
int cmd_ping6(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: ping6 <host>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "IPv6 ping requires IPv6 socket implementation.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'ping' for IPv4.\n" COLOR_RESET);
    return 1;
}

int cmd_fping(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: fping <host1> [host2...]\n" COLOR_RESET);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        printf(COLOR_WHITE "%s: " COLOR_RESET, argv[i]);
        int result = ping_host(argv[i], 1, 5000);
        if (result == 0) {
            printf(COLOR_GREEN "alive\n" COLOR_RESET);
        } else {
            printf(COLOR_RED "unreachable\n" COLOR_RESET);
        }
    }
    
    return 0;
}

int cmd_hping(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: hping <host> [options]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Hping requires advanced packet crafting.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'ping' for basic ICMP ping.\n" COLOR_RESET);
    return 1;
}

int cmd_traceroute_ext(int argc, char **argv, shell_context_t *ctx) {
    return cmd_trace_route(argc, argv, ctx);
}

int cmd_tracepath(int argc, char **argv, shell_context_t *ctx) {
    return cmd_trace_route(argc, argv, ctx);
}

int cmd_mtr(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: mtr <host>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "MTR requires interactive terminal UI.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'trace-route' for basic route tracing.\n" COLOR_RESET);
    return 1;
}

int cmd_pathping(int argc, char **argv, shell_context_t *ctx) {
    return cmd_trace_route(argc, argv, ctx);
}

/* Network Configuration */
int cmd_ifconfig(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    struct ifaddrs *ifaddr, *ifa;
    
    if (getifaddrs(&ifaddr) == -1) {
        printf(COLOR_GREY "Cannot get network interfaces\n" COLOR_RESET);
        return 1;
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        
        printf(COLOR_WHITE "%s" COLOR_RESET, ifa->ifa_name);
        
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sin = (struct sockaddr_in *)ifa->ifa_addr;
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &sin->sin_addr, ip, INET_ADDRSTRLEN);
            printf(COLOR_GREY "  IPv4: %s" COLOR_RESET, ip);
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)ifa->ifa_addr;
            char ip[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &sin6->sin6_addr, ip, INET6_ADDRSTRLEN);
            printf(COLOR_GREY "  IPv6: %s" COLOR_RESET, ip);
        }
        
        printf("\n");
    }
    
    freeifaddrs(ifaddr);
    return 0;
}

int cmd_ip_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: ip <command> [args...]\n" COLOR_RESET);
        printf(COLOR_GREY "Commands: addr, link, route, etc.\n" COLOR_RESET);
        return 1;
    }
    
    if (strcmp(argv[1], "addr") == 0 || strcmp(argv[1], "a") == 0) {
        return cmd_ifconfig(1, (char*[]){"ifconfig"}, ctx);
    } else if (strcmp(argv[1], "route") == 0 || strcmp(argv[1], "r") == 0) {
        return cmd_route_ext(1, (char*[]){"route"}, ctx);
    }
    
    printf(COLOR_GREY "IP command requires netlink socket implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_route_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    FILE *fp = fopen("/proc/net/route", "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot read routing table\n" COLOR_RESET);
        return 1;
    }
    
    char line[256];
    printf(COLOR_WHITE "%-16s %-16s %-16s %-8s %-8s\n" COLOR_RESET,
           "Destination", "Gateway", "Genmask", "Flags", "Iface");
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    
    /* Skip header */
    if (fgets(line, sizeof(line), fp) != NULL) {
        while (fgets(line, sizeof(line), fp) != NULL) {
            char iface[16], dest[32], gw[32], flags[8];
            unsigned long d, g, m;
            int f, mtu, window, irtt;
            
            if (sscanf(line, "%s %lx %lx %x %d %d %d %lx",
                      iface, &d, &g, &f, &mtu, &window, &irtt, &m) >= 4) {
                struct in_addr addr;
                addr.s_addr = d;
                inet_ntop(AF_INET, &addr, dest, sizeof(dest));
                addr.s_addr = g;
                inet_ntop(AF_INET, &addr, gw, sizeof(gw));
                addr.s_addr = m;
                char mask[32];
                inet_ntop(AF_INET, &addr, mask, sizeof(mask));
                
                snprintf(flags, sizeof(flags), "%04x", f);
                printf(COLOR_WHITE "%-16s %-16s %-16s %-8s %-8s\n" COLOR_RESET,
                       dest, gw, mask, flags, iface);
            }
        }
    }
    
    fclose(fp);
    return 0;
}

int cmd_netstat(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    bool show_listening = false;
    bool show_all = false;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--listening") == 0) {
            show_listening = true;
        } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) {
            show_all = true;
        }
    }
    
    /* Read /proc/net/tcp */
    FILE *fp = fopen("/proc/net/tcp", "r");
    if (fp == NULL) {
        printf(COLOR_GREY "Cannot read network connections\n" COLOR_RESET);
        return 1;
    }
    
    char line[4096];
    printf(COLOR_WHITE "%-5s %-20s %-20s %-10s\n" COLOR_RESET,
           "Proto", "Local Address", "Remote Address", "State");
    printf(COLOR_GREY "─────────────────────────────────────────────────────────────\n" COLOR_RESET);
    
    /* Skip header */
    if (fgets(line, sizeof(line), fp) != NULL) {
        while (fgets(line, sizeof(line), fp) != NULL) {
            unsigned long local_addr, remote_addr;
            int local_port, remote_port, state;
            
            if (sscanf(line, "%*d: %lx:%x %lx:%x %x", 
                      &local_addr, &local_port, &remote_addr, &remote_port, &state) >= 5) {
                struct in_addr addr;
                char local[64], remote[64];
                
                addr.s_addr = local_addr;
                inet_ntop(AF_INET, &addr, local, sizeof(local));
                snprintf(local + strlen(local), sizeof(local) - strlen(local), ":%d", ntohs(local_port));
                
                addr.s_addr = remote_addr;
                inet_ntop(AF_INET, &addr, remote, sizeof(remote));
                snprintf(remote + strlen(remote), sizeof(remote) - strlen(remote), ":%d", ntohs(remote_port));
                
                const char *state_str = "UNKNOWN";
                if (state == 1) state_str = "ESTABLISHED";
                else if (state == 2) state_str = "SYN_SENT";
                else if (state == 3) state_str = "SYN_RECV";
                else if (state == 4) state_str = "FIN_WAIT1";
                else if (state == 5) state_str = "FIN_WAIT2";
                else if (state == 6) state_str = "TIME_WAIT";
                else if (state == 7) state_str = "CLOSE";
                else if (state == 8) state_str = "CLOSE_WAIT";
                else if (state == 9) state_str = "LAST_ACK";
                else if (state == 10) state_str = "LISTEN";
                
                if (show_listening && state != 10) continue;
                if (!show_all && state != 1 && state != 10) continue;
                
                printf(COLOR_WHITE "tcp   %-20s %-20s %-10s\n" COLOR_RESET,
                       local, remote, state_str);
            }
        }
    }
    
    fclose(fp);
    return 0;
}

int cmd_ss(int argc, char **argv, shell_context_t *ctx) {
    return cmd_netstat(argc, argv, ctx);
}

int cmd_iwconfig(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Iwconfig requires wireless extensions.\n" COLOR_RESET);
    return 1;
}

int cmd_iwlist(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Iwlist requires wireless extensions.\n" COLOR_RESET);
    return 1;
}

int cmd_nmcli(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Nmcli requires NetworkManager.\n" COLOR_RESET);
    return 1;
}

int cmd_nmtui(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Nmtui requires NetworkManager TUI.\n" COLOR_RESET);
    return 1;
}

int cmd_wpa_cli(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Wpa_cli requires WPA supplicant.\n" COLOR_RESET);
    return 1;
}

int cmd_hostname_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc > 1) {
        if (sethostname(argv[1], strlen(argv[1])) != 0) {
            printf(COLOR_GREY "Cannot set hostname: %s\n" COLOR_RESET, strerror(errno));
            return 1;
        }
        printf(COLOR_GREEN "Hostname set to: %s\n" COLOR_RESET, argv[1]);
        return 0;
    }
    
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, hostname);
        return 0;
    }
    
    return 1;
}

int cmd_domainname(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Domain name requires NIS/YP support.\n" COLOR_RESET);
    return 1;
}

int cmd_dnsdomainname(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    struct hostent *he = gethostbyname("localhost");
    if (he != NULL && he->h_name != NULL) {
        char *dot = strchr(he->h_name, '.');
        if (dot != NULL) {
            printf(COLOR_WHITE "%s\n" COLOR_RESET, dot + 1);
            return 0;
        }
    }
    
    printf(COLOR_GREY "Cannot determine DNS domain\n" COLOR_RESET);
    return 1;
}

/* Network Scanning & Discovery */
int cmd_nmap_ext(int argc, char **argv, shell_context_t *ctx) {
    /* Check if nmap is installed via Kali tools */
    extern int check_kali_tool_installed(const char *tool_name);
    extern int execute_kali_tool(const char *tool_name, int argc, char **argv, shell_context_t *ctx);
    
    if (check_kali_tool_installed("nmap")) {
        /* Use installed nmap */
        char *new_argv[argc + 1];
        new_argv[0] = "nmap";
        for (int i = 1; i < argc; i++) {
            new_argv[i] = argv[i];
        }
        new_argv[argc] = NULL;
        return execute_kali_tool("nmap", argc, new_argv, ctx);
    } else {
        /* Fall back to native scan */
        printf(COLOR_GREY "Nmap not installed. Using native port scanner.\n" COLOR_RESET);
        printf(COLOR_GREY "Install full nmap with: void-install-kali nmap\n" COLOR_RESET);
        return cmd_scan(argc, argv, ctx);
    }
}

int cmd_masscan(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: masscan <target>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Masscan requires high-performance packet generation.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'scan' for basic port scanning.\n" COLOR_RESET);
    return 1;
}

int cmd_zmap(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Zmap requires internet-scale scanning capabilities.\n" COLOR_RESET);
    return 1;
}

int cmd_arp_scan(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: arp-scan <network>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "ARP scan requires raw socket access.\n" COLOR_RESET);
    return 1;
}

int cmd_netdiscover(int argc, char **argv, shell_context_t *ctx) {
    return cmd_arp_scan(argc, argv, ctx);
}

int cmd_nbtscan(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "NBTScan requires NetBIOS protocol implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_smbclient(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "SMB client requires SMB/CIFS protocol implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_enum4linux(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Enum4linux requires SMB enumeration tools.\n" COLOR_RESET);
    return 1;
}

/* Network Monitoring */
int cmd_tcpdump(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Tcpdump requires raw socket and packet capture.\n" COLOR_RESET);
    return 1;
}

int cmd_wireshark(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Wireshark requires GUI and packet capture library.\n" COLOR_RESET);
    return 1;
}

int cmd_tshark(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Tshark requires packet capture library.\n" COLOR_RESET);
    return 1;
}

int cmd_ngrep(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Ngrep requires packet capture and pattern matching.\n" COLOR_RESET);
    return 1;
}

int cmd_iftop(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Iftop requires interactive terminal UI.\n" COLOR_RESET);
    return 1;
}

int cmd_nethogs_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Nethogs requires per-process network statistics.\n" COLOR_RESET);
    return 1;
}

int cmd_bmon(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Bmon requires interactive terminal UI.\n" COLOR_RESET);
    return 1;
}

int cmd_vnstat(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Vnstat requires network statistics database.\n" COLOR_RESET);
    return 1;
}

int cmd_iptraf(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Iptraf requires interactive terminal UI.\n" COLOR_RESET);
    return 1;
}

int cmd_nload(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Nload requires interactive terminal UI.\n" COLOR_RESET);
    return 1;
}

int cmd_slurm(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Slurm requires interactive terminal UI.\n" COLOR_RESET);
    return 1;
}

int cmd_speedometer(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Speedometer requires interactive terminal UI.\n" COLOR_RESET);
    return 1;
}

/* Network Services */
int cmd_curl_ext(int argc, char **argv, shell_context_t *ctx) {
    return cmd_intercept(argc, argv, ctx);
}

int cmd_wget(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: wget <url> [output]\n" COLOR_RESET);
        return 1;
    }
    
    const char *url = argv[1];
    const char *output = (argc > 2) ? argv[2] : NULL;
    
    http_response_t *resp = http_get(url);
    if (resp == NULL) {
        printf(COLOR_GREY "Failed to download: %s\n" COLOR_RESET, url);
        return 1;
    }
    
    if (output != NULL) {
        FILE *fp = fopen(output, "wb");
        if (fp != NULL) {
            fwrite(resp->body, 1, resp->body_len, fp);
            fclose(fp);
            printf(COLOR_GREEN "Downloaded: %s -> %s\n" COLOR_RESET, url, output);
        } else {
            printf(COLOR_GREY "Cannot write to: %s\n" COLOR_RESET, output);
            return 1;
        }
    } else {
        /* Extract filename from URL */
        const char *filename = strrchr(url, '/');
        if (filename == NULL) filename = "index.html";
        else filename++;
        
        FILE *fp = fopen(filename, "wb");
        if (fp != NULL) {
            fwrite(resp->body, 1, resp->body_len, fp);
            fclose(fp);
            printf(COLOR_GREEN "Downloaded: %s\n" COLOR_RESET, filename);
        }
    }
    
    http_response_free(resp);
    return 0;
}

int cmd_aria2c(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Aria2c requires multi-connection download manager.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'wget' for basic downloads.\n" COLOR_RESET);
    return 1;
}

int cmd_axel(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Axel requires multi-connection downloader.\n" COLOR_RESET);
    return 1;
}

int cmd_httpie(int argc, char **argv, shell_context_t *ctx) {
    return cmd_http(argc, argv, ctx);
}

int cmd_http(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: http <method> <url> [data]\n" COLOR_RESET);
        return 1;
    }
    
    const char *method = argv[1];
    const char *url = (argc > 2) ? argv[2] : argv[1];
    
    if (strcmp(method, "GET") == 0 || strcmp(method, "get") == 0) {
        return cmd_intercept(2, (char*[]){ "intercept", url }, ctx);
    }
    
    printf(COLOR_GREY "HTTP method '%s' not yet implemented.\n" COLOR_RESET, method);
    printf(COLOR_GREY "Use 'intercept' for GET requests.\n" COLOR_RESET);
    return 1;
}

int cmd_httpx(int argc, char **argv, shell_context_t *ctx) {
    return cmd_http(argc, argv, ctx);
}

/* Note: feroxbuster, gobuster, nikto, sqlmap, wpscan are implemented in security_ext.c */
/* They check for Kali tools installation and execute accordingly */

int cmd_dirb(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Dirb requires web directory scanning.\n" COLOR_RESET);
    return 1;
}

/* DNS Operations */
static int resolve_hostname(const char *hostname, char *ip, size_t ip_len) {
    struct hostent *he = gethostbyname(hostname);
    if (he == NULL || he->h_addr_list[0] == NULL) {
        return -1;
    }
    
    struct in_addr addr;
    memcpy(&addr, he->h_addr_list[0], sizeof(addr));
    inet_ntop(AF_INET, &addr, ip, ip_len);
    return 0;
}

int cmd_nslookup(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: nslookup <domain>\n" COLOR_RESET);
        return 1;
    }
    
    const char *domain = argv[1];
    char ip[INET_ADDRSTRLEN];
    
    if (resolve_hostname(domain, ip, sizeof(ip)) == 0) {
        printf(COLOR_WHITE "Name: %s\n" COLOR_RESET, domain);
        printf(COLOR_WHITE "Address: %s\n" COLOR_RESET, ip);
        return 0;
    }
    
    printf(COLOR_GREY "Cannot resolve: %s\n" COLOR_RESET, domain);
    return 1;
}

int cmd_dig(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: dig <domain> [type]\n" COLOR_RESET);
        return 1;
    }
    
    const char *domain = argv[1];
    struct hostent *he = gethostbyname(domain);
    
    if (he == NULL) {
        printf(COLOR_GREY "Cannot resolve: %s\n" COLOR_RESET, domain);
        return 1;
    }
    
    printf(COLOR_WHITE "; <<>> DiG <<>> %s\n" COLOR_RESET, domain);
    printf(COLOR_GREY ";; QUESTION SECTION:\n" COLOR_RESET);
    printf(COLOR_WHITE ";%s\t\tIN\tA\n\n" COLOR_RESET, domain);
    printf(COLOR_GREY ";; ANSWER SECTION:\n" COLOR_RESET);
    
    for (int i = 0; he->h_addr_list[i] != NULL; i++) {
        struct in_addr addr;
        memcpy(&addr, he->h_addr_list[i], sizeof(addr));
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr, ip, sizeof(ip));
        printf(COLOR_WHITE "%s\t\tIN\tA\t%s\n" COLOR_RESET, domain, ip);
    }
    
    return 0;
}

int cmd_host_ext(int argc, char **argv, shell_context_t *ctx) {
    return cmd_nslookup(argc, argv, ctx);
}

int cmd_getent(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: getent <database> <key>\n" COLOR_RESET);
        return 1;
    }
    
    const char *database = argv[1];
    const char *key = argv[2];
    
    if (strcmp(database, "hosts") == 0) {
        return cmd_nslookup(2, (char*[]){ "nslookup", key }, ctx);
    } else if (strcmp(database, "passwd") == 0) {
        struct passwd *pw = getpwnam(key);
        if (pw != NULL) {
            printf(COLOR_WHITE "%s:x:%d:%d:%s:%s:%s\n" COLOR_RESET,
                   pw->pw_name, pw->pw_uid, pw->pw_gid,
                   pw->pw_gecos, pw->pw_dir, pw->pw_shell);
            return 0;
        }
    } else if (strcmp(database, "group") == 0) {
        struct group *gr = getgrnam(key);
        if (gr != NULL) {
            printf(COLOR_WHITE "%s:x:%d:" COLOR_RESET, gr->gr_name, gr->gr_gid);
            for (int i = 0; gr->gr_mem[i] != NULL; i++) {
                printf("%s%s", (i > 0) ? "," : "", gr->gr_mem[i]);
            }
            printf("\n");
            return 0;
        }
    }
    
    printf(COLOR_GREY "Database '%s' not supported or key not found\n" COLOR_RESET, database);
    return 1;
}

int cmd_resolveip(int argc, char **argv, shell_context_t *ctx) {
    return cmd_nslookup(argc, argv, ctx);
}

int cmd_dnsenum(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "DNS enumeration requires DNS protocol implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_dnsrecon(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "DNS recon requires DNS protocol implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_fierce(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Fierce requires DNS scanning implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_sublist3r(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Sublist3r requires subdomain enumeration.\n" COLOR_RESET);
    return 1;
}

int cmd_amass(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Amass requires subdomain enumeration.\n" COLOR_RESET);
    return 1;
}

/* Network Protocols */
int cmd_telnet(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: telnet <host> <port>\n" COLOR_RESET);
        return 1;
    }
    
    const char *host = argv[1];
    int port = atoi(argv[2]);
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf(COLOR_GREY "Cannot create socket\n" COLOR_RESET);
        return 1;
    }
    
    struct hostent *he = gethostbyname(host);
    if (he == NULL) {
        close(sock);
        printf(COLOR_GREY "Cannot resolve host: %s\n" COLOR_RESET, host);
        return 1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
    
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(sock);
        printf(COLOR_GREY "Cannot connect to %s:%d\n" COLOR_RESET, host, port);
        return 1;
    }
    
    printf(COLOR_GREEN "Connected to %s:%d\n" COLOR_RESET, host, port);
    printf(COLOR_GREY "Type 'exit' to disconnect\n" COLOR_RESET);
    
    /* Simple telnet client */
    char buffer[4096];
    fd_set readfds;
    
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);
        
        if (select(sock + 1, &readfds, NULL, NULL, NULL) > 0) {
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                    if (strncmp(buffer, "exit", 4) == 0) {
                        break;
                    }
                    send(sock, buffer, strlen(buffer), 0);
                }
            }
            
            if (FD_ISSET(sock, &readfds)) {
                ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
                if (n <= 0) {
                    break;
                }
                buffer[n] = '\0';
                printf("%s", buffer);
            }
        }
    }
    
    close(sock);
    return 0;
}

int cmd_nc(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: nc <host> <port>\n" COLOR_RESET);
        return 1;
    }
    
    /* Netcat - similar to telnet */
    return cmd_telnet(argc, argv, ctx);
}

int cmd_ncat(int argc, char **argv, shell_context_t *ctx) {
    return cmd_nc(argc, argv, ctx);
}

int cmd_socat(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Socat requires advanced socket operations.\n" COLOR_RESET);
    return 1;
}

int cmd_openssl_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: openssl <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "OpenSSL requires SSL/TLS library implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_ssh_ext(int argc, char **argv, shell_context_t *ctx) {
    return cmd_rift(argc, argv, ctx);
}

int cmd_scp_ext(int argc, char **argv, shell_context_t *ctx) {
    return cmd_wormhole(argc, argv, ctx);
}

int cmd_sftp_ext(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "SFTP requires SSH protocol implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_rsync(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: rsync <src> <dst>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Rsync requires rsync protocol implementation.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'cp' for local file copying.\n" COLOR_RESET);
    return 1;
}

int cmd_rsh(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Rsh requires remote shell protocol.\n" COLOR_RESET);
    return 1;
}

int cmd_rlogin(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Rlogin requires remote login protocol.\n" COLOR_RESET);
    return 1;
}

int cmd_ftp(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: ftp <host>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "FTP requires FTP protocol implementation.\n" COLOR_RESET);
    return 1;
}

int cmd_lftp(int argc, char **argv, shell_context_t *ctx) {
    return cmd_ftp(argc, argv, ctx);
}

int cmd_wput(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: wput <file> <url>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Wput requires FTP/HTTP upload implementation.\n" COLOR_RESET);
    return 1;
}

/* Network Security */
int cmd_iptables(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: iptables <command> [args...]\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_GREY "Iptables requires netfilter kernel module access.\n" COLOR_RESET);
    return 1;
}

int cmd_ip6tables(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Ip6tables requires IPv6 netfilter support.\n" COLOR_RESET);
    return 1;
}

int cmd_ufw(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "UFW requires firewall management daemon.\n" COLOR_RESET);
    return 1;
}

int cmd_firewalld(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Firewalld requires firewall daemon.\n" COLOR_RESET);
    return 1;
}

int cmd_fail2ban_client(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "Fail2ban requires fail2ban daemon.\n" COLOR_RESET);
    return 1;
}

int cmd_tcpwrappers(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    printf(COLOR_GREY "TCP wrappers require hosts.allow/deny configuration.\n" COLOR_RESET);
    return 1;
}

