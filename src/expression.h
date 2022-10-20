/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *
 * @file expression.h
 * @brief 
 * @date 17.10.2022
 */

#ifndef IFJ_PROJ_EXPRESSION_H
#define IFJ_PROJ_EXPRESSION_H

#include "lexical_token.h"
#include <stdbool.h>

#define PRECEDENCE_TABLE_SIZE 10

const char precedence_table[PRECEDENCE_TABLE_SIZE][PRECEDENCE_TABLE_SIZE] = {
        //+-  */   (    )    i    $    <>   #
        {'>', '>', '<', '>', '>', '<', '>', '>', '>', '>'}, // +-
        {'>', '>', '<', '>', '>', '<', '>', '>', '>', '>'}, // */
        {'<', '<', '<', '=', ' ', '<', ' ', '<', '<', ' '}, // (
        {'>', '>', ' ', '>', '>', ' ', '>', '>', '>', '>'}, // )
        {'<', '<', '<', ' ', '=', '<', ' ', '<', '<', ' '}, // i
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // $
        {'<', '<', '<', ' ', ' ', '<', ' ', '<', '<', ' '}, // <>
        {'<', '<', '<', ' ', ' ', '<', ' ', '<', '<', ' '}, // #
};

int process_expression(lexical_token_t *token);

#endif //IFJ_PROJ_EXPRESSION_H
