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

#define GET_NEXT_TOKEN(fd) \
    free_lexical_token(lexical_token); \
    lexical_token = get_token(fd);
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
    SYN_TOKEN_NULL,
    SYN_TOKEN_STRING,
    SYN_TOKEN_INTEGER,
    SYN_TOKEN_FLOAT,
    SYN_TOKEN_ADD,
    SYN_TOKEN_SUB,
    SYN_TOKEN_MUL,
    SYN_TOKEN_DIV,
    SYN_TOKEN_NEGATE,
    SYN_TOKEN_CONCAT,
    SYN_TOKEN_NOT,
    SYN_TOKEN_OR,
    SYN_TOKEN_AND,
    SYN_TOKEN_LESS,
    SYN_TOKEN_LESS_EQUAL,
    SYN_TOKEN_GREATER,
    SYN_TOKEN_GREATER_EQUAL,
    SYN_TOKEN_EQUAL,
    SYN_TOKEN_NOT_EQUAL,
    SYN_TOKEN_TYPED_EQUAL,
    SYN_TOKEN_TYPED_NOT_EQUAL,
    SYN_TOKEN_ASSIGN,
    SYN_TOKEN_SEMICOLON,
    SYN_TOKEN_COLON,
    SYN_TOKEN_COMMA,
    SYN_TOKEN_KEYWORD_IF,
    SYN_TOKEN_KEYWORD_ELSE,
    SYN_TOKEN_KEYWORD_FUNCTION,
    SYN_TOKEN_KEYWORD_RETURN,
    SYN_TOKEN_KEYWORD_INT,
    SYN_TOKEN_KEYWORD_FLOAT,
    SYN_TOKEN_KEYWORD_STRING,
    SYN_TOKEN_KEYWORD_VOID,
    SYN_TOKEN_LEFT_PARENTHESIS,
    SYN_TOKEN_RIGHT_PARENTHESIS,
    SYN_TOKEN_LEFT_CURLY_BRACKETS,
    SYN_TOKEN_RIGHT_CURLY_BRACKETS,
    SYN_TOKEN_PHP_OPEN,
    SYN_TOKEN_PHP_CLOSE,
} syntax_tree_token_type;

/**
 * Syntax tree node enumeration
 */
typedef enum {
    SYN_NODE_IDENTIFIER = 1 << 0,
    SYN_NODE_STRING = 1 << 1,
    SYN_NODE_INTEGER = 1 << 2,
    SYN_NODE_FLOAT = 1 << 3,
    SYN_NODE_SEQUENCE = 1 << 4,
    SYN_NODE_ARGS = 1 << 5,
    SYN_NODE_ADD = 1 << 6,
    SYN_NODE_SUB = 1 << 7,
    SYN_NODE_MUL = 1 << 8,
    SYN_NODE_DIV = 1 << 9,
    SYN_NODE_NEGATE = 1 << 10,
    SYN_NODE_CONCAT = 1 << 11,
    SYN_NODE_NOT = 1 << 12,
    SYN_NODE_OR = 1 << 13,
    SYN_NODE_AND = 1 << 14,
    SYN_NODE_LESS = 1 << 15,
    SYN_NODE_LESS_EQUAL = 1 << 16,
    SYN_NODE_GREATER = 1 << 17,
    SYN_NODE_GREATER_EQUAL = 1 << 18,
    SYN_NODE_EQUAL = 1 << 19,
    SYN_NODE_NOT_EQUAL = 1 << 20,
    SYN_NODE_TYPED_EQUAL = 1 << 21,
    SYN_NODE_TYPED_NOT_EQUAL = 1 << 22,
    SYN_NODE_CALL = 1 << 23,
    SYN_NODE_ASSIGN = 1 << 24,
    SYN_NODE_KEYWORD_IF = 1 << 25,
    SYN_NODE_KEYWORD_WHILE = 1 << 26,
    SYN_NODE_KEYWORD_RETURN = 1 << 27,
    SYN_NODE_KEYWORD_NULL = 1 << 28,
    SYN_NODE_FUNCTION_DECLARATION = 1 << 29,
    SYN_NODE_FUNCTION_ARG = 1 << 30,
} syntax_tree_node_type;

typedef enum {
    PREORDER,
    INORDER,
    POSTORDER,
} syntax_tree_traversal_type;

typedef struct syntax_abstract_tree_attr syntax_abstract_tree_attr_t;

typedef struct syntax_abstract_tree syntax_abstract_tree_t;

struct syntax_abstract_tree_attr {
    syntax_tree_token_type token_type;
};
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
    syntax_abstract_tree_attr_t *attrs;
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
 * Parse function declaration arguments
 * @param fd File descriptor
 * @param args Syntax abstract tree with current/next argument
 * @return Syntax abstract tree with all arguments
 */
syntax_abstract_tree_t *f_args(FILE *fd, syntax_abstract_tree_t *args);

/**
 * Parse function declaration
 * @param fd File descriptor
 * @return Syntax abstract tree with function declaration
 */
syntax_abstract_tree_t *f_dec_stats(FILE *fd);

/**
 * Parses expression in brackets
 * @param fd File descriptor
 * @return Syntax abstract tree with expression in brackets
 */
syntax_abstract_tree_t *parenthesis_expression(FILE *fd);

/**
 * Parses expression
 * @param fd File descriptor
 * @param precedence Precedence
 * @return Syntax abstract tree with expression
 */
syntax_abstract_tree_t *expression(FILE *fd, int precedence);

/**
 * Parses function call arguments
 * @param fd File descriptor
 * @return Syntax abstract tree with function call arguments
 */
syntax_abstract_tree_t *args(FILE *fd);

/**
 * Parses statement non-terminal
 * @param fd File descriptor
 * @return Syntax abstract tree with statement
 */
syntax_abstract_tree_t *stmt(FILE *fd);

/**
 * Parses all tree from file stream
 * @param fd File descriptor
 * @return Syntax abstract tree with all tree
 */
syntax_abstract_tree_t *load_syntax_tree(FILE *fd);


/**
 * Prints the syntax abstract tree using the inorder traversal
 * @param output Output file stream
 * @param tree Syntax abstract tree
 */
void syntax_abstract_tree_print(FILE *output, syntax_abstract_tree_t *tree);

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

void *process_tree_using(syntax_abstract_tree_t *tree, void (*process)(syntax_abstract_tree_t *),
                         syntax_tree_traversal_type traversal_type);


/**
 * Checks if the AST contains simple expression. Such expression can be compiled using one variable
 * @param tree Syntax abstract tree
 * @return True if the node contains simple expression, false otherwise
 */
bool is_simple_expression(syntax_abstract_tree_t *tree);

void free_syntax_tree(syntax_abstract_tree_t *tree);

#endif //IFJ_PROJ_SYNTAX_ANALYZER_H
