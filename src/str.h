#ifndef IFJ_PROJ_STRING_H
#define IFJ_PROJ_STRING_H

#include <string.h>
#include <stdlib.h>

#define STRING_ALLOCATION_SIZE sizeof(char)

typedef struct string {
    char *value;
    size_t length;
    size_t capacity;
} string_t;

string_t *string_base_init();

string_t *string_init(const char* value);

void string_append_char(string_t *str, char c);

void string_append_string(string_t *str, const char *value);

void string_clear(string_t *str);

#endif //IFJ_PROJ_STRING_H
