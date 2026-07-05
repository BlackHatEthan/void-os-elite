/*
 * Native WiFi scan for Void OS Elite.
 * Linux: uses nl80211 netlink (no iw/nmcli). Other platforms: stub.
 */
#include "wifi_native.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#if defined(__linux__)
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <linux/nl80211.h>
#include <net/if.h>
#include <stdlib.h>

#ifndef NLA_F_NESTED
#define NLA_F_NESTED (1 << 15)
#endif
#ifndef NLA_TYPE_MASK
#define NLA_TYPE_MASK 0x1fff
#endif

static int nl80211_family_id(int sock);
static int do_trigger_scan(int sock, int fam, unsigned int ifindex);
static int do_get_scan(int sock, int fam, unsigned int ifindex);

int wifi_native_available(void) {
    return 1;
}

int wifi_native_scan(const char *iface) {
    int sock;
    struct sockaddr_nl addr;
    int fam;
    unsigned int ifindex;

    if (!iface) iface = "wlan0";
    ifindex = if_nametoindex(iface);
    if (ifindex == 0) {
        /* Try common names */
        ifindex = if_nametoindex("wlan0");
        if (ifindex == 0) ifindex = if_nametoindex("wlp2s0");
        if (ifindex == 0) ifindex = if_nametoindex("wlp3s0");
        if (ifindex == 0) {
            fprintf(stderr, "No WiFi interface (wlan0/wlp2s0/wlp3s0) found.\n");
            return -1;
        }
    }

    sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
    if (sock < 0) return -1;

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        close(sock);
        return -1;
    }

    fam = nl80211_family_id(sock);
    if (fam < 0) {
        close(sock);
        return -1;
    }

    if (do_trigger_scan(sock, fam, ifindex) != 0) {
        close(sock);
        return -1;
    }
    sleep(2);
    if (do_get_scan(sock, fam, ifindex) != 0) {
        close(sock);
        return -1;
    }
    close(sock);
    return 0;
}

/* Resolve nl80211 family id via genl ctrl */
static int nl80211_family_id(int sock) {
    char buf[1024];
    struct sockaddr_nl addr;
    struct nlmsghdr *nh;
    struct genlmsghdr *gh;
    int len, ret = -1;

    /* Build GENL_CTRL_CMD_GETFAMILY request for "nl80211" */
    struct {
        struct nlmsghdr nh;
        struct genlmsghdr gh;
        char buf[256];
    } req;
    memset(&req, 0, sizeof(req));
    req.nh.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
    req.nh.nlmsg_type = GENL_ID_CTRL;
    req.nh.nlmsg_flags = NLM_F_REQUEST;
    req.gh.cmd = CTRL_CMD_GETFAMILY;
    req.gh.version = 1;
    /* Add ATTR_FAMILY_NAME = "nl80211" as nested attribute would need more code;
     * use simple approach: send and parse reply for GENL_ID_CTRL. */
    (void)req;
    /* Minimal: we need to add attribute CTRL_ATTR_FAMILY_NAME = "nl80211".
     * NLA format: [nla_len (2 bytes)][nla_type (2 bytes)]["nl80211\0"]
     * nla_len = 4 + 9 = 13, nla_type = CTRL_ATTR_FAMILY_NAME (1).
     */
    len = NLMSG_LENGTH(GENL_HDRLEN);
    nh = (struct nlmsghdr *)buf;
    /* NLA: 4 byte header + "nl80211\0" (8 bytes) = 12 */
    nh->nlmsg_len = len + 12;
    nh->nlmsg_type = GENL_ID_CTRL;
    nh->nlmsg_flags = NLM_F_REQUEST;
    nh->nlmsg_seq = 1;
    gh = (struct genlmsghdr *)(buf + NLMSG_HDRLEN);
    gh->cmd = CTRL_CMD_GETFAMILY;
    gh->version = 1;
    {
        unsigned short *nla_len = (unsigned short *)(buf + len);
        unsigned short *nla_type = (unsigned short *)(buf + len + 2);
        *nla_len = 12;  /* 4 (NLA header) + 8 ("nl80211\0") */
        *nla_type = CTRL_ATTR_FAMILY_NAME;
        memcpy(buf + len + 4, "nl80211", 8);
    }
    len = nh->nlmsg_len;
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    if (sendto(sock, buf, len, 0, (struct sockaddr *)&addr, sizeof(addr)) != (ssize_t)len) {
        return -1;
    }
    len = recv(sock, buf, sizeof(buf), 0);
    if (len < (int)NLMSG_HDRLEN) return -1;
    nh = (struct nlmsghdr *)buf;
    if (nh->nlmsg_type == NLMSG_ERROR) return -1;
    /* Parse reply: find CTRL_ATTR_FAMILY_ID (2) in response */
    {
        int off = NLMSG_HDRLEN + GENL_HDRLEN;
        while (off + 4 <= len) {
            unsigned short nla_l = *(unsigned short *)(buf + off);
            unsigned short nla_t = *(unsigned short *)(buf + off + 2);
            if (nla_l < 4) break;
            if (nla_t == CTRL_ATTR_FAMILY_ID && nla_l >= 6) {
                ret = *(unsigned short *)(buf + off + 4);
                if (ret == 0 && nla_l >= 8) ret = *(unsigned int *)(buf + off + 4);
                return ret;
            }
            off += (nla_l + 3) & ~3;
        }
    }
    return -1;
}

static int do_trigger_scan(int sock, int fam, unsigned int ifindex) {
    char buf[256];
    struct nlmsghdr *nh;
    struct genlmsghdr *gh;
    int len;
    struct sockaddr_nl addr;

    memset(buf, 0, sizeof(buf));
    nh = (struct nlmsghdr *)buf;
    gh = (struct genlmsghdr *)(buf + NLMSG_HDRLEN);
    len = NLMSG_HDRLEN + GENL_HDRLEN;
    /* NL80211_ATTR_IFINDEX */
    *(unsigned short *)(buf + len) = 8;       /* nla_len */
    *(unsigned short *)(buf + len + 2) = 3;  /* NL80211_ATTR_IFINDEX */
    *(unsigned int *)(buf + len + 4) = ifindex;
    len += 8;
    nh->nlmsg_len = len;
    nh->nlmsg_type = fam;
    nh->nlmsg_flags = NLM_F_REQUEST;
    nh->nlmsg_seq = 2;
    gh->cmd = NL80211_CMD_TRIGGER_SCAN;
    gh->version = 1;
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    if (sendto(sock, buf, len, 0, (struct sockaddr *)&addr, sizeof(addr)) != (ssize_t)len)
        return -1;
    len = recv(sock, buf, sizeof(buf), 0);
    if (len < (int)NLMSG_HDRLEN) return -1;
    nh = (struct nlmsghdr *)buf;
    if (nh->nlmsg_type == NLMSG_ERROR) {
        struct { int error; struct nlmsghdr nh; } *err = (void *)buf;
        if (err->error != 0) return -1;
    }
    return 0;
}

static void parse_bss(const unsigned char *data, int len);
static int parse_attrs(const unsigned char *data, int len, int nest_bss);

static int do_get_scan(int sock, int fam, unsigned int ifindex) {
    char buf[32768];
    struct nlmsghdr *nh;
    struct genlmsghdr *gh;
    int len, off;
    struct sockaddr_nl addr;

    memset(buf, 0, sizeof(buf));
    nh = (struct nlmsghdr *)buf;
    gh = (struct genlmsghdr *)(buf + NLMSG_HDRLEN);
    len = NLMSG_HDRLEN + GENL_HDRLEN;
    *(unsigned short *)(buf + len) = 8;
    *(unsigned short *)(buf + len + 2) = NL80211_ATTR_IFINDEX;
    *(unsigned int *)(buf + len + 4) = ifindex;
    len += 8;
    nh->nlmsg_len = len;
    nh->nlmsg_type = fam;
    nh->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    nh->nlmsg_seq = 3;
    gh->cmd = NL80211_CMD_GET_SCAN;
    gh->version = 1;
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    if (sendto(sock, buf, len, 0, (struct sockaddr *)&addr, sizeof(addr)) != (ssize_t)len)
        return -1;

    printf("%-32s %6s %10s\n", "SSID", "SIGNAL", "SECURITY");
    printf("-------------------------------- -------- ----------\n");
    for (;;) {
        len = recv(sock, buf, sizeof(buf), 0);
        if (len <= 0) break;
        off = 0;
        while (off + (int)NLMSG_HDRLEN <= len) {
            nh = (struct nlmsghdr *)(buf + off);
            if (nh->nlmsg_len < NLMSG_HDRLEN || off + nh->nlmsg_len > len) break;
            if (nh->nlmsg_type == NLMSG_DONE) return 0;
            if (nh->nlmsg_type == NLMSG_ERROR) return 0;
            if (nh->nlmsg_type == (unsigned int)fam) {
                gh = (struct genlmsghdr *)(buf + off + NLMSG_HDRLEN);
                if (gh->cmd == NL80211_CMD_NEW_SCAN_RESULTS) {
                    parse_attrs((unsigned char *)(buf + off + NLMSG_HDRLEN + GENL_HDRLEN),
                               nh->nlmsg_len - NLMSG_HDRLEN - GENL_HDRLEN, 1);
                }
            }
            off += (nh->nlmsg_len + 3) & ~3;
        }
    }
    return 0;
}

static void parse_bss(const unsigned char *data, int len) {
    int off = 0;
    char ssid[33];
    int signal = -255;
    int secure = 0;
    memset(ssid, 0, sizeof(ssid));
    while (off + 4 <= len) {
        unsigned short nla_len = *(unsigned short *)(data + off);
        unsigned short nla_type = *(unsigned short *)(data + off + 2) & NLA_TYPE_MASK;
        if (nla_len < 4) break;
        if (off + nla_len > len) break;
        if (nla_type == NL80211_ATTR_BSS_SSID && nla_len > 4) {
            int ssid_len = nla_len - 4;
            if (ssid_len > 32) ssid_len = 32;
            memcpy(ssid, data + off + 4, ssid_len);
            ssid[ssid_len] = '\0';
        } else if (nla_type == NL80211_ATTR_BSS_SIGNAL_MBM && nla_len >= 8) {
            signal = (int)(*(int *)(data + off + 4)) / 100; /* mBm -> dBm */
        } else if (nla_type == NL80211_ATTR_BSS_INFORMATION_ELEMENTS && nla_len > 4) {
            /* Simplified: presence of IEs often means encrypted */
            secure = 1;
        }
        off += (nla_len + 3) & ~3;
    }
    if (ssid[0] == '\0') memcpy(ssid, "(hidden)", 9);
    printf("%-32s %5d dBm %s\n", ssid, signal, secure ? "WPA/WPA2" : "open");
}

static int parse_attrs(const unsigned char *data, int len, int nest_bss) {
    int off = 0;
    while (off + 4 <= len) {
        unsigned short nla_len = *(unsigned short *)(data + off);
        unsigned short nla_type = *(unsigned short *)(data + off + 2) & NLA_TYPE_MASK;
        if (nla_len < 4) break;
        if (off + nla_len > len) break;
        if (nla_type == NL80211_ATTR_BSS && nest_bss && nla_len > 4) {
            parse_bss(data + off + 4, nla_len - 4);
        }
        off += (nla_len + 3) & ~3;
    }
    return 0;
}

#elif defined(__APPLE__)
/* macOS: use airport -s (built-in; full path so no PATH dependency) */
#include <stdlib.h>

int wifi_native_available(void) {
    return 1;
}

int wifi_native_scan(const char *iface) {
    (void)iface;
    FILE *fp;
    char line[512];
    /* Full path: airport is not in PATH by default on macOS */
    const char *cmd = "\"/System/Library/PrivateFrameworks/Apple80211.framework/Resources/airport\" -s 2>/dev/null";
    
    fp = popen(cmd, "r");
    if (!fp) return -1;
    
    printf("%-32s %6s %10s\n", "SSID", "SIGNAL", "SECURITY");
    printf("-------------------------------- -------- ----------\n");
    
    if (fgets(line, sizeof(line), fp) != NULL) {
        /* Skip header line */
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        char ssid[33] = "";
        char bssid[32] = "";
        int rssi = 0;
        int ch = 0;
        char sec[64] = "";
        /* Format: "   SSID   BSSID   RSSI CH ... SECURITY" */
        if (sscanf(line, " %32s %31s %d %d %*s %*s %*s %63s", ssid, bssid, &rssi, &ch, sec) >= 3) {
            printf("%-32s %5d dBm %s\n", ssid[0] ? ssid : "(hidden)", rssi, sec[0] ? sec : "open");
        }
    }
    pclose(fp);
    return 0;
}

#else
/* Windows / other: use stub (on Windows, run Void in WSL for WiFi) */
int wifi_native_available(void) {
    return 0;
}

int wifi_native_scan(const char *iface) {
    (void)iface;
    return -1;
}
#endif
