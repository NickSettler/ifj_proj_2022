/**
 * Imperative IFJ22 language compiler implementation
 * @author xkalut00, Maksim Kalutski
 * @file   code_generator.c
 * @brief  Code generator source file
 * @date   ??.10.2022
 */

#include "code_generator.h"

void generate_move(char *variable_frame, char *variable, char *symbol_frame, char *symbol) {
    fprintf(fd, "MOVE %s@%s %s@%s\n", variable_frame, variable, symbol_frame, symbol);
}

void generate_create_frame() {
    fprintf(fd, "CREATEFRAME\n");
}

void generate_push_frame() {
    fprintf(fd, "PUSHFRAME\n");
}

void generate_pop_frame() {
    fprintf(fd, "POPFRAME\n");
}

void generate_declaration(char *variable_frame, char *variable) {
    fprintf(fd, "DEFVAR %s@%s\n", variable_frame, variable);
}

void generate_call(char *label) {
    fprintf(fd, "CALL %s\n", label);
}

void generate_add_on_top(char *type, char *variable) {
    fprintf(fd, "PUSHS %s@%s\n", type, variable);
}

void generate_pop_from_top(char *variable_frame, char *variable) {
    fprintf(fd, "POPS %s@%s\n", variable_frame, variable);
}

void generate_clear_stack() {
    fprintf(fd, "LABEL clear_stack\n");
    fprintf(fd, "MOVE GT@clear_var bool@true\n");
    fprintf(fd, "JUMPIFEQ clear_stack_end GF@clear_top_var bool@true\n");
    fprintf(fd, "POPS GT@_clear_stack\n");
    fprintf(fd, "JUMP clear_stack\n");
    fprintf(fd, "RETURN\n");
}

void generate_operation(instructions_t instruction, char *result, char *symbol1, char *symbol2) {
    switch (instruction) {
        case ADD:
            fprintf(fd, "ADD LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case SUB:
            fprintf(fd, "SUB LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case MUL:
            fprintf(fd, "MUL LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case IMUL:
            fprintf(fd, "IMUL LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case DIV:
            fprintf(fd, "DIV LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case IDIV:
            fprintf(fd, "IDIV LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case LT:
            fprintf(fd, "LT LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case GT:
            fprintf(fd, "GT LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case EQ:
            fprintf(fd, "EQ LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case NOTLT:
            fprintf(fd, "LT LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            fprintf(fd, "NOT LF@%s LF@%s\n", result, result);
            break;
        case NOTGT:
            fprintf(fd, "GT LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            fprintf(fd, "NOT LF@%s LF@%s\n", result, result);
            break;
        case NOTEQ:
            fprintf(fd, "EQ LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            fprintf(fd, "NOT LF@%s LF@%s\n", result, result);
            break;
        case AND:
            fprintf(fd, "AND LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case OR:
            fprintf(fd, "OR LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case NOT:
            fprintf(fd, "NOT LF@%s LF@%s\n", result, symbol1);
            break;
        default:
            fprintf(stderr, "Unknown operation");
    }
}

void generate_stack_operation(stack_instructions_t instruction, char *result, char *symbol1, char *symbol2) {
    switch (instruction) {
        case ADDS:
            fprintf(fd, "ADDS LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case SUBS:
            fprintf(fd, "SUBS LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case MULS:
            fprintf(fd, "MULS LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case DIVS:
            fprintf(fd, "DIVS LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case IDIVS:
            fprintf(fd, "IDIVS LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        default:
            fprintf(stderr, "Unknown operation");
    }
}

void generate_string_relational_operation(instructions_t instruction, char *result, char *symbol1, char *symbol2) {
    switch (instruction) {
        case CONCAT:
            fprintf(fd, "CONCAT LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case STRLEN:
            fprintf(fd, "STRLEN LF@%s LF@%s\n", result, symbol1);
            break;
        case GETCHAR:
            fprintf(fd, "GETCHAR LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case SETCHAR:
            fprintf(fd, "SETCHAR LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case LT:
            fprintf(fd, "LT LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case GT:
            fprintf(fd, "GT LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case EQ:
            fprintf(fd, "EQ LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            break;
        case NOTLT:
            fprintf(fd, "LT LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            fprintf(fd, "NOT LF@%s LF@%s\n", result, result);
            break;
        case NOTGT:
            fprintf(fd, "GT LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            fprintf(fd, "NOT LF@%s LF@%s\n", result, result);
            break;
        case NOTEQ:
            fprintf(fd, "EQ LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
            fprintf(fd, "NOT LF@%s LF@%s\n", result, result);
            break;
        default:
            fprintf(stderr, "Unknown operation");
    }
}

void generate_int_to_float() {
    fprintf(fd, "LABEL float2int\n");
    fprintf(fd, "CREATEFRAME\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR LF@retval1\n");
    fprintf(fd, "MOVE LF@retval1 nil@nil\n");
    fprintf(fd, "DEFVAR LF@float2int\n");
    fprintf(fd, "MOVE LF@float2int LF@1\n");
    fprintf(fd, "INT2FLOAT LF@retval1 LF@float2int\n");
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_float_to_int() {
    fprintf(fd, "LABEL int2float\n");
    fprintf(fd, "CREATEFRAME\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR LF@retval1\n");
    fprintf(fd, "MOVE LF@retval1 nil@nil\n");
    fprintf(fd, "DEFVAR LF@int2float\n");
    fprintf(fd, "MOVE LF@int2float LF@1\n");
    fprintf(fd, "FLOAT2INT LF@retval1 LF@int2float\n");
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_int_to_char() {
    fprintf(fd, "LABEL int2char\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR LF@retval1\n");
    fprintf(fd, "MOVE LF@retval1 nil@nil\n");
    fprintf(fd, "DEFVAR LF@i\n");
    fprintf(fd, "MOVE LF@i LF@1\n");
    fprintf(fd, "DEFVAR LF@i\n");
    fprintf(fd, "MOVE LF@tmp nil@nil\n");
    fprintf(fd, "TYPE LF@tmp LF@int\n");
    fprintf(fd, "JUMPIFEQ error_label LF@tmp string@nil\n");
    fprintf(fd, "LT LF@tmp LF@i int@0\n");
    fprintf(fd, "JUMPIFEQ nil_return LF@tmp bool@true\n");
    fprintf(fd, "GT LF@tmp LF@i int@255\n");
    fprintf(fd, "JUMPIFEQ nil_return LF@tmp bool@true\n");
    fprintf(fd, "INT2CHAR LF@retval1 LF@int\n");
    fprintf(fd, "LABEL nil_return\n");
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_string_to_int() {
    fprintf(fd, "LABEL stri2int\n");
    fprintf(fd, "CREATEFRAME\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR LF@s\n");
    fprintf(fd, "MOVE LF@s LF@1\n");
    fprintf(fd, "DEFVAR LF@i\n");
    fprintf(fd, "MOVE LF@i LF@2\n");
    fprintf(fd, "DEFVAR LF@lenght\n");
    fprintf(fd, "MOVE LF@length nil@nil\n");
    fprintf(fd, "DEFVAR LF@tmp\n");
    fprintf(fd, "MOVE LF@tmp nil@nil\n");
    fprintf(fd, "TYPE LF@tmp LF@s\n");
    fprintf(fd, "JUMPIFEQ error_label LF@tmp string@nil\n");
    fprintf(fd, "TYPE LF@tmp LF@i\n");
    fprintf(fd, "JUMPIFEQ error_label LF@tmp string@nil\n");
    fprintf(fd, "STRLEN LF@length LF@s\n");
    fprintf(fd, "LT LF@tmp LF@i int@0\n");
    fprintf(fd, "JUMPIFEQ nil_return LF@tmp bool@true\n");
    fprintf(fd, "GT LF@tmp LF@i LF@length\n");
    fprintf(fd, "JUMPIFEQ nil_return LF@tmp bool@true\n");
    fprintf(fd, "STRI2INT LF@retval1 LF@s LF@i\n");
    fprintf(fd, "LABEL nil_return\n");
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_conversion_operation(stack_conversion_t instruction) {
    switch (instruction) {
        case INT2FLOATS:
            fprintf(fd, "INT2FLOATS\n");
            break;
        case FLOAT2INTS:
            fprintf(fd, "FLOAT2INTS\n");
            break;
        case INT2CHARS:
            fprintf(fd, "INT2CHARS\n");
            break;
        case STRI2INTS:
            fprintf(fd, "STRI2INTS\n");
            break;
        default:
            fprintf(stderr, "Unknown operation");
    }
}

void generate_reads() {
    fprintf(fd, "LABEL reads\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR LF@retval1\n");
    fprintf(fd, "MOVE LF@retval1\n");
    fprintf(fd, "READ LF@retval1 string\n");
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_readi() {
    fprintf(fd, "LABEL readi\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR LF@retval1\n");
    fprintf(fd, "MOVE LF@retval1\n");
    fprintf(fd, "READ LF@retval1 int\n");
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_readf() {
    fprintf(fd, "LABEL readf\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR LF@retval1\n");
    fprintf(fd, "MOVE LF@retval1\n");
    fprintf(fd, "READ LF@retval1 float\n");
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_write() {
    fprintf(fd, "LABEL write\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR LF@write_to\n");
    fprintf(fd, "DEFVAR LF@check_for_nil\n");
    fprintf(fd, "POPS LF@write_to\n");
    fprintf(fd, "TYPE LF@check_for_nil LF@write_to\n");
    fprintf(fd, "JUMPIFEQ its_nil\n");
    fprintf(fd, "WRITE LF@write_to\n");
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
    fprintf(fd, "LABEL its_nil\n");
    fprintf(fd, "WRITE string@nil\n");
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN");
}

void generate_substr() {
    fprintf(fd, "LABEL substr\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR LF@retval1\n");
    fprintf(fd, "MOVE LF@retval1 string@\n");

    fprintf(fd, "DEFVAR LF@s\n");           // ("s", 1)
    fprintf(fd, "MOVE LF@s int@1\n");

    fprintf(fd, "DEFVAR LF@i\n");           // ("i", 2)
    fprintf(fd, "MOVE LF@i int@2\n");

    fprintf(fd, "DEFVAR LF@j\n");           // ("j", 3)
    fprintf(fd, "MOVE LF@j int@3\n");

    fprintf(fd, "DEFVAR LF@tmp\n");         // ("tmp",)
    fprintf(fd, "MOVE LF@tmp nil@nil\n");

    fprintf(fd, "DEFVAR LF@length\n");      // ("length")
    fprintf(fd, "MOVE LF@length nil@nil\n");

    fprintf(fd, "DEFVAR LF@sing LF@s\n");   // ("sing")
    fprintf(fd, "MOVE LF@sing nil@nil\n");

    fprintf(fd, "TYPE LF@tmp LF@s\n");
    fprintf(fd, "JUMPIFEQ error LF@tmp string@nil\n");
    fprintf(fd, "TYPE LF@tmp LF@i\n");
    fprintf(fd, "JUMPIFEQ error LF@tmp string@nil\n");
    fprintf(fd, "TYPE LF@tmp LF@j\n");
    fprintf(fd, "JUMPIFEQ error LF@tmp string@nil\n");

    fprintf(fd, "STRLEN LF@length LF@s\n");
    fprintf(fd, "LT LF@tmp LF@i int@1\n");
    fprintf(fd, "JUMPIFEQ return LF@tmp bool@false\n");
    fprintf(fd, "GT LF@tmp LF@j LF@lenght\n");
    fprintf(fd, "JUMPIFEQ return LF@tmp bool@false\n");

    fprintf(fd, "GT LF@tmp LF@j LF@length\n");
    fprintf(fd, "JUMPIFEQ return LF@tmp bool@true\n");
    fprintf(fd, "LT LF@tmp LF@j int@1\n");
    fprintf(fd, "JUMPIFEQ return LF@tmp bool@true\n");

    fprintf(fd, "GT LF@tmp LF@i LF@j\n");
    fprintf(fd, "JUMPIFEQ return LF@tmp bool@true\n");

    fprintf(fd, "SUB LF@i LF@i int@1\n");
    fprintf(fd, "SUB LF@j LF@j int@1\n");

    fprintf(fd, "LABEL loop\n");
    fprintf(fd, "GETCHAR LF@sing LF@s LF@i\n");
    fprintf(fd, "CONCAT LF@retval1 LF@retval1 LF@sing\n");

    fprintf(fd, "ADD LF@i LF@i int@1\n");
    fprintf(fd, "LT LF@tmp LF@i LF@j\n");
    fprintf(fd, "JUMPIFEQ return LF@tmp bool@true\n");
    fprintf(fd, "JUMP loop\n");

    fprintf(fd, "LABEL return\n");
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_end() {
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}
