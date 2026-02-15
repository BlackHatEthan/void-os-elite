#ifndef FILESYSTEM_EXT_H
#define FILESYSTEM_EXT_H

#include "../../include/void.h"

/* Category 2: Additional Filesystem Operations (200 commands) */

/* Basic File Operations */
int cmd_touch(int argc, char **argv, shell_context_t *ctx);
int cmd_mkdir_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_rmdir_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_rm(int argc, char **argv, shell_context_t *ctx);
int cmd_cp(int argc, char **argv, shell_context_t *ctx);
int cmd_mv(int argc, char **argv, shell_context_t *ctx);
int cmd_ln(int argc, char **argv, shell_context_t *ctx);
int cmd_symlink_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_readlink_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_stat_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_file_type(int argc, char **argv, shell_context_t *ctx);
int cmd_mime(int argc, char **argv, shell_context_t *ctx);
int cmd_size(int argc, char **argv, shell_context_t *ctx);
int cmd_du(int argc, char **argv, shell_context_t *ctx);
int cmd_df(int argc, char **argv, shell_context_t *ctx);
int cmd_find_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_locate(int argc, char **argv, shell_context_t *ctx);
int cmd_updatedb(int argc, char **argv, shell_context_t *ctx);
int cmd_where(int argc, char **argv, shell_context_t *ctx);
int cmd_realpath_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_basename_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_dirname_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_pathjoin(int argc, char **argv, shell_context_t *ctx);
int cmd_pathsplit(int argc, char **argv, shell_context_t *ctx);
int cmd_pathnorm(int argc, char **argv, shell_context_t *ctx);
int cmd_pathabs(int argc, char **argv, shell_context_t *ctx);
int cmd_pathrel(int argc, char **argv, shell_context_t *ctx);

/* File Attributes */
int cmd_chmod_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_chown_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_chgrp_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_umask_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_getfacl(int argc, char **argv, shell_context_t *ctx);
int cmd_setfacl(int argc, char **argv, shell_context_t *ctx);
int cmd_lsattr(int argc, char **argv, shell_context_t *ctx);
int cmd_chattr(int argc, char **argv, shell_context_t *ctx);
int cmd_getfattr(int argc, char **argv, shell_context_t *ctx);
int cmd_setfattr(int argc, char **argv, shell_context_t *ctx);

/* File Comparison */
int cmd_diff(int argc, char **argv, shell_context_t *ctx);
int cmd_diff3(int argc, char **argv, shell_context_t *ctx);
int cmd_sdiff(int argc, char **argv, shell_context_t *ctx);
int cmd_cmp(int argc, char **argv, shell_context_t *ctx);
int cmd_comm(int argc, char **argv, shell_context_t *ctx);
int cmd_patch(int argc, char **argv, shell_context_t *ctx);
int cmd_diffstat(int argc, char **argv, shell_context_t *ctx);

/* File Compression & Archives */
int cmd_gzip(int argc, char **argv, shell_context_t *ctx);
int cmd_gunzip(int argc, char **argv, shell_context_t *ctx);
int cmd_bzip2(int argc, char **argv, shell_context_t *ctx);
int cmd_bunzip2(int argc, char **argv, shell_context_t *ctx);
int cmd_xz(int argc, char **argv, shell_context_t *ctx);
int cmd_unxz(int argc, char **argv, shell_context_t *ctx);
int cmd_zip(int argc, char **argv, shell_context_t *ctx);
int cmd_unzip(int argc, char **argv, shell_context_t *ctx);
int cmd_tar(int argc, char **argv, shell_context_t *ctx);
int cmd_zcat(int argc, char **argv, shell_context_t *ctx);
int cmd_zless(int argc, char **argv, shell_context_t *ctx);
int cmd_zmore(int argc, char **argv, shell_context_t *ctx);
int cmd_zcmp(int argc, char **argv, shell_context_t *ctx);
int cmd_zdiff(int argc, char **argv, shell_context_t *ctx);

/* File Integrity & Checksums */
int cmd_md5sum(int argc, char **argv, shell_context_t *ctx);
int cmd_sha1sum(int argc, char **argv, shell_context_t *ctx);
int cmd_sha256sum(int argc, char **argv, shell_context_t *ctx);
int cmd_sha512sum(int argc, char **argv, shell_context_t *ctx);
int cmd_cksum(int argc, char **argv, shell_context_t *ctx);
int cmd_sum(int argc, char **argv, shell_context_t *ctx);
int cmd_b2sum(int argc, char **argv, shell_context_t *ctx);
int cmd_verify(int argc, char **argv, shell_context_t *ctx);

/* File Monitoring */
int cmd_inotifywait(int argc, char **argv, shell_context_t *ctx);
int cmd_inotifywatch(int argc, char **argv, shell_context_t *ctx);
int cmd_fswatch(int argc, char **argv, shell_context_t *ctx);
int cmd_entr(int argc, char **argv, shell_context_t *ctx);
int cmd_watch_ext(int argc, char **argv, shell_context_t *ctx);

/* File Conversion */
int cmd_dos2unix(int argc, char **argv, shell_context_t *ctx);
int cmd_unix2dos(int argc, char **argv, shell_context_t *ctx);
int cmd_mac2unix(int argc, char **argv, shell_context_t *ctx);
int cmd_unix2mac(int argc, char **argv, shell_context_t *ctx);
int cmd_iconv_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_uuencode(int argc, char **argv, shell_context_t *ctx);
int cmd_uudecode(int argc, char **argv, shell_context_t *ctx);
int cmd_hexdump(int argc, char **argv, shell_context_t *ctx);
int cmd_od(int argc, char **argv, shell_context_t *ctx);
int cmd_xxd(int argc, char **argv, shell_context_t *ctx);
int cmd_strings_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_bin2hex(int argc, char **argv, shell_context_t *ctx);
int cmd_hex2bin(int argc, char **argv, shell_context_t *ctx);

/* Special File Operations */
int cmd_shred(int argc, char **argv, shell_context_t *ctx);
int cmd_wipe(int argc, char **argv, shell_context_t *ctx);
int cmd_srm(int argc, char **argv, shell_context_t *ctx);
int cmd_secure_delete(int argc, char **argv, shell_context_t *ctx);
int cmd_truncate_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_fallocate(int argc, char **argv, shell_context_t *ctx);
int cmd_mktemp(int argc, char **argv, shell_context_t *ctx);
int cmd_tempfile(int argc, char **argv, shell_context_t *ctx);

#endif /* FILESYSTEM_EXT_H */

