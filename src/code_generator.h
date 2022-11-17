/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *  xkalut00, Maksim Kalutski
 *  xmoise01, Nikita Moiseev
 *
 * @file   code_generator.h
 * @brief  Code generator header file
 * @date   13.10.2022
 */

#ifndef IFJ_PROJ_2022_CODE_GENERATOR_H
#define IFJ_PROJ_2022_CODE_GENERATOR_H

#include <stdio.h>
#include "str.h"
#include "syntax_analyzer.h"

/**
 * @brief file for writing the code
 */
FILE *fd;

int tmp_var_counter;

/**
 * @brief structure with frames
 */
typedef enum {
    CODE_GENERATOR_INT_CONSTANT,
    CODE_GENERATOR_FLOAT_CONSTANT,
    CODE_GENERATOR_STRING_CONSTANT,
    CODE_GENERATOR_GLOBAL_FRAME,
    CODE_GENERATOR_LOCAL_FRAME,
    CODE_GENERATOR_TEMPORARY_FRAME,
} frames_t;

/**
 * @brief array of frames
 */
static const char *frames[] = {
        "int", "float", "string", "GF", "LF", "TF",
};

/**
 * @brief structure with instructions
 */
typedef enum {
    CODE_GEN_ADD_INSTRUCTION,
    CODE_GEN_SUB_INSTRUCTION,
    CODE_GEN_MUL_INSTRUCTION,
    CODE_GEN_DIV_INSTRUCTION,
    CODE_GEN_IDIV_INSTRUCTION,
    CODE_GEN_ADDS_INSTRUCTION,
    CODE_GEN_SUBS_INSTRUCTION,
    CODE_GEN_MULS_INSTRUCTION,
    CODE_GEN_DIVS_INSTRUCTION,
    CODE_GEN_IDIVS_INSTRUCTION,
    CODE_GEN_LT_INSTRUCTION,
    CODE_GEN_GT_INSTRUCTION,
    CODE_GEN_EQ_INSTRUCTION,
    CODE_GEN_LTS_INSTRUCTION,
    CODE_GEN_GTS_INSTRUCTION,
    CODE_GEN_EQS_INSTRUCTION,
    CODE_GEN_NOTLT_INSTRUCTION,
    CODE_GEN_NOTGT_INSTRUCTION,
    CODE_GEN_NOTEQ_INSTRUCTION,
    CODE_GEN_AND_INSTRUCTION,
    CODE_GEN_OR_INSTRUCTION,
    CODE_GEN_NOT_INSTRUCTION,
    CODE_GEN_ANDS_INSTRUCTION,
    CODE_GEN_ORS_INSTRUCTION,
    CODE_GEN_NOTS_INSTRUCTION,
    CODE_GEN_INT2FLOAT_INSTRUCTION,
    CODE_GEN_FLOAT2INT_INSTRUCTION,
    CODE_GEN_INT2CHAR_INSTRUCTION,
    CODE_GEN_STRI2INT_INSTRUCTION,
    CODE_GEN_CONCAT_INSTRUCTION,
    CODE_GEN_STRLEN_INSTRUCTION,
    CODE_GEN_GETCHAR_INSTRUCTION,
    CODE_GEN_SETCHAR_INSTRUCTION,
} instructions_t;

/**
 * @brief array of instructions
 */
static const char *instructions[] = {
        "ADD", "SUB", "MUL", "DIV", "IDIV", "ADDS", "SUBS", "MULS", "DIVS", "IDIVS",
        "LT", "GT", "EQ", "LTS", "GTS", "EQS", "NOTLT", "NOTGT", "NOTEQ", "AND", "OR", "NOT",
        "ANDS", "ORS", "NOTS", "INT2FLOAT", "FLOAT2INT", "INT2CHAR", "STRI2INT", "CONCAT", "STRLEN", "GETCHAR",
        "SETCHAR",
};

/**
 * Generates move instruction
 * @param variable_frame frame of variable
 * @param variable variable
 * @param symbol_frame frame of symbol
 * @param symbol symbol
 */
void generate_move(frames_t variable_frame, char *variable, frames_t symbol_frame, char *symbol);

/**
 * Generates label
 * @param label
 */
void generate_label(char *label);

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
 * @param *frame variable frame
 */
void generate_clear_stack(frames_t frame);

/**
 * Generates getting type instruction
 * @param variable_frame variable frame
 * @param variable variable name
 * @param symbol_frame symbol frame
 * @param symbol symbol name
 */
void generate_type(frames_t variable_frame, char *variable, frames_t symbol_frame, char *symbol);

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
void generate_operation(instructions_t instruction, frames_t result_frame, char *result, frames_t symbol1_frame,
                        char *symbol1, frames_t symbol2_frame, char *symbol2);

void generate_jump(char *label);

void
generate_conditional_jump(bool is_equal, char *label, frames_t frame, char *symbol1, frames_t frame2, char *symbol2);

void generate_header();

/**
 * @brief generate an integer to decimal conversion
 * @param *frame variable frame
 */
void generate_int_to_float(frames_t frame);

/**
 * @brief generate a decimal to integer conversion
 * @param frame symbol frame
 */
void generate_float_to_int(frames_t frame);

/**
 * Generates function to convert to float
 */
void generate_floatval();

/**
 * Generates function to convert to int
 */
void generate_intval();

/**
 * @brief generate a decimal to string conversion
 * @param *variable variable name
 */
void generate_int_to_char(frames_t frame);

/**
 * @brief generate a string to decimal conversion
 * @param *symbol symbol name
 */
void generate_string_to_int(frames_t frame);

/**
 * @brief read string from standard input
 * @param *frame variable frame
 */
void generate_reads(frames_t frame);

/**
 * @brief read int from standard input
 * @param frame variable frame
 */
void generate_readi(frames_t frame);

/**
 * @brief read float from standard input
 * @param frame variable frame
 */
void generate_readf(frames_t frame);

/**
 * @brief print value to standard output
 * @param frame symbol frame
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

/**
 * Processes tree value to required target language format
 * @param tree tree node to process
 */
void process_node_value(syntax_abstract_tree_t *tree);

/**
 * Returns frame of the node
 * @param tree tree node
 * @return frame of the node
 */
frames_t get_node_frame(syntax_abstract_tree_t *tree);

/**
 * Parses math expression
 * @param tree tree node
 */
void parse_expression(syntax_abstract_tree_t *tree);

/**
 * Parses relational expression
 * @param tree tree node
 */
void parse_relational_expression(syntax_abstract_tree_t *tree);

/**
 * Parses syntax tree assign node
 * @param tree syntax tree assign node
 */
void parse_assign(syntax_abstract_tree_t *tree);

/**
 * Parses syntax tree
 * @param tree syntax tree node
 */
void parse_tree(syntax_abstract_tree_t *tree);

#endif //IFJ_PROJ_2022_CODE_GENERATOR_H
