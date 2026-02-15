#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "../../include/void.h"
#include <stddef.h>

/* Network and WiFi Management Commands */

/* WiFi Management */
int cmd_wifi_scan(int argc, char **argv, shell_context_t *ctx);
int cmd_wifi_connect(int argc, char **argv, shell_context_t *ctx);
int cmd_wifi_disconnect(int argc, char **argv, shell_context_t *ctx);
int cmd_wifi_status(int argc, char **argv, shell_context_t *ctx);
int cmd_wifi_list(int argc, char **argv, shell_context_t *ctx);
int cmd_wifi_on(int argc, char **argv, shell_context_t *ctx);
int cmd_wifi_off(int argc, char **argv, shell_context_t *ctx);

/* Network Interface Management */
int cmd_net_up(int argc, char **argv, shell_context_t *ctx);
int cmd_net_down(int argc, char **argv, shell_context_t *ctx);
int cmd_net_restart(int argc, char **argv, shell_context_t *ctx);
int cmd_net_status(int argc, char **argv, shell_context_t *ctx);
int cmd_net_dhcp(int argc, char **argv, shell_context_t *ctx);
int cmd_net_static(int argc, char **argv, shell_context_t *ctx);

/* Connectivity Testing */
int cmd_net_test(int argc, char **argv, shell_context_t *ctx);
int cmd_net_check(int argc, char **argv, shell_context_t *ctx);

/* DNS Management */
int cmd_dns_set(int argc, char **argv, shell_context_t *ctx);
int cmd_dns_get(int argc, char **argv, shell_context_t *ctx);
int cmd_dns_test(int argc, char **argv, shell_context_t *ctx);

/* Helper Functions */
int wifi_scan_networks(char ***networks, int *count);
int wifi_connect_to_network(const char *ssid, const char *password);
int wifi_get_status(char *interface, char *ssid, char *ip);
int network_interface_up(const char *interface);
int network_interface_down(const char *interface);
int network_get_ip(const char *interface, char *ip, size_t ip_len);
int network_configure_dhcp(const char *interface);
int network_configure_static(const char *interface, const char *ip, const char *netmask, const char *gateway);
int test_internet_connectivity(void);
int get_dns_servers(char **dns1, char **dns2);

#endif /* NETWORK_MANAGER_H */

