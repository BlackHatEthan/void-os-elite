#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stdbool.h>

/* Native HTTP client - no external dependencies */
typedef struct {
    int status_code;
    char *headers;
    char *body;
    size_t body_len;
} http_response_t;

/* Make HTTP GET request */
http_response_t *http_get(const char *url);

/* Make HTTP POST request */
http_response_t *http_post(const char *url, const char *data, size_t data_len);

/* Free HTTP response */
void http_response_free(http_response_t *resp);

/* Parse URL into components */
typedef struct {
    char *protocol;
    char *host;
    int port;
    char *path;
} url_components_t;

url_components_t *parse_url(const char *url);
void url_components_free(url_components_t *components);

#endif /* HTTP_CLIENT_H */

