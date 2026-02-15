#include "ping_native.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>

/* Calculate checksum for ICMP packet */
static unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;
    
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    
    if (len == 1) {
        sum += *(unsigned char *)buf << 8;
    }
    
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    result = ~sum;
    return result;
}

/* Native ping implementation */
int ping_host(const char *hostname, int count, int timeout_ms) {
    if (hostname == NULL || count <= 0) {
        return -1;
    }
    
    /* Resolve hostname */
    struct hostent *he = gethostbyname(hostname);
    if (he == NULL) {
        printf(COLOR_GREY "Failed to resolve hostname: %s\n" COLOR_RESET, hostname);
        return -1;
    }
    
    struct sockaddr_in addr;
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
    addr.sin_family = AF_INET;
    
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET_ADDRSTRLEN);
    
    printf(COLOR_WHITE "PING %s (%s):\n" COLOR_RESET, hostname, ip_str);
    
    /* Create raw socket (requires root on most systems) */
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        /* Fallback: Use TCP connect to port 80 as ping alternative */
        printf(COLOR_GREY "Raw socket not available. Using TCP connect test...\n" COLOR_RESET);
        
        int success = 0;
        for (int i = 0; i < count; i++) {
            int tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (tcp_sock < 0) {
                continue;
            }
            
            struct sockaddr_in tcp_addr = addr;
            tcp_addr.sin_port = htons(80);
            
            struct timeval tv;
            tv.tv_sec = timeout_ms / 1000;
            tv.tv_usec = (timeout_ms % 1000) * 1000;
            setsockopt(tcp_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
            setsockopt(tcp_sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
            
            clock_t start = clock();
            int result = connect(tcp_sock, (struct sockaddr *)&tcp_addr, sizeof(tcp_addr));
            clock_t end = clock();
            
            if (result == 0) {
                double elapsed = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
                printf(COLOR_GREEN "Connected to %s: time=%.2fms\n" COLOR_RESET, ip_str, elapsed);
                success++;
            } else {
                printf(COLOR_GREY "Connection to %s failed\n" COLOR_RESET, ip_str);
            }
            
            close(tcp_sock);
            
            if (i < count - 1) {
                sleep(1);
            }
        }
        
        printf(COLOR_WHITE "\n%d/%d successful connections\n" COLOR_RESET, success, count);
        return (success > 0) ? 0 : -1;
    }
    
    /* ICMP ping */
    struct icmp packet;
    memset(&packet, 0, sizeof(packet));
    packet.icmp_type = ICMP_ECHO;
    packet.icmp_code = 0;
    packet.icmp_id = getpid();
    
    int success = 0;
    for (int seq = 0; seq < count; seq++) {
        packet.icmp_seq = seq;
        packet.icmp_cksum = 0;
        packet.icmp_cksum = checksum(&packet, sizeof(packet));
        
        clock_t start = clock();
        
        if (sendto(sock, &packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            printf(COLOR_GREY "Failed to send packet\n" COLOR_RESET);
            continue;
        }
        
        char recv_buffer[1024];
        struct sockaddr_in recv_addr;
        socklen_t addr_len = sizeof(recv_addr);
        
        struct timeval tv;
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        
        ssize_t recv_len = recvfrom(sock, recv_buffer, sizeof(recv_buffer), 0,
                                    (struct sockaddr *)&recv_addr, &addr_len);
        
        clock_t end = clock();
        
        if (recv_len > 0) {
            double elapsed = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
            printf(COLOR_GREEN "64 bytes from %s: icmp_seq=%d time=%.2fms\n" COLOR_RESET,
                   ip_str, seq, elapsed);
            success++;
        } else {
            printf(COLOR_GREY "Request timeout for icmp_seq=%d\n" COLOR_RESET, seq);
        }
        
        if (seq < count - 1) {
            sleep(1);
        }
    }
    
    close(sock);
    
    printf(COLOR_WHITE "\n--- %s ping statistics ---\n" COLOR_RESET, hostname);
    printf(COLOR_WHITE "%d packets transmitted, %d received\n" COLOR_RESET, count, success);
    
    return (success > 0) ? 0 : -1;
}

