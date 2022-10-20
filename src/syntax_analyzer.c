/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *
 * @file syntax_analyzer.c
 * @brief 
 * @date 17.10.2022
 */

#include "syntax_analyzer.h"

static FILE *fd;

syntax_abstract_tree_t *
make_node(syntax_ast_node_type type, syntax_abstract_tree_t *left, syntax_abstract_tree_t *right) {
    syntax_abstract_tree_t *tree = malloc(sizeof(syntax_abstract_tree_t));
    if (tree == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for syntax tree node");
    }

    tree->type = type;
    tree->left = left;
    tree->right = right;

    return tree;
}

syntax_abstract_tree_t *make_leaf(syntax_ast_node_type type, string_t *value) {
    syntax_abstract_tree_t *tree = malloc(sizeof(syntax_abstract_tree_t));
    if (tree == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for syntax tree node");
    }

    tree->type = type;
    tree->value = value;

    return tree;
}

syntax_abstract_tree_t *stmt() {
    syntax_abstract_tree_t *tree = NULL, *v;
    string_t *token_string = string_init("");

    switch (lexical_token->type) {
        case IDENTIFIER:
            v = make_leaf(SYN_IDENTIFIER, string_init(lexical_token->value));
            lexical_token = get_next_token(fd, lexical_token);
        default:
        SYNTAX_ERROR("Unexpected token: %s", lexical_token->value);
    }

    return tree;
}

syntax_abstract_tree_t *load_syntax_tree() {
    fd = test_lex_input("$a = 3;");

    string_t *token_string = string_init("");
    LEXICAL_FSM_TOKENS token_type = get_next_token(fd, token_string);

    lexical_token = malloc(sizeof(lexical_token_t));
    if (lexical_token == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for lexical token");
    }

    lexical_token->type = token_type;
    lexical_token->value = token_string->value;

    if (lexical_token->type == SEMICOLON) {
        return NULL;
    }

    printf("Current token: %s [%d]\n", lexical_token->value, lexical_token->type);

    syntax_ast_node_type type = get_token_type(lexical_token->type);
    if (lexical_token->type != END_OF_FILE) {
        return make_leaf(type, string_init(lexical_token->value));
    }

    syntax_abstract_tree_t *left = load_syntax_tree();
    syntax_abstract_tree_t *right = load_syntax_tree();
    return make_node(type, left, right);
}

syntax_ast_node_type get_token_type(LEXICAL_FSM_TOKENS token) {
    switch (token) {
        case IDENTIFIER:
            return SYN_IDENTIFIER;
        case STRING:
            return SYN_STRING;
        case INTEGER:
            return SYN_INTEGER;
        case FLOAT:
            return SYN_FLOAT;
        case ASSIGN:
            return SYN_ASSIGN;
        default:
            return -1;
    }
}
