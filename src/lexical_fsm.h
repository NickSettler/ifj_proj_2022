#ifndef IFJ_PROJ_2022_LEXICAL_FSM_H
#define IFJ_PROJ_2022_LEXICAL_FSM_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "str.h"
#include "errors.h"

/**
 * PHP Lexical Analyzer Tokens
 */
typedef enum LEXICAL_FSM_TOKENS {
    // Special characters
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_SQUARE_BRACKET,
    RIGHT_SQUARE_BRACKET,
    LEFT_CURLY_BRACKET,
    RIGHT_CURLY_BRACKET,
    COMMA,
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

    // Assignment operator
    ASSIGN,

    // Keywords
    IF,
    ELSEIF,
    ELSE,
    WHILE,
    FOR,
    DO,
    RETURN,

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
    IDENTIFIER_START_STATE,
    IDENTIFIER_STATE,

    // Arithmetic states
    EQUAL_STATE,
    ARITHMETIC_STATE,

    // Data types states
    INTEGER_STATE,
    FLOAT_STATE,
    STRING_STATE,
    STRING_ESCAPE_STATE,
} LEXICAL_FSM_STATES;

LEXICAL_FSM_TOKENS get_next_token(string_t *token);

#endif //IFJ_PROJ_2022_LEXICAL_FSM_H
