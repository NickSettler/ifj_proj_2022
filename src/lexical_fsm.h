/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *   xpasyn00, Nikita Pasynkov
 *
 * @file lexical_fsm.h
 * @brief Lexical Analyser FSM
 * @date 05.10.2022
 */

#ifndef IFJ_PROJ_2022_LEXICAL_FSM_H
#define IFJ_PROJ_2022_LEXICAL_FSM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "str.h"
#include "errors.h"

#define LEXICAL_TOKEN_STACK_INITIAL_SIZE 1

/**
 * Lexical Analyzer Tokens
 */
typedef enum LEXICAL_FSM_TOKENS {
    // Special characters
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_CURLY_BRACKETS,
    RIGHT_CURLY_BRACKETS,
    LEFT_SQUARE_BRACKETS,
    RIGHT_SQUARE_BRACKETS,
    COMMA,
    COLON,
    SEMICOLON,

    // Special PHP brackets
    OPEN_PHP_BRACKET,
    CLOSE_PHP_BRACKET,

    // Arithmetic operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    PLUS_ASSIGN,
    MINUS_ASSIGN,
    MULTIPLY_ASSIGN,
    DIVIDE_ASSIGN,
    INCREMENT,
    DECREMENT,

    // Relational operators
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    EQUAL,
    NOT_EQUAL,
    TYPED_EQUAL,
    TYPED_NOT_EQUAL,

    // Logical operators
    LOGICAL_NOT,
    LOGICAL_AND,
    LOGICAL_OR,

    // Assignment operator
    ASSIGN,

    // Keywords
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FUNCTION,
    KEYWORD_RETURN,
    KEYWORD_NULL,
    KEYWORD_VOID,
    KEYWORD_INTEGER,
    KEYWORD_FLOAT,
    KEYWORD_STRING,
    KEYWORD_DECLARE,

    // Identifiers
    IDENTIFIER,

    // Data types
    INTEGER,
    FLOAT,
    STRING,

    // End Of File
    END_OF_FILE,
} LEXICAL_FSM_TOKENS;

typedef enum LEXICAL_FSM_STATES {
    // Basic states
    START,

    // Control states
    KEYWORD_STATE,
    IDENTIFIER_STATE,

    // PHP brackets state
    PHP_BRACKET_STATE,

    // Start of file declare state
    DECLARE_STATE,

    // Arithmetic states
    ARITHMETIC_STATE,
    EQUAL_STATE,
    LOGICAL_STATE,

    // Square parenthesis states
    SQUARE_PARENTHESIS_STATE,

    // Data types states
    INTEGER_STATE,
    FLOAT_STATE,
    STRING_STATE,
    STRING_ESCAPE_STATE,
} LEXICAL_FSM_STATES;

/**
 * @struct lexical_token_t
 * Lexical token structure
 *
 * @var lexical_token_t::type
 * Lexical token type
 *
 * @var lexical_token_t::value
 * Lexical token value
 */
typedef struct lexical_token {
    LEXICAL_FSM_TOKENS type;
    char *value;
} lexical_token_t;


/**
 * @struct lexical_token_stack_t
 * Lexical token stack structure
 *
 * @var lexical_token_stack_t::items
 * Lexical token stack items
 *
 * @var lexical_token_stack_t::top
 * Pointer to top of lexical token stack
 *
 * @var lexical_token_stack_t::capacity
 * Lexical token stack capacity
 */
typedef struct lexical_token_stack {
    lexical_token_t *items;
    int top;
    int capacity;
} lexical_token_stack_t;

/**
 * Get next lexical token type from file stream
 * @param fd file stream
 * @param token pointer to string to store token
 * @return token type
 */
LEXICAL_FSM_TOKENS get_next_token(FILE *fd, string_t *token);

/**
 * Generate file stream from text for lexical analysis
 * @param input text
 * @return pointer to file stream
 */
FILE *test_lex_input(char *input);

/**
 * Get next lexical token
 * @param fd file stream
 * @return pointer to lexical token
 */
lexical_token_t *get_token(FILE *fd);

/**
 * Initializes lexical token stack
 * @return pointer to lexical token stack
 */
lexical_token_stack_t *lexical_token_stack_init();

/**
 * Destroys lexical token stack
 * @param stack pointer to lexical token stack
 */
void lexical_token_stack_destroy(lexical_token_stack_t *stack);

/**
 * Increases lexical token stack capacity twice
 * @param stack
 */
void lexical_token_stack_increase(lexical_token_stack_t *stack);

/**
 * Checks if lexical token stack is empty
 * @param stack pointer to lexical token stack
 * @return true if stack is empty, false otherwise
 */
bool lexical_token_stack_empty(lexical_token_stack_t *stack);

/**
 * Pushes lexical token to stack
 * @param stack pointer to lexical token stack
 * @param token lexical token
 */
void lexical_token_stack_push(lexical_token_stack_t *stack, lexical_token_t token);

/**
 * Pops lexical token from stack
 * @param stack pointer to lexical token stack
 * @return lexical token
 */
lexical_token_t lexical_token_stack_pop(lexical_token_stack_t *stack);

/**
 * Returns lexical token stack size
 * @param stack pointer to lexical token stack
 * @return size of lexical token stack
 */
int lexical_token_stack_size(lexical_token_stack_t *stack);

#endif //IFJ_PROJ_2022_LEXICAL_FSM_H
