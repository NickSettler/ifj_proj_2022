/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *
 * @file expression.c
 * @brief 
 * @date 17.10.2022
 */

#include "expression.h"

int process_expression(lexical_token_t *token) {
    bool done = false;

    while (!done) {
        int a, b;

        if (token->type == EOF || token->type == SEMICOLON) {
            done = true;
        }
    }

    return 0;
}