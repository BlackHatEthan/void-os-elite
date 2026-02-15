#ifndef PING_NATIVE_H
#define PING_NATIVE_H

/* Native ping implementation using ICMP */
int ping_host(const char *hostname, int count, int timeout_ms);

#endif /* PING_NATIVE_H */

