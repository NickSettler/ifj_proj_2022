#ifndef IFJ_PROJ_2022_LEXICAL_FSM_H
#define IFJ_PROJ_2022_LEXICAL_FSM_H


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/**
 * PHP Lexical Analyzer Tokens
 */
typedef enum LEXICAL_FSM_TOKENS {
    // special characters
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_SQUARE_BRACKET,
    RIGHT_SQUARE_BRACKET,
    LEFT_CURLY_BRACKET,
    RIGHT_CURLY_BRACKET,
    COMMA,
    SEMICOLON,

    // arithmetic operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,

    // relational operators
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    EQUAL,
    NOT_EQUAL,

    // assignment operator
    ASSIGN,

    // keywords
    IF,
    ELSEIF,
    ELSE,
    WHILE,
    FOR,
    DO,
    RETURN,

    // identifiers
    IDENTIFIER,

    // constants
    INTEGER,
    FLOAT,
    STRING,

    // end of file
    END_OF_FILE,

    // FSM states
    START,
    KEYWORD_STATE,  // before it becomes identifier/declaration
    IDENTIFIER_STATE,
    INTEGER_STATE,
    FLOAT_STATE,
    STRING_STATE,

    // end of tokens
    END_OF_TOKENS
} LEXICAL_FSM_TOKENS;

int get_next_token(char *lexeme);

#endif //IFJ_PROJ_2022_LEXICAL_FSM_H