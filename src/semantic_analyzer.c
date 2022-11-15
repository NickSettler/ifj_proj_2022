#include "semantic_analyzer.h"
#include "symtable.h"

void printInorder(syntax_abstract_tree_t *tree) {
    if (tree == NULL)
        return;

    switch (tree->type) {
        case SYN_NODE_IDENTIFIER:
            printf("%s\n ", tree->value->value);
            break;
        case SYN_NODE_INTEGER:
            printf("%s\n", tree->value->value);
            break;
        case SYN_NODE_ASSIGN:
            printf("Assign\n");
            break;
        case SYN_NODE_KEYWORD_IF:
            printf("If\n");
            break;
        case SYN_NODE_GREATER:
            printf("> \n");
            break;
        default:
            printf("Unknown node type\n");
    }
    printInorder(tree->left);
    printInorder(tree->middle);
    printInorder(tree->right);
}

void tree_traversal(syntax_abstract_tree_t *tree) {
    if (tree == NULL)
        return;
    tree_traversal(tree->left);
    switch (tree->right->type) {
        case SYN_NODE_ASSIGN: {
            syntax_abstract_tree_t *id_node = tree->right->left;
            create_global_id_node(id_node);
            find_token(id_node->value->value)->type = get_data_type(tree->right->right);
            break;
        }
        case SYN_NODE_KEYWORD_IF:
        case SYN_NODE_KEYWORD_WHILE:
            defined(tree->right->left);
            tree_traversal(tree->right->middle);
            if (tree->right->right != NULL) {
                tree_traversal(tree->right->right);
            }
            break;
        case SYN_NODE_FUNCTION_DECLARATION:
            // TODO: create local BST function
            break;
    }
}

//TODO: Add type casting for strings
int type_check(int type_1, int type_2) {
    return type_1 > type_2 ? type_1 : type_2;
}

int get_data_type(syntax_abstract_tree_t *tree) {
    if (tree == NULL) {
        return -1;
    }
    switch (tree->type) {
        case SYN_NODE_IDENTIFIER:
            defined(tree);
            return find_token(tree->value->value)->type;
        case SYN_NODE_INTEGER:
            return 0; //int
        case SYN_NODE_FLOAT:
            return 1; //float
        case SYN_NODE_CONCAT:
        case SYN_NODE_STRING:
            return 2; //string
        case SYN_NODE_ADD:
        case SYN_NODE_SUB:
        case SYN_NODE_MUL:
        case SYN_NODE_NEGATE:
            string_conversion(tree);
            return type_check(get_data_type(tree->left), get_data_type(tree->right));
        case SYN_NODE_DIV:
            return 1;
    }
    get_data_type(tree->left);
    get_data_type(tree->right);
}

void create_global_id_node(syntax_abstract_tree_t *tree) {
    insert_token(tree->value->value);
    find_token(tree->value->value)->defined = true;
    find_token(tree->value->value)->global = true;
}

void defined(syntax_abstract_tree_t *tree) {
    if (tree == NULL) {
        return;
    }
    switch (tree->type) {
        case SYN_NODE_IDENTIFIER:
            if (find_token(tree->value->value) == false) {
                SEMANTIC_UNDEF_VAR_ERROR("Variable %s used before declaration", tree->value->value);
            }
            break;
    }
    defined(tree->left);
    defined(tree->right);
}

bool is_str_an_int(string_t *str) {
    regex_t regex;
    int regex_comp = regcomp(&regex, "^[\"\'][0-9]+[\"\']$", REG_EXTENDED);
    int return_value = regexec(&regex, str->value, 0, NULL, 0);
    if (!return_value) {
        return true;
    } else {
        return false;
    }
}

bool is_str_a_float(string_t *str) {
    regex_t regex;
    int regex_comp = regcomp(&regex, "^[\"\'][0-9]+\\.?[0-9]+([eE][+-]?[0-9]+)?[\"\']$", REG_EXTENDED);
    int return_value = regexec(&regex, str->value, 0, NULL, 0);
    if (!return_value) {
        return true;
    } else {
        return false;
    }
}

int string_conversion(syntax_abstract_tree_t *tree) {
    if (tree->left->type == SYN_NODE_STRING ||
        (tree->left->type == SYN_NODE_IDENTIFIER && find_token(tree->left->value->value)->type == 2)) {
        if (is_str_an_int(tree->left->value)) {
            return 0;
        }
        if (is_str_a_float(tree->left->value)) {
            return 1;
        }

    } if (tree->right->type == SYN_NODE_STRING ||
               (tree->right->type == SYN_NODE_IDENTIFIER && find_token(tree->right->value->value)->type == 2)) {
        if (is_str_an_int(tree->right->value)) {
            return 0;
        }
        if (is_str_a_float(tree->right->value)) {
            return 1;
        }
    }
    SEMANTIC_TYPE_COMPAT_ERROR("Wrong type of operand in expression")
}