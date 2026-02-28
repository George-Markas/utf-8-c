#pragma once

#include <stdint.h>
#include <stdbool.h>

#define UTF8_ERROR_INVALID (-1)

typedef uint32_t utf8_char_t;

/**
 * Encode a codepoint.
 * @param[in] codepoint The codepoint to encode.
 * @return The encoding.
 * @note This function does not check if the given codepoint is valid.
 */
utf8_char_t utf8_encode(uint32_t codepoint);

/**
 * Decode an encoding.
 * @param[in] encoding The encoding to decode.
 * @return The codepoint.
 * @note This function does not check if the given encoding is valid.
 */
uint32_t utf8_decode(utf8_char_t encoding);

/**
 * Check if an encoding is valid UTF-8.
 * @param[in] encoding The encoding to validate.
 * @return True if the encoding is valid, false if it isn't.
 */
bool utf8_validate_encoding(utf8_char_t encoding);

/**
 * Encode a codepoint as a UTF-8 string in the byte array pointed to by `buffer`.
 * The array must be of sufficient size.
 * @param[in] codepoint The codepoint to encode.
 * @param[out] buffer The destination buffer.
 * @return The number of bytes written or zero on failure.
 */
uint8_t utf8_encode_to_str(uint32_t codepoint, char *buffer);

/**
 * Get the next encoding from the UTF-8 sequence being pointed to by `str`.
 * If a valid encoding could be read, it is stored in the variable pointed to
 * by `next`.
 * @param[in] str The buffer to read from.
 * @param[out] next Reference to the variable in which the encoding will be stored.
 * @return The number of bytes read (zero if the encoding is the null terminator)
 * or negative on failure.
 */
int utf8_next(const char *str, utf8_char_t *next);