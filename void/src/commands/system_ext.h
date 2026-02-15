#ifndef SYSTEM_EXT_H
#define SYSTEM_EXT_H

#include "../../include/void.h"

/* Category 5: System Administration (150 commands) */

/* System Information */
int cmd_uname_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_arch(int argc, char **argv, shell_context_t *ctx);
int cmd_uptime(int argc, char **argv, shell_context_t *ctx);
int cmd_w_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_who_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_id_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_groups_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_last(int argc, char **argv, shell_context_t *ctx);
int cmd_lastlog(int argc, char **argv, shell_context_t *ctx);
int cmd_finger(int argc, char **argv, shell_context_t *ctx);
int cmd_users(int argc, char **argv, shell_context_t *ctx);
int cmd_logname(int argc, char **argv, shell_context_t *ctx);

/* System Monitoring */
int cmd_vmstat(int argc, char **argv, shell_context_t *ctx);
int cmd_iostat(int argc, char **argv, shell_context_t *ctx);
int cmd_mpstat(int argc, char **argv, shell_context_t *ctx);
int cmd_sar(int argc, char **argv, shell_context_t *ctx);
int cmd_free(int argc, char **argv, shell_context_t *ctx);
int cmd_lscpu(int argc, char **argv, shell_context_t *ctx);
int cmd_lspci(int argc, char **argv, shell_context_t *ctx);
int cmd_lsusb(int argc, char **argv, shell_context_t *ctx);
int cmd_lsblk(int argc, char **argv, shell_context_t *ctx);
int cmd_blkid(int argc, char **argv, shell_context_t *ctx);
int cmd_fdisk(int argc, char **argv, shell_context_t *ctx);
int cmd_parted(int argc, char **argv, shell_context_t *ctx);
int cmd_gparted(int argc, char **argv, shell_context_t *ctx);
int cmd_lsmod(int argc, char **argv, shell_context_t *ctx);
int cmd_modinfo(int argc, char **argv, shell_context_t *ctx);
int cmd_modprobe(int argc, char **argv, shell_context_t *ctx);
int cmd_rmmod(int argc, char **argv, shell_context_t *ctx);
int cmd_insmod(int argc, char **argv, shell_context_t *ctx);
int cmd_depmod(int argc, char **argv, shell_context_t *ctx);
int cmd_dmesg(int argc, char **argv, shell_context_t *ctx);
int cmd_journalctl(int argc, char **argv, shell_context_t *ctx);

/* System Control */
int cmd_shutdown(int argc, char **argv, shell_context_t *ctx);
int cmd_reboot(int argc, char **argv, shell_context_t *ctx);
int cmd_halt(int argc, char **argv, shell_context_t *ctx);
int cmd_poweroff(int argc, char **argv, shell_context_t *ctx);
int cmd_init_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_service(int argc, char **argv, shell_context_t *ctx);
int cmd_chkconfig(int argc, char **argv, shell_context_t *ctx);
int cmd_update_rc_d(int argc, char **argv, shell_context_t *ctx);
int cmd_systemd_analyze(int argc, char **argv, shell_context_t *ctx);
int cmd_systemd_cgtop(int argc, char **argv, shell_context_t *ctx);
int cmd_systemd_cgls(int argc, char **argv, shell_context_t *ctx);

/* User & Group Management */
int cmd_useradd(int argc, char **argv, shell_context_t *ctx);
int cmd_userdel(int argc, char **argv, shell_context_t *ctx);
int cmd_usermod(int argc, char **argv, shell_context_t *ctx);
int cmd_passwd(int argc, char **argv, shell_context_t *ctx);
int cmd_chage(int argc, char **argv, shell_context_t *ctx);
int cmd_chfn(int argc, char **argv, shell_context_t *ctx);
int cmd_chsh(int argc, char **argv, shell_context_t *ctx);
int cmd_groupadd(int argc, char **argv, shell_context_t *ctx);
int cmd_groupdel(int argc, char **argv, shell_context_t *ctx);
int cmd_groupmod(int argc, char **argv, shell_context_t *ctx);
int cmd_gpasswd(int argc, char **argv, shell_context_t *ctx);
int cmd_newgrp(int argc, char **argv, shell_context_t *ctx);
int cmd_su(int argc, char **argv, shell_context_t *ctx);
int cmd_sudo(int argc, char **argv, shell_context_t *ctx);
int cmd_visudo(int argc, char **argv, shell_context_t *ctx);

/* Package Management */
int cmd_apt(int argc, char **argv, shell_context_t *ctx);
int cmd_apt_get(int argc, char **argv, shell_context_t *ctx);
int cmd_apt_cache(int argc, char **argv, shell_context_t *ctx);
int cmd_dpkg(int argc, char **argv, shell_context_t *ctx);
int cmd_rpm(int argc, char **argv, shell_context_t *ctx);
int cmd_yum(int argc, char **argv, shell_context_t *ctx);
int cmd_dnf(int argc, char **argv, shell_context_t *ctx);
int cmd_pacman(int argc, char **argv, shell_context_t *ctx);
int cmd_zypper(int argc, char **argv, shell_context_t *ctx);
int cmd_emerge(int argc, char **argv, shell_context_t *ctx);
int cmd_brew(int argc, char **argv, shell_context_t *ctx);
int cmd_snap(int argc, char **argv, shell_context_t *ctx);
int cmd_flatpak(int argc, char **argv, shell_context_t *ctx);
int cmd_pip(int argc, char **argv, shell_context_t *ctx);
int cmd_npm(int argc, char **argv, shell_context_t *ctx);
int cmd_cargo(int argc, char **argv, shell_context_t *ctx);
int cmd_go_pkg(int argc, char **argv, shell_context_t *ctx);

/* System Logs */
int cmd_logwatch(int argc, char **argv, shell_context_t *ctx);
int cmd_logrotate(int argc, char **argv, shell_context_t *ctx);
int cmd_rsyslog(int argc, char **argv, shell_context_t *ctx);
int cmd_syslog_ng(int argc, char **argv, shell_context_t *ctx);

#endif /* SYSTEM_EXT_H */

