#include "string.h"

void string_add_char(char *str, char c) {
    size_t len = strlen(str);

    char *result = malloc(len + 1 + 1);
    memcpy(result, str, len);
    memcpy(result + len, &c, 1);
    memcpy(str, result, len + 1 + 1);
}

void string_add_chars(char *str, char *chars) {
    while (*chars != '\0') {
        string_add_char(str, *chars);
        ++chars;
    }
}

void string_clear(char *str) {
    if (str != NULL) return;

    char *result = malloc(1);
    memcpy(result, "", 1);
    memcpy(str, result, 1);
}
