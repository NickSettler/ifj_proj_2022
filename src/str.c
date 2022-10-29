/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *
 * @file str.h
 * @brief Custom String Implementation
 * @date 05.10.2022
 */

#include "str.h"
#include <stdio.h>
#include <stdarg.h>

string_t *string_base_init() {
    string_t *string = (string_t *) malloc(sizeof(string_t));
    if (string == NULL)
        return NULL;

    string->value = (char *) malloc(STRING_ALLOCATION_SIZE);
    if (string->value == NULL) {
        free(string);
        return NULL;
    }

    string->capacity = STRING_ALLOCATION_SIZE;
    return string;
}

string_t *string_init(const char *value) {
    string_t *string = string_base_init();

    string->value = (char *) malloc(sizeof(char) * strlen(value));
    strcpy(string->value, value);
    string->length = strlen(value);
    string->capacity = strlen(value);

    return string;
}

void string_append_char(string_t *str, char c) {
    if (str == NULL)
        return;

    if (str->length + 1 >= str->capacity) {
        size_t new_capacity = str->capacity + STRING_ALLOCATION_SIZE;
        char *new_value = (char *) realloc(str->value, new_capacity);
        if (new_value == NULL)
            return;

        str->value = new_value;
        str->capacity = new_capacity;
    }

    str->value[str->length++] = c;
    str->value[str->length] = '\0';
}

void string_append_string(string_t *str, const char *value, ...) {
    if (str == NULL || value == NULL)
        return;

    va_list args;
    va_start(args, value);

    char *extra_value = (char *) malloc(sizeof(char) * strlen(value));

    vsprintf(extra_value, value, args);

    va_end(args);

    size_t new_length = strlen(extra_value);

    if (str->length + new_length >= str->capacity) {
        size_t new_capacity = str->capacity + new_length;
        char *new_value = (char *) realloc(str->value, new_capacity);
        if (new_value == NULL)
            return;

        str->value = new_value;
        str->capacity = new_capacity;
    }

    strcat(str->value, extra_value);
    str->length += new_length;

    free(extra_value);

    str->value[str->length] = '\0';
}

void string_clear(string_t *str) {
    if (str == NULL) return;

    str->length = 0;
    str->value[str->length] = '\0';
}
