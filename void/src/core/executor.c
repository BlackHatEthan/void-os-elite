#include "executor.h"
#include "parser.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../commands/core.h"
#include "../commands/filesystem.h"
#include "../commands/filesystem_ext.h"
#include "../commands/process.h"
#include "../commands/process_ext.h"
#include "../commands/network_ext.h"
#include "../commands/network_manager.h"
#include "../commands/system_ext.h"
#include "../commands/text.h"
#include "../commands/development.h"
#include "../commands/database.h"
#include "../commands/security_ext.h"
#include "../commands/system_utils.h"
#include "../commands/void_os.h"
#include "../commands/void_help.h"
#include "../commands/multimedia.h"
#include "../commands/archive.h"
#include "../commands/misc.h"
#include "../commands/environment.h"
#include "../commands/alias.h"
#include "../commands/environment_ext.h"
#include "../commands/git.h"
#include "../commands/context.h"
#include "../commands/utils.h"
#include "../commands/network.h"
#include "../commands/system.h"
#include "../commands/security.h"
#include "../commands/plugin.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

/* Built-in command handlers */
static int cmd_exit(int argc, char **argv, shell_context_t *ctx);
static int cmd_echo(int argc, char **argv, shell_context_t *ctx);
static int cmd_pwd(int argc, char **argv, shell_context_t *ctx);
static int cmd_cd(int argc, char **argv, shell_context_t *ctx);
static int cmd_whoami(int argc, char **argv, shell_context_t *ctx);
static int cmd_identity(int argc, char **argv, shell_context_t *ctx);

/* Built-in commands table */
static struct {
    const char *name;
    command_func_t func;
} builtins[] = {
    {"exit", cmd_exit},
    {"echo", cmd_echo},
    {"pwd", cmd_pwd},
    {"cd", cmd_cd},
    {"whoami", cmd_whoami},
    {"identity", cmd_identity},
    {"pulse", cmd_pulse},
    {"warp", cmd_warp},
    {"vanish", cmd_vanish},
    {"echoes", cmd_echoes},
    {"shroud", cmd_shroud},
    {"transmute", cmd_transmute},
    {"veil", cmd_veil},
    {"seal", cmd_seal},
    {"watch-dir", cmd_watch},
    {"scavenge", cmd_scavenge},
    {"marrow", cmd_marrow},
    {"dissect", cmd_dissect},
    {"entropy-check", cmd_entropy_check},
    {"fossil", cmd_fossil},
    {"spawn", cmd_spawn},
    {"jobs", cmd_jobs},
    {"ps", cmd_ps},
    {"terminate", cmd_terminate},
    {"foreground", cmd_foreground},
    {"silence", cmd_silence},
    {"awaken", cmd_awaken},
    {"env", cmd_env},
    {"export", cmd_export},
    {"unset", cmd_unset},
    {"reload", cmd_reload},
    {"alias", cmd_alias},
    {"unalias", cmd_unalias},
    {"aliases", cmd_aliases},
    {"mimic", cmd_mimic},
    {"tether", cmd_tether},
    {"anchor", cmd_anchor},
    {"sync", cmd_sync},
    {"transmit", cmd_transmit},
    {"commit", cmd_commit},
    {"status", cmd_status},
    {"context", cmd_context},
    {"summon", cmd_summon},
    {"forgepy", cmd_forgepy},
    {"multitool", cmd_multitool},
    {"fabricate", cmd_fabricate},
    {"refactor", cmd_refactor},
    {"minify", cmd_minify},
    {"blueprint", cmd_blueprint},
    {"debug-vision", cmd_debug_vision},
    {"intercept", cmd_intercept},
    {"ping", cmd_ping},
    {"ports", cmd_ports},
    {"scan", cmd_scan},
    {"serve", cmd_serve},
    {"tunnel-info", cmd_tunnel_info},
    {"throb", cmd_throb},
    {"rift", cmd_rift},
    {"wormhole", cmd_wormhole},
    {"beacon", cmd_beacon},
    {"trace-route", cmd_trace_route},
    {"ghost-ping", cmd_ghost_ping},
    {"entropy", cmd_entropy},
    {"pulse-graph", cmd_pulse_graph},
    {"synapse", cmd_synapse},
    {"organelle", cmd_organelle},
    {"vitals", cmd_vitals},
    {"tempest", cmd_tempest},
    {"neural-map", cmd_neural_map},
    {"overclock-view", cmd_overclock_view},
    {"voltage", cmd_voltage},
    {"startup-check", cmd_startup_check},
    {"perm-audit", cmd_perm_audit},
    {"net-watch", cmd_net_watch},
    {"incantation", cmd_incantation},
    {"black-hole", cmd_black_hole},
    {"echo-chamber", cmd_echo_chamber},
    {"stasis", cmd_stasis},
    {"black-box", cmd_black_box},
    {"signal-lost", cmd_signal_lost},
    {"chronos", cmd_chronos},
    {"singularity", cmd_singularity},
    {"glitch", cmd_glitch},
    {"forge", cmd_forge},
    /* Category 1: Core Shell Commands */
    {"history", cmd_history},
    {"history-search", cmd_history_search},
    {"history-clear", cmd_history_clear},
    {"repeat", cmd_repeat},
    {"time", cmd_time_cmd},
    {"which", cmd_which_cmd},
    {"whereis", cmd_whereis_cmd},
    {"type", cmd_type_cmd},
    {"help", cmd_help_cmd},
    {"man", cmd_man_cmd},
    {"info", cmd_info_cmd},
    {"apropos", cmd_apropos_cmd},
    {"whatis", cmd_whatis_cmd},
    {"cat", cmd_cat},
    {"tac", cmd_tac},
    {"head", cmd_head},
    {"tail", cmd_tail},
    {"less", cmd_less},
    {"more", cmd_more},
    {"tee", cmd_tee},
    {"sponge", cmd_sponge},
    {"grep", cmd_grep_core},
    {"egrep", cmd_egrep},
    {"fgrep", cmd_fgrep},
    {"sed", cmd_sed},
    {"awk", cmd_awk},
    {"cut", cmd_cut},
    {"paste", cmd_paste},
    {"join", cmd_join},
    {"sort", cmd_sort},
    {"uniq", cmd_uniq},
    {"wc", cmd_wc},
    {"tr", cmd_tr},
    {"fold", cmd_fold},
    {"fmt", cmd_fmt},
    {"nl", cmd_nl},
    {"pr", cmd_pr},
    {"split", cmd_split},
    {"csplit", cmd_csplit},
    /* Category 2: Filesystem Operations */
    {"touch", cmd_touch},
    {"mkdir", cmd_mkdir_ext},
    {"rmdir", cmd_rmdir_ext},
    {"rm", cmd_rm},
    {"cp", cmd_cp},
    {"mv", cmd_mv},
    {"ln", cmd_ln},
    {"symlink", cmd_symlink_ext},
    {"readlink", cmd_readlink_ext},
    {"stat", cmd_stat_ext},
    {"file", cmd_file_type},
    {"mime", cmd_mime},
    {"size", cmd_size},
    {"du", cmd_du},
    {"df", cmd_df},
    {"find", cmd_find_ext},
    {"locate", cmd_locate},
    {"updatedb", cmd_updatedb},
    {"where", cmd_where},
    {"realpath", cmd_realpath_ext},
    {"basename", cmd_basename_ext},
    {"dirname", cmd_dirname_ext},
    {"pathjoin", cmd_pathjoin},
    {"pathsplit", cmd_pathsplit},
    {"pathnorm", cmd_pathnorm},
    {"pathabs", cmd_pathabs},
    {"pathrel", cmd_pathrel},
    {"chmod", cmd_chmod_ext},
    {"chown", cmd_chown_ext},
    {"chgrp", cmd_chgrp_ext},
    {"umask", cmd_umask_ext},
    {"getfacl", cmd_getfacl},
    {"setfacl", cmd_setfacl},
    {"lsattr", cmd_lsattr},
    {"chattr", cmd_chattr},
    {"getfattr", cmd_getfattr},
    {"setfattr", cmd_setfattr},
    {"diff", cmd_diff},
    {"diff3", cmd_diff3},
    {"sdiff", cmd_sdiff},
    {"cmp", cmd_cmp},
    {"comm", cmd_comm},
    {"patch", cmd_patch},
    {"diffstat", cmd_diffstat},
    {"gzip", cmd_gzip},
    {"gunzip", cmd_gunzip},
    {"bzip2", cmd_bzip2},
    {"bunzip2", cmd_bunzip2},
    {"xz", cmd_xz},
    {"unxz", cmd_unxz},
    {"zip", cmd_zip},
    {"unzip", cmd_unzip},
    {"tar", cmd_tar},
    {"zcat", cmd_zcat},
    {"zless", cmd_zless},
    {"zmore", cmd_zmore},
    {"zcmp", cmd_zcmp},
    {"zdiff", cmd_zdiff},
    {"md5sum", cmd_md5sum},
    {"sha1sum", cmd_sha1sum},
    {"sha256sum", cmd_sha256sum},
    {"sha512sum", cmd_sha512sum},
    {"cksum", cmd_cksum},
    {"sum", cmd_sum},
    {"b2sum", cmd_b2sum},
    {"verify", cmd_verify},
    {"inotifywait", cmd_inotifywait},
    {"inotifywatch", cmd_inotifywatch},
    {"fswatch", cmd_fswatch},
    {"entr", cmd_entr},
    {"dos2unix", cmd_dos2unix},
    {"unix2dos", cmd_unix2dos},
    {"mac2unix", cmd_mac2unix},
    {"unix2mac", cmd_unix2mac},
    {"iconv", cmd_iconv_ext},
    {"uuencode", cmd_uuencode},
    {"uudecode", cmd_uudecode},
    {"hexdump", cmd_hexdump},
    {"od", cmd_od},
    {"xxd", cmd_xxd},
    {"strings", cmd_strings_ext},
    {"bin2hex", cmd_bin2hex},
    {"hex2bin", cmd_hex2bin},
    {"shred", cmd_shred},
    {"wipe", cmd_wipe},
    {"srm", cmd_srm},
    {"secure-delete", cmd_secure_delete},
    {"truncate", cmd_truncate_ext},
    {"fallocate", cmd_fallocate},
    {"mktemp", cmd_mktemp},
    {"tempfile", cmd_tempfile},
    /* Category 3: Process Management */
    {"kill", cmd_kill_ext},
    {"killall", cmd_killall},
    {"pkill", cmd_pkill},
    {"xkill", cmd_xkill},
    {"killpg", cmd_killpg_ext},
    {"pkillall", cmd_pkillall},
    {"skill", cmd_skill},
    {"snice", cmd_snice},
    {"renice", cmd_renice},
    {"nice", cmd_nice_ext},
    {"nohup", cmd_nohup},
    {"setsid", cmd_setsid_ext},
    {"disown", cmd_disown},
    {"fg", cmd_fg_ext},
    {"bg", cmd_bg_ext},
    {"wait", cmd_wait_ext},
    {"waitpid", cmd_waitpid_ext},
    {"pstree", cmd_pstree},
    {"pgrep", cmd_pgrep},
    {"pidof", cmd_pidof},
    {"fuser", cmd_fuser},
    {"lsof", cmd_lsof_ext},
    {"pmap", cmd_pmap},
    {"procinfo", cmd_procinfo},
    {"pidstat", cmd_pidstat},
    {"top", cmd_top_ext},
    {"htop", cmd_htop},
    {"btop", cmd_btop},
    {"glances", cmd_glances},
    {"nmon", cmd_nmon},
    {"iotop", cmd_iotop},
    {"powertop", cmd_powertop},
    {"nethogs", cmd_nethogs_ext},
    {"strace", cmd_strace_ext},
    {"ltrace", cmd_ltrace},
    {"ptrace", cmd_ptrace_ext},
    {"gdb", cmd_gdb_ext},
    {"lldb", cmd_lldb_ext},
    {"valgrind", cmd_valgrind},
    {"perf", cmd_perf},
    {"timeout", cmd_timeout},
    {"watch", cmd_watch_process},
    {"at", cmd_at},
    {"atq", cmd_atq},
    {"atrm", cmd_atrm},
    {"batch", cmd_batch},
    {"cron", cmd_cron},
    {"crontab", cmd_crontab},
    {"anacron", cmd_anacron},
    {"systemd-run", cmd_systemd_run},
    {"systemctl", cmd_systemctl},
    {"ulimit", cmd_ulimit_ext},
    {"prlimit", cmd_prlimit},
    {"limit", cmd_limit_ext},
    {"quota", cmd_quota},
    {"edquota", cmd_edquota},
    {"repquota", cmd_repquota},
    /* Category 4: Networking */
    {"ping6", cmd_ping6},
    {"fping", cmd_fping},
    {"hping", cmd_hping},
    {"traceroute", cmd_traceroute_ext},
    {"tracepath", cmd_tracepath},
    {"mtr", cmd_mtr},
    {"pathping", cmd_pathping},
    {"ifconfig", cmd_ifconfig},
    {"ip", cmd_ip_ext},
    {"route", cmd_route_ext},
    {"netstat", cmd_netstat},
    {"ss", cmd_ss},
    {"iwconfig", cmd_iwconfig},
    {"iwlist", cmd_iwlist},
    {"nmcli", cmd_nmcli},
    {"nmtui", cmd_nmtui},
    {"wpa-cli", cmd_wpa_cli},
    {"hostname", cmd_hostname_ext},
    {"domainname", cmd_domainname},
    {"dnsdomainname", cmd_dnsdomainname},
    {"nmap", cmd_nmap_ext},
    {"masscan", cmd_masscan},
    {"zmap", cmd_zmap},
    {"arp-scan", cmd_arp_scan},
    {"netdiscover", cmd_netdiscover},
    {"nbtscan", cmd_nbtscan},
    {"smbclient", cmd_smbclient},
    {"enum4linux", cmd_enum4linux},
    {"tcpdump", cmd_tcpdump},
    {"wireshark", cmd_wireshark},
    {"tshark", cmd_tshark},
    {"ngrep", cmd_ngrep},
    {"iftop", cmd_iftop},
    {"bmon", cmd_bmon},
    {"vnstat", cmd_vnstat},
    {"iptraf", cmd_iptraf},
    {"nload", cmd_nload},
    {"slurm", cmd_slurm},
    {"speedometer", cmd_speedometer},
    {"curl", cmd_curl_ext},
    {"wget", cmd_wget},
    {"aria2c", cmd_aria2c},
    {"axel", cmd_axel},
    {"httpie", cmd_httpie},
    {"http", cmd_http},
    {"httpx", cmd_httpx},
    {"dirb", cmd_dirb},
    {"nslookup", cmd_nslookup},
    {"dig", cmd_dig},
    {"host", cmd_host_ext},
    /* Network & WiFi Management */
    {"wifi-scan", cmd_wifi_scan},
    {"wifi-connect", cmd_wifi_connect},
    {"wifi-disconnect", cmd_wifi_disconnect},
    {"wifi-status", cmd_wifi_status},
    {"wifi-list", cmd_wifi_list},
    {"wifi-on", cmd_wifi_on},
    {"wifi-off", cmd_wifi_off},
    {"net-up", cmd_net_up},
    {"net-down", cmd_net_down},
    {"net-restart", cmd_net_restart},
    {"net-status", cmd_net_status},
    {"net-dhcp", cmd_net_dhcp},
    {"net-static", cmd_net_static},
    {"net-test", cmd_net_test},
    {"net-check", cmd_net_check},
    {"dns-set", cmd_dns_set},
    {"dns-get", cmd_dns_get},
    {"dns-test", cmd_dns_test},
    {"getent", cmd_getent},
    {"resolveip", cmd_resolveip},
    {"dnsenum", cmd_dnsenum},
    {"dnsrecon", cmd_dnsrecon},
    {"fierce", cmd_fierce},
    {"sublist3r", cmd_sublist3r},
    {"amass", cmd_amass},
    {"telnet", cmd_telnet},
    {"nc", cmd_nc},
    {"ncat", cmd_ncat},
    {"socat", cmd_socat},
    {"openssl", cmd_openssl_ext},
    {"ssh", cmd_ssh_ext},
    {"scp", cmd_scp_ext},
    {"sftp", cmd_sftp_ext},
    {"rsync", cmd_rsync},
    {"rsh", cmd_rsh},
    {"rlogin", cmd_rlogin},
    {"ftp", cmd_ftp},
    {"lftp", cmd_lftp},
    {"wput", cmd_wput},
    {"iptables", cmd_iptables},
    {"ip6tables", cmd_ip6tables},
    {"ufw", cmd_ufw},
    {"firewalld", cmd_firewalld},
    {"fail2ban-client", cmd_fail2ban_client},
    {"tcpwrappers", cmd_tcpwrappers},
    /* Category 5: System Administration */
    {"uname", cmd_uname_ext},
    {"arch", cmd_arch},
    {"uptime", cmd_uptime},
    {"w", cmd_w_ext},
    {"who", cmd_who_ext},
    {"id", cmd_id_ext},
    {"groups", cmd_groups_ext},
    {"last", cmd_last},
    {"lastlog", cmd_lastlog},
    {"finger", cmd_finger},
    {"users", cmd_users},
    {"logname", cmd_logname},
    {"vmstat", cmd_vmstat},
    {"iostat", cmd_iostat},
    {"mpstat", cmd_mpstat},
    {"sar", cmd_sar},
    {"free", cmd_free},
    {"lscpu", cmd_lscpu},
    {"lspci", cmd_lspci},
    {"lsusb", cmd_lsusb},
    {"lsblk", cmd_lsblk},
    {"blkid", cmd_blkid},
    {"fdisk", cmd_fdisk},
    {"parted", cmd_parted},
    {"gparted", cmd_gparted},
    {"lsmod", cmd_lsmod},
    {"modinfo", cmd_modinfo},
    {"modprobe", cmd_modprobe},
    {"rmmod", cmd_rmmod},
    {"insmod", cmd_insmod},
    {"depmod", cmd_depmod},
    {"dmesg", cmd_dmesg},
    {"journalctl", cmd_journalctl},
    {"shutdown", cmd_shutdown},
    {"reboot", cmd_reboot},
    {"halt", cmd_halt},
    {"poweroff", cmd_poweroff},
    {"init", cmd_init_ext},
    {"service", cmd_service},
    {"chkconfig", cmd_chkconfig},
    {"update-rc.d", cmd_update_rc_d},
    {"systemd-analyze", cmd_systemd_analyze},
    {"systemd-cgtop", cmd_systemd_cgtop},
    {"systemd-cgls", cmd_systemd_cgls},
    {"useradd", cmd_useradd},
    {"userdel", cmd_userdel},
    {"usermod", cmd_usermod},
    {"passwd", cmd_passwd},
    {"chage", cmd_chage},
    {"chfn", cmd_chfn},
    {"chsh", cmd_chsh},
    {"groupadd", cmd_groupadd},
    {"groupdel", cmd_groupdel},
    {"groupmod", cmd_groupmod},
    {"gpasswd", cmd_gpasswd},
    {"newgrp", cmd_newgrp},
    {"su", cmd_su},
    {"sudo", cmd_sudo},
    {"visudo", cmd_visudo},
    {"apt", cmd_apt},
    {"apt-get", cmd_apt_get},
    {"apt-cache", cmd_apt_cache},
    {"dpkg", cmd_dpkg},
    {"rpm", cmd_rpm},
    {"yum", cmd_yum},
    {"dnf", cmd_dnf},
    {"pacman", cmd_pacman},
    {"zypper", cmd_zypper},
    {"emerge", cmd_emerge},
    {"brew", cmd_brew},
    {"snap", cmd_snap},
    {"flatpak", cmd_flatpak},
    {"pip", cmd_pip},
    {"npm", cmd_npm},
    {"cargo", cmd_cargo},
    {"go", cmd_go_pkg},
    {"logwatch", cmd_logwatch},
    {"logrotate", cmd_logrotate},
    {"rsyslog", cmd_rsyslog},
    {"syslog-ng", cmd_syslog_ng},
    /* Category 6: Text Processing & Editing */
    {"vi", cmd_vi},
    {"vim", cmd_vim},
    {"nano", cmd_nano},
    {"emacs", cmd_emacs},
    {"ed", cmd_ed},
    {"expand", cmd_expand},
    {"unexpand", cmd_unexpand},
    {"col", cmd_col},
    {"colrm", cmd_colrm},
    {"column", cmd_column},
    {"rev", cmd_rev},
    {"shuf", cmd_shuf},
    {"most", cmd_most},
    {"ag", cmd_ag},
    {"rg", cmd_rg},
    {"ack", cmd_ack},
    {"ugrep", cmd_ugrep},
    {"sift", cmd_sift},
    {"pt", cmd_pt},
    {"wc-l", cmd_wc_l},
    {"wc-w", cmd_wc_w},
    {"wc-c", cmd_wc_c},
    {"wc-m", cmd_wc_m},
    {"wc-L", cmd_wc_L},
    /* Category 7: Development Tools */
    {"svn", cmd_svn},
    {"hg", cmd_hg},
    {"bzr", cmd_bzr},
    {"cvs", cmd_cvs},
    {"fossil", cmd_fossil_scm},
    {"make", cmd_make_ext},
    {"cmake", cmd_cmake},
    {"autoconf", cmd_autoconf},
    {"automake", cmd_automake},
    {"configure", cmd_configure},
    {"ninja", cmd_ninja},
    {"bazel", cmd_bazel},
    {"buck", cmd_buck},
    {"gradle", cmd_gradle},
    {"maven", cmd_maven},
    {"ant", cmd_ant},
    {"scons", cmd_scons},
    {"waf", cmd_waf},
    {"gcc", cmd_gcc},
    {"g++", cmd_gpp},
    {"clang", cmd_clang_ext},
    {"clang++", cmd_clangpp},
    {"rustc", cmd_rustc},
    {"javac", cmd_javac},
    {"python", cmd_python_ext},
    {"python3", cmd_python3_ext},
    {"ruby", cmd_ruby_ext},
    {"node", cmd_node_ext},
    {"perl", cmd_perl_ext},
    {"php", cmd_php_ext},
    {"lua", cmd_lua_ext},
    {"tclsh", cmd_tclsh},
    {"cppcheck", cmd_cppcheck},
    {"clang-tidy", cmd_clang_tidy},
    {"pylint", cmd_pylint},
    {"flake8", cmd_flake8},
    {"mypy", cmd_mypy},
    {"eslint", cmd_eslint},
    {"jshint", cmd_jshint},
    {"rubocop", cmd_rubocop},
    {"golint", cmd_golint},
    {"gometalinter", cmd_gometalinter},
    {"staticcheck", cmd_staticcheck},
    {"shellcheck", cmd_shellcheck},
    {"clang-format", cmd_clang_format},
    {"indent", cmd_indent},
    {"astyle", cmd_astyle},
    {"uncrustify", cmd_uncrustify},
    {"black", cmd_black},
    {"autopep8", cmd_autopep8},
    {"yapf", cmd_yapf},
    {"prettier", cmd_prettier},
    {"js-beautify", cmd_js_beautify},
    {"gofmt", cmd_gofmt},
    {"rustfmt", cmd_rustfmt},
    {"pytest", cmd_pytest},
    {"unittest", cmd_unittest_ext},
    {"nose", cmd_nose},
    {"mocha", cmd_mocha},
    {"jest", cmd_jest},
    {"rspec", cmd_rspec},
    {"go-test", cmd_go_test},
    {"cargo-test", cmd_cargo_test},
    {"ctest", cmd_ctest},
    /* Category 8: Database Operations */
    {"mysql", cmd_mysql},
    {"mysqladmin", cmd_mysqladmin},
    {"mysqldump", cmd_mysqldump},
    {"psql", cmd_psql},
    {"pg_dump", cmd_pg_dump},
    {"pg_restore", cmd_pg_restore},
    {"sqlite3", cmd_sqlite3},
    {"sqlcmd", cmd_sqlcmd},
    {"mongo", cmd_mongo},
    {"mongosh", cmd_mongosh},
    {"redis-cli", cmd_redis_cli},
    {"cassandra-cli", cmd_cassandra_cli},
    {"cqlsh", cmd_cqlsh},
    {"hbase", cmd_hbase_shell},
    {"neo4j", cmd_neo4j},
    {"mongodump", cmd_mongodump},
    {"redis-dump", cmd_redis_dump},
    /* Category 9: Security & Encryption */
    {"gpg", cmd_gpg_ext},
    {"gpg2", cmd_gpg2},
    {"pgp", cmd_pgp},
    {"sha1sum", cmd_sha1sum_ext},
    {"sha512sum", cmd_sha512sum_ext},
    {"b2sum", cmd_b2sum_ext},
    {"cksum", cmd_cksum_ext},
    {"sum", cmd_sum_ext},
    {"burpsuite", cmd_burpsuite},
    {"metasploit", cmd_metasploit},
    {"aircrack-ng", cmd_aircrack_ng},
    {"reaver", cmd_reaver},
    {"hashcat", cmd_hashcat},
    {"john", cmd_john},
    {"hydra", cmd_hydra},
    {"sqlmap", cmd_sqlmap},
    {"nikto", cmd_nikto},
    {"wpscan", cmd_wpscan},
    {"gobuster", cmd_gobuster},
    {"feroxbuster", cmd_feroxbuster},
    {"void-install-kali", cmd_void_install_kali},
    {"hosts.allow", cmd_hosts_allow},
    {"hosts.deny", cmd_hosts_deny},
    {"ssh-keygen", cmd_ssh_keygen},
    {"ssh-add", cmd_ssh_add},
    {"ssh-agent", cmd_ssh_agent},
    {"ssh-copy-id", cmd_ssh_copy_id},
    {"pass", cmd_pass},
    {"keepassxc-cli", cmd_keepassxc_cli},
    /* Category 10: System Utilities */
    {"date", cmd_date_ext},
    {"cal", cmd_cal},
    {"ncal", cmd_ncal},
    {"hwclock", cmd_hwclock},
    {"timedatectl", cmd_timedatectl},
    {"tzselect", cmd_tzselect},
    {"zdump", cmd_zdump},
    /* Category 11: Void OS Specific */
    {"void-info", cmd_void_info},
    {"void-status", cmd_void_status},
    {"void-health", cmd_void_health},
    {"void-update", cmd_void_update},
    {"void-backup", cmd_void_backup},
    {"void-restore", cmd_void_restore},
    {"void-config", cmd_void_config},
    {"void-theme", cmd_void_theme},
    {"void-plugins", cmd_void_plugins},
    {"void-install-plugin", cmd_void_install_plugin},
    {"void-remove-plugin", cmd_void_remove_plugin},
    {"void-enable-plugin", cmd_void_enable_plugin},
    {"void-disable-plugin", cmd_void_disable_plugin},
    {"void-reload", cmd_void_reload_ext},
    {"void-version", cmd_void_version},
    {"void-license", cmd_void_license},
    {"void-credits", cmd_void_credits},
    {"void-docs", cmd_void_docs},
    {"void-examples", cmd_void_examples},
    {"void-tutorial", cmd_void_tutorial},
    {"void-cheatsheet", cmd_void_cheatsheet},
    {"void-shortcuts", cmd_void_shortcuts},
    {"void-aliases", cmd_void_aliases_ext},
    {"void-history", cmd_void_history_ext},
    {"void-stats", cmd_void_stats},
    {"void-benchmark", cmd_void_benchmark},
    {"void-test", cmd_void_test},
    {"void-debug", cmd_void_debug},
    {"void-trace", cmd_void_trace},
    {"void-profile", cmd_void_profile},
    {"void-optimize", cmd_void_optimize},
    {"void-clean", cmd_void_clean},
    {"void-verify", cmd_void_verify},
    {"void-repair", cmd_void_repair},
    {"void-reset", cmd_void_reset},
    {"void-export-config", cmd_void_export_config},
    {"void-import-config", cmd_void_import_config},
    {"void-migrate", cmd_void_migrate},
    {"void-rollback", cmd_void_rollback},
    {"void-snapshot", cmd_void_snapshot},
    {"void-restore-snapshot", cmd_void_restore_snapshot},
    {"void-list-snapshots", cmd_void_list_snapshots},
    {"void-delete-snapshot", cmd_void_delete_snapshot},
    {"void-compare-snapshots", cmd_void_compare_snapshots},
    {"void-merge-snapshots", cmd_void_merge_snapshots},
    {"void-clone", cmd_void_clone},
    {"void-sync", cmd_void_sync_ext},
    {"void-backup-remote", cmd_void_backup_remote},
    {"void-restore-remote", cmd_void_restore_remote},
    {"void-help", cmd_void_help},
    /* Category 12: Multimedia & Media */
    {"convert", cmd_convert},
    {"identify", cmd_identify},
    {"mogrify", cmd_mogrify},
    {"composite", cmd_composite},
    {"montage", cmd_montage},
    {"ffmpeg", cmd_ffmpeg},
    {"ffplay", cmd_ffplay},
    {"ffprobe", cmd_ffprobe},
    {"imagemagick", cmd_imagemagick},
    {"gimp", cmd_gimp},
    {"inkscape", cmd_inkscape},
    {"sox", cmd_sox},
    {"mpg123", cmd_mpg123},
    {"mpg321", cmd_mpg321},
    {"ogg123", cmd_ogg123},
    {"flac", cmd_flac},
    {"lame", cmd_lame},
    {"opusenc", cmd_opusenc},
    {"opusdec", cmd_opusdec},
    {"vlc", cmd_vlc},
    {"mplayer", cmd_mplayer},
    {"mpv", cmd_mpv},
    /* Category 13: Archive & Compression */
    {"lzma", cmd_lzma},
    {"unlzma", cmd_unlzma},
    {"zstd", cmd_zstd},
    {"unzstd", cmd_unzstd},
    {"lz4", cmd_lz4},
    {"unlz4", cmd_unlz4},
    {"brotli", cmd_brotli},
    {"unbrotli", cmd_unbrotli},
    {"ar", cmd_ar},
    {"cpio", cmd_cpio},
    {"pax", cmd_pax},
    /* Category 14: Miscellaneous Utilities */
    {"fortune", cmd_fortune},
    {"cowsay", cmd_cowsay},
    {"cowthink", cmd_cowthink},
    {"figlet", cmd_figlet},
    {"toilet", cmd_toilet},
    {"banner", cmd_banner},
    {"sl", cmd_sl},
    {"cmatrix", cmd_cmatrix},
    {"hollywood", cmd_hollywood},
    {"rig", cmd_rig},
    {"random", cmd_random},
    {"dice", cmd_dice},
    {"coin", cmd_coin},
    {"shuffle", cmd_shuffle_list},
    {"pick", cmd_pick},
    {"yes", cmd_yes},
    {"factor", cmd_factor},
    {"seq", cmd_seq},
    {"jot", cmd_jot},
    {"tldr", cmd_tldr},
    {NULL, NULL}
};

char *resolve_alias(const char *name, shell_context_t *ctx) {
    if (name == NULL || ctx == NULL) {
        return NULL;
    }
    
    for (int i = 0; i < ctx->alias_count && i < MAX_ALIASES; i++) {
        if (ctx->aliases[i] != NULL && 
            strcmp(ctx->aliases[i]->name, name) == 0) {
            return string_dup(ctx->aliases[i]->value);
        }
    }
    
    return NULL;
}

int execute_command(int argc, char **argv, shell_context_t *ctx) {
    if (argc == 0 || argv == NULL || argv[0] == NULL || ctx == NULL) {
        return 1;
    }
    
    const char *cmd_name = argv[0];
    
    /* Check for built-in commands */
    for (int i = 0; builtins[i].name != NULL; i++) {
        if (strcmp(cmd_name, builtins[i].name) == 0) {
            return builtins[i].func(argc, argv, ctx);
        }
    }
    
    /* Try to resolve alias */
    char *alias_value = resolve_alias(cmd_name, ctx);
    if (alias_value != NULL) {
        /* Parse alias value and execute */
        char **alias_argv = malloc(MAX_ARGS * sizeof(char*));
        if (alias_argv != NULL) {
            int alias_argc = parse_command(alias_value, alias_argv, MAX_ARGS);
            if (alias_argc > 0) {
                /* Replace first arg with alias command, append original args */
                int total_args = alias_argc + argc - 1;
                if (total_args < MAX_ARGS) {
                    char **new_argv = malloc((total_args + 1) * sizeof(char*));
                    if (new_argv != NULL) {
                        /* Copy alias args */
                        for (int i = 0; i < alias_argc; i++) {
                            new_argv[i] = alias_argv[i];
                        }
                        /* Append original args (skip first) */
                        for (int i = 1; i < argc; i++) {
                            new_argv[alias_argc + i - 1] = string_dup(argv[i]);
                        }
                        new_argv[total_args] = NULL;
                        
                        int result = execute_command(total_args, new_argv, ctx);
                        
                        /* Free allocated args */
                        for (int i = 0; i < total_args; i++) {
                            free(new_argv[i]);
                        }
                        free(new_argv);
                        
                        free(alias_value);
                        free(alias_argv);
                        return result;
                    }
                }
            }
            free_parsed_args(alias_argv, alias_argc);
        }
        free(alias_value);
    }
    
    /* Execute external command */
    return execute_external(cmd_name, argv, ctx);
}

int execute_external(const char *command, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (command == NULL || argv == NULL) {
        return 1;
    }
    
    /* Void OS is standalone - we don't execute external commands from PATH */
    /* This maintains the standalone nature of the OS */
    printf(COLOR_GREY "Command '%s' not found in Void OS.\n" COLOR_RESET, command);
    printf(COLOR_GREY "Void OS is a standalone CLI OS - all commands are built-in.\n" COLOR_RESET);
    printf(COLOR_GREY "Use 'help' to see available commands.\n" COLOR_RESET);
    return 1;
}

/* Built-in command implementations */
static int cmd_exit(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    if (ctx != NULL) {
        ctx->should_exit = true;
    }
    return 0;
}

static int cmd_echo(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    for (int i = 1; i < argc; i++) {
        printf("%s", argv[i]);
        if (i < argc - 1) {
            printf(" ");
        }
    }
    printf("\n");
    return 0;
}

static int cmd_pwd(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        if (ctx != NULL) {
            free(ctx->cwd);
            ctx->cwd = string_dup(cwd);
        }
        return 0;
    }
    return 1;
}

static int cmd_cd(int argc, char **argv, shell_context_t *ctx) {
    const char *dir = (argc > 1) ? argv[1] : getenv("HOME");
    
    if (dir == NULL) {
        dir = "~";
    }
    
    char *expanded = expand_path(dir);
    if (expanded == NULL) {
        expanded = string_dup(dir);
    }
    
    if (chdir(expanded) != 0) {
        LOG_ERROR("Failed to change directory to '%s': %s", expanded, strerror(errno));
        if (errno == ENOENT) {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
        } else if (errno == EACCES) {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PERMISSION_DENIED);
        } else {
            printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_PROCESS_ERROR);
        }
        free(expanded);
        return 1;
    }
    
    /* Update context */
    if (ctx != NULL) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            free(ctx->cwd);
            ctx->cwd = string_dup(cwd);
        }
    }
    
    free(expanded);
    return 0;
}

static int cmd_whoami(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    
    if (ctx != NULL && ctx->username != NULL) {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, ctx->username);
    } else {
        const char *user = getenv("USER");
        if (user != NULL) {
            printf(COLOR_WHITE "%s\n" COLOR_RESET, user);
        } else {
            struct passwd *pw = getpwuid(getuid());
            if (pw != NULL) {
                printf(COLOR_WHITE "%s\n" COLOR_RESET, pw->pw_name);
            } else {
                printf(COLOR_GREY "void\n" COLOR_RESET);
            }
        }
    }
    return 0;
}

static int cmd_identity(int argc, char **argv, shell_context_t *ctx) {
    if (ctx == NULL) {
        return 1;
    }
    
    if (argc < 2) {
        /* Display current identity */
        if (ctx->username != NULL) {
            printf(COLOR_WHITE "Current identity: %s\n" COLOR_RESET, ctx->username);
        } else {
            printf(COLOR_GREY "No custom identity set.\n" COLOR_RESET);
        }
        return 0;
    }
    
    /* Set new identity */
    free(ctx->username);
    ctx->username = string_dup(argv[1]);
    if (ctx->username == NULL) {
        LOG_ERROR("Failed to set identity");
        return 1;
    }
    
    printf(COLOR_GREEN "Identity set to: %s\n" COLOR_RESET, ctx->username);
    return 0;
}

