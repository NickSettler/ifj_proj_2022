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

struct {
    char *text, *enum_text;
    syntax_ast_node_type token_type;
    bool right_associative, is_binary, is_unary;
    int precedence;
    syntax_ast_node_type node_type;
} attributes[] = {
        {"EOF",     "End_of_file", SYN_EOF,        false, false, false, -1, -1},
        {"ID",      "Identifier",  SYN_IDENTIFIER, false, false, false, -1, SYN_IDENTIFIER},
        {"STRING",  "String",      SYN_STRING,     false, false, false, -1, SYN_STRING},
        {"INTEGER", "Integer",     SYN_INTEGER,    false, false, false, -1, SYN_INTEGER},
        {"FLOAT",   "Float",       SYN_FLOAT,      false, false, false, -1, SYN_FLOAT},
        {"+",       "Op_add",      SYN_ADD,        false, true,  false, 12, SYN_ADD},
        {"-",       "Op_sub",      SYN_SUB,        false, true,  false, 12, SYN_SUB},
        {"-",       "Op_negate",   SYN_NEGATE,     false, false, true,  14, SYN_NEGATE},
        {"=",       "Op_Assign",   SYN_ASSIGN,     false, false, false, -1, SYN_ASSIGN},
        {";",       "Semicolon",   SYN_SEMICOLON,  false, false, false, -1, -1}
};


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

syntax_abstract_tree_t *expression(int precedence) {
    syntax_abstract_tree_t *x = NULL, *node;
    syntax_ast_node_type op;

    string_t *token_string = string_init("");
    LEXICAL_FSM_TOKENS token_type;

    switch (lexical_token->type) {
        case INTEGER:
            x = make_leaf(SYN_INTEGER, string_init(lexical_token->value));
            token_type = get_next_token(fd, token_string);
            lexical_token->type = token_type;
            lexical_token->value = token_string->value;
            break;
        default:
        SYNTAX_ERROR("Expected expression, got: %s", lexical_token->value);
    }

    syntax_ast_node_type lt_type = get_token_type(lexical_token->type);

    while (attributes[lt_type].is_binary && attributes[lt_type].precedence >= precedence) {
        syntax_ast_node_type op = get_token_type(lexical_token->type);

        token_type = get_next_token(fd, token_string);
        lexical_token->type = token_type;
        lexical_token->value = token_string->value;

        int q = attributes[op].precedence;
        if (!attributes[op].right_associative) {
            q++;
        }

        node = expression(q);
        x = make_node(attributes[op].node_type, x, node);
    }

    return x;
}

syntax_abstract_tree_t *stmt() {
    syntax_abstract_tree_t *tree = NULL, *v, *e;
    string_t *token_string = string_init("");
    LEXICAL_FSM_TOKENS token_type;

    switch (lexical_token->type) {
        case IDENTIFIER:
            v = make_leaf(SYN_IDENTIFIER, string_init(lexical_token->value));
            token_type = get_next_token(fd, token_string);
            lexical_token->type = token_type;
            lexical_token->value = token_string->value;
            if (lexical_token->type != ASSIGN) {
                SYNTAX_ERROR("Expected assignment");
            }
            token_type = get_next_token(fd, token_string);
            lexical_token->type = token_type;
            lexical_token->value = token_string->value;
            e = expression(0);
            tree = make_node(SYN_ASSIGN, v, e);
            if (lexical_token->type != SEMICOLON) {
                SYNTAX_ERROR("Expected semicolon");
            }
            token_type = get_next_token(fd, token_string);
            lexical_token->type = token_type;
            lexical_token->value = token_string->value;
            break;
        case END_OF_FILE:
            break;
        default:
        SYNTAX_ERROR("Expected expression, got: %s", lexical_token->value);
    }

    return tree;
}

syntax_abstract_tree_t *load_syntax_tree() {
    fd = test_lex_input("$a = 4;");

    string_t *token_string = string_init("");
    LEXICAL_FSM_TOKENS token_type = get_next_token(fd, token_string);

    lexical_token = malloc(sizeof(lexical_token_t));
    if (lexical_token == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for lexical token");
    }

    lexical_token->type = token_type;
    lexical_token->value = token_string->value;

    syntax_abstract_tree_t *tree = NULL;

    while (lexical_token->type != END_OF_FILE) {
        tree = make_node(SYN_SEQUENCE, tree, stmt());
    }

    return tree;
}

syntax_ast_node_type get_token_type(LEXICAL_FSM_TOKENS token) {
    switch (token) {
        case END_OF_FILE:
            return SYN_EOF;
        case IDENTIFIER:
            return SYN_IDENTIFIER;
        case STRING:
            return SYN_STRING;
        case INTEGER:
            return SYN_INTEGER;
        case FLOAT:
            return SYN_FLOAT;
        case PLUS:
            return SYN_ADD;
        case MINUS:
            return SYN_SUB;
        case ASSIGN:
            return SYN_ASSIGN;
        case SEMICOLON:
            return SYN_SEMICOLON;
        default:
            return -1;
    }
}
