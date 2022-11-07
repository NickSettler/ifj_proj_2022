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
#include "errors.h"
#include "lexical_fsm.h"

/**
 * Syntax non-terminal symbols enumeration
 */
typedef enum {
    PROGRAM,
} syntax_non_terminal_t;

/**
 * Syntax token type enumeration
 */
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
    SYN_TOKEN_COMMA,
    SYN_TOKEN_KEYWORD_IF,
    SYN_TOKEN_KEYWORD_ELSE,
    SYN_TOKEN_LEFT_PARENTHESIS,
    SYN_TOKEN_RIGHT_PARENTHESIS,
    SYN_TOKEN_LEFT_CURLY_BRACKETS,
    SYN_TOKEN_RIGHT_CURLY_BRACKETS,
} syntax_tree_token_type;

/**
 * Syntax tree node enumeration
 */
typedef enum {
    SYN_NODE_IDENTIFIER,
    SYN_NODE_STRING,
    SYN_NODE_INTEGER,
    SYN_NODE_FLOAT,
    SYN_NODE_SEQUENCE,
    SYN_NODE_ARGS,
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
    SYN_NODE_CALL,
    SYN_NODE_ASSIGN,
    SYN_NODE_KEYWORD_IF,
    SYN_NODE_KEYWORD_WHILE,
} syntax_tree_node_type;

typedef struct syntax_abstract_tree syntax_abstract_tree_t;
/**
 * @struct syntax_ast_t
 * Syntax abstract tree structure
 *
 * @var syntax_ast_t::type
 * Type of the node
 *
 * @var syntax_ast_t::left
 * Left child node
 *
 * @var syntax_ast_t::right
 * Right child node
 *
 * @var syntax_ast_t::value
 * Value of the node
 */
struct syntax_abstract_tree {
    syntax_tree_node_type type;
    syntax_abstract_tree_t *left;
    syntax_abstract_tree_t *middle;
    syntax_abstract_tree_t *right;
    string_t *value;
};

static lexical_token_t *lexical_token;

/**
 * Makes a new syntax abstract tree node
 * @param type Type of the node
 * @param left Left child
 * @param right Right child
 * @return New syntax abstract tree node
 */
syntax_abstract_tree_t *
make_binary_node(syntax_tree_node_type type, syntax_abstract_tree_t *left, syntax_abstract_tree_t *right);

/**
 * Makes a new syntax abstract tree node without children
 * @param type Type of the node
 * @param value Value of the node
 * @return New syntax abstract tree node
 */
syntax_abstract_tree_t *make_binary_leaf(syntax_tree_node_type type, string_t *value);

/**
 * Prints the syntax abstract tree using the inorder traversal
 * @param output Output file stream
 * @param tree Syntax abstract tree
 */
void syntax_abstract_tree_print(FILE *output, syntax_abstract_tree_t *tree);

/**
 * Checks if the token matches the expected token, otherwise throws an error
 * @param msg Error message
 * @param type Expected token type
 */
void expect_token(const char *msg, syntax_tree_token_type type);

/**
 * Converts lexical analyzer token to syntax analyzer token
 * @param token Lexical analyzer token
 * @return Syntax analyzer token
 */
syntax_tree_token_type get_token_type(LEXICAL_FSM_TOKENS token);

/**
 * Parses expression in brackets
 * @param fd File descriptor
 * @return Syntax abstract tree
 */
syntax_abstract_tree_t *parenthesis_expression(FILE *fd);

/**
 * Parses expression
 * @param fd File descriptor
 * @param precedence Precedence
 * @return Syntax abstract tree
 */
syntax_abstract_tree_t *expression(FILE *fd, int precedence);

/**
 * Parses statement non-terminal
 * @param fd File descriptor
 * @return Syntax abstract tree
 */
syntax_abstract_tree_t *stmt(FILE *fd);

/**
 * Parses all tree from file stream
 * @param fd File descriptor
 * @return Syntax abstract tree
 */
syntax_abstract_tree_t *load_syntax_tree(FILE *fd);

/**
 * Goes through the syntax tree and checks nodes
 * @param tree Syntax abstract tree
 * @param check Check function
 * @return true is some node satisfies the check function, otherwise false
 */
bool check_tree_using(syntax_abstract_tree_t *tree, bool (*check)(syntax_abstract_tree_t *));

/**
 * Gets node using the check function
 * @param tree Syntax abstract tree
 * @param check Check function
 * @return node if this node satisfies the check function, otherwise NULL
 */
syntax_abstract_tree_t *get_from_tree_using(syntax_abstract_tree_t *tree, bool (*check)(syntax_abstract_tree_t *));

/**
 * Checks if AST contains defined variable
 * @param node Syntax abstract tree
 * @return True if the node contains defined variable, false otherwise
 */
bool is_defined(syntax_abstract_tree_t *tree);

/**
 * Checks if the AST contains undefined variable
 * @param tree Syntax abstract tree
 * @return True if the node contains undefined variable, false otherwise
 */
bool is_undefined(syntax_abstract_tree_t *tree);

#endif //IFJ_PROJ_SYNTAX_ANALYZER_H
