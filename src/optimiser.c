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

bool can_detect_bool(syntax_abstract_tree_t *tree) {
    if (!tree) return true;

    if ((tree->type & (SYN_NODE_INTEGER | SYN_NODE_FLOAT | SYN_NODE_STRING | SYN_NODE_KEYWORD_NULL)) == 0) {
        return false;
    }

    return true;
}

bool is_true(syntax_abstract_tree_t *tree) {
    if (!tree) return true;

    if (tree->type == SYN_NODE_INTEGER) {
        if (!strcmp(tree->value->value, "0")) return false;

        return true;
    } else if (tree->type == SYN_NODE_FLOAT) {
        double tree_number = strtod(tree->value->value, &numbers_buffer);
        double int_value = (int) tree_number;

        return int_value != 0;
    } else if (tree->type == SYN_NODE_STRING) {
        if (!strcmp(tree->value->value, "\"\"")) return false;

        return true;
    } else if (tree->type == SYN_NODE_KEYWORD_NULL) {
        return false;
    }

    return true;
}

bool is_unused(syntax_abstract_tree_t *tree) {
    if (!tree) return true;

    if (tree->type == SYN_NODE_IDENTIFIER) {
        bool same_identifier_name = !strcmp(tree->value->value, optimiser_params->current_unused_variable_name->value);

        if (same_identifier_name) {
            return false;
        }
    }

    return true;
}

void optimise_unreachable_while(syntax_abstract_tree_t *tree) {
    if (!tree || !tree->right) return;

    if (tree->right->type != SYN_NODE_KEYWORD_WHILE) return;

    syntax_abstract_tree_t *cond_copy = tree_copy(tree->right->left);
    process_tree_using(cond_copy, optimize_expression, POSTORDER);
    bool is_cond_false = !check_tree_using(cond_copy, is_true);

    free_syntax_tree(cond_copy);
    cond_copy = NULL;

    if (is_cond_false) {
        free_syntax_tree(tree->right);
        tree->right = NULL;
    } else {
        optimize_node(tree->right->right, OPTIMISE_UNREACHABLE_CODE);
    }
}

void optimise_unreachable_if(syntax_abstract_tree_t *tree) {
    if (!tree || !tree->right) return;

    if (tree->right->type != SYN_NODE_KEYWORD_IF) return;

    if (!check_tree_using(tree->right->left, can_detect_bool)) return;

    bool is_expr_true = check_tree_using(tree->right->left, is_true);

    free_syntax_tree(tree->right->left);
    tree->right->left = NULL;

    bool has_else = tree->right->right != NULL;

    if (is_expr_true) {
        if (has_else) {
            free_syntax_tree(tree->right->right);
            tree->right->right = NULL;
        }

        tree->right = tree->right->middle;

        optimize_node(tree->right, OPTIMISE_UNREACHABLE_CODE);
    } else {
        if (has_else) {
            tree->right = tree->right->right;

            if (tree->right->type == SYN_NODE_KEYWORD_IF) {
                optimise_unreachable_if(tree);
            }
        } else {
            free_syntax_tree(tree->right);
            tree->right = NULL;
        }

        free_syntax_tree(tree->middle);
        tree->middle = NULL;

        optimize_node(tree->right, OPTIMISE_UNREACHABLE_CODE);
    }
}

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
    if (!tree) return;

    syntax_abstract_tree_t *trees[1000] = {NULL};

    int i = 0;
    int current_level = -1;

    while (tree && tree->type == SYN_NODE_SEQUENCE) {
        trees[i++] = tree;

        bool is_same_tree = compare_syntax_tree(tree->right->right, optimiser_params->current_replaced_variable_tree);

        if (current_level == -1 && tree->right && tree->right->type == SYN_NODE_ASSIGN &&
            !strcmp(tree->right->left->value->value, optimiser_params->current_replaced_variable_name->value) &&
            is_same_tree)
            current_level = i;
        tree = tree->left;
    }

    if (current_level == -1) current_level = i;

    for (int j = current_level - 1; j >= 0; j--) {
        syntax_abstract_tree_t *current = trees[j];

        bool is_same_tree = compare_syntax_tree(current->right->right,
                                                optimiser_params->current_replaced_variable_tree);

        if (current->right && current->right->type == SYN_NODE_ASSIGN &&
            !strcmp(current->right->left->value->value, optimiser_params->current_replaced_variable_name->value) &&
            is_same_tree)
            continue;

        if (current->right->type == SYN_NODE_ASSIGN) {
            if (!strcmp(current->right->left->value->value, optimiser_params->current_replaced_variable_name->value)) {
                process_tree_using(current->right->right, replace_variable_usage_internal, POSTORDER);
                break;
            }
        }

        if (current->right->type == SYN_NODE_KEYWORD_IF) {
            process_tree_using(current->right->left, replace_variable_usage_internal, POSTORDER);
            if (current->right->middle) {
                replace_variable_usage(current->right->middle, current_tree);
                syntax_abstract_tree_t *tmp_tree = optimiser_params->root_tree;
                optimiser_params->root_tree = current->right->middle;
                optimize_node(current->right->middle, OPTIMISE_EXPRESSION);
                optimiser_params->root_tree = tmp_tree;
                optimize_node(current->right->middle, OPTIMISE_UNREACHABLE_CODE);
            }
            if (current->right->right) {
                if (current->right->right->type == SYN_NODE_KEYWORD_IF)
                    process_tree_using(current->right->right->left, replace_variable_usage_internal, POSTORDER);

                replace_variable_usage(current->right->right, current_tree);
                syntax_abstract_tree_t *tmp_tree = optimiser_params->root_tree;
                optimiser_params->root_tree = current->right->right;
                optimize_node(current->right->right, OPTIMISE_EXPRESSION);
                optimiser_params->root_tree = tmp_tree;
                optimize_node(current->right->middle, OPTIMISE_UNREACHABLE_CODE);
            }
            break;
        }

        if (current->right->type == SYN_NODE_KEYWORD_WHILE) {
            syntax_abstract_tree_t *cond_copy = tree_copy(current->right->left);
            process_tree_using(cond_copy, replace_variable_usage_internal, POSTORDER);
            process_tree_using(cond_copy, optimize_expression, POSTORDER);
            bool is_cond_false = !check_tree_using(cond_copy, is_true);
            free_syntax_tree(cond_copy);
            cond_copy = NULL;

            if (is_cond_false) {
                free_syntax_tree(current->right);
                trees[j]->right = NULL;
            } else {
                optimize_node(current->right->right, OPTIMISE_UNREACHABLE_CODE);
            }

            continue;
        }

        process_tree_using(current->right, replace_variable_usage_internal, POSTORDER);
    }
}

void remove_unused_variables(syntax_abstract_tree_t *tree) {
    if (!tree) return;

    syntax_abstract_tree_t *trees[1000] = {NULL};

    int i = 0;
    int current_level = -1;

    while (tree && tree->type == SYN_NODE_SEQUENCE) {
        trees[i++] = tree;

        bool is_same_tree = compare_syntax_tree(tree->right, optimiser_params->current_unused_variable_tree);

        if (current_level == -1 && tree->right && tree->right->type == SYN_NODE_ASSIGN &&
            !strcmp(tree->right->left->value->value, optimiser_params->current_unused_variable_name->value) &&
            is_same_tree)
            current_level = i - 1;
        tree = tree->left;
    }

    bool is_used_in_code = false;

    for (int j = current_level; j >= 0; j--) {
        syntax_abstract_tree_t *current = trees[j];

        bool is_same_tree = compare_syntax_tree(current->right, optimiser_params->current_unused_variable_tree);

        if (current->right && current->right->type == SYN_NODE_ASSIGN &&
            !strcmp(current->right->left->value->value, optimiser_params->current_unused_variable_name->value) &&
            is_same_tree)
            continue;

        if (!is_used_in_code) {
            bool is_used_in_current = !check_tree_using(current->right, is_unused);
            is_used_in_code = is_used_in_current;

            if (is_used_in_current) break;
        }
    }

    if (!is_used_in_code) {
        free_syntax_tree(trees[current_level]->right);
        trees[current_level]->right = NULL;
    }
}

void optimize_expression(syntax_abstract_tree_t *tree) {
    if ((tree->type &
         (SYN_NODE_ADD | SYN_NODE_SUB | SYN_NODE_MUL | SYN_NODE_DIV | SYN_NODE_TYPED_EQUAL | SYN_NODE_TYPED_NOT_EQUAL |
          SYN_NODE_LESS | SYN_NODE_LESS_EQUAL | SYN_NODE_GREATER | SYN_NODE_GREATER_EQUAL | SYN_NODE_AND | SYN_NODE_OR |
          SYN_NODE_NOT | SYN_NODE_INTEGER | SYN_NODE_FLOAT | SYN_NODE_STRING)) == 0)
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
        case SYN_NODE_AND: {
            GET_NODE_NUMBERS

            bool result = left_number && right_number;

            REPLACE_TREE_VALUE(result ? "1" : "0", SYN_NODE_INTEGER)
            break;
        }
        case SYN_NODE_OR: {
            GET_NODE_NUMBERS

            bool result = left_number || right_number;

            REPLACE_TREE_VALUE(result ? "1" : "0", SYN_NODE_INTEGER)
            break;
        }
        case SYN_NODE_NOT: {
            GET_NODE_NUMBER(left)

            bool result = !left_number;

            REPLACE_TREE_VALUE(result ? "1" : "0", SYN_NODE_INTEGER)
            break;
        }
        case SYN_NODE_INTEGER:
        case SYN_NODE_FLOAT:
        case SYN_NODE_STRING: {
            change_node_type(tree, TYPE_INT);
        }
        default:
            break;
    }
}

void optimize_node(syntax_abstract_tree_t *tree, optimise_type_t optimise_type) {
    if (!tree) return;

    optimize_node(tree->left, optimise_type);

    if (!tree->right) return;

    switch (tree->right->type) {
        case SYN_NODE_ASSIGN: {
            if (optimise_type == OPTIMISE_EXPRESSION) {
                process_tree_using(tree->right, optimize_expression, POSTORDER);
                if (tree->right->right->type & (SYN_NODE_INTEGER | SYN_NODE_FLOAT)) {
                    optimiser_params->current_replaced_variable_name = tree->right->left->value;
                    optimiser_params->current_replaced_variable_tree = tree->right->right;
                    replace_variable_usage(optimiser_params->root_tree, tree->right);
                }
            }
            if (optimise_type == OPTIMISE_UNUSED_VARIABLES) {
                optimiser_params->current_unused_variable_name = tree->right->left->value;
                optimiser_params->current_unused_variable_tree = tree->right;
                remove_unused_variables(optimiser_params->root_tree);
            }
            break;
        }
        case SYN_NODE_KEYWORD_IF: {
            if (optimise_type == OPTIMISE_EXPRESSION) {
                process_tree_using(tree->right, optimize_expression, POSTORDER);
            }
            if (optimise_type == OPTIMISE_UNREACHABLE_CODE) {
                optimise_unreachable_if(tree);
            }
        }
        case SYN_NODE_KEYWORD_WHILE: {
            if (optimise_type == OPTIMISE_UNREACHABLE_CODE) {
                optimise_unreachable_while(tree);
            }
        }
        case SYN_NODE_CALL: {
            if (optimise_type == OPTIMISE_EXPRESSION) {
                process_tree_using(tree->right, optimize_expression, POSTORDER);
                optimiser_params->current_replaced_variable_name = tree->right->left->value;
                optimiser_params->current_replaced_variable_tree = tree->right->right;
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
    optimize_node(tree, OPTIMISE_UNREACHABLE_CODE);
    optimize_node(tree, OPTIMISE_UNUSED_VARIABLES);
}

void init_optimiser() {
    optimiser_parameters_t *params = (optimiser_parameters_t *) malloc(sizeof(optimiser_parameters_t));

    params->root_tree = NULL;
    params->current_unused_variable_name = NULL;
    params->current_replaced_variable_tree = NULL;
    params->current_replaced_variable_name = NULL;
    params->current_replaced_variable_tree = NULL;

    optimiser_params = params;
}
