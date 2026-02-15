#include "base64_native.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *base64_encode_native(const unsigned char *data, size_t input_len, size_t *output_len) {
    if (data == NULL || input_len == 0) {
        return NULL;
    }
    
    size_t encoded_len = ((input_len + 2) / 3) * 4;
    char *encoded = malloc(encoded_len + 1);
    if (encoded == NULL) {
        return NULL;
    }
    
    size_t i, j = 0;
    for (i = 0; i < input_len - 2; i += 3) {
        encoded[j++] = base64_chars[(data[i] >> 2) & 0x3F];
        encoded[j++] = base64_chars[((data[i] & 0x3) << 4) | ((data[i + 1] & 0xF0) >> 4)];
        encoded[j++] = base64_chars[((data[i + 1] & 0xF) << 2) | ((data[i + 2] & 0xC0) >> 6)];
        encoded[j++] = base64_chars[data[i + 2] & 0x3F];
    }
    
    if (i < input_len) {
        encoded[j++] = base64_chars[(data[i] >> 2) & 0x3F];
        if (i == (input_len - 1)) {
            encoded[j++] = base64_chars[((data[i] & 0x3) << 4)];
            encoded[j++] = '=';
        } else {
            encoded[j++] = base64_chars[((data[i] & 0x3) << 4) | ((data[i + 1] & 0xF0) >> 4)];
            encoded[j++] = base64_chars[((data[i + 1] & 0xF) << 2)];
        }
        encoded[j++] = '=';
    }
    
    encoded[j] = '\0';
    if (output_len) {
        *output_len = j;
    }
    
    return encoded;
}

static int base64_char_value(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    if (c == '=') return 0;
    return -1;
}

unsigned char *base64_decode_native(const char *data, size_t input_len, size_t *output_len) {
    if (data == NULL || input_len == 0) {
        return NULL;
    }
    
    /* Calculate output length */
    size_t padding = 0;
    if (input_len > 0 && data[input_len - 1] == '=') padding++;
    if (input_len > 1 && data[input_len - 2] == '=') padding++;
    
    size_t decoded_len = (input_len * 3) / 4 - padding;
    unsigned char *decoded = malloc(decoded_len + 1);
    if (decoded == NULL) {
        return NULL;
    }
    
    size_t i, j = 0;
    for (i = 0; i < input_len - padding; i += 4) {
        int c1 = base64_char_value(data[i]);
        int c2 = base64_char_value(data[i + 1]);
        int c3 = base64_char_value(data[i + 2]);
        int c4 = base64_char_value(data[i + 3]);
        
        if (c1 < 0 || c2 < 0 || c3 < 0 || c4 < 0) {
            free(decoded);
            return NULL;
        }
        
        decoded[j++] = (c1 << 2) | (c2 >> 4);
        decoded[j++] = ((c2 & 0xF) << 4) | (c3 >> 2);
        decoded[j++] = ((c3 & 0x3) << 6) | c4;
    }
    
    /* Handle padding */
    if (padding > 0 && i < input_len) {
        int c1 = base64_char_value(data[i]);
        int c2 = base64_char_value(data[i + 1]);
        
        if (c1 >= 0 && c2 >= 0) {
            decoded[j++] = (c1 << 2) | (c2 >> 4);
            if (padding == 1 && i + 2 < input_len) {
                int c3 = base64_char_value(data[i + 2]);
                if (c3 >= 0) {
                    decoded[j++] = ((c2 & 0xF) << 4) | (c3 >> 2);
                }
            }
        }
    }
    
    decoded[j] = '\0';
    if (output_len) {
        *output_len = j;
    }
    
    return decoded;
}

