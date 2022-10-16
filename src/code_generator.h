/**
 * Imperative IFJ22 language compiler implementation
 * @author xkalut00, Maksim Kalutski
 * @file   code_generator.h
 * @brief  Code generator header file
 * @date   ??.10.2022
 */

#ifndef IFJ_PROJ_2022_CODE_GENERATOR_H
#define IFJ_PROJ_2022_CODE_GENERATOR_H

#include <stdio.h>

/**
 * @brief file for writing the code
 */
FILE *fd;

/**
 * @brief structure with instructions for switch cases
 */
typedef enum {
    ADD,
    SUB,
    MUL,
    IMUL,
    DIV,
    IDIV,
    LT,
    GT,
    EQ,
    NOTLT,
    NOTGT,
    NOTEQ,
    AND,
    OR,
    NOT,
    CONCAT,
    STRLEN,
    GETCHAR,
    SETCHAR,
} instructions_t;

/**
 * @brief structure with stack instructions for switch cases
 */
typedef enum {
    ADDS,
    SUBS,
    MULS,
    DIVS,
    IDIVS,
    ANDS,
    ORS,
    NOTS,
} stack_instructions_t;

/**
 * @brief structure with stack conversion operations for switch cases
 */
typedef enum {
    INT2FLOATS,
    FLOAT2INTS,
    INT2CHARS,
    STRI2INTS,
} stack_conversion_t;

/**
 * @brief generating a value assignment to a variable
 * @param *variable_frame variable frame
 * @param *variable variable name
 * @param *symbol_frame symbol frame
 * @param *symbol symbol name
 */
void generate_move(char *variable_frame, char *variable, char *symbol_frame, char *symbol);

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
 * @param *variable_frame variable frame
 * @param *variable variable name
 */
void generate_declaration(char *variable_frame, char *variable);

/**
 * @brief generating a jump on the label
 * @param *label specified label
 */
void generate_call(char *label);

/**
 * @brief placing the value on top of the data stack
 * @param *type variable type
 * @param *variable variable name
 */
void generate_add_on_top(char *type, char *variable);

/**
 * @brief placing the value on top of the data stack
 * @param *type variable type
 * @param *variable variable name
 */
void generate_pop_from_top(char *variable_frame, char *variable);

/**
 * @brief erases the data stack
 */
void generate_clear_stack();

/**
 * @brief function for choosing a float operation using a switch case
 *        ADD - sums symbol1 and symbol2 and stores the result in result
 *        SUB - subtracts symbol2 from symbol1 and stores the result in result
 *        MUL - multiplies symbol1 and symbol2 and stores the result in result
 *        DIV - divides symbol1 by symbol2 and stores the result in result
 *        LT  - compares for lowness symbol1 and symbol2 and stores the result in result
 *        GT  - compares for greatness symbol1 and symbol2 and stores the result in result
 *        EQ  - compares for equality symbol1 and symbol2 and stores the result in result
 *        NOTLT/NOTGT/NOTEQ - negation of LT/GT/EQ
 * @param *operation float operation
 * @param *result result
 * @param *symbol1 first symbol
 * @param *symbol2 second symbol
 */
void generate_operation(instructions_t instruction, char *result, char *symbol1, char *symbol2);

/**
 * @brief function for choosing an arithmetic operation using a switch case
 *        ADDS/SUBS/MULS/DIVS/IDIVS are stack versions of ADD/SUB/MUL/DIV/IDIV
 * @param *operation stack operation
 * @param *result result
 * @param *symbol1 first symbol
 * @param *symbol2 second symbol
 */
void generate_stack_operation(stack_instructions_t instruction, char *result, char *symbol1, char *symbol2);

/**
 * @brief function for choosing a stack relational operation using a switch case
 *        CONCAT - concatenates symbol1 and symbol2 and stores the result in result
 *        STRLEN - gets the length of symbol1 and stores the result in result
 *        GETCHAR - gets the character at position symbol2 from symbol1 and stores the result in result
 *        SETCHAR - sets the character at position symbol2 from symbol1 to symbol3
 *        LT  - compares for lowness symbol1 and symbol2 and stores the result in result
 *        GT  - compares for greatness symbol1 and symbol2 and stores the result in result
 *        EQ  - compares for equality symbol1 and symbol2 and stores the result in result
 *        NOTLT/NOTGT/NOTEQ - negation of LT/GT/EQ
 * @param *operation stack relational operation
 * @param *result result
 * @param *symbol1 first symbol
 * @param *symbol2 second symbol
 * @param *symbol3 third symbol
 */
void generate_string_relational_operation(instructions_t instruction, char *result, char *symbol1, char *symbol2);

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
 * @brief function for choosing an conversion operation using a switch case
 */
void generate_conversion_operation(stack_conversion_t operation);

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
