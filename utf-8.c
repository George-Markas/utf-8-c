/*
    +--------------------+----------+----------+----------+----------+
    | Range              | Byte 1   | Byte 2   | Byte 3   | Byte 4   |
    +--------------------+----------+----------+----------+----------+
    | 0x0000 - 0x007F    | 0xxxxxxx |          |          |          |
    +--------------------+----------+----------+----------+----------+
    | 0x0080 - 0x07FF    | 110xxxxx | 10xxxxxx |          |          |
    +--------------------+----------+----------+----------+----------+
    | 0x0800 - 0xFFFF    | 1110xxxx | 10xxxxxx | 10xxxxxx |          |
    +--------------------+----------+----------+----------+----------+
    | 0x10000 - 0x10FFFF | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |
    +--------------------+----------+----------+----------+----------+
*/

#include "utf-8.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PUT_REPLACEMENT_CHARACTER(buffer, index) \
    buffer[index++] = 0xEF; \
    buffer[index++] = 0xBF; \
    buffer[index++] = 0xBD

static uint8_t utf8_length_uint32_t(const utf8_char_t encoding) {
    return (4 - (__builtin_clz(encoding | 0xFF) / 8));
}

static uint8_t utf8_length_char(const char *str) {
    // @formatter:off
    if (((str[0] & 0b10000000) >> 7) == 0b0)     return 1;
    if (((str[0] & 0b11100000) >> 5) == 0b110)   return 2;
    if (((str[0] & 0b11110000) >> 4) == 0b1110)  return 3;
    if (((str[0] & 0b11111000) >> 3) == 0b11110) return 4;
    // @formatter:on
    return 0;
}

utf8_char_t utf8_encode(const uint32_t codepoint) {
    utf8_char_t encoding = 0;

    // @formatter:off
    if (codepoint <= 0x007F) {
        encoding = codepoint;
    } else if (0x0080 <= codepoint && codepoint <= 0x07FF) {
        encoding = (0b11000000 | ((codepoint >> 6) & 0b00011111)) << 8 |
                   (0b10000000 | (codepoint        & 0b00111111));
    } else if (0x0800 <= codepoint && codepoint <= 0xFFFF) {
        encoding = (0b11100000 | ((codepoint >> 12) & 0b00001111)) << 16 |
                   (0b10000000 | ((codepoint >> 6)  & 0b00111111)) << 8  |
                   (0b10000000 | (codepoint         & 0b00111111));
    } else if (0x10000 <= codepoint && codepoint <= 0x10FFFF) {
        encoding = (0b11110000 | ((codepoint >> 18) & 0b00000111)) << 24 |
                   (0b10000000 | ((codepoint >> 12) & 0b00111111)) << 16 |
                   (0b10000000 | ((codepoint >> 6)  & 0b00111111)) << 8  |
                   (0b10000000 | (codepoint         & 0b00111111));
    }
    // @formatter:on

    return encoding;
}

uint32_t utf8_decode(const utf8_char_t encoding) {
    const uint8_t encoding_length = utf8_length_uint32_t(encoding);

    // @formatter:off
    uint32_t codepoint = 0;
    switch (encoding_length) {
        case 1: return encoding;
        case 2:
            codepoint = ((encoding >> 8) & 0b00011111) << 6 |
                        (encoding        & 0b00111111);

            return codepoint;
        case 3:
            codepoint = ((encoding >> 16) & 0b00001111) << 12 |
                        ((encoding >> 8)  & 0b00111111) << 6  |
                        (encoding         & 0b00111111);

            return codepoint;
        default:
            codepoint = ((encoding >> 24) & 0b00000111) << 18 |
                        ((encoding >> 16) & 0b00111111) << 12 |
                        ((encoding >> 8)  & 0b00111111) << 6  |
                        (encoding         & 0b00111111);

            return codepoint;
    }
    // @formatter:on
}

bool utf8_validate_encoding(const utf8_char_t encoding) {
    /* U+0000 - U+007F */
    if (encoding <= 0x007F) return true;

    /* U+0080 - U+07FF */
    if (0xC280 <= encoding && encoding <= 0xDFBF)
        // Ensure '110xxxxx 10xxxxxx' form
        return ((encoding & 0xE0C0) == 0xC080);

    /* Reject UTF-16 surrogates */
    if (0xEDA080 <= encoding && encoding <= 0xEDBFBF) return false;

    /* U+0800 - U+FFFF */
    if (0xE0A080 <= encoding && encoding <= 0xEFBFBF)
        // Ensure '1110xxxx 10xxxxxx 10xxxxxx' form
        return ((encoding & 0xF0C0C0) == 0xE08080);

    /* U+10000 - U+10FFFF */
    if (0xF0908080 <= encoding && encoding <= 0xF48FBFBF)
        // Ensure '11110xxx 10xxxxxx 10xxxxxx 10xxxxxx' form
        return ((encoding & 0xF8C0C0C0) == 0xF0808080);

    return false;
}

uint8_t utf8_encode_to_str(const uint32_t codepoint, char *buffer) {
    const utf8_char_t encoding_length = utf8_encode(codepoint);
    if (!utf8_validate_encoding(encoding_length)) return 0;
    const uint8_t length = utf8_length_uint32_t(encoding_length);

    for (int i = 0; i < length; i++) {
        buffer[i] = (char) ((encoding_length >> (8 * (length - 1 - i))) & 0xFF);
    }

    return length;
}

int utf8_next(const char *str, utf8_char_t *next) {
    const uint8_t encoding_length = utf8_length_char(str);
    utf8_char_t encoding = 0;

    for (int i = 0; i < encoding_length && str[i] != '\0'; i++) {
        encoding = (encoding << 8) | (str[i] & 0xFF);
    }

    if (encoding_length == 0 || !utf8_validate_encoding(encoding)) return UTF8_ERROR_INVALID;
    if (encoding && next) *next = encoding;

    return encoding ? encoding_length : 0; // Account for '\0'
}

utf8_str_t utf8_str_t_new(const char *str) {
    utf8_str_t ret = {.length = 0, .str = NULL};
    if (!str) return ret;

    // Worst case scenario, every encoding is invalid, thus we need at most
    // 3 * N bytes + 1 byte for the null terminator, where N is the number
    // of encodings and 3 is the byte length of the replacement character U+FFFD.
    const size_t bytes = strlen(str);
    const size_t buffer_capacity = bytes * 3 + 1;
    char *buffer = malloc(sizeof(char) * buffer_capacity);
    assert(buffer);

    size_t i = 0, buffer_length = 0;
    while (i < bytes) {
        const uint8_t encoding_length = utf8_length_char(str);
        // TODO: The replacement logic needs more polish
        if (encoding_length) {
            utf8_char_t encoding = 0;
            for (size_t j = i; j < i + encoding_length; j++) {
                encoding = (encoding << 8) | (str[j] & 0xFF);
            }

            if (utf8_validate_encoding(encoding)) {
                memcpy(buffer + buffer_length, str + i, encoding_length);
                buffer_length += encoding_length;
                i += encoding_length;
            } else {
                PUT_REPLACEMENT_CHARACTER(buffer, buffer_length);
                i += encoding_length;
            }
        } else {
            PUT_REPLACEMENT_CHARACTER(buffer, buffer_length);
            i += encoding_length;
        }
    }

    if (buffer_length < buffer_capacity) {
        char *shrunken_buffer = realloc(buffer, buffer_length + 1);
        buffer = shrunken_buffer;
    }

    buffer[buffer_length] = '\0';
    ret.str = buffer;
    ret.length = buffer_length;

    return ret;
}

void utf8_str_t_free(utf8_str_t str) {
    free((void *) str.str);
    str.str = NULL;
    str.length = 0;
}
