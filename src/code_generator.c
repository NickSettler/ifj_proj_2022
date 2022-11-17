/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *  xkalut00, Maksim Kalutski
 *  xmoise01, Nikita Moiseev
 *
 * @file   code_generator.c
 * @brief  Code generator source file
 * @date   13.10.2022
 */

#include "code_generator.h"

void generate_move(frames_t variable_frame, char *variable, frames_t symbol_frame, char *symbol) {
    fprintf(fd, "MOVE %s@%s %s@%s\n", frames[variable_frame], variable, frames[symbol_frame], symbol);
}

void generate_label(char *label) {
    fprintf(fd, "LABEL %s\n", label);
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

void generate_type(frames_t variable_frame, char *variable, frames_t symbol_frame, char *symbol) {
    fprintf(fd, "TYPE %s@%s %s@%s\n", frames[variable_frame], variable, frames[symbol_frame], symbol);
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
               instruction == CODE_GEN_STRLEN_INSTRUCTION || instruction == CODE_GEN_INT2FLOAT_INSTRUCTION ||
               instruction == CODE_GEN_FLOAT2INT_INSTRUCTION || instruction == CODE_GEN_INT2CHAR_INSTRUCTION) {
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

void generate_floatval() {
    char *stack_variable = "$process_var";
    char *type_variable = "$type_var";

    generate_label("floatval");
    generate_create_frame();
    generate_push_frame();

    generate_declaration(CODE_GENERATOR_LOCAL_FRAME, stack_variable);
    generate_pop_from_top(CODE_GENERATOR_LOCAL_FRAME, stack_variable);

    generate_declaration(CODE_GENERATOR_LOCAL_FRAME, type_variable);
    generate_type(CODE_GENERATOR_LOCAL_FRAME, type_variable, CODE_GENERATOR_LOCAL_FRAME, stack_variable);

    // TODO: create separate function for all jumps
    fprintf(fd, "JUMPIFEQ floatval_end %s@%s string@float\n", frames[CODE_GENERATOR_LOCAL_FRAME], type_variable);
    fprintf(fd, "JUMPIFEQ floatval_int %s@%s string@int\n", frames[CODE_GENERATOR_LOCAL_FRAME], type_variable);
    fprintf(fd, "JUMP floatval_end\n");

    generate_label("floatval_int");
    fprintf(fd, "INT2FLOAT %s@%s %s@%s\n", frames[CODE_GENERATOR_LOCAL_FRAME], stack_variable,
            frames[CODE_GENERATOR_LOCAL_FRAME], stack_variable);
    fprintf(fd, "JUMP floatval_end\n");

    generate_label("floatval_end");
    generate_add_on_top(CODE_GENERATOR_LOCAL_FRAME, stack_variable);
    generate_end();
}

void generate_intval() {
    char *stack_variable = "$process_var";
    char *type_variable = "$type_var";

    generate_label("intval");
    generate_create_frame();
    generate_push_frame();

    generate_declaration(CODE_GENERATOR_LOCAL_FRAME, stack_variable);
    generate_pop_from_top(CODE_GENERATOR_LOCAL_FRAME, stack_variable);

    generate_declaration(CODE_GENERATOR_LOCAL_FRAME, type_variable);
    generate_type(CODE_GENERATOR_LOCAL_FRAME, type_variable, CODE_GENERATOR_LOCAL_FRAME, stack_variable);

    // TODO: create separate function for all jumps
    fprintf(fd, "JUMPIFEQ intval_end %s@%s string@int\n", frames[CODE_GENERATOR_LOCAL_FRAME], type_variable);
    fprintf(fd, "JUMPIFEQ intval_float %s@%s string@float\n", frames[CODE_GENERATOR_LOCAL_FRAME], type_variable);
    fprintf(fd, "JUMP intval_end\n");

    generate_label("intval_float");
    fprintf(fd, "FLOAT2INT %s@%s %s@%s\n", frames[CODE_GENERATOR_LOCAL_FRAME], stack_variable,
            frames[CODE_GENERATOR_LOCAL_FRAME], stack_variable);
    fprintf(fd, "JUMP intval_end\n");

    generate_label("intval_end");
    generate_add_on_top(CODE_GENERATOR_LOCAL_FRAME, stack_variable);
    generate_end();
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

void generate_write() {
    char *stack_string_var = "$stack_string";
    char *counter_var = "$i";

    generate_label("write");
    generate_create_frame();
    generate_push_frame();

    generate_declaration(CODE_GENERATOR_LOCAL_FRAME, stack_string_var);
    generate_declaration(CODE_GENERATOR_LOCAL_FRAME, counter_var);

    generate_pop_from_top(CODE_GENERATOR_LOCAL_FRAME, counter_var);

    generate_label("write_loop");

    fprintf(fd, "JUMPIFEQ write_end LF@%s int@0\n", counter_var);
    generate_pop_from_top(CODE_GENERATOR_LOCAL_FRAME, stack_string_var);

    fprintf(fd, "WRITE LF@%s\n", stack_string_var);
    generate_operation(CODE_GEN_SUB_INSTRUCTION, CODE_GENERATOR_LOCAL_FRAME, counter_var, CODE_GENERATOR_LOCAL_FRAME,
                       counter_var, CODE_GENERATOR_INT_CONSTANT, "1");
    fprintf(fd, "JUMP write_loop\n");

    generate_label("write_end");
    generate_end();
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

void process_node_value(syntax_abstract_tree_t *tree) {
    if (tree == NULL) return;

    if (tree->type == SYN_NODE_FLOAT) {
        char *tmp = malloc(sizeof(char) * 100);
        double d = strtod(tree->value->value, &tmp);
        char *var_tmp = malloc(sizeof(char) * 100);
        sprintf(var_tmp, "%a", d);
        string_replace(tree->value, var_tmp);
    } else if (tree->type == SYN_NODE_STRING) {
        string_replace(tree->value, string_substr(tree->value, 1, tree->value->length - 1)->value);
    }
}

frames_t get_node_frame(syntax_abstract_tree_t *tree) {
    if (tree == NULL) return -1;

    switch (tree->type) {
        case SYN_NODE_INTEGER:
            return CODE_GENERATOR_INT_CONSTANT;
        case SYN_NODE_FLOAT:
            return CODE_GENERATOR_FLOAT_CONSTANT;
        case SYN_NODE_STRING:
            return CODE_GENERATOR_STRING_CONSTANT;
        case SYN_NODE_IDENTIFIER: {
            // TODO: get frame from symbol table
            return CODE_GENERATOR_GLOBAL_FRAME;
        }
        default:
            return -1;
    }
}

void parse_expression(syntax_abstract_tree_t *tree) {
    if (tree->type != SYN_NODE_ADD && tree->type != SYN_NODE_SUB && tree->type != SYN_NODE_MUL &&
        tree->type != SYN_NODE_DIV)
        return;

    instructions_t instruction =
            tree->type == SYN_NODE_ADD ? CODE_GEN_ADD_INSTRUCTION :
            tree->type == SYN_NODE_SUB ? CODE_GEN_SUB_INSTRUCTION :
            tree->type == SYN_NODE_MUL ? CODE_GEN_MUL_INSTRUCTION :
            CODE_GEN_DIV_INSTRUCTION;

    bool is_left_const = tree->left->type == SYN_NODE_INTEGER || tree->left->type == SYN_NODE_FLOAT ||
                         tree->left->type == SYN_NODE_STRING || tree->left->type == SYN_NODE_IDENTIFIER;
    bool is_right_const = tree->right->type == SYN_NODE_INTEGER || tree->right->type == SYN_NODE_FLOAT ||
                          tree->right->type == SYN_NODE_STRING || tree->right->type == SYN_NODE_IDENTIFIER;

    if (is_left_const && is_right_const) {
        /* TODO: some operations might not need to be stored in separate variables
         * $a = 1 + 2 * 3 can be stored just in $a - there is no need to store 2 * 3 in $tmp_1
         * and then add it to $a
         */
        string_t *operation_var_name = string_init("tmp_");
        string_append_string(operation_var_name, "%d", ++tmp_var_counter);

        tree->value = operation_var_name;
        tree->type = SYN_NODE_IDENTIFIER;

        frames_t left_frame = get_node_frame(tree->left);
        frames_t right_frame = get_node_frame(tree->right);

        process_node_value(tree->left);
        process_node_value(tree->right);

        generate_declaration(CODE_GENERATOR_GLOBAL_FRAME, operation_var_name->value);
        generate_operation(instruction,
                           CODE_GENERATOR_GLOBAL_FRAME,
                           operation_var_name->value,
                           left_frame,
                           tree->left->value->value,
                           right_frame,
                           tree->right->value->value);
    }
}

void parse_relational_expression(syntax_abstract_tree_t *tree) {
    if (tree->type == SYN_NODE_ADD || tree->type == SYN_NODE_SUB || tree->type == SYN_NODE_MUL ||
        tree->type == SYN_NODE_DIV)
        process_tree_using(tree, parse_expression, POSTORDER);

    if (tree->type != SYN_NODE_LESS && tree->type != SYN_NODE_LESS_EQUAL && tree->type != SYN_NODE_GREATER &&
        tree->type != SYN_NODE_GREATER_EQUAL && tree->type != SYN_NODE_EQUAL && tree->type != SYN_NODE_NOT_EQUAL &&
        tree->type != SYN_NODE_NOT && tree->type != SYN_NODE_AND && tree->type != SYN_NODE_OR)
        return;

    instructions_t instruction = tree->type == SYN_NODE_LESS ? CODE_GEN_LTS_INSTRUCTION :
                                 tree->type == SYN_NODE_GREATER ? CODE_GEN_GT_INSTRUCTION :
                                 tree->type == SYN_NODE_EQUAL ? CODE_GEN_EQ_INSTRUCTION :
                                 tree->type == SYN_NODE_OR ? CODE_GEN_OR_INSTRUCTION :
                                 tree->type == SYN_NODE_AND ? CODE_GEN_AND_INSTRUCTION :
                                 tree->type == SYN_NODE_NOT ? CODE_GEN_NOT_INSTRUCTION :
                                 CODE_GEN_NOTEQ_INSTRUCTION;

    bool is_left_const = tree->left->type == SYN_NODE_INTEGER || tree->left->type == SYN_NODE_FLOAT ||
                         tree->left->type == SYN_NODE_STRING || tree->left->type == SYN_NODE_IDENTIFIER;
    bool is_right_const = tree->right->type == SYN_NODE_INTEGER || tree->right->type == SYN_NODE_FLOAT ||
                          tree->right->type == SYN_NODE_STRING || tree->right->type == SYN_NODE_IDENTIFIER;

    if (is_left_const && is_right_const) {
        string_t *operation_var_name = string_init("tmp_");
        string_append_string(operation_var_name, "%d", ++tmp_var_counter);

        tree->value = operation_var_name;
        tree->type = SYN_NODE_IDENTIFIER;

        frames_t left_frame = get_node_frame(tree->left);
        frames_t right_frame = get_node_frame(tree->right);

        process_node_value(tree->left);
        process_node_value(tree->right);

        generate_declaration(CODE_GENERATOR_GLOBAL_FRAME, operation_var_name->value);
        generate_operation(instruction,
                           CODE_GENERATOR_GLOBAL_FRAME,
                           operation_var_name->value,
                           left_frame,
                           tree->left->value->value,
                           right_frame,
                           tree->right->value->value);
    }
}

void parse_assign(syntax_abstract_tree_t *tree) {
    if (tree->type != SYN_NODE_ASSIGN) return;

    bool is_relational = tree->right->type == SYN_NODE_LESS || tree->right->type == SYN_NODE_LESS_EQUAL ||
                         tree->right->type == SYN_NODE_GREATER || tree->right->type == SYN_NODE_GREATER_EQUAL ||
                         tree->right->type == SYN_NODE_EQUAL || tree->right->type == SYN_NODE_NOT_EQUAL ||
                         tree->right->type == SYN_NODE_NOT || tree->right->type == SYN_NODE_AND ||
                         tree->right->type == SYN_NODE_OR;

    // TODO: remove redundant variable declaration and move
    generate_declaration(CODE_GENERATOR_GLOBAL_FRAME, tree->left->value->value);
    process_tree_using(tree->right, is_relational ? parse_relational_expression : parse_expression, POSTORDER);
    generate_move(CODE_GENERATOR_GLOBAL_FRAME, tree->left->value->value, tree->right->value->value);
}

void parse_tree(syntax_abstract_tree_t *tree) {
    if (tree->type != SYN_NODE_SEQUENCE) return;

    if (tree->right && tree->right->type == SYN_NODE_ASSIGN) {
        parse_assign(tree->right);
    }
}
