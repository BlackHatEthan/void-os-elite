#ifndef HASH_NATIVE_H
#define HASH_NATIVE_H

#include <stddef.h>

/* Native hash implementations - MD5 and SHA256 */
char *hash_md5_native(const unsigned char *data, size_t len);
char *hash_sha256_native(const unsigned char *data, size_t len);

#endif /* HASH_NATIVE_H */

