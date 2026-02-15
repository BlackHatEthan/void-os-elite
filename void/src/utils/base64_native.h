#ifndef BASE64_NATIVE_H
#define BASE64_NATIVE_H

/* Native Base64 encoding/decoding - no external dependencies */
char *base64_encode_native(const unsigned char *data, size_t input_len, size_t *output_len);
unsigned char *base64_decode_native(const char *data, size_t input_len, size_t *output_len);

#endif /* BASE64_NATIVE_H */

