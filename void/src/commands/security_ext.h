#ifndef SECURITY_EXT_H
#define SECURITY_EXT_H

#include "../../include/void.h"

/* Category 9: Security & Encryption (100 commands) */

/* Encryption & Hashing (additional) */
int cmd_gpg_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_gpg2(int argc, char **argv, shell_context_t *ctx);
int cmd_pgp(int argc, char **argv, shell_context_t *ctx);
int cmd_sha1sum_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_sha512sum_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_b2sum_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_cksum_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_sum_ext(int argc, char **argv, shell_context_t *ctx);

/* Security Scanning (additional) */
int cmd_burpsuite(int argc, char **argv, shell_context_t *ctx);
int cmd_metasploit(int argc, char **argv, shell_context_t *ctx);
int cmd_aircrack_ng(int argc, char **argv, shell_context_t *ctx);
int cmd_reaver(int argc, char **argv, shell_context_t *ctx);
int cmd_hashcat(int argc, char **argv, shell_context_t *ctx);
int cmd_john(int argc, char **argv, shell_context_t *ctx);
int cmd_hydra(int argc, char **argv, shell_context_t *ctx);
int cmd_sqlmap(int argc, char **argv, shell_context_t *ctx);
int cmd_nikto(int argc, char **argv, shell_context_t *ctx);
int cmd_wpscan(int argc, char **argv, shell_context_t *ctx);
int cmd_gobuster(int argc, char **argv, shell_context_t *ctx);
int cmd_feroxbuster(int argc, char **argv, shell_context_t *ctx);
int cmd_void_install_kali(int argc, char **argv, shell_context_t *ctx);

/* Firewall & Access Control (additional) */
int cmd_hosts_allow(int argc, char **argv, shell_context_t *ctx);
int cmd_hosts_deny(int argc, char **argv, shell_context_t *ctx);

/* Authentication (additional) */
int cmd_ssh_keygen(int argc, char **argv, shell_context_t *ctx);
int cmd_ssh_add(int argc, char **argv, shell_context_t *ctx);
int cmd_ssh_agent(int argc, char **argv, shell_context_t *ctx);
int cmd_ssh_copy_id(int argc, char **argv, shell_context_t *ctx);
int cmd_pass(int argc, char **argv, shell_context_t *ctx);
int cmd_keepassxc_cli(int argc, char **argv, shell_context_t *ctx);

#endif /* SECURITY_EXT_H */

