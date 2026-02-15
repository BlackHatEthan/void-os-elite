#ifndef MULTIMEDIA_H
#define MULTIMEDIA_H

#include "../../include/void.h"

/* Category 12: Multimedia & Media (50 commands) */

/* Image Operations */
int cmd_convert(int argc, char **argv, shell_context_t *ctx);
int cmd_identify(int argc, char **argv, shell_context_t *ctx);
int cmd_mogrify(int argc, char **argv, shell_context_t *ctx);
int cmd_composite(int argc, char **argv, shell_context_t *ctx);
int cmd_montage(int argc, char **argv, shell_context_t *ctx);
int cmd_ffmpeg(int argc, char **argv, shell_context_t *ctx);
int cmd_ffplay(int argc, char **argv, shell_context_t *ctx);
int cmd_ffprobe(int argc, char **argv, shell_context_t *ctx);
int cmd_imagemagick(int argc, char **argv, shell_context_t *ctx);
int cmd_gimp(int argc, char **argv, shell_context_t *ctx);
int cmd_inkscape(int argc, char **argv, shell_context_t *ctx);

/* Audio Operations */
int cmd_sox(int argc, char **argv, shell_context_t *ctx);
int cmd_mpg123(int argc, char **argv, shell_context_t *ctx);
int cmd_mpg321(int argc, char **argv, shell_context_t *ctx);
int cmd_ogg123(int argc, char **argv, shell_context_t *ctx);
int cmd_flac(int argc, char **argv, shell_context_t *ctx);
int cmd_lame(int argc, char **argv, shell_context_t *ctx);
int cmd_opusenc(int argc, char **argv, shell_context_t *ctx);
int cmd_opusdec(int argc, char **argv, shell_context_t *ctx);

/* Video Operations */
int cmd_vlc(int argc, char **argv, shell_context_t *ctx);
int cmd_mplayer(int argc, char **argv, shell_context_t *ctx);
int cmd_mpv(int argc, char **argv, shell_context_t *ctx);

#endif /* MULTIMEDIA_H */

