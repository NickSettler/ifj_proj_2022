/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *
 * @file optimiser.h
 * @brief 
 * @date 27.11.2022
 */

#ifndef IFJ_PROJ_OPTIMISER_H
#define IFJ_PROJ_OPTIMISER_H

#define GET_NODE_NUMBERS \
    if ((tree->left->type & (SYN_NODE_FLOAT | SYN_NODE_INTEGER)) == 0 || \
        (tree->right->type & (SYN_NODE_FLOAT | SYN_NODE_INTEGER)) == 0) \
        break; \
    char *buffer; \
    double left_number = strtod(tree->left->value->value, &buffer); \
    double right_number = strtod(tree->right->value->value, &buffer);

#define PROCESS_DECIMAL(result) \
    int result_int = (int) result;\
    double decimal = result - result_int; \
    char *result_str = (char *) malloc(1024); \
    if (decimal == 0) \
    sprintf(result_str, "%d", result_int); \
    else \
    sprintf(result_str, "%.10f", result); \
    bool is_float = tree->left->type == SYN_NODE_FLOAT || tree->right->type == SYN_NODE_FLOAT || tree->type == SYN_NODE_DIV;

#define REPLACE_TREE_VALUE(str, dec) \
    tree->type = is_float ? SYN_NODE_FLOAT : SYN_NODE_INTEGER; \
    tree->value = string_init(str); \
    free_syntax_tree(tree->left); \
    free_syntax_tree(tree->right); \
    tree->left = NULL; \
    tree->right = NULL;

#include "syntax_analyzer.h"

void optimize_expression(syntax_abstract_tree_t *tree);

void optimize_node(syntax_abstract_tree_t *tree);

void optimize_tree(syntax_abstract_tree_t *tree);

#endif //IFJ_PROJ_OPTIMISER_H
