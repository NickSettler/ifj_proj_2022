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
#include <ctype.h>
#include "str.h"
#include "syntax_analyzer.h"
#include "symtable.h"

/**
 * @brief file for writing the code
 */
static FILE *fd;

static char *tmp_var_name = "$$__TMP_";
static char *loop_label_name = "$$__LOOP_";
static char *condition_label_name = "$$__COND_";

/**
 * @brief structure with frames
 */
typedef enum {
    CODE_GENERATOR_NULL_CONSTANT,
    CODE_GENERATOR_INT_CONSTANT,
    CODE_GENERATOR_FLOAT_CONSTANT,
    CODE_GENERATOR_STRING_CONSTANT,
    CODE_GENERATOR_BOOL_CONSTANT,
    CODE_GENERATOR_GLOBAL_FRAME,
    CODE_GENERATOR_LOCAL_FRAME,
    CODE_GENERATOR_TEMPORARY_FRAME,
} frames_t;

/**
 * @brief array of frames
 */
static const char *frames[] = {
        "nil", "int", "float", "string", "bool", "GF", "LF", "TF",
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
    CODE_GEN_READI_INSTRUCTION,
    CODE_GEN_READF_INSTRUCTION,
    CODE_GEN_READS_INSTRUCTION,
    CODE_GEN_WRITE_INSTRUCTION,
    CODE_GEN_CONCAT_INSTRUCTION,
    CODE_GEN_STRLEN_INSTRUCTION,
    CODE_GEN_GETCHAR_INSTRUCTION,
    CODE_GEN_SETCHAR_INSTRUCTION,
    CODE_GEN_JUMPIFEQS_INSTRUCTION,
    CODE_GEN_JUMPIFNEQS_INSTRUCTION,
} instructions_t;

/**
 * @brief array of instructions
 */
static const char *instructions[] = {
        "ADD", "SUB", "MUL", "DIV", "IDIV", "ADDS", "SUBS", "MULS", "DIVS", "IDIVS", "LT", "GT", "EQ", "LTS", "GTS",
        "EQS", "NOTLT", "NOTGT", "NOTEQ", "AND", "OR", "NOT", "ANDS", "ORS", "NOTS", "INT2FLOAT", "FLOAT2INT",
        "INT2CHAR", "STRI2INT", "READ", "READ", "READ", "WRITE", "CONCAT", "STRLEN", "GETCHAR", "SETCHAR", "JUMPIFEQS",
        "JUMPIFNEQS",
};

typedef struct code_generator_parameters {
    int tmp_var_counter;
    int loop_counter;
    int condition_counter;
    instructions_t current_callee_instruction;
    bool is_in_function;
} code_generator_parameters_t;

static code_generator_parameters_t *code_generator_parameters;

void set_code_gen_output(FILE *output_fd);

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

void generate_exit(int exit_code);

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

void generate_floatval();

/**
 * Generates function to convert to int
 */
void generate_intval();

/**
 * Generates conversion function header
 * @param label function label
 * @param process_variable variable to work with
 * @param type_variable variable with type
 */
void generate_conversion_base(char *label, char *process_variable, char *type_variable);

/**
 * Generates function to convert to number data types
 */
void generate_number_conversion_functions();

/**
 * Generates function to convert to string data type
 */
void generate_strval();

/**
 * Generates internal substring function
 */
void generate_substr();

/**
 * Generates internal character ordinal value function
 */
void generate_ord();

/**
 * @brief generate end of code
 */
void generate_end();

void generate_variable_inline_cast(syntax_abstract_tree_t *tree, data_type cast_to);

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
void parse_expression(syntax_abstract_tree_t *tree, string_t *result);

/**
 * Parses relational expression
 * @param tree tree node
 */
void parse_relational_expression(syntax_abstract_tree_t *tree, string_t *result);

/**
 * Parses syntax tree assign node
 * @param tree syntax tree assign node
 */
void parse_assign(syntax_abstract_tree_t *tree);

/**
 * Generates function arguments
 * @param tree syntax tree function call argument node
 */
void parse_function_arg(syntax_abstract_tree_t *tree);

/**
 * Generates function call
 * @param tree syntax tree function call node
 */
void parse_function_call(syntax_abstract_tree_t *tree, string_t *result);

/**
 * Generates while loop
 * @param tree syntax tree while node
 */
void parse_loop(syntax_abstract_tree_t *tree);

/**
 * Generates if statement
 * @param tree syntax tree if node
 */
void parse_condition(syntax_abstract_tree_t *tree);

void parse_func_dec(syntax_abstract_tree_t *tree);

/**
 * Parses syntax tree
 * @param tree syntax tree node
 */
void parse_tree(syntax_abstract_tree_t *tree);

/**
 * Initializes code generator
 * @return code generator parameters
 */
void code_generator_init();

#endif //IFJ_PROJ_2022_CODE_GENERATOR_H
