#ifndef IFJ_PROJ_STRING_H
#define IFJ_PROJ_STRING_H

#include <string.h>
#include <stdlib.h>

#define STRING_ALLOCATION_SIZE sizeof(char)

/**
 * @struct string_t
 * String structure
 *
 * @var string_t::value
 * String value
 *
 * @var string_t::length
 * String length
 *
 * @var string_t::capacity
 * String capacity
 */
typedef struct string {
    char *value;
    size_t length;
    size_t capacity;
} string_t;

/**
 * Initializes string
 * @return pointer to string
 */
string_t *string_base_init();

/**
 * Initializes string with value
 * @param value string value
 * @return pointer to string
 */
string_t *string_init(const char *value);

/**
 * Appends character to string
 * @param str pointer to string
 * @param c character
 */
void string_append_char(string_t *str, char c);

/**
 * Appends string to string
 * @param str pointer to string
 * @param value string value
 */
void string_append_string(string_t *str, const char *value);

/**
 * Clears string
 * @param str pointer to string
 */
void string_clear(string_t *str);

#endif //IFJ_PROJ_STRING_H
