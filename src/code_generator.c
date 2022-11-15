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

void generate_clear_stack(frames_t frame) {
    fprintf(fd, "LABEL clear_stack\n");
    fprintf(fd, "MOVE %s@clear_var bool@true\n", frames[frame]);
    fprintf(fd, "JUMPIFEQ clear_stack_end %s@clear_top_var bool@true\n", frames[frame]);
    fprintf(fd, "POPS %s@_clear_stack\n", frames[frame]);
    fprintf(fd, "JUMP clear_stack\n");
    fprintf(fd, "RETURN\n");
}

void generate_operation(instructions_t instruction, frames_t result_frame, char *result, frames_t symbol1_frame,
                        char *symbol1, frames_t symbol2_frame, char *symbol2) {
    if (instruction == CODE_GEN_NOTLT_INSTRUCTION) {
        fprintf(fd, "LT %s@%s %s@%s %s@%s\n", frames[result_frame], result, frames[symbol1_frame], symbol1,
                frames[symbol2_frame], symbol2);
        fprintf(fd, "NOT %s@%s %s@%s\n", frames[result_frame], result, frames[result_frame], result);
    } else if (instruction == CODE_GEN_NOTGT_INSTRUCTION) {
        fprintf(fd, "GT %s@%s %s@%s %s@%s\n", frames[result_frame], result, frames[symbol1_frame], symbol1,
                frames[symbol2_frame], symbol2);
        fprintf(fd, "NOT %s@%s %s@%s\n", frames[result_frame], result, frames[result_frame], result);
    } else if (instruction == CODE_GEN_NOTEQ_INSTRUCTION) {
        fprintf(fd, "EQ %s@%s %s@%s %s@%s\n", frames[result_frame], result, frames[symbol1_frame], symbol1,
                frames[symbol2_frame], symbol2);
        fprintf(fd, "NOT %s@%s %s@%s\n", frames[result_frame], result, frames[result_frame], result);
    } else if (instruction == CODE_GEN_NOT_INSTRUCTION || instruction == CODE_GEN_NOTS_INSTRUCTION ||
               instruction == CODE_GEN_STRLEN_INSTRUCTION || instruction == CODE_GEN_INT2FLOATS_INSTRUCTION ||
               instruction == CODE_GEN_FLOAT2INTS_INSTRUCTION || instruction == CODE_GEN_INT2CHARS_INSTRUCTION) {
        fprintf(fd, "%s %s@%s %s@%s\n", instructions[instruction], frames[result_frame], result, frames[symbol1_frame],
                symbol1);
    } else {
        fprintf(fd, "%s %s@%s %s@%s %s@%s\n", instructions[instruction], frames[result_frame], result,
                frames[symbol1_frame], symbol1,
                frames[symbol2_frame], symbol2);
    }
}

void generate_int_to_float(frames_t frame) {
    fprintf(fd, "LABEL float2int\n");
    fprintf(fd, "CREATEFRAME\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR %s@retval1\n", frames[frame]);
    fprintf(fd, "MOVE %s@retval1 nil@nil\n", frames[frame]);
    fprintf(fd, "DEFVAR %s@float2int\n", frames[frame]);
    fprintf(fd, "MOVE %s@float2int %s@1\n", frames[frame], frames[frame]);
    fprintf(fd, "INT2FLOAT %s@retval1 %s@float2int\n", frames[frame], frames[frame]);
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_float_to_int(frames_t frame) {
    fprintf(fd, "LABEL int2float\n");
    fprintf(fd, "CREATEFRAME\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR %s@retval1\n", frames[frame]);
    fprintf(fd, "MOVE %s@retval1 nil@nil\n", frames[frame]);
    fprintf(fd, "DEFVAR %s@int2float\n", frames[frame]);
    fprintf(fd, "MOVE %s@int2float %s@1\n", frames[frame], frames[frame]);
    fprintf(fd, "FLOAT2INT %s@retval1 %s@int2float\n", frames[frame], frames[frame]);
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_int_to_char(frames_t frame) {
    fprintf(fd, "LABEL int2char\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR %s@retval1\n", frames[frame]);
    fprintf(fd, "MOVE %s@retval1 nil@nil\n", frames[frame]);
    fprintf(fd, "DEFVAR %s@i\n", frames[frame]);
    fprintf(fd, "MOVE %s@i %s@1\n", frames[frame], frames[frame]);
    fprintf(fd, "DEFVAR %s@i\n", frames[frame]);
    fprintf(fd, "MOVE %s@tmp nil@nil\n", frames[frame]);
    fprintf(fd, "TYPE %s@tmp %s@int\n", frames[frame], frames[frame]);
    fprintf(fd, "JUMPIFEQ error_label %s@tmp string@nil\n", frames[frame]);
    fprintf(fd, "LT %s@tmp %s@i int@0\n", frames[frame], frames[frame]);
    fprintf(fd, "JUMPIFEQ nil_return %s@tmp bool@true\n", frames[frame]);
    fprintf(fd, "GT %s@tmp %s@i int@255\n", frames[frame], frames[frame]);
    fprintf(fd, "JUMPIFEQ nil_return %s@tmp bool@true\n", frames[frame]);
    fprintf(fd, "INT2CHAR %s@retval1 %s@int\n", frames[frame], frames[frame]);
    fprintf(fd, "LABEL nil_return\n");
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_string_to_int(frames_t frame) {
    fprintf(fd, "LABEL stri2int\n");
    fprintf(fd, "CREATEFRAME\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR %s@s\n", frames[frame]);
    fprintf(fd, "MOVE %s@s %s@1\n", frames[frame], frames[frame]);
    fprintf(fd, "DEFVAR %s@i\n", frames[frame]);
    fprintf(fd, "MOVE %s@i %s@2\n", frames[frame], frames[frame]);
    fprintf(fd, "DEFVAR %s@lenght\n", frames[frame]);
    fprintf(fd, "MOVE %s@length nil@nil\n", frames[frame]);
    fprintf(fd, "DEFVAR %s@tmp\n", frames[frame]);
    fprintf(fd, "MOVE %s@tmp nil@nil\n", frames[frame]);
    fprintf(fd, "TYPE %s@tmp %s@s\n", frames[frame], frames[frame]);
    fprintf(fd, "JUMPIFEQ error_label %s@tmp string@nil\n", frames[frame]);
    fprintf(fd, "TYPE %s@tmp %s@i\n", frames[frame], frames[frame]);
    fprintf(fd, "JUMPIFEQ error_label %s@tmp string@nil\n", frames[frame]);
    fprintf(fd, "STRLEN %s@length %s@s\n", frames[frame], frames[frame]);
    fprintf(fd, "LT %s@tmp %s@i int@0\n", frames[frame], frames[frame]);
    fprintf(fd, "JUMPIFEQ nil_return %s@tmp bool@true\n", frames[frame]);
    fprintf(fd, "GT %s@tmp %s@i %s@length\n", frames[frame], frames[frame], frames[frame]);
    fprintf(fd, "JUMPIFEQ nil_return %s@tmp bool@true\n", frames[frame]);
    fprintf(fd, "STRI2INT %s@retval1 %s@s %s@i\n", frames[frame], frames[frame], frames[frame]);
    fprintf(fd, "LABEL nil_return\n");
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_reads(frames_t frame) {
    fprintf(fd, "LABEL reads\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR %s@retval1\n", frames[frame]);
    fprintf(fd, "MOVE %s@retval1\n", frames[frame]);
    fprintf(fd, "READ %s@retval1 string\n", frames[frame]);
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_readi(frames_t frame) {
    fprintf(fd, "LABEL readi\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR %s@retval1\n", frames[frame]);
    fprintf(fd, "MOVE %s@retval1\n", frames[frame]);
    fprintf(fd, "READ %s@retval1 int\n", frames[frame]);
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_readf(frames_t frame) {
    fprintf(fd, "LABEL readf\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR %s@retval1\n", frames[frame]);
    fprintf(fd, "MOVE %s@retval1\n", frames[frame]);
    fprintf(fd, "READ %s@retval1 float\n", frames[frame]);
    fprintf(fd, "POPFRAME\n");
    fprintf(fd, "RETURN\n");
}

void generate_write(frames_t frame) {
    fprintf(fd, "LABEL write\n");
    fprintf(fd, "PUSHFRAME\n");
    fprintf(fd, "DEFVAR %s@write_to\n", frames[frame]);
    fprintf(fd, "DEFVAR %s@check_for_nil\n", frames[frame]);
    fprintf(fd, "POPS %s@write_to\n", frames[frame]);
    fprintf(fd, "TYPE %s@check_for_nil %s@write_to\n", frames[frame], frames[frame]);
    fprintf(fd, "JUMPIFEQ its_nil\n");
    fprintf(fd, "WRITE %s@write_to\n", frames[frame]);
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
