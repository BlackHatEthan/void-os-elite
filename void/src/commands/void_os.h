#ifndef VOID_OS_H
#define VOID_OS_H

#include "../../include/void.h"

/* Category 11: Void OS Specific Commands (50 commands) */

/* Void-Styled Operations */
int cmd_void_info(int argc, char **argv, shell_context_t *ctx);
int cmd_void_status(int argc, char **argv, shell_context_t *ctx);
int cmd_void_health(int argc, char **argv, shell_context_t *ctx);
int cmd_void_update(int argc, char **argv, shell_context_t *ctx);
int cmd_void_backup(int argc, char **argv, shell_context_t *ctx);
int cmd_void_restore(int argc, char **argv, shell_context_t *ctx);
int cmd_void_config(int argc, char **argv, shell_context_t *ctx);
int cmd_void_theme(int argc, char **argv, shell_context_t *ctx);
int cmd_void_plugins(int argc, char **argv, shell_context_t *ctx);
int cmd_void_install_plugin(int argc, char **argv, shell_context_t *ctx);
int cmd_void_remove_plugin(int argc, char **argv, shell_context_t *ctx);
int cmd_void_enable_plugin(int argc, char **argv, shell_context_t *ctx);
int cmd_void_disable_plugin(int argc, char **argv, shell_context_t *ctx);
int cmd_void_reload_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_void_version(int argc, char **argv, shell_context_t *ctx);
int cmd_void_license(int argc, char **argv, shell_context_t *ctx);
int cmd_void_credits(int argc, char **argv, shell_context_t *ctx);
int cmd_void_docs(int argc, char **argv, shell_context_t *ctx);
int cmd_void_examples(int argc, char **argv, shell_context_t *ctx);
int cmd_void_tutorial(int argc, char **argv, shell_context_t *ctx);
int cmd_void_cheatsheet(int argc, char **argv, shell_context_t *ctx);
int cmd_void_shortcuts(int argc, char **argv, shell_context_t *ctx);
int cmd_void_aliases_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_void_history_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_void_stats(int argc, char **argv, shell_context_t *ctx);
int cmd_void_benchmark(int argc, char **argv, shell_context_t *ctx);
int cmd_void_test(int argc, char **argv, shell_context_t *ctx);
int cmd_void_debug(int argc, char **argv, shell_context_t *ctx);
int cmd_void_trace(int argc, char **argv, shell_context_t *ctx);
int cmd_void_profile(int argc, char **argv, shell_context_t *ctx);
int cmd_void_optimize(int argc, char **argv, shell_context_t *ctx);
int cmd_void_clean(int argc, char **argv, shell_context_t *ctx);
int cmd_void_verify(int argc, char **argv, shell_context_t *ctx);
int cmd_void_repair(int argc, char **argv, shell_context_t *ctx);
int cmd_void_reset(int argc, char **argv, shell_context_t *ctx);
int cmd_void_export_config(int argc, char **argv, shell_context_t *ctx);
int cmd_void_import_config(int argc, char **argv, shell_context_t *ctx);
int cmd_void_migrate(int argc, char **argv, shell_context_t *ctx);
int cmd_void_rollback(int argc, char **argv, shell_context_t *ctx);
int cmd_void_snapshot(int argc, char **argv, shell_context_t *ctx);
int cmd_void_restore_snapshot(int argc, char **argv, shell_context_t *ctx);
int cmd_void_list_snapshots(int argc, char **argv, shell_context_t *ctx);
int cmd_void_delete_snapshot(int argc, char **argv, shell_context_t *ctx);
int cmd_void_compare_snapshots(int argc, char **argv, shell_context_t *ctx);
int cmd_void_merge_snapshots(int argc, char **argv, shell_context_t *ctx);
int cmd_void_clone(int argc, char **argv, shell_context_t *ctx);
int cmd_void_sync_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_void_backup_remote(int argc, char **argv, shell_context_t *ctx);
int cmd_void_restore_remote(int argc, char **argv, shell_context_t *ctx);
int cmd_void_help(int argc, char **argv, shell_context_t *ctx);

#endif /* VOID_OS_H */

