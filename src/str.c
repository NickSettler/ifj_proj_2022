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
#include "errors.h"
#include <stdio.h>
#include <stdarg.h>

string_t *string_base_init() {
    string_t *string = (string_t *) malloc(sizeof(string_t));
    if (string == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for string");
    }

    string->value = (char *) malloc(STRING_ALLOCATION_SIZE);
    if (string->value == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for string");
    }

    string->length = 0;
    string->value[0] = '\0';
    string->capacity = STRING_ALLOCATION_SIZE;
    return string;
}

string_t *string_init(const char *value) {
    string_t *string = string_base_init();

    string->value = (char *) malloc(sizeof(char) * strlen(value) + 4);
    if (!string->value) {
        INTERNAL_ERROR("Failed to allocate memory for string");
    }

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
        char *new_value = (char *) malloc(new_capacity + 4);
        if (new_value == NULL) {
            INTERNAL_ERROR("Failed to allocate memory for string");
        }

        memcpy(new_value, str->value, str->length);
        free(str->value);
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

    char *extra_value = (char *) malloc(sizeof(char) * strlen(value) + 4);

    vsprintf(extra_value, value, args);

    va_end(args);

    size_t new_length = strlen(extra_value);

    if (str->length + new_length >= str->capacity) {
        size_t new_capacity = str->capacity + new_length;
        char *new_value = (char *) malloc(new_capacity + 4);
        if (new_value == NULL) {
            INTERNAL_ERROR("Failed to allocate memory for string");
        }

        strncpy(new_value, str->value, str->length);
        new_value[str->length] = '\0';

        free(str->value);
        str->value = new_value;
        str->capacity = new_capacity;
    }

    strncat(str->value, extra_value, new_length);
    str->length += new_length;
    str->value[str->length] = '\0';

    free(extra_value);
}

void string_clear(string_t *str) {
    if (str == NULL) return;

    str->length = 0;
    str->value[str->length] = '\0';
}

void string_replace(string_t *str, char *value) {
    if (str == NULL || value == NULL)
        return;

    size_t new_length = strlen(value);

    if (new_length >= str->capacity) {
        size_t new_capacity = new_length;
        char *new_value = (char *) malloc(new_capacity + 4);
        if (new_value == NULL) {
            INTERNAL_ERROR("Failed to allocate memory for string");
        }

        free(str->value);
        str->value = new_value;
        str->capacity = new_capacity;
    }

    memcpy(str->value, value, new_length);
    str->length = new_length;
    str->value[str->length] = '\0';
}

string_t *string_substr(string_t *str, int start, int end) {
    if (str == NULL)
        return NULL;

    if (start < 0 || end < 0 || start > end || end > str->length)
        return NULL;

    string_t *substr = string_base_init();

    for (int i = start; i < end; i++)
        string_append_char(substr, str->value[i]);

    return substr;
}

bool string_check_by(string_t *str, int (*func)(int)) {
    if (str == NULL) return false;

    for (size_t i = 0; i < str->length; i++) {
        if (!func(str->value[i])) return false;
    }

    return true;
}

void string_convert_by(string_t *str, int (*func)(int)) {
    if (str == NULL) return;

    for (size_t i = 0; i < str->length; i++) {
        str->value[i] = (char) func(str->value[i]);
    }
}

void string_free(string_t *str) {
    if (str == NULL) return;

    free(str->value);
    free(str);
}
