/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *
 * @file str.h
 * @brief Custom String Implementation
 * @date 05.10.2022
 */

#ifndef IFJ_PROJ_STRING_H
#define IFJ_PROJ_STRING_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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
void string_append_string(string_t *str, const char *value, ...);

/**
 * Clears string
 * @param str pointer to string
 */
void string_clear(string_t *str);

/**
 * Replaces string value
 * @param str pointer to string
 * @param value string value
 */
void string_replace(string_t *str, char *value);

/**
 * Gets substring value of a string from start to end
 * @param str pointer to string
 * @param start start index
 * @param end end index
 * @return substring value
 */
string_t *string_substr(string_t *str, int start, int end);

/**
 * Calls a function on all characters in string
 * @param str pointer to a string
 * @param func pointer to a function
 * @return true if every function call returns true, false otherwise
 */
bool string_check_by(string_t *str, int (*func)(int));

/**
 * Calls a void function on all characters in string
 * @param str pointer to a string
 * @param func pointer to a function
 */
void string_convert_by(string_t *str, int (*func)(int));

void string_free(string_t *str);

#endif //IFJ_PROJ_STRING_H
