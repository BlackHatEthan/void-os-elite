#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "../../include/void.h"

/* Category 13: Archive & Compression (50 commands) */

/* Compression */
int cmd_lzma(int argc, char **argv, shell_context_t *ctx);
int cmd_unlzma(int argc, char **argv, shell_context_t *ctx);
int cmd_zstd(int argc, char **argv, shell_context_t *ctx);
int cmd_unzstd(int argc, char **argv, shell_context_t *ctx);
int cmd_lz4(int argc, char **argv, shell_context_t *ctx);
int cmd_unlz4(int argc, char **argv, shell_context_t *ctx);
int cmd_brotli(int argc, char **argv, shell_context_t *ctx);
int cmd_unbrotli(int argc, char **argv, shell_context_t *ctx);

/* Archives */
int cmd_ar(int argc, char **argv, shell_context_t *ctx);
int cmd_cpio(int argc, char **argv, shell_context_t *ctx);
int cmd_pax(int argc, char **argv, shell_context_t *ctx);

#endif /* ARCHIVE_H */

