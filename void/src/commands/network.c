#include "network.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../utils/http_client.h"
#include "../utils/ping_native.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <errno.h>

int cmd_intercept(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: intercept <url>\n" COLOR_RESET);
        printf(COLOR_GREY "Example: intercept https://api.example.com/data\n" COLOR_RESET);
        return 1;
    }
    
    const char *url = argv[1];
    
    printf(COLOR_WHITE "Request: " COLOR_BLUE "%s\n" COLOR_RESET, url);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    /* Use native HTTP client */
    http_response_t *resp = http_get(url);
    if (resp == NULL) {
        printf(COLOR_GREY "Failed to connect to %s\n" COLOR_RESET, url);
        return 1;
    }
    
    /* Display headers */
    if (resp->headers != NULL) {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, resp->headers);
    }
    
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    /* Display body (first 1000 chars) */
    if (resp->body != NULL && resp->body_len > 0) {
        size_t display_len = (resp->body_len > 1000) ? 1000 : resp->body_len;
        printf(COLOR_WHITE "%.*s\n" COLOR_RESET, (int)display_len, resp->body);
        if (resp->body_len > 1000) {
            printf(COLOR_GREY "... (%zu more bytes)\n" COLOR_RESET, resp->body_len - 1000);
        }
    }
    
    http_response_free(resp);
    return 0;
}

int cmd_ping(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: ping <host> [count]\n" COLOR_RESET);
        return 1;
    }
    
    const char *host = argv[1];
    int count = (argc > 2) ? atoi(argv[2]) : 4;
    
    if (count <= 0) count = 4;
    
    /* Use native ping implementation */
    int result = ping_host(host, count, 5000); /* 5 second timeout */
    return (result == 0) ? 0 : 1;
}

int cmd_ports(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_WHITE "Listening Ports:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    #ifdef __linux__
    /* Read from /proc/net/tcp */
    FILE *fp = fopen("/proc/net/tcp", "r");
    if (fp != NULL) {
        char line[256];
        int count = 0;
        while (fgets(line, sizeof(line), fp) != NULL && count < 20) {
            if (count == 0) {
                count++;
                continue; /* Skip header */
            }
            
            unsigned int local_addr, local_port;
            if (sscanf(line, "%*d: %x:%x", &local_addr, &local_port) == 2) {
                printf(COLOR_WHITE "Port: " COLOR_BLUE "%d\n" COLOR_RESET, ntohs(local_port));
                count++;
            }
        }
        fclose(fp);
    }
    #else
    /* macOS/BSD - read from sysctl or /proc-like interfaces */
    /* Try to read from /proc/net/tcp if available (Linux compatibility) */
    FILE *fp = fopen("/proc/net/tcp", "r");
    if (fp != NULL) {
        char line[256];
        int count = 0;
        while (fgets(line, sizeof(line), fp) != NULL && count < 20) {
            if (count == 0) {
                count++;
                continue;
            }
            unsigned int local_addr, local_port;
            if (sscanf(line, "%*d: %x:%x", &local_addr, &local_port) == 2) {
                printf(COLOR_WHITE "Port: " COLOR_BLUE "%d\n" COLOR_RESET, ntohs(local_port));
                count++;
            }
        }
        fclose(fp);
    } else {
        printf(COLOR_GREY "Port listing not available on this system.\n" COLOR_RESET);
        printf(COLOR_GREY "Use 'net-watch' for network connection monitoring.\n" COLOR_RESET);
    }
    #endif
    
    return 0;
}

/* Native port scanner */
static int scan_port(const char *host, int port, int timeout_ms) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }
    
    struct hostent *he = gethostbyname(host);
    if (he == NULL) {
        close(sock);
        return -1;
    }
    
    struct sockaddr_in addr;
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    
    int result = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    close(sock);
    
    return (result == 0) ? 1 : 0;
}

int cmd_scan(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: scan <host> [port-range]\n" COLOR_RESET);
        printf(COLOR_GREY "Example: scan 192.168.1.1 1-1000\n" COLOR_RESET);
        return 1;
    }
    
    const char *host = argv[1];
    int start_port = 1;
    int end_port = 1024;
    
    if (argc > 2) {
        if (sscanf(argv[2], "%d-%d", &start_port, &end_port) != 2) {
            end_port = atoi(argv[2]);
            if (end_port > 0) {
                start_port = end_port;
            }
        }
    }
    
    printf(COLOR_WHITE "Scanning %s: ports %d-%d\n" COLOR_RESET, host, start_port, end_port);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    int open_count = 0;
    for (int port = start_port; port <= end_port && port <= 65535; port++) {
        if (scan_port(host, port, 1000) > 0) {
            printf(COLOR_GREEN "Port %d: OPEN\n" COLOR_RESET, port);
            open_count++;
        }
        
        if (port % 100 == 0) {
            printf(COLOR_GREY "Scanned %d ports...\n" COLOR_RESET, port - start_port + 1);
        }
    }
    
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_WHITE "Scan complete: %d open ports found\n" COLOR_RESET, open_count);
    
    return 0;
}

/* Native HTTP server */
static void handle_http_request(int client_sock, const char *request) {
    char response[4096];
    char *method = strtok((char*)request, " ");
    char *path = strtok(NULL, " ");
    
    if (path == NULL) path = "/";
    
    /* Simple file server - serve files from current directory */
    char filepath[PATH_MAX];
    if (strcmp(path, "/") == 0) {
        strcpy(filepath, ".");
    } else {
        snprintf(filepath, sizeof(filepath), ".%s", path);
    }
    
    FILE *fp = fopen(filepath, "r");
    if (fp != NULL) {
        fseek(fp, 0, SEEK_END);
        long file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        
        char *file_content = malloc(file_size + 1);
        if (file_content != NULL) {
            fread(file_content, 1, file_size, fp);
            file_content[file_size] = '\0';
            
            snprintf(response, sizeof(response),
                     "HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/plain\r\n"
                     "Content-Length: %ld\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "%s",
                     file_size, file_content);
            
            free(file_content);
        } else {
            snprintf(response, sizeof(response),
                     "HTTP/1.1 500 Internal Server Error\r\n"
                     "Connection: close\r\n"
                     "\r\n");
        }
        fclose(fp);
    } else {
        snprintf(response, sizeof(response),
                 "HTTP/1.1 404 Not Found\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "File not found");
    }
    
    send(client_sock, response, strlen(response), 0);
    close(client_sock);
}

int cmd_serve(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    int port = 8000;
    
    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            printf(COLOR_GREY "Invalid port number. Using default 8000.\n" COLOR_RESET);
            port = 8000;
        }
    }
    
    printf(COLOR_WHITE "Starting native HTTP server on port %d...\n" COLOR_RESET, port);
    printf(COLOR_GREY "Serving current directory. Press Ctrl+C to stop.\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        printf(COLOR_GREY "Failed to create socket.\n" COLOR_RESET);
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(server_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf(COLOR_GREY "Failed to bind to port %d.\n" COLOR_RESET, port);
        close(server_sock);
        return 1;
    }
    
    if (listen(server_sock, 5) < 0) {
        printf(COLOR_GREY "Failed to listen on socket.\n" COLOR_RESET);
        close(server_sock);
        return 1;
    }
    
    printf(COLOR_GREEN "Server listening on http://localhost:%d\n" COLOR_RESET, port);
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        
        if (client_sock < 0) {
            continue;
        }
        
        char request[4096];
        ssize_t n = recv(client_sock, request, sizeof(request) - 1, 0);
        if (n > 0) {
            request[n] = '\0';
            handle_http_request(client_sock, request);
        } else {
            close(client_sock);
        }
    }
    
    close(server_sock);
    return 0;
}

int cmd_tunnel_info(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_WHITE "Network Interfaces:\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    struct ifaddrs *ifaddr, *ifa;
    
    if (getifaddrs(&ifaddr) == -1) {
        LOG_ERROR("Failed to get network interfaces: %s", strerror(errno));
        printf(COLOR_GREY "Failed to get network information.\n" COLOR_RESET);
        return 1;
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }
        
        int family = ifa->ifa_addr->sa_family;
        
        if (family == AF_INET || family == AF_INET6) {
            char host[NI_MAXHOST];
            int s = getnameinfo(ifa->ifa_addr,
                               (family == AF_INET) ? sizeof(struct sockaddr_in) :
                               sizeof(struct sockaddr_in6),
                               host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            
            if (s == 0) {
                const char *family_str = (family == AF_INET) ? "IPv4" : "IPv6";
                printf(COLOR_WHITE "%-10s " COLOR_BLUE "%-15s" COLOR_RESET " %s\n",
                       ifa->ifa_name, host, family_str);
            }
        }
    }
    
    freeifaddrs(ifaddr);
    return 0;
}

int cmd_throb(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *interface = (argc > 1) ? argv[1] : "eth0";
    
    printf(COLOR_WHITE "Monitoring bandwidth: %s\n" COLOR_RESET, interface);
    printf(COLOR_GREY "Press Ctrl+C to stop.\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    #ifdef __linux__
    /* Read from /proc/net/dev */
    FILE *fp = fopen("/proc/net/dev", "r");
    if (fp != NULL) {
        char line[512];
        while (fgets(line, sizeof(line), fp) != NULL) {
            if (strstr(line, interface) != NULL) {
                unsigned long long rx_bytes, tx_bytes;
                char ifname[64];
                sscanf(line, "%s %llu", ifname, &rx_bytes);
                /* Parse more fields if needed */
                printf(COLOR_WHITE "%s: RX=%llu bytes, TX=%llu bytes\n" COLOR_RESET,
                       ifname, rx_bytes, tx_bytes);
            }
        }
        fclose(fp);
    } else {
        printf(COLOR_GREY "Failed to read network statistics.\n" COLOR_RESET);
        return 1;
    }
    #else
    printf(COLOR_GREY "Network interface monitoring requires /proc/net/dev (Linux).\n" COLOR_RESET);
    return 1;
    #endif
    
    return 0;
}

int cmd_rift(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: rift <user@host> [port]\n" COLOR_RESET);
        printf(COLOR_GREY "Note: Native SSH client not yet implemented.\n" COLOR_RESET);
        printf(COLOR_GREY "This command will provide native SSH functionality in future release.\n" COLOR_RESET);
        return 1;
    }
    
    /* TODO: Implement native SSH client using libssh or raw socket protocol */
    printf(COLOR_GREY "Native SSH client implementation pending.\n" COLOR_RESET);
    printf(COLOR_GREY "For now, use external SSH client directly.\n" COLOR_RESET);
    return 1;
}

int cmd_wormhole(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: wormhole <file> <user@host:path>\n" COLOR_RESET);
        printf(COLOR_GREY "Note: Native SCP client not yet implemented.\n" COLOR_RESET);
        return 1;
    }
    
    /* TODO: Implement native SCP using SSH protocol */
    printf(COLOR_GREY "Native SCP client implementation pending.\n" COLOR_RESET);
    printf(COLOR_GREY "For now, use external SCP client directly.\n" COLOR_RESET);
    return 1;
}

int cmd_beacon(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    int port = (argc > 1) ? atoi(argv[1]) : 8080;
    
    if (port <= 0 || port > 65535) {
        printf(COLOR_GREY "Invalid port: %d\n" COLOR_RESET, port);
        return 1;
    }
    
    printf(COLOR_WHITE "Opening listening port: %d\n" COLOR_RESET, port);
    printf(COLOR_GREY "Press Ctrl+C to stop.\n" COLOR_RESET);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    /* Native TCP listener */
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        printf(COLOR_GREY "Failed to create socket.\n" COLOR_RESET);
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(server_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf(COLOR_GREY "Failed to bind to port %d.\n" COLOR_RESET, port);
        close(server_sock);
        return 1;
    }
    
    if (listen(server_sock, 5) < 0) {
        printf(COLOR_GREY "Failed to listen.\n" COLOR_RESET);
        close(server_sock);
        return 1;
    }
    
    printf(COLOR_GREEN "Listening on port %d...\n" COLOR_RESET, port);
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        
        if (client_sock < 0) {
            continue;
        }
        
        char buffer[1024];
        ssize_t n = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf(COLOR_WHITE "Received: %s\n" COLOR_RESET, buffer);
        }
        
        close(client_sock);
    }
    
    close(server_sock);
    return 0;
}

int cmd_trace_route(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: trace-route <host>\n" COLOR_RESET);
        return 1;
    }
    
    const char *host = argv[1];
    
    printf(COLOR_WHITE "Tracing route to %s (max 30 hops):\n" COLOR_RESET, host);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    /* Simple traceroute using TCP connect with increasing TTL */
    /* Note: Full traceroute requires raw sockets and ICMP, which needs root */
    /* This is a simplified version using TCP */
    
    struct hostent *he = gethostbyname(host);
    if (he == NULL) {
        printf(COLOR_GREY "Failed to resolve hostname.\n" COLOR_RESET);
        return 1;
    }
    
    struct sockaddr_in target;
    memcpy(&target.sin_addr, he->h_addr_list[0], he->h_length);
    target.sin_family = AF_INET;
    target.sin_port = htons(80);
    
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &target.sin_addr, ip_str, INET_ADDRSTRLEN);
    
    printf(COLOR_WHITE "Target: %s (%s)\n" COLOR_RESET, host, ip_str);
    printf(COLOR_GREY "Note: Full traceroute requires root privileges for ICMP.\n" COLOR_RESET);
    printf(COLOR_GREY "This is a simplified connection test.\n" COLOR_RESET);
    
    /* Try to connect to common ports to test reachability */
    int test_ports[] = {80, 443, 22, 0};
    for (int i = 0; test_ports[i] != 0; i++) {
        target.sin_port = htons(test_ports[i]);
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock >= 0) {
            struct timeval tv;
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
            
            if (connect(sock, (struct sockaddr *)&target, sizeof(target)) == 0) {
                printf(COLOR_GREEN "Port %d: REACHABLE\n" COLOR_RESET, test_ports[i]);
            }
            close(sock);
        }
    }
    
    return 0;
}

int cmd_ghost_ping(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: ghost-ping <host> <port> [protocol]\n" COLOR_RESET);
        printf(COLOR_GREY "Protocol: tcp (default) or udp\n" COLOR_RESET);
        return 1;
    }
    
    const char *host = argv[1];
    int port = atoi(argv[2]);
    const char *proto = (argc > 3) ? argv[3] : "tcp";
    
    if (port <= 0 || port > 65535) {
        printf(COLOR_GREY "Invalid port: %d\n" COLOR_RESET, port);
        return 1;
    }
    
    printf(COLOR_WHITE "Sending %s packet to %s:%d\n" COLOR_RESET, proto, host, port);
    
    /* Resolve hostname */
    struct hostent *he = gethostbyname(host);
    if (he == NULL) {
        printf(COLOR_GREY "Failed to resolve hostname.\n" COLOR_RESET);
        return 1;
    }
    
    struct sockaddr_in addr;
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    int sock_type = (strcmp(proto, "udp") == 0) ? SOCK_DGRAM : SOCK_STREAM;
    int sock = socket(AF_INET, sock_type, 0);
    if (sock < 0) {
        printf(COLOR_GREY "Failed to create socket.\n" COLOR_RESET);
        return 1;
    }
    
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    
    if (strcmp(proto, "udp") == 0) {
        /* UDP: just send a packet */
        const char *data = "";
        if (sendto(sock, data, 0, 0, (struct sockaddr *)&addr, sizeof(addr)) >= 0) {
            printf(COLOR_GREEN "UDP packet sent successfully.\n" COLOR_RESET);
            close(sock);
            return 0;
        }
    } else {
        /* TCP: try to connect */
        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
            printf(COLOR_GREEN "TCP connection established.\n" COLOR_RESET);
            close(sock);
            return 0;
        }
    }
    
    close(sock);
    printf(COLOR_GREY "Connection failed or timed out.\n" COLOR_RESET);
    return 1;
}

