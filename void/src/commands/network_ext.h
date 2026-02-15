#ifndef NETWORK_EXT_H
#define NETWORK_EXT_H

#include "../../include/void.h"

/* Category 4: Networking (150 commands) */

/* Basic Network Operations */
int cmd_ping6(int argc, char **argv, shell_context_t *ctx);
int cmd_fping(int argc, char **argv, shell_context_t *ctx);
int cmd_hping(int argc, char **argv, shell_context_t *ctx);
int cmd_traceroute_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_tracepath(int argc, char **argv, shell_context_t *ctx);
int cmd_mtr(int argc, char **argv, shell_context_t *ctx);
int cmd_pathping(int argc, char **argv, shell_context_t *ctx);

/* Network Configuration */
int cmd_ifconfig(int argc, char **argv, shell_context_t *ctx);
int cmd_ip_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_route_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_netstat(int argc, char **argv, shell_context_t *ctx);
int cmd_ss(int argc, char **argv, shell_context_t *ctx);
int cmd_iwconfig(int argc, char **argv, shell_context_t *ctx);
int cmd_iwlist(int argc, char **argv, shell_context_t *ctx);
int cmd_nmcli(int argc, char **argv, shell_context_t *ctx);
int cmd_nmtui(int argc, char **argv, shell_context_t *ctx);
int cmd_wpa_cli(int argc, char **argv, shell_context_t *ctx);
int cmd_hostname_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_domainname(int argc, char **argv, shell_context_t *ctx);
int cmd_dnsdomainname(int argc, char **argv, shell_context_t *ctx);

/* Network Scanning & Discovery */
int cmd_nmap_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_masscan(int argc, char **argv, shell_context_t *ctx);
int cmd_zmap(int argc, char **argv, shell_context_t *ctx);
int cmd_arp_scan(int argc, char **argv, shell_context_t *ctx);
int cmd_netdiscover(int argc, char **argv, shell_context_t *ctx);
int cmd_nbtscan(int argc, char **argv, shell_context_t *ctx);
int cmd_smbclient(int argc, char **argv, shell_context_t *ctx);
int cmd_enum4linux(int argc, char **argv, shell_context_t *ctx);

/* Network Monitoring */
int cmd_tcpdump(int argc, char **argv, shell_context_t *ctx);
int cmd_wireshark(int argc, char **argv, shell_context_t *ctx);
int cmd_tshark(int argc, char **argv, shell_context_t *ctx);
int cmd_ngrep(int argc, char **argv, shell_context_t *ctx);
int cmd_iftop(int argc, char **argv, shell_context_t *ctx);
int cmd_nethogs_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_bmon(int argc, char **argv, shell_context_t *ctx);
int cmd_vnstat(int argc, char **argv, shell_context_t *ctx);
int cmd_iptraf(int argc, char **argv, shell_context_t *ctx);
int cmd_nload(int argc, char **argv, shell_context_t *ctx);
int cmd_slurm(int argc, char **argv, shell_context_t *ctx);
int cmd_speedometer(int argc, char **argv, shell_context_t *ctx);

/* Network Services */
int cmd_curl_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_wget(int argc, char **argv, shell_context_t *ctx);
int cmd_aria2c(int argc, char **argv, shell_context_t *ctx);
int cmd_axel(int argc, char **argv, shell_context_t *ctx);
int cmd_httpie(int argc, char **argv, shell_context_t *ctx);
int cmd_http(int argc, char **argv, shell_context_t *ctx);
int cmd_httpx(int argc, char **argv, shell_context_t *ctx);
int cmd_dirb(int argc, char **argv, shell_context_t *ctx);
/* Note: feroxbuster, gobuster, nikto, sqlmap, wpscan are in security_ext.h */

/* DNS Operations */
int cmd_nslookup(int argc, char **argv, shell_context_t *ctx);
int cmd_dig(int argc, char **argv, shell_context_t *ctx);
int cmd_host_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_getent(int argc, char **argv, shell_context_t *ctx);
int cmd_resolveip(int argc, char **argv, shell_context_t *ctx);
int cmd_dnsenum(int argc, char **argv, shell_context_t *ctx);
int cmd_dnsrecon(int argc, char **argv, shell_context_t *ctx);
int cmd_fierce(int argc, char **argv, shell_context_t *ctx);
int cmd_sublist3r(int argc, char **argv, shell_context_t *ctx);
int cmd_amass(int argc, char **argv, shell_context_t *ctx);

/* Network Protocols */
int cmd_telnet(int argc, char **argv, shell_context_t *ctx);
int cmd_nc(int argc, char **argv, shell_context_t *ctx);
int cmd_ncat(int argc, char **argv, shell_context_t *ctx);
int cmd_socat(int argc, char **argv, shell_context_t *ctx);
int cmd_openssl_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_ssh_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_scp_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_sftp_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_rsync(int argc, char **argv, shell_context_t *ctx);
int cmd_rsh(int argc, char **argv, shell_context_t *ctx);
int cmd_rlogin(int argc, char **argv, shell_context_t *ctx);
int cmd_ftp(int argc, char **argv, shell_context_t *ctx);
int cmd_lftp(int argc, char **argv, shell_context_t *ctx);
int cmd_wput(int argc, char **argv, shell_context_t *ctx);

/* Network Security */
int cmd_iptables(int argc, char **argv, shell_context_t *ctx);
int cmd_ip6tables(int argc, char **argv, shell_context_t *ctx);
int cmd_ufw(int argc, char **argv, shell_context_t *ctx);
int cmd_firewalld(int argc, char **argv, shell_context_t *ctx);
int cmd_fail2ban_client(int argc, char **argv, shell_context_t *ctx);
int cmd_tcpwrappers(int argc, char **argv, shell_context_t *ctx);

#endif /* NETWORK_EXT_H */

