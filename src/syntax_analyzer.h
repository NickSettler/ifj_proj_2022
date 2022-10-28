/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *
 * @file syntax_analyzer.h
 * @brief 
 * @date 17.10.2022
 */

#ifndef IFJ_PROJ_SYNTAX_ANALYZER_H
#define IFJ_PROJ_SYNTAX_ANALYZER_H

#include <stdbool.h>
#include "lexical_fsm.h"

typedef enum {
    PROGRAM,
} syntax_non_terminal_t;

typedef enum {
    SYN_TOKEN_EOF,
    SYN_TOKEN_IDENTIFIER,
    SYN_TOKEN_STRING,
    SYN_TOKEN_INTEGER,
    SYN_TOKEN_FLOAT,
    SYN_TOKEN_ADD,
    SYN_TOKEN_SUB,
    SYN_TOKEN_MUL,
    SYN_TOKEN_DIV,
    SYN_TOKEN_NEGATE,
    SYN_TOKEN_LESS,
    SYN_TOKEN_LESS_EQUAL,
    SYN_TOKEN_GREATER,
    SYN_TOKEN_GREATER_EQUAL,
    SYN_TOKEN_EQUAL,
    SYN_TOKEN_NOT_EQUAL,
    SYN_TOKEN_ASSIGN,
    SYN_TOKEN_SEMICOLON,
    SYN_TOKEN_KEYWORD_IF,
    SYN_TOKEN_LEFT_PARENTHESIS,
    SYN_TOKEN_RIGHT_PARENTHESIS,
    SYN_TOKEN_LEFT_CURLY_BRACKETS,
    SYN_TOKEN_RIGHT_CURLY_BRACKETS,
} syntax_tree_token_type;

typedef enum {
    SYN_NODE_IDENTIFIER,
    SYN_NODE_STRING,
    SYN_NODE_INTEGER,
    SYN_NODE_FLOAT,
    SYN_NODE_SEQUENCE,
    SYN_NODE_ADD,
    SYN_NODE_SUB,
    SYN_NODE_MUL,
    SYN_NODE_DIV,
    SYN_NODE_NEGATE,
    SYN_NODE_LESS,
    SYN_NODE_LESS_EQUAL,
    SYN_NODE_GREATER,
    SYN_NODE_GREATER_EQUAL,
    SYN_NODE_EQUAL,
    SYN_NODE_NOT_EQUAL,
    SYN_NODE_ASSIGN,
    SYN_NODE_KEYWORD_IF
} syntax_tree_node_type;

typedef struct syntax_abstract_tree syntax_abstract_tree_t;
struct syntax_abstract_tree {
    syntax_tree_node_type type;
    syntax_abstract_tree_t *left;
    syntax_abstract_tree_t *right;
    string_t *value;
} syntax_ast_t;

static lexical_token_t *lexical_token;

syntax_abstract_tree_t *
make_node(syntax_tree_node_type type, syntax_abstract_tree_t *left, syntax_abstract_tree_t *right);

syntax_abstract_tree_t *make_leaf(syntax_tree_node_type type, string_t *value);

void syntax_abstract_tree_print(FILE *output, syntax_abstract_tree_t *tree);

void expect_token(const char *msg, syntax_tree_token_type type);

syntax_tree_token_type get_token_type(LEXICAL_FSM_TOKENS token);

syntax_abstract_tree_t *parenthesis_expression(FILE *fd);

syntax_abstract_tree_t *expression(FILE *fd, int precedence);

syntax_abstract_tree_t *stmt(FILE *fd);

syntax_abstract_tree_t *load_syntax_tree(FILE *fd);

#endif //IFJ_PROJ_SYNTAX_ANALYZER_H
