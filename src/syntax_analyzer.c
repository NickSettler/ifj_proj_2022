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
    syntax_tree_token_type token_type;
    bool right_associative, is_binary, is_unary;
    int precedence;
    syntax_tree_node_type node_type;
} attributes[] = {
        {"EOF",     "End_of_file", SYN_TOKEN_EOF,        false, false, false, -1, (syntax_tree_node_type) -1},
        {"ID",      "Identifier",  SYN_TOKEN_IDENTIFIER, false, false, false, -1, SYN_NODE_IDENTIFIER},
        {"STRING",  "String",      SYN_TOKEN_STRING,     false, false, false, -1, SYN_NODE_STRING},
        {"INTEGER", "Integer",     SYN_TOKEN_INTEGER,    false, false, false, -1, SYN_NODE_INTEGER},
        {"FLOAT",   "Float",       SYN_TOKEN_FLOAT,      false, false, false, -1, SYN_NODE_FLOAT},
        {"+",       "Op_add",      SYN_TOKEN_ADD,        false, true,  false, 12, SYN_NODE_ADD},
        {"-",       "Op_sub",      SYN_TOKEN_SUB,        false, true,  false, 12, SYN_NODE_SUB},
        {"*",       "Op_multiply", SYN_TOKEN_MUL,        false, true,  false, 13, SYN_NODE_MUL},
        {"/",       "Op_divide",   SYN_TOKEN_DIV,        false, true,  false, 13, SYN_NODE_DIV},
        {"-",       "Op_negate",   SYN_TOKEN_NEGATE,     false, false, true,  14, SYN_NODE_NEGATE},
        {"=",       "Op_Assign",   SYN_TOKEN_ASSIGN,     false, false, false, -1, SYN_NODE_ASSIGN},
        {";",       "Semicolon",   SYN_TOKEN_SEMICOLON,  false, false, false, -1, (syntax_tree_node_type) -1}
};

syntax_abstract_tree_t *
make_node(syntax_tree_node_type type, syntax_abstract_tree_t *left, syntax_abstract_tree_t *right) {
    syntax_abstract_tree_t *tree = (syntax_abstract_tree_t *) malloc(sizeof(syntax_abstract_tree_t));
    if (tree == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for syntax tree node");
    }

    tree->type = type;
    tree->left = left;
    tree->right = right;

    return tree;
}

syntax_abstract_tree_t *make_leaf(syntax_tree_node_type type, string_t *value) {
    syntax_abstract_tree_t *tree = (syntax_abstract_tree_t *) malloc(sizeof(syntax_abstract_tree_t));
    if (tree == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for syntax tree node");
    }

    tree->type = type;
    tree->value = value;

    return tree;
}

int syntax_abstract_tree_height(syntax_abstract_tree_t *tree) {
    if (tree == NULL) {
        return 0;
    } else {
        int left_height = syntax_abstract_tree_height(tree->left);
        int right_height = syntax_abstract_tree_height(tree->right);

        return left_height > right_height ? left_height + 1 : right_height + 1;
    }
}

void syntax_abstract_tree_print(FILE *output, syntax_abstract_tree_t *tree) {
    int height = syntax_abstract_tree_height(tree);
    int i;
    for (i = 1; i <= height; i++) {
        syntax_abstract_tree_print_level(output, tree, i);
    }
}

void syntax_abstract_tree_print_level(FILE *output, syntax_abstract_tree_t *tree, int level) {
    if (tree == NULL) return;

    if (level == 1) {
        fprintf(output, "%d", tree->type);
    } else if (level > 1) {
        syntax_abstract_tree_print_level(output, tree->left, level - 1);
        syntax_abstract_tree_print_level(output, tree->right, level - 1);
    }
}

syntax_abstract_tree_t *expression(FILE *fd, int precedence) {
    syntax_abstract_tree_t *x = NULL, *node;

    switch (lexical_token->type) {
        case IDENTIFIER:
            x = make_leaf(SYN_NODE_IDENTIFIER, string_init(lexical_token->value));
            lexical_token = get_token(fd);
            break;
        case INTEGER:
            x = make_leaf(SYN_NODE_INTEGER, string_init(lexical_token->value));
            lexical_token = get_token(fd);
            break;
        default:
        SYNTAX_ERROR("Expected expression, got: %s", lexical_token->value);
    }


    while (attributes[get_token_type(lexical_token->type)].is_binary &&
           attributes[get_token_type(lexical_token->type)].precedence >= precedence) {
        syntax_tree_token_type op = get_token_type(lexical_token->type);

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
            v = make_leaf(SYN_NODE_IDENTIFIER, string_init(lexical_token->value));
            lexical_token = get_token(fd);
            if (lexical_token->type != ASSIGN) {
                SYNTAX_ERROR("Expected assignment");
            }
            lexical_token = get_token(fd);
            e = expression(fd, 0);
            tree = make_node(SYN_NODE_ASSIGN, v, e);
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
        tree = make_node(SYN_NODE_SEQUENCE, tree, stmt(fd));
    }

    return tree;
}

syntax_tree_token_type get_token_type(LEXICAL_FSM_TOKENS token) {
    switch (token) {
        case END_OF_FILE:
            return SYN_TOKEN_EOF;
        case IDENTIFIER:
            return SYN_TOKEN_IDENTIFIER;
        case STRING:
            return SYN_TOKEN_STRING;
        case INTEGER:
            return SYN_TOKEN_INTEGER;
        case FLOAT:
            return SYN_TOKEN_FLOAT;
        case PLUS:
            return SYN_TOKEN_ADD;
        case MINUS:
            return SYN_TOKEN_SUB;
        case MULTIPLY:
            return SYN_TOKEN_MUL;
        case DIVIDE:
            return SYN_TOKEN_DIV;
        case ASSIGN:
            return SYN_TOKEN_ASSIGN;
        case SEMICOLON:
            return SYN_TOKEN_SEMICOLON;
        default:
            return (syntax_tree_token_type) -1;
    }
}
