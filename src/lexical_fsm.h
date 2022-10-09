#ifndef IFJ_PROJ_2022_LEXICAL_FSM_H
#define IFJ_PROJ_2022_LEXICAL_FSM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "str.h"
#include "errors.h"

/**
 * PHP Lexical Analyzer Tokens
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

LEXICAL_FSM_TOKENS get_next_token(FILE *fd, string_t *token);

#endif //IFJ_PROJ_2022_LEXICAL_FSM_H
