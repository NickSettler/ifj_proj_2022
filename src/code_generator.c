/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *  xkalut00, Maksim Kalutski
 *
 * @file   code_generator.c
 * @brief  Code generator source file
 * @date   13.10.2022
 */

#include "code_generator.h"

void generate_move(frames_t frame, char *variable, char *symbol) {
    fprintf(fd, "MOVE %s@%s %s@%s\n", frames[frame], variable, frames[frame], symbol);
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

void generate_declaration(frames_t frame, char *variable) {
    fprintf(fd, "DEFVAR %s@%s\n", frames[frame], variable);
}

void generate_add_on_top(frames_t frame, char *variable) {
    fprintf(fd, "PUSHS %s@%s\n", frames[frame], variable);
}

void generate_pop_from_top(frames_t frame, char *variable) {
    fprintf(fd, "POPS %s@%s\n", frames[frame], variable);
}

void generate_clear_stack() {
    fprintf(fd, "LABEL clear_stack\n");
    fprintf(fd, "MOVE GT@clear_var bool@true\n");
    fprintf(fd, "JUMPIFEQ clear_stack_end GF@clear_top_var bool@true\n");
    fprintf(fd, "POPS GT@_clear_stack\n");
    fprintf(fd, "JUMP clear_stack\n");
    fprintf(fd, "RETURN\n");
}

void generate_operation(instructions_t instruction, frames_t frame, char *result, char *symbol1, char *symbol2) {
    if (instruction == NOTLT) {
        fprintf(fd, "LT %s@%s %s@%s %s@%s\n", frames[frame], result, frames[frame], symbol1, frames[frame], symbol2);
        fprintf(fd, "NOT %s@%s %s@%s\n", frames[frame], result, frames[frame], result);
    } else if (instruction == NOTGT) {
        fprintf(fd, "GT %s@%s %s@%s %s@%s\n", frames[frame], result, frames[frame], symbol1, frames[frame], symbol2);
        fprintf(fd, "NOT %s@%s %s@%s\n", frames[frame], result, frames[frame], result);
    } else if (instruction == NOTEQ) {
        fprintf(fd, "EQ %s@%s %s@%s %s@%s\n", frames[frame], result, frames[frame], symbol1, frames[frame], symbol2);
        fprintf(fd, "NOT %s@%s %s@%s\n", frames[frame], result, frames[frame], result);
    } else if (instruction == NOT || instruction == NOTS || instruction == STRLEN || instruction == INT2FLOATS ||
               instruction == FLOAT2INTS ||
               instruction == INT2CHARS) {
        fprintf(fd, "%s %s@%s %s@%s\n", instructions[instruction], frames[frame], result, frames[frame], symbol1);
    } else {
        fprintf(fd, "%s %s@%s %s@%s %s@%s\n", instructions[instruction], frames[frame], result, frames[frame], symbol1,
                frames[frame], symbol2);
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
