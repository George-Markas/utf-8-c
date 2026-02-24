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
#include <stdio.h>

#define UTF8_LENGTH(str) utf8_length[(((uint8_t *) str)[0] & 0xFF) >> 4]

// @formatter:off
static uint8_t const utf8_length[] = {
 /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 3, 4
};
// @formatter:on

bool utf8_is_valid(const utf8_char encoding) {
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

uint8_t utf8_char_length(const utf8_char encoding) {
    return (4 - (__builtin_clz(encoding | 0xFF) / 8));
}

utf8_char utf8_encode(const uint32_t codepoint) {
    utf8_char encoding = 0;

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

uint32_t utf8_decode(const utf8_char encoding) {
    const uint8_t length = utf8_char_length(encoding);

    // @formatter:off
    uint32_t codepoint = 0;
    switch (length) {
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

uint8_t utf8_next(const char *str, utf8_char *next) {
    uint8_t length = UTF8_LENGTH(str);
    utf8_char encoding = 0;

    for (int i = 0; i < length && str[i] != '\0'; i++) {
        encoding = (encoding << 8) | (str[i] & 0xFF);
    }

    if (length == 0 || !utf8_is_valid(encoding)) {
        fprintf(stderr, "\x1B[0;31mInvalid UTF-8: 0x%08X\x1B[0m\n", encoding);
        encoding = REPLACEMENT_CHARACTER;
        length = 3;
    }

    if (encoding && next) *next = encoding;

    return encoding ? length : 0; // Account for '\0'
}

utf8_str utf8_str_wrap(const char *str) {
    if (!str) goto invalid;

    size_t offset = 0;
    utf8_char encoding = 0;
    while (str[offset] != '\0') {
        const uint8_t length = utf8_next(str + offset, &encoding);
        if (encoding == REPLACEMENT_CHARACTER) goto invalid;
        offset += length;
    }

    return (utf8_str) { .length = offset, .str = str };

invalid:
    return (utf8_str) { .length = 0, .str = NULL };
}
