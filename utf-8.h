#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define UTF8_ERROR_INVALID (-1)

typedef uint32_t utf8_char_t;

/**
 * This is a wrapper type for UTF-8 strings. Its byte array may or may not
 * be heap allocated. The string is considered empty when `str` is NULL or
 * when `length` is zero, ideally both.
 */
typedef struct Utf8_String {
    size_t length; // Byte size excluding the null terminator
    const char *str;
} utf8_str_t;

/**
 * @brief Encode a codepoint.
 * @param[in] codepoint The codepoint to encode.
 * @return An encoding.
 * @note This function does not check if the given codepoint is valid.
 */
utf8_char_t utf8_encode(uint32_t codepoint);

/**
 * @brief Decode an encoding.
 * @param[in] encoding The encoding to decode.
 * @return A codepoint.
 * @note This function does not check if the given encoding is valid.
 */
uint32_t utf8_decode(utf8_char_t encoding);

/**
 * @brief Check if an encoding is valid UTF-8.
 * @param[in] encoding The encoding to validate.
 * @return True if the encoding is valid, false if it isn't.
 */
bool utf8_validate_encoding(utf8_char_t encoding);

/**
 * @brief Encode a codepoint as a string.
 * @details The string is written to the byte array pointed to by `buffer`.
 * Said byte array must be of sufficient size.
 * @param[in] codepoint The codepoint to encode.
 * @param[out] buffer The destination buffer.
 * @return The number of bytes written or zero on failure.
 */
uint8_t utf8_encode_to_str(uint32_t codepoint, char *buffer);

/**
 * @brief Get the next encoding from a string.
 * @details If a valid encoding could be read, it is stored in the variable pointed to by `next`.
 * @param[in] str The buffer to read from.
 * @param[out] next Reference to the variable in which the encoding will be stored.
 * @return The number of bytes read (zero if the encoding is the null terminator)
 * or negative on failure.
 */
int utf8_next(const char *str, utf8_char_t *next);

/**
 * @brief Convert a string to a utf8_str_t string.
 * @details Invalid encodings will be replaced with the replacement character U+FFFD.
 * @param[in] str The buffer to read from.
 * @return A utf8_str_t string.
 * @note The resulting utf8_str_t string is heap allocated, and thus, it is the caller's
 * responsibility to free using utf8_str_t_free().
 */
utf8_str_t utf8_str_t_new(const char *str);

/**
 * Free a heap allocated utf8_str_t string.
 * @param[in] str The utf8_str_t string to free.
 */
void utf8_str_t_free(utf8_str_t str);