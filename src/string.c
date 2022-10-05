#include "string.h"
#include <stdio.h>

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

void string_append_string(string_t *str, const char *value) {
    if (str == NULL || value == NULL)
        return;

    size_t value_length = strlen(value);

    if (str->length + value_length + 1 >= str->capacity) {
        size_t new_capacity = str->capacity + value_length + STRING_ALLOCATION_SIZE;
        char *new_value = (char *) realloc(str->value, new_capacity);
        if (new_value == NULL)
            return;

        str->value = new_value;
        str->capacity = new_capacity;
    }
    strcat(str->value, value);
    str->length += value_length;
    str->value[str->length] = '\0';
}

void string_clear(string_t *str) {
    if (str == NULL) return;

    str->length = 0;
    str->value[str->length] = '\0';
}
