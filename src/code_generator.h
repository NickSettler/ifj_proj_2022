/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *  xkalut00, Maksim Kalutski
 *
 * @file   code_generator.h
 * @brief  Code generator header file
 * @date   13.10.2022
 */

#ifndef IFJ_PROJ_2022_CODE_GENERATOR_H
#define IFJ_PROJ_2022_CODE_GENERATOR_H

#include <stdio.h>
#include "str.h"

/**
 * @brief file for writing the code
 */
FILE *fd;

/**
 * @brief structure with frames
 */
typedef enum {
    GF,
    LF,
    TF,
} frames_t;

/**
 * @brief array of frames
 */
static const char *frames[] = {
        "GF", "LF", "TF",
};

/**
 * @brief structure with instructions
 */
typedef enum {
    ADD,
    SUB,
    MUL,
    DIV,
    IDIV,
    ADDS,
    SUBS,
    MULS,
    DIVS,
    IDIVS,
    LT,
    GT,
    EQ,
    LTS,
    GTS,
    EQS,
    NOTLT,
    NOTGT,
    NOTEQ,
    AND,
    OR,
    NOT,
    ANDS,
    ORS,
    NOTS,
    INT2FLOATS,
    FLOAT2INTS,
    INT2CHARS,
    STRI2INTS,
    CONCAT,
    STRLEN,
    GETCHAR,
    SETCHAR,
} instructions_t;

/**
 * @brief array of instructions
 */
static const char *instructions[] = {
        "ADD", "SUB", "MUL", "DIV", "IDIV", "ADDS", "SUBS", "MULS", "DIVS", "IDIVS",
        "LT", "GT", "EQ", "LTS", "GTS", "EQS", "NOTLT", "NOTGT", "NOTEQ", "AND", "OR", "NOT",
        "ANDS", "ORS", "NOTS", "INT2FLOATS", "FLOAT2INTS", "INT2CHARS", "STRI2INTS",
        "CONCAT", "STRLEN", "GETCHAR", "SETCHAR",
};

/**
 * @brief generating a value assignment to a variable
 * @param *frame variable frame
 * @param *variable variable name
 * @param *symbol symbol name
 */
void generate_move(frames_t frame, char *variable, char *symbol);

/**
 * @brief generating a new temporary frame
 */
void generate_create_frame();

/**
 * @brief generating a temporary frame move to the frame stack
 */
void generate_push_frame();

/**
 * @brief generating a current frame move to the temporary
 */
void generate_pop_frame();

/**
 * @brief generating declaration of a new variable in the frame
 * @param *frame variable frame
 * @param *variable variable name
 */
void generate_declaration(frames_t frame, char *variable);

/**
 * @brief generating a jump on the label
 * @param *label specified label
 */
void generate_call(char *label);

/**
 * @brief placing the value on top of the data stack
 * @param *frame variable frame
 * @param *variable variable name
 */
void generate_add_on_top(frames_t frame, char *variable);

/**
 * @brief placing the value on top of the data stack
 * @param *frame variable frame
 * @param *variable variable name
 */
void generate_pop_from_top(frames_t frame, char *variable);

/**
 * @brief erases the data stack
 */
void generate_clear_stack();

/**
 * @brief function for choosing a float operation using a switch case
 *        ADD                        —  sums symbol1 and symbol2 and stores the result in result
 *        SUB                        —  subtracts symbol2 from symbol1 and stores the result in result
 *        MUL                        —  multiplies symbol1 and symbol2 and stores the result in result
 *        DIV                        —  divides symbol1 by symbol2 and stores the result in result
 *        IDIV                       —  divides symbol1 by symbol2 and stores the integer result in result
 *        ADDS/SUBS/MULS/DIVS/IDIVS  —  stack versions of ADD/SUB/MUL/DIV/IDIV
 *        LT                         —  compares for lowness symbol1 and symbol2 and stores the result in result
 *        GT                         —  compares for greatness symbol1 and symbol2 and stores the result in result
 *        EQ                         —  compares for equality symbol1 and symbol2 and stores the result in result
 *        LTS/GTS/EQS                —  stack versions of LT/GT/EQ
 *        NOTLT/NOTGT/NOTEQ          —  negation of LT/GT/EQ
 *        AND/OR/NOT                 —  logical AND/OR/NOT
 *        ANDS/ORS/NOTS              —  stack versions of AND/OR/NOT
 *        INT2FLOATS                 —  converts symbol1 to float and stores the result in result
 *        FLOAT2INTS                 —  converts symbol1 to integer and stores the result in result
 *        INT2CHARS                  —  converts symbol1 to char and stores the result in result
 *        STRI2INTS                  —  converts symbol1 to integer and stores the result in result
 *        CONCAT                     —  concatenates symbol1 and symbol2 and stores the result in result
 *        STRLEN                     —  gets the length of symbol1 and stores the result in result
 *        GETCHAR                    —  gets the character at position symbol2 from symbol1 and stores the result in result
 *        SETCHAR                    —  sets the character at position symbol2 from symbol1 to symbol3
 *
 * @param instruction instruction name
 * @param frame symbol frame
 * @param *result result
 * @param *symbol1 first symbol
 * @param *symbol2 second symbol
 */
void generate_operation(instructions_t instruction, frames_t frame, char *result, char *symbol1, char *symbol2);

/**
 * @brief generate an integer to decimal conversion
 */
void generate_int_to_float();

/**
 * @brief generate a decimal to integer conversion
 */
void generate_float_to_int();

/**
 * @brief generate a decimal to string conversion
 */
void generate_int_to_char();

/**
 * @brief generate a string to decimal conversion
 */
void generate_string_to_int();

/**
 * @brief read string from standard input
 */
void generate_reads();

/**
 * @brief read int from standard input
 */
void generate_readi();

/**
 * @brief read float from standard input
 */
void generate_readf();

/**
 * @brief print value to standard output
 */
void generate_write();

/**
 * @brief own substring function
 */
void generate_substr();

/**
 * @brief generate end of code
 */
void generate_end();

#endif //IFJ_PROJ_2022_CODE_GENERATOR_H
