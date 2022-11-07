/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *
 * @file errors.c
 * @brief Error handling
 * @date 24.09.2022
 */

#include <string.h>
#include "errors.h"

char *get_readable_error_char(char *string) {
    if (!strcmp(string, "\0")) {
        return "EOF";
    }

    return string;
}
