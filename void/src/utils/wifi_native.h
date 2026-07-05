#ifndef WIFI_NATIVE_H
#define WIFI_NATIVE_H

#include <stddef.h>

/* Opaque result: pass to wifi_native_scan() and it prints networks. */
/* Returns 0 on success (scan done, results printed), -1 on failure. */
int wifi_native_scan(const char *iface);

/* 1 if native scan is available (e.g. Linux with nl80211), 0 otherwise */
int wifi_native_available(void);

#endif /* WIFI_NATIVE_H */
