#pragma once

#include <stdint.h>
#include <stdbool.h>

#define REPLACEMENT_CHARACTER 0xEFBFBD
#define UTF8_CHAR_LENGTH(encoding) (4 - (__builtin_clz(encoding | 0xFF) / 8));

typedef uint32_t utf8_char;

/**
 * Check if an encoding is valid UTF-8.
 * @param encoding The encoding to validate.
 * @return True if the encoding is valid, false if it isn't.
 */
bool utf8_is_valid(utf8_char encoding);

/**
 * Encode a Unicode codepoint using UTF-8.
 * @param codepoint The Unicode codepoint to encode.
 * @return The encoding.
 */
utf8_char utf8_encode(uint32_t codepoint);

/**
 * Decode a UTF-8 encoding.
 * @param encoding The UTF-8 encoding to decode.
 * @return The Unicode codepoint.
 */
uint32_t utf8_decode(utf8_char encoding);

/**
 * Get the next UTF-8 encoding from a string.
 * @param str The string to read from.
 * @param next Where the encoding will be saved.
 * @return The UTF-8 encoding's byte length or zero in case the encoding
 * is the null terminator.
 * @note Subsequent calls of this function on the same string won't
 * increment the pointer, do so manually if needed using the returned length.
 * In case the encoding is not valid UTF-8, `next` will be set to
 * 0xEFBFBD (the replacement character U+FFFD).
 */
uint8_t utf8_next(const char *str, utf8_char *next);