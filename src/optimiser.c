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

void replace_variable_usage_internal(syntax_abstract_tree_t *tree) {
    if (!tree) return;

    if (tree->type &
        (SYN_NODE_ADD | SYN_NODE_SUB | SYN_NODE_MUL | SYN_NODE_DIV | SYN_NODE_TYPED_EQUAL | SYN_NODE_TYPED_NOT_EQUAL |
         SYN_NODE_LESS | SYN_NODE_LESS_EQUAL | SYN_NODE_GREATER | SYN_NODE_GREATER_EQUAL)) {
        if (tree->left->type == SYN_NODE_IDENTIFIER &&
            !strcmp(tree->left->value->value, optimiser_params->current_replaced_variable_name->value)) {
            tree->left = tree_copy(optimiser_params->current_replaced_variable_tree);
        }
        if (tree->right->type == SYN_NODE_IDENTIFIER &&
            !strcmp(tree->right->value->value, optimiser_params->current_replaced_variable_name->value)) {
            tree->right = tree_copy(optimiser_params->current_replaced_variable_tree);
        }
    }

    if (tree->type & SYN_NODE_ARGS) {
        if (tree->left->type == SYN_NODE_IDENTIFIER &&
            !strcmp(tree->left->value->value, optimiser_params->current_replaced_variable_name->value)) {
            tree->left = tree_copy(optimiser_params->current_replaced_variable_tree);
        }
    }
}

void replace_variable_usage(syntax_abstract_tree_t *tree, syntax_abstract_tree_t *current_tree) {
    if (!tree)
        return;

    syntax_abstract_tree_t *trees[1000] = {NULL};

    int i = 0;
    int current_level = 0;

    while (tree && tree->type == SYN_NODE_SEQUENCE) {
        trees[i++] = tree;
        tree = tree->left;
        if (tree && tree->right == current_tree) current_level = i;
    }

    for (int j = current_level - 1; j >= 0; j--) {
        syntax_abstract_tree_t *current = trees[j];
        if (current->right == current_tree) continue;

        if (current->right->type == SYN_NODE_ASSIGN) {
            if (!strcmp(current->right->left->value->value, optimiser_params->current_replaced_variable_name->value)) {
                process_tree_using(current->right->right, replace_variable_usage_internal, POSTORDER);
                break;
            }
        }

        process_tree_using(current->right, replace_variable_usage_internal, POSTORDER);
    }
}

void optimize_expression(syntax_abstract_tree_t *tree) {
    if ((tree->type &
         (SYN_NODE_ADD | SYN_NODE_SUB | SYN_NODE_MUL | SYN_NODE_DIV | SYN_NODE_TYPED_EQUAL | SYN_NODE_TYPED_NOT_EQUAL |
          SYN_NODE_LESS | SYN_NODE_LESS_EQUAL | SYN_NODE_GREATER | SYN_NODE_GREATER_EQUAL)) == 0)
        return;

    switch (tree->type) {
        case SYN_NODE_ADD: {
            GET_NODE_NUMBERS

            double result = left_number + right_number;

            PROCESS_DECIMAL(result)
            REPLACE_TREE_VALUE(result_str, is_float ? SYN_NODE_FLOAT : SYN_NODE_INTEGER)
            break;
        }
        case SYN_NODE_SUB: {
            GET_NODE_NUMBERS

            double result = left_number - right_number;

            PROCESS_DECIMAL(result)
            REPLACE_TREE_VALUE(result_str, is_float ? SYN_NODE_FLOAT : SYN_NODE_INTEGER)
            break;
        }
        case SYN_NODE_MUL: {
            GET_NODE_NUMBERS

            double result = left_number * right_number;

            PROCESS_DECIMAL(result)
            REPLACE_TREE_VALUE(result_str, is_float ? SYN_NODE_FLOAT : SYN_NODE_INTEGER)
            break;
        }
        case SYN_NODE_DIV: {
            GET_NODE_NUMBERS

            double result = left_number / right_number;

            PROCESS_DECIMAL(result)
            REPLACE_TREE_VALUE(result_str, is_float ? SYN_NODE_FLOAT : SYN_NODE_INTEGER)
            break;
        }
        case SYN_NODE_TYPED_EQUAL: {
            GET_NODE_NUMBERS

            bool result = left_number == right_number;

            REPLACE_TREE_VALUE(result ? "1" : "0", SYN_NODE_INTEGER)
            break;
        }
        case SYN_NODE_TYPED_NOT_EQUAL: {
            GET_NODE_NUMBERS

            bool result = left_number != right_number;

            REPLACE_TREE_VALUE(result ? "1" : "0", SYN_NODE_INTEGER)
            break;
        }
        case SYN_NODE_LESS: {
            GET_NODE_NUMBERS

            bool result = left_number < right_number;

            REPLACE_TREE_VALUE(result ? "1" : "0", SYN_NODE_INTEGER)
            break;
        }
        case SYN_NODE_LESS_EQUAL: {
            GET_NODE_NUMBERS

            bool result = left_number <= right_number;

            REPLACE_TREE_VALUE(result ? "1" : "0", SYN_NODE_INTEGER)
            break;
        }
        case SYN_NODE_GREATER: {
            GET_NODE_NUMBERS

            bool result = left_number > right_number;

            REPLACE_TREE_VALUE(result ? "1" : "0", SYN_NODE_INTEGER)
            break;
        }
        case SYN_NODE_GREATER_EQUAL: {
            GET_NODE_NUMBERS

            bool result = left_number >= right_number;

            REPLACE_TREE_VALUE(result ? "1" : "0", SYN_NODE_INTEGER)
            break;
        }
        default:
            break;
    }
}

void optimize_node(syntax_abstract_tree_t *tree, optimise_type_t optimise_type) {
    if (!tree) return;

    optimize_node(tree->left, optimise_type);

    switch (tree->right->type) {
        case SYN_NODE_ASSIGN: {
            if (optimise_type == OPTIMISE_EXPRESSION) {
                process_tree_using(tree->right, optimize_expression, POSTORDER);
                optimiser_params->current_replaced_variable_name = tree->right->left->value;
                optimiser_params->current_replaced_variable_tree = tree->right->right;
                replace_variable_usage(optimiser_params->root_tree, tree->right);
            }

            if (optimise_type == OPTIMISE_UNUSED_VARIABLES) {
                optimiser_params->current_unused_variable = tree->right->left->value;
                optimize_unused_variables(optimiser_params->root_tree, tree->right);
            }
            break;
        }
        case SYN_NODE_KEYWORD_IF: {
            if (optimise_type == OPTIMISE_EXPRESSION) {
                process_tree_using(tree->right, optimize_expression, POSTORDER);
                optimiser_params->current_replaced_variable_name = tree->right->left->value;
                optimiser_params->current_replaced_variable_tree = tree->right->right;
                replace_variable_usage(optimiser_params->root_tree, tree->right);
            }
        }
        case SYN_NODE_CALL: {
            if (optimise_type == OPTIMISE_EXPRESSION) {
                process_tree_using(tree->right, optimize_expression, POSTORDER);
                optimiser_params->current_replaced_variable_name = tree->right->left->value;
                optimiser_params->current_replaced_variable_tree = tree->right->right;
                replace_variable_usage(optimiser_params->root_tree, tree->right);
            }
        }
        default:
            break;
    }
}

void optimize_tree(syntax_abstract_tree_t *tree) {
    if (!optimiser_params)
        init_optimiser();

    optimiser_params->root_tree = tree;

    optimize_node(tree, OPTIMISE_EXPRESSION);
//    optimize_node(tree, OPTIMISE_UNUSED_VARIABLES);
}

void init_optimiser() {
    optimiser_parameters_t *params = (optimiser_parameters_t *) malloc(sizeof(optimiser_parameters_t));

    params->root_tree = NULL;
    params->current_unused_variable = NULL;
    params->current_replaced_variable_name = NULL;
    params->current_replaced_variable_tree = NULL;

    optimiser_params = params;
}
