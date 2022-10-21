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
#include "lexical_token.h"

typedef enum {
    PROGRAM,
} syntax_non_terminal_t;

typedef enum {
    // Token types
    SYN_EOF,
    SYN_IDENTIFIER,
    SYN_STRING,
    SYN_INTEGER,
    SYN_FLOAT,
    SYN_ADD,
    SYN_SUB,
    SYN_NEGATE,
    SYN_ASSIGN,
    SYN_SEMICOLON,

    // Node types
    SYN_SEQUENCE,
} syntax_ast_node_type;

typedef struct syntax_abstract_tree syntax_abstract_tree_t;
struct syntax_abstract_tree {
    syntax_ast_node_type type;
    syntax_abstract_tree_t *left;
    syntax_abstract_tree_t *right;
    string_t *value;
} syntax_ast_t;

static lexical_token_t *lexical_token;

syntax_abstract_tree_t *
make_node(syntax_ast_node_type type, syntax_abstract_tree_t *left, syntax_abstract_tree_t *right);

syntax_abstract_tree_t *make_leaf(syntax_ast_node_type type, string_t *value);

syntax_ast_node_type get_token_type(LEXICAL_FSM_TOKENS token);

syntax_abstract_tree_t *expression(int precedence);

syntax_abstract_tree_t *stmt();

syntax_abstract_tree_t *load_syntax_tree();

#endif //IFJ_PROJ_SYNTAX_ANALYZER_H
