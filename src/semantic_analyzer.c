#include "semantic_analyzer.h"
#include "syntax_analyzer.h"


void semantic_tree_check(syntax_abstract_tree_t *tree) {
    if (tree == NULL)
        return;
    init_symtable();
    process_tree_using(tree, process_tree, INORDER);
}

void process_tree(syntax_abstract_tree_t *tree) {
    if (tree == NULL)
        return;
    if (tree->type != SYN_NODE_SEQUENCE)
        return;
    switch (tree->right->type) {
        case SYN_NODE_ASSIGN: {
            process_assign(tree->right);
            break;
        }
        case SYN_NODE_KEYWORD_IF:
        case SYN_NODE_KEYWORD_WHILE: {
            process_if_while(tree->right);
            break;
        }
        default:
            break;
    }
}

void process_assign(syntax_abstract_tree_t *tree) {
    check_tree_using(tree->right, check_defined);
    syntax_abstract_tree_t *id_node = tree->left;
    create_global_token(id_node);
    check_tree_for_float(tree->right);
    find_token(id_node->value->value)->type = get_data_type(tree->right);
}

void process_if_while(syntax_abstract_tree_t *tree) {
    check_tree_using(tree->left, check_defined);
    process_tree(tree->right);
    process_tree(tree->middle);
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

data_type get_data_type(syntax_abstract_tree_t *tree) {
    if (tree == NULL)
        return (data_type) -1;

    switch (tree->type) {
        case SYN_NODE_IDENTIFIER:
            check_tree_using(tree, check_defined);;
            return find_token(tree->value->value)->type;
        case SYN_NODE_INTEGER:
            return TYPE_INT;
        case SYN_NODE_FLOAT:
            return TYPE_FLOAT;
        case SYN_NODE_CONCAT:
            check_tree_for_string(tree);
        case SYN_NODE_STRING:
            return TYPE_STRING;
        case SYN_NODE_ADD:
        case SYN_NODE_SUB:
        case SYN_NODE_MUL:
        case SYN_NODE_NEGATE:
            if (!check_tree_using(tree, is_only_numbers)) {
                SEMANTIC_TYPE_COMPAT_ERROR("Cannot use string in arithmetic expression")
            }
            return type_check(get_data_type(tree->left), get_data_type(tree->right));
        case SYN_NODE_DIV:
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
            if (find_token(tree->value->value)->type == TYPE_FLOAT) {
                return false;
            } else {
                return true;
            }
        default:
            return true;
    }
}

bool is_only_numbers(syntax_abstract_tree_t *tree) {
    if (tree->type == SYN_NODE_STRING ||
        (tree->type == SYN_NODE_IDENTIFIER && find_token(tree->value->value)->type == TYPE_STRING)) {
        return false;
    }
    return true;
}