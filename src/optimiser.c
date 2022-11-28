/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *
 * @file optimiser.c
 * @brief 
 * @date 27.11.2022
 */

#include <stdlib.h>
#include "optimiser.h"

void optimize_expression(syntax_abstract_tree_t *tree) {
    if ((tree->type & (SYN_NODE_ADD | SYN_NODE_SUB | SYN_NODE_MUL | SYN_NODE_DIV)) == 0)
        return;

    switch (tree->type) {
        case SYN_NODE_ADD: {
            GET_NODE_NUMBERS

            double result = left_number + right_number;

            PROCESS_DECIMAL(result)
            REPLACE_TREE_VALUE(result_str, decimal)
            break;
        }
        case SYN_NODE_SUB: {
            GET_NODE_NUMBERS

            double result = left_number - right_number;

            PROCESS_DECIMAL(result)
            REPLACE_TREE_VALUE(result_str, decimal)
            break;
        }
        case SYN_NODE_MUL: {
            GET_NODE_NUMBERS

            double result = left_number * right_number;

            PROCESS_DECIMAL(result)
            REPLACE_TREE_VALUE(result_str, decimal)
            break;
        }
        case SYN_NODE_DIV: {
            GET_NODE_NUMBERS

            double result = left_number / right_number;

            PROCESS_DECIMAL(result)
            REPLACE_TREE_VALUE(result_str, decimal)
            break;
        }
        default:
            break;
    }
}

void optimize_node(syntax_abstract_tree_t *tree) {
    if (!tree) return;

    optimize_node(tree->left);

    switch (tree->right->type) {
        case SYN_NODE_ASSIGN: {
            process_tree_using(tree->right, optimize_expression, POSTORDER);
            break;
        }
        default:
            break;
    }
}

void optimize_tree(syntax_abstract_tree_t *tree) {
    optimize_node(tree);
}
