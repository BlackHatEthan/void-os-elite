#ifndef PROCESS_EXT_H
#define PROCESS_EXT_H

#include "../../include/void.h"

/* Category 3: Process Management (100 commands) */

/* Process Control */
int cmd_kill_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_killall(int argc, char **argv, shell_context_t *ctx);
int cmd_pkill(int argc, char **argv, shell_context_t *ctx);
int cmd_xkill(int argc, char **argv, shell_context_t *ctx);
int cmd_killpg_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_pkillall(int argc, char **argv, shell_context_t *ctx);
int cmd_skill(int argc, char **argv, shell_context_t *ctx);
int cmd_snice(int argc, char **argv, shell_context_t *ctx);
int cmd_renice(int argc, char **argv, shell_context_t *ctx);
int cmd_nice_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_nohup(int argc, char **argv, shell_context_t *ctx);
int cmd_setsid_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_disown(int argc, char **argv, shell_context_t *ctx);
int cmd_fg_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_bg_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_wait_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_waitpid_ext(int argc, char **argv, shell_context_t *ctx);

/* Process Information */
int cmd_pstree(int argc, char **argv, shell_context_t *ctx);
int cmd_pgrep(int argc, char **argv, shell_context_t *ctx);
int cmd_pidof(int argc, char **argv, shell_context_t *ctx);
int cmd_fuser(int argc, char **argv, shell_context_t *ctx);
int cmd_lsof_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_pmap(int argc, char **argv, shell_context_t *ctx);
int cmd_procinfo(int argc, char **argv, shell_context_t *ctx);
int cmd_pidstat(int argc, char **argv, shell_context_t *ctx);
int cmd_top_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_htop(int argc, char **argv, shell_context_t *ctx);
int cmd_btop(int argc, char **argv, shell_context_t *ctx);
int cmd_glances(int argc, char **argv, shell_context_t *ctx);
int cmd_nmon(int argc, char **argv, shell_context_t *ctx);
int cmd_iotop(int argc, char **argv, shell_context_t *ctx);
int cmd_powertop(int argc, char **argv, shell_context_t *ctx);

/* Process Tracing & Debugging */
int cmd_strace_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_ltrace(int argc, char **argv, shell_context_t *ctx);
int cmd_ptrace_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_gdb_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_lldb_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_valgrind(int argc, char **argv, shell_context_t *ctx);
int cmd_perf(int argc, char **argv, shell_context_t *ctx);
int cmd_timeout(int argc, char **argv, shell_context_t *ctx);
int cmd_watch_process(int argc, char **argv, shell_context_t *ctx);

/* Process Scheduling */
int cmd_at(int argc, char **argv, shell_context_t *ctx);
int cmd_atq(int argc, char **argv, shell_context_t *ctx);
int cmd_atrm(int argc, char **argv, shell_context_t *ctx);
int cmd_batch(int argc, char **argv, shell_context_t *ctx);
int cmd_cron(int argc, char **argv, shell_context_t *ctx);
int cmd_crontab(int argc, char **argv, shell_context_t *ctx);
int cmd_anacron(int argc, char **argv, shell_context_t *ctx);
int cmd_systemd_run(int argc, char **argv, shell_context_t *ctx);
int cmd_systemctl(int argc, char **argv, shell_context_t *ctx);

/* Process Limits */
int cmd_ulimit_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_prlimit(int argc, char **argv, shell_context_t *ctx);
int cmd_limit_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_quota(int argc, char **argv, shell_context_t *ctx);
int cmd_edquota(int argc, char **argv, shell_context_t *ctx);
int cmd_repquota(int argc, char **argv, shell_context_t *ctx);

#endif /* PROCESS_EXT_H */

