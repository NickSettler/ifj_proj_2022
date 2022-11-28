/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *   xpasyn00, Nikita Pasynkov
 *   xmaroc00, Elena Marochkina
 *
 * @file semantic_analyzer.c
 * @brief Semantic analyzer
 * @date 05.10.2022
 */

#include "semantic_analyzer.h"
#include "syntax_analyzer.h"

semantic_analyzer_t *semantic_state;
extern tree_node_t *symtable;

void semantic_tree_check(syntax_abstract_tree_t *tree) {
    if (tree == NULL)
        return;
    init_symtable();
    semantic_state = init_semantic_state();
    semantic_tree_check(tree->left);
    process_tree(tree->right);
}

semantic_analyzer_t *init_semantic_state() {
    semantic_analyzer_t *result = (semantic_analyzer_t *) malloc(sizeof(semantic_analyzer_t));
    if (result == 0) {
        INTERNAL_ERROR("Malloc for semantic analyzer failed");
    }
    result->FUNCTION_SCOPE = false;
    result->function_name = NULL;
    result->argument_count = 0;
    result->symtable_ptr = symtable;
    result->used_functions = (semantic_internal_functions) 0;
    return result;
}

semantic_analyzer_t *get_semantic_state() {
    return semantic_state;
}

void process_tree(syntax_abstract_tree_t *tree) {
    if (tree == NULL)
        return;
    switch (tree->type) {
        case SYN_NODE_SEQUENCE:
            process_tree(tree->left);
            process_tree(tree->right);
            break;
        case SYN_NODE_ASSIGN: {
            process_assign(tree);
            break;
        }
        case SYN_NODE_KEYWORD_IF:
        case SYN_NODE_KEYWORD_WHILE: {
            process_if_while(tree);
            break;
        }
        case SYN_NODE_FUNCTION_DECLARATION: {
            semantic_state->FUNCTION_SCOPE = true;
            process_function_declaration(tree);
            semantic_state->FUNCTION_SCOPE = false;
            semantic_state->symtable_ptr = symtable;
            break;
        }
        case SYN_NODE_CALL: {
            bool is_function_declared = check_tree_using(tree->left, is_defined);
            if (!is_function_declared) {
                SEMANTIC_FUNC_UNDEF_ERROR("Function is not declared")
            }
            char *temp_function_name = semantic_state->function_name;
            semantic_state->function_name = tree->left->value->value;
            process_call(tree);
            semantic_state->function_name = temp_function_name;
            break;
        }
        default:
            break;
    }
}

void process_assign(syntax_abstract_tree_t *tree) {
    syntax_abstract_tree_t *id_node = tree->left;
    check_tree_using(tree->right, check_defined);
    if (semantic_state->FUNCTION_SCOPE == false) {
        create_global_token(id_node);
    } else {
        create_local_token(id_node, semantic_state->function_name);
        semantic_state_ptr();
    }
    find_element(semantic_state->symtable_ptr, id_node->value->value)->type = get_data_type(tree->right);
    check_tree_for_float(tree->right);

}

void process_if_while(syntax_abstract_tree_t *tree) {
    check_tree_using(tree->left, check_defined);
    process_tree(tree->right);
    process_tree(tree->middle);
}

void process_function_declaration(syntax_abstract_tree_t *tree) {
    if (check_tree_using(tree->left, is_defined)) {
        SEMANTIC_FUNC_UNDEF_ERROR("Function is already declared");
    }

    syntax_abstract_tree_t *id_node = tree->left;
    semantic_state->function_name = id_node->value->value;
    semantic_state->argument_count = 0; // reset argument count

    insert_function(semantic_state->function_name);
    set_return_type(tree);
    find_element(semantic_state->symtable_ptr, semantic_state->function_name)->argument_count = count_arguments(
            tree->middle); // set argument count to function
    create_args_array();
    insert_arguments(tree->middle);

    semantic_state_ptr();
    if (tree->right != NULL) {
        process_tree(tree->right);
    }
    process_tree(tree->right);
    check_for_return_value(tree->right);
    semantic_state->symtable_ptr = symtable;
}

void check_for_return_value(syntax_abstract_tree_t *tree) {
    bool func_has_return_type = find_token(semantic_state->function_name)->type != TYPE_ALL;

    if (tree == NULL) {
        if (func_has_return_type) {
            SEMANTIC_FUNC_RET_ERROR("Function has no return");
        }
        return;
    }

    bool has_return = tree->right->type == SYN_NODE_KEYWORD_RETURN;
    bool type_match = find_token(semantic_state->function_name)->type & get_data_type(tree->right->right);

    if (has_return && !func_has_return_type) {
        data_type type_from_return_expression = get_data_type(tree->right->right);
        find_token(semantic_state->function_name)->type = type_from_return_expression;
    }

    if (!has_return && func_has_return_type) {
        if (find_token(semantic_state->function_name)->type != TYPE_VOID) {
            SEMANTIC_FUNC_RET_ERROR("Missing return value in function %s", semantic_state->function_name)
        }
    }

    if (has_return) {
        check_tree_using(tree->right->right, check_defined);
        if (!type_match) {
            SEMANTIC_FUNC_RET_ERROR("Wrong return value in function %s", semantic_state->function_name);
        }
    }
}

void process_call(syntax_abstract_tree_t *tree) {
    bool is_var = tree->left->value->value[0] == '$';
    tree_node_t *func = find_element(is_var ? semantic_state->symtable_ptr : symtable, semantic_state->function_name);
    data_type *arg_ptr = func->args_array;
    int counter = func->argument_count - 1;
    int arg_call_counter = count_arguments(tree->right);


    if (!strcmp(tree->left->value->value, "readi"))
        semantic_state->used_functions = (semantic_internal_functions) (semantic_state->used_functions |
                                                                        SEMANTIC_READI);
    else if (!strcmp(tree->left->value->value, "readf"))
        semantic_state->used_functions = (semantic_internal_functions) (semantic_state->used_functions |
                                                                        SEMANTIC_READF);
    else if (!strcmp(tree->left->value->value, "reads"))
        semantic_state->used_functions = (semantic_internal_functions) (semantic_state->used_functions |
                                                                        SEMANTIC_READS);
    else if (!strcmp(tree->left->value->value, "write"))
        semantic_state->used_functions = (semantic_internal_functions) (semantic_state->used_functions |
                                                                        SEMANTIC_WRITE);
    else if (!strcmp(tree->left->value->value, "strlen"))
        semantic_state->used_functions = (semantic_internal_functions) (semantic_state->used_functions |
                                                                        SEMANTIC_STRLEN);
    else if (!strcmp(tree->left->value->value, "ord"))
        semantic_state->used_functions = (semantic_internal_functions) (semantic_state->used_functions | SEMANTIC_ORD);
    else if (!strcmp(tree->left->value->value, "chr"))
        semantic_state->used_functions = (semantic_internal_functions) (semantic_state->used_functions | SEMANTIC_CHR);
    else if (!strcmp(tree->left->value->value, "substring"))
        semantic_state->used_functions = (semantic_internal_functions) (semantic_state->used_functions |
                                                                        SEMANTIC_SUBSTRING);
    else if (!strcmp(tree->left->value->value, "intval"))
        semantic_state->used_functions = (semantic_internal_functions) (semantic_state->used_functions |
                                                                        SEMANTIC_INTVAL);
    else if (!strcmp(tree->left->value->value, "floatval"))
        semantic_state->used_functions = (semantic_internal_functions) (semantic_state->used_functions |
                                                                        SEMANTIC_FLOATVAL);

    if (strcmp(semantic_state->function_name, "write")) {
        if (arg_call_counter != func->argument_count) {
            SEMANTIC_FUNC_ARG_ERROR("Wrong number of arguments");
        }
    }
    compare_arguments(tree->right, arg_ptr, counter);
}

void compare_arguments(syntax_abstract_tree_t *tree, data_type *arg_array_ptr, int counter) {
    if (tree == NULL)
        return;

    // only for write function call
    if (counter < 0) {
        get_data_type(tree->left);
        compare_arguments(tree->right, arg_array_ptr, counter);
        return;
    }

    if (arg_array_ptr[counter] == TYPE_ALL) {
        compare_arguments(tree->right, arg_array_ptr, counter - 1);
    } else if (arg_array_ptr[counter] != get_data_type(tree->left)) {
        SEMANTIC_FUNC_ARG_ERROR("Wrong type of argument with value %s", tree->left->value->value);
    }
    compare_arguments(tree->right, arg_array_ptr, counter - 1);
}

void semantic_state_ptr() {
    tree_node_t *function_node = find_token(semantic_state->function_name);
    semantic_state->symtable_ptr = function_node->function_tree;
}

bool check_defined(syntax_abstract_tree_t *tree) {
    if (tree == NULL)
        return true;
    if (tree->type == SYN_NODE_IDENTIFIER) {
        if (check_tree_using(tree, is_defined) == 0) {
            SEMANTIC_UNDEF_VAR_ERROR("Variable %s used before declaration", tree->value->value);
        } else {
            return true;
        }
    }

    return true;
}

bool is_defined(syntax_abstract_tree_t *tree) {
    if (tree->type == SYN_NODE_IDENTIFIER) {
        bool is_var = tree->value->value[0] == '$';
        tree_node_t *node = find_element(is_var ? semantic_state->symtable_ptr : symtable, tree->value->value);
        if (!node) return false;

        return node->defined == true;
    }

    return true;
}

data_type get_data_type(syntax_abstract_tree_t *tree) {
    if (tree == NULL)
        return (data_type) -1;

    switch (tree->type) {
        case SYN_NODE_CALL: {
            bool is_function_declared = check_tree_using(tree->left, is_defined);
            if (!is_function_declared) {
                SEMANTIC_FUNC_UNDEF_ERROR("Function is not declared");
            }
            semantic_state->function_name = tree->left->value->value;
            process_call(tree);
            return find_element(semantic_state->symtable_ptr, semantic_state->function_name)->type;
        }
        case SYN_NODE_IDENTIFIER:
            check_tree_using(tree, check_defined);
            return find_element(semantic_state->symtable_ptr, tree->value->value)->type;
        case SYN_NODE_INTEGER:
            return TYPE_INT;
        case SYN_NODE_FLOAT:
            return TYPE_FLOAT;
        case SYN_NODE_CONCAT:
            check_tree_for_string(tree);
        case SYN_NODE_STRING:
            return TYPE_STRING;
        case SYN_NODE_KEYWORD_NULL:
            return TYPE_NULL;
        case SYN_NODE_KEYWORD_VOID:
            return TYPE_VOID;
        case SYN_NODE_ADD:
        case SYN_NODE_SUB:
        case SYN_NODE_MUL:
        case SYN_NODE_EQUAL:
        case SYN_NODE_LESS:
        case SYN_NODE_GREATER:
        case SYN_NODE_LESS_EQUAL:
        case SYN_NODE_GREATER_EQUAL:
        case SYN_NODE_NOT_EQUAL:
        case SYN_NODE_NEGATE:
            check_tree_using(tree, check_defined);
            if (!check_tree_using(tree, is_only_numbers)) {
                SEMANTIC_TYPE_COMPAT_ERROR("Cannot use string in arithmetic expression")
            }
            return type_check(get_data_type(tree->left), get_data_type(tree->right));
        case SYN_NODE_DIV:
            check_tree_using(tree, check_defined);
            if (!check_tree_using(tree, is_only_numbers)) {
                SEMANTIC_TYPE_COMPAT_ERROR("Cannot use string in arithmetic expression")
            }
            return TYPE_FLOAT;
        default:
            break;
    }
    get_data_type(tree->left);
    get_data_type(tree->right);
}

void check_tree_for_float(syntax_abstract_tree_t *tree) {
    if (!check_tree_using(tree, is_node_an_int)) {
        process_tree_using(tree, replace_node_int_to_float, INORDER);
    }
}

void replace_node_int_to_float(syntax_abstract_tree_t *tree) {
    if (tree == NULL) {
        return;
    }
    switch (tree->type) {
        case SYN_NODE_INTEGER:
            tree->type = SYN_NODE_FLOAT;
            string_append_string(tree->value, ".0");
            break;
        default:
            return;
    }
}

void check_tree_for_string(syntax_abstract_tree_t *tree) {
    if (!check_tree_using(tree, is_only_numbers)) {
        process_tree_using(tree, replace_node_to_string, POSTORDER);
    }
}

void replace_node_to_string(syntax_abstract_tree_t *tree) {
    if (tree == NULL) {
        return;
    }
    switch (tree->type) {
        case SYN_NODE_INTEGER:
        case SYN_NODE_FLOAT: {
            tree->type = SYN_NODE_STRING;
            string_t *temp = string_init("");
            string_append_string(temp, "\"%s\"", tree->value->value);
            string_replace(tree->value, temp->value);
            break;
        }
        default: {
            return;
        }
    }
}

data_type type_check(data_type type_1, data_type type_2) {
    return type_1 > type_2 ? type_1 : type_2;
}

bool is_node_an_int(syntax_abstract_tree_t *tree) {
    switch (tree->type) {
        case SYN_NODE_FLOAT:
        case SYN_NODE_DIV:
            return false;
        case SYN_NODE_IDENTIFIER:
            if (find_element(semantic_state->symtable_ptr, tree->value->value)->type == TYPE_FLOAT) {
                return false;
            }
            return true;
        default:
            return true;
    }
}

bool is_only_numbers(syntax_abstract_tree_t *tree) {
    if (tree->type == SYN_NODE_STRING ||
        (tree->type == SYN_NODE_IDENTIFIER &&
         find_element(semantic_state->symtable_ptr, tree->value->value)->type == TYPE_STRING)) {
        return false;
    }
    return true;
}

void insert_arguments(syntax_abstract_tree_t *tree) {
    if (tree == NULL)
        return;

    switch (tree->left->type) {
        case SYN_NODE_IDENTIFIER: {
            semantic_state->argument_count++;
            if (find_token(semantic_state->function_name) == NULL) {
                SEMANTIC_UNDEF_VAR_ERROR("Function %s used before declaration", semantic_state->function_name)
            }
            create_local_token(tree->left, semantic_state->function_name);
            char *arg_value = tree->left->value->value;
            tree_node_t *arg_node = find_token(semantic_state->function_name)->function_tree;
            tree_node_t *arg = find_element(arg_node, arg_value);
            arg->type =
                    tree->left->attrs->token_type == SYN_TOKEN_KEYWORD_INT ? TYPE_INT :
                    tree->left->attrs->token_type == SYN_TOKEN_KEYWORD_FLOAT ? TYPE_FLOAT :
                    tree->left->attrs->token_type == SYN_TOKEN_KEYWORD_STRING ? TYPE_STRING : TYPE_ALL;
            find_token(semantic_state->function_name)->argument_count = semantic_state->argument_count;
            arg->argument_type = arg->type;
            find_token(semantic_state->function_name)->argument_type = (data_type) (arg->type |
                                                                                    arg_node->argument_type);
            data_type *arg_ptr = find_element(semantic_state->symtable_ptr, semantic_state->function_name)->args_array;
            arg_ptr[semantic_state->argument_count - 1] = arg->type;
            break;
        }
        default:
            break;
    }
    insert_arguments(tree->right);
}

int count_arguments(syntax_abstract_tree_t *tree) {
    if (tree == NULL)
        return 0;
    return 1 + count_arguments(tree->right);
}

void create_args_array() {
    data_type *args = (data_type *) malloc(sizeof(data_type) * semantic_state->argument_count);
    find_element(semantic_state->symtable_ptr, semantic_state->function_name)->args_array = args;
}

void set_return_type(syntax_abstract_tree_t *tree) {
    if (tree == NULL) {
        return;
    }
    switch (tree->attrs->token_type) {
        case SYN_TOKEN_KEYWORD_INT:
            find_token(semantic_state->function_name)->type = TYPE_INT;
            break;
        case SYN_TOKEN_KEYWORD_FLOAT:
            find_token(semantic_state->function_name)->type = TYPE_FLOAT;
            break;
        case SYN_TOKEN_KEYWORD_STRING:
            find_token(semantic_state->function_name)->type = TYPE_STRING;
            break;
        case SYN_TOKEN_KEYWORD_VOID:
            find_token(semantic_state->function_name)->type = TYPE_VOID;
            break;
        case SYN_TOKEN_EOF:
            find_token(semantic_state->function_name)->type = TYPE_ALL;
            break;
        default:
            break;
    }
}
