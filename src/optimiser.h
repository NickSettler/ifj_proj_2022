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

static char *numbers_buffer;

#define GET_NODE_NUMBER(child) \
    if ((tree->child->type & (SYN_NODE_FLOAT | SYN_NODE_INTEGER)) == 0) \
        break; \
    double child##_number = strtod(tree->child->value->value, &numbers_buffer);

#define GET_NODE_NUMBERS \
    GET_NODE_NUMBER(left)\
    GET_NODE_NUMBER(right)

#define PROCESS_DECIMAL(result) \
    int result_int = (int) result;\
    double decimal = result - result_int; \
    char *result_str = (char *) malloc(1024); \
    if (decimal == 0) \
    sprintf(result_str, "%d", result_int); \
    else \
    sprintf(result_str, "%.10f", result); \
    bool is_float = tree->left->type == SYN_NODE_FLOAT || tree->right->type == SYN_NODE_FLOAT || tree->type == SYN_NODE_DIV;

#define REPLACE_TREE_VALUE(str, t) \
    tree->type = t; \
    tree->value = string_init(str); \
    free_syntax_tree(tree->left); \
    free_syntax_tree(tree->right); \
    tree->left = NULL; \
    tree->right = NULL;

#include "syntax_analyzer.h"

typedef struct optimiser_parameters {
    syntax_abstract_tree_t *root_tree;
    string_t *current_unused_variable;
    string_t *current_replaced_variable_name;
    syntax_abstract_tree_t *current_replaced_variable_tree;
} optimiser_parameters_t;

typedef enum {
    OPTIMISE_EXPRESSION = 1 << 0,
    OPTIMISE_UNUSED_VARIABLES = 1 << 1,
    OPTIMISE_UNREACHABLE_CODE = 1 << 2,
} optimise_type_t;

optimiser_parameters_t *optimiser_params;

/**
 * Checks if optimiser can determine if the expression is true or false
 * @param tree abstract syntax tree
 * @return true if it can determine, false otherwise
 */
bool can_detect_bool(syntax_abstract_tree_t *tree);

/**
 * Checks if the expression is true
 * @param tree abstract syntax tree
 * @return true if it is true, false otherwise
 */
bool is_true(syntax_abstract_tree_t *tree);

/**
 * Optimises unreachable if statement
 * @param tree abstract syntax tree
 */
void optimise_unreachable_if(syntax_abstract_tree_t *tree);

/**
 * Internal function of variables replacement
 * @param tree tree to replace in
 */
void replace_variable_usage_internal(syntax_abstract_tree_t *tree);

/**
 * Replaces variable usage in tree
 * @param tree tree to replace in
 * @param current_tree current tree with replacing variable
 */
void replace_variable_usage(syntax_abstract_tree_t *tree, syntax_abstract_tree_t *current_tree);

/**
 * Optimise math expression
 * @param tree tree to optimise
 */
void optimize_expression(syntax_abstract_tree_t *tree);

/**
 * Optimize SYN_NODE_SEQUENCE node
 * @param tree node to optimize
 * @param optimise_type type of optimisation
 */
void optimize_node(syntax_abstract_tree_t *tree, optimise_type_t optimise_type);

/**
 * Optimize syntax tree
 * @param tree syntax tree
 */
void optimize_tree(syntax_abstract_tree_t *tree);

/**
 * Initialize optimiser options
 */
void init_optimiser();

#endif //IFJ_PROJ_OPTIMISER_H
