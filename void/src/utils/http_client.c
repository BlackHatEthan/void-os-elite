#include "http_client.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

/* Parse URL into components */
url_components_t *parse_url(const char *url) {
    if (url == NULL) return NULL;
    
    url_components_t *comp = calloc(1, sizeof(url_components_t));
    if (comp == NULL) return NULL;
    
    char *url_copy = strdup(url);
    if (url_copy == NULL) {
        free(comp);
        return NULL;
    }
    
    /* Default values */
    comp->port = 80;
    comp->path = strdup("/");
    
    /* Parse protocol */
    char *proto_end = strstr(url_copy, "://");
    if (proto_end != NULL) {
        *proto_end = '\0';
        comp->protocol = strdup(url_copy);
        url_copy = proto_end + 3;
        
        if (strcmp(comp->protocol, "https") == 0) {
            comp->port = 443;
        }
    } else {
        comp->protocol = strdup("http");
    }
    
    /* Parse host and path */
    char *path_start = strchr(url_copy, '/');
    if (path_start != NULL) {
        *path_start = '\0';
        comp->path = strdup(path_start);
    }
    
    /* Parse port */
    char *port_start = strchr(url_copy, ':');
    if (port_start != NULL) {
        *port_start = '\0';
        comp->port = atoi(port_start + 1);
    }
    
    comp->host = strdup(url_copy);
    free(url_copy);
    
    return comp;
}

void url_components_free(url_components_t *components) {
    if (components == NULL) return;
    free(components->protocol);
    free(components->host);
    free(components->path);
    free(components);
}

/* Resolve hostname to IP */
static int resolve_host(const char *hostname, struct sockaddr_in *addr) {
    struct hostent *he = gethostbyname(hostname);
    if (he == NULL) {
        return -1;
    }
    
    addr->sin_family = AF_INET;
    addr->sin_port = 0;
    memcpy(&addr->sin_addr, he->h_addr_list[0], he->h_length);
    return 0;
}

/* Make HTTP GET request - native implementation */
http_response_t *http_get(const char *url) {
    url_components_t *url_comp = parse_url(url);
    if (url_comp == NULL) {
        return NULL;
    }
    
    /* Create socket */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        url_components_free(url_comp);
        return NULL;
    }
    
    /* Resolve host */
    struct sockaddr_in server_addr;
    if (resolve_host(url_comp->host, &server_addr) < 0) {
        close(sock);
        url_components_free(url_comp);
        return NULL;
    }
    
    server_addr.sin_port = htons(url_comp->port);
    
    /* Connect */
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        url_components_free(url_comp);
        return NULL;
    }
    
    /* Build HTTP request */
    char request[4096];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: Void-OS/1.0\r\n"
             "Connection: close\r\n"
             "\r\n",
             url_comp->path, url_comp->host);
    
    /* Send request */
    if (send(sock, request, strlen(request), 0) < 0) {
        close(sock);
        url_components_free(url_comp);
        return NULL;
    }
    
    /* Read response */
    http_response_t *resp = calloc(1, sizeof(http_response_t));
    if (resp == NULL) {
        close(sock);
        url_components_free(url_comp);
        return NULL;
    }
    
    char buffer[4096];
    size_t total_len = 0;
    char *response_data = NULL;
    
    while (1) {
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;
        
        buffer[n] = '\0';
        response_data = realloc(response_data, total_len + n + 1);
        if (response_data == NULL) {
            free(resp);
            close(sock);
            url_components_free(url_comp);
            return NULL;
        }
        
        memcpy(response_data + total_len, buffer, n);
        total_len += n;
        response_data[total_len] = '\0';
    }
    
    close(sock);
    url_components_free(url_comp);
    
    if (response_data == NULL) {
        free(resp);
        return NULL;
    }
    
    /* Parse response */
    char *header_end = strstr(response_data, "\r\n\r\n");
    if (header_end == NULL) {
        header_end = strstr(response_data, "\n\n");
    }
    
    if (header_end != NULL) {
        *header_end = '\0';
        resp->headers = strdup(response_data);
        resp->body = strdup(header_end + (strstr(response_data, "\r\n\r\n") ? 4 : 2));
        resp->body_len = strlen(resp->body);
        
        /* Parse status code */
        if (sscanf(resp->headers, "HTTP/%*s %d", &resp->status_code) != 1) {
            resp->status_code = 200;
        }
    } else {
        resp->headers = strdup(response_data);
        resp->body = strdup("");
        resp->body_len = 0;
        resp->status_code = 200;
    }
    
    free(response_data);
    return resp;
}

/* Make HTTP POST request */
http_response_t *http_post(const char *url, const char *data, size_t data_len) {
    url_components_t *url_comp = parse_url(url);
    if (url_comp == NULL) {
        return NULL;
    }
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        url_components_free(url_comp);
        return NULL;
    }
    
    struct sockaddr_in server_addr;
    if (resolve_host(url_comp->host, &server_addr) < 0) {
        close(sock);
        url_components_free(url_comp);
        return NULL;
    }
    
    server_addr.sin_port = htons(url_comp->port);
    
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        url_components_free(url_comp);
        return NULL;
    }
    
    char request[8192];
    int req_len = snprintf(request, sizeof(request),
                          "POST %s HTTP/1.1\r\n"
                          "Host: %s\r\n"
                          "User-Agent: Void-OS/1.0\r\n"
                          "Content-Type: application/x-www-form-urlencoded\r\n"
                          "Content-Length: %zu\r\n"
                          "Connection: close\r\n"
                          "\r\n",
                          url_comp->path, url_comp->host, data_len);
    
    if (req_len + data_len < sizeof(request) - 1) {
        memcpy(request + req_len, data, data_len);
        req_len += data_len;
        request[req_len] = '\0';
    }
    
    if (send(sock, request, req_len, 0) < 0) {
        close(sock);
        url_components_free(url_comp);
        return NULL;
    }
    
    http_response_t *resp = calloc(1, sizeof(http_response_t));
    if (resp == NULL) {
        close(sock);
        url_components_free(url_comp);
        return NULL;
    }
    
    char buffer[4096];
    size_t total_len = 0;
    char *response_data = NULL;
    
    while (1) {
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;
        
        buffer[n] = '\0';
        response_data = realloc(response_data, total_len + n + 1);
        if (response_data == NULL) {
            free(resp);
            close(sock);
            url_components_free(url_comp);
            return NULL;
        }
        
        memcpy(response_data + total_len, buffer, n);
        total_len += n;
        response_data[total_len] = '\0';
    }
    
    close(sock);
    url_components_free(url_comp);
    
    if (response_data == NULL) {
        free(resp);
        return NULL;
    }
    
    char *header_end = strstr(response_data, "\r\n\r\n");
    if (header_end == NULL) {
        header_end = strstr(response_data, "\n\n");
    }
    
    if (header_end != NULL) {
        *header_end = '\0';
        resp->headers = strdup(response_data);
        resp->body = strdup(header_end + (strstr(response_data, "\r\n\r\n") ? 4 : 2));
        resp->body_len = strlen(resp->body);
        
        if (sscanf(resp->headers, "HTTP/%*s %d", &resp->status_code) != 1) {
            resp->status_code = 200;
        }
    } else {
        resp->headers = strdup(response_data);
        resp->body = strdup("");
        resp->body_len = 0;
        resp->status_code = 200;
    }
    
    free(response_data);
    return resp;
}

void http_response_free(http_response_t *resp) {
    if (resp == NULL) return;
    free(resp->headers);
    free(resp->body);
    free(resp);
}

