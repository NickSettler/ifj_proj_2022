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
        {"EOF",     "End_of_file", SYN_EOF,        false, false, false, -1, (syntax_ast_node_type) -1},
        {"ID",      "Identifier",  SYN_IDENTIFIER, false, false, false, -1, SYN_IDENTIFIER},
        {"STRING",  "String",      SYN_STRING,     false, false, false, -1, SYN_STRING},
        {"INTEGER", "Integer",     SYN_INTEGER,    false, false, false, -1, SYN_INTEGER},
        {"FLOAT",   "Float",       SYN_FLOAT,      false, false, false, -1, SYN_FLOAT},
        {"+",       "Op_add",      SYN_ADD,        false, true,  false, 12, SYN_ADD},
        {"-",       "Op_sub",      SYN_SUB,        false, true,  false, 12, SYN_SUB},
        {"*",       "Op_multiply", SYN_MUL,        false, true,  false, 13, SYN_MUL},
        {"/",       "Op_divide",   SYN_DIV,        false, true,  false, 13, SYN_DIV},
        {"-",       "Op_negate",   SYN_NEGATE,     false, false, true,  14, SYN_NEGATE},
        {"=",       "Op_Assign",   SYN_ASSIGN,     false, false, false, -1, SYN_ASSIGN},
        {";",       "Semicolon",   SYN_SEMICOLON,  false, false, false, -1, (syntax_ast_node_type) -1}
};

syntax_abstract_tree_t *
make_node(syntax_ast_node_type type, syntax_abstract_tree_t *left, syntax_abstract_tree_t *right) {
    syntax_abstract_tree_t *tree = (syntax_abstract_tree_t *) malloc(sizeof(syntax_abstract_tree_t));
    if (tree == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for syntax tree node");
    }

    tree->type = type;
    tree->left = left;
    tree->right = right;

    return tree;
}

syntax_abstract_tree_t *make_leaf(syntax_ast_node_type type, string_t *value) {
    syntax_abstract_tree_t *tree = (syntax_abstract_tree_t *) malloc(sizeof(syntax_abstract_tree_t));
    if (tree == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for syntax tree node");
    }

    tree->type = type;
    tree->value = value;

    return tree;
}

syntax_abstract_tree_t *expression(FILE *fd, int precedence) {
    syntax_abstract_tree_t *x = NULL, *node;
    syntax_ast_node_type op;

    switch (lexical_token->type) {
        case INTEGER:
            x = make_leaf(SYN_INTEGER, string_init(lexical_token->value));
            lexical_token = get_token(fd);
            break;
        default:
        SYNTAX_ERROR("Expected expression, got: %s", lexical_token->value);
    }


    while (attributes[get_token_type(lexical_token->type)].is_binary &&
           attributes[get_token_type(lexical_token->type)].precedence >= precedence) {
        syntax_ast_node_type op = get_token_type(lexical_token->type);

        lexical_token = get_token(fd);

        int q = attributes[op].precedence;
        if (!attributes[op].right_associative) {
            q++;
        }

        node = expression(fd, q);
        x = make_node(attributes[op].node_type, x, node);
    }

    return x;
}

syntax_abstract_tree_t *stmt(FILE *fd) {
    syntax_abstract_tree_t *tree = NULL, *v, *e;

    switch (lexical_token->type) {
        case IDENTIFIER:
            v = make_leaf(SYN_IDENTIFIER, string_init(lexical_token->value));
            lexical_token = get_token(fd);
            if (lexical_token->type != ASSIGN) {
                SYNTAX_ERROR("Expected assignment");
            }
            lexical_token = get_token(fd);
            e = expression(fd, 0);
            tree = make_node(SYN_ASSIGN, v, e);
            if (lexical_token->type != SEMICOLON) {
                SYNTAX_ERROR("Expected semicolon");
            }
            lexical_token = get_token(fd);
            break;
        case INTEGER:
            tree = expression(fd, 0);
            if (lexical_token->type != SEMICOLON) {
                SYNTAX_ERROR("Expected semicolon");
            }
            lexical_token = get_token(fd);
            break;
        case END_OF_FILE:
            break;
        default:
        SYNTAX_ERROR("Expected expression, got: %s", lexical_token->value);
    }

    return tree;
}

syntax_abstract_tree_t *load_syntax_tree(FILE *fd) {
    lexical_token = get_token(fd);

    syntax_abstract_tree_t *tree = NULL;

    while (lexical_token->type != END_OF_FILE) {
        tree = make_node(SYN_SEQUENCE, tree, stmt(fd));
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
        case MULTIPLY:
            return SYN_MUL;
        case DIVIDE:
            return SYN_DIV;
        case ASSIGN:
            return SYN_ASSIGN;
        case SEMICOLON:
            return SYN_SEMICOLON;
        default:
            return (syntax_ast_node_type) -1;
    }
}
