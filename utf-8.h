#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define REPLACEMENT_CHARACTER 0xEFBFBD

typedef uint32_t utf8_char;

typedef struct Utf8_String {
    size_t length; // Byte length, excluding the null terminator
    const char *str;
} utf8_str;

/**
 * Check if an encoding is valid UTF-8.
 * @param encoding The encoding to validate.
 * @return True if the encoding is valid, false if it isn't.
 */
bool utf8_is_valid(utf8_char encoding);

/**
 * Get the byte length of an encoding.
 * @param encoding The encoding whose length to get.
 * @return The encoding's byte length.
 * @details This function does not check if the encoding is valid
 * UTF-8, use utf8_is_valid() if needed.
 */
uint8_t utf8_char_length(utf8_char encoding);

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
 * Get the next encoding from a string.
 * @param str The string to read from.
 * @param next Where the encoding will be saved.
 * @return The UTF-8 encoding's byte length or zero in case the encoding
 * is the null terminator.
 * @details This function won't increment the pointer, do so manually
 * if needed using the returned length. In case the encoding is not
 * valid UTF-8, `next` will be set to 0xEFBFBD (the replacement character U+FFFD).
 */
uint8_t utf8_next(const char *str, utf8_char *next);

/**
 * Get a `utf8_str` struct from a string.
 * @param str The string.
 * @return A `utf8_str` struct. On failure, str and length will be set to
 * NULL and 0 respectively.
 * @details Failure is when the given string contains invalid UTF-8, or is NULL.
 */
utf8_str utf8_str_make(const char *str);

/**
 * Concatenate two `utf8_str` strings.
 * @param str1 First string.
 * @param str2 Second string.
 * @return The resulting 'utf8_str' string.
 * @details The resulting string is heap allocated, and thus, it is the caller's
 * responsibility to free using utf8_str_free().
 */
utf8_str utf8_str_join(utf8_str str1, utf8_str str2);

/**
 * Free a heap allocated `utf8_str` string.
 * @param str The string to free.
 */
void utf8_str_free(utf8_str str);
