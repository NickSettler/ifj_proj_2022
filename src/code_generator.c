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
#include "semantic_analyzer.h"

void set_code_gen_output(FILE *output_fd) {
    fd = output_fd;
}

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

void generate_call(char *label) {
    fprintf(fd, "CALL %s\n", label);
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
    } else if (instruction == CODE_GEN_READI_INSTRUCTION || instruction == CODE_GEN_READF_INSTRUCTION ||
               instruction == CODE_GEN_READS_INSTRUCTION) {
        fprintf(fd, "%s %s@%s %s\n", instructions[instruction], frames[result_frame], result,
                instruction == CODE_GEN_READI_INSTRUCTION ? "int" : instruction == CODE_GEN_READF_INSTRUCTION ? "float"
                                                                                                              : "string");
    } else if (instruction == CODE_GEN_WRITE_INSTRUCTION) {
        fprintf(fd, "%s %s@%s\n", instructions[instruction], frames[result_frame], result);
    } else {
        fprintf(fd, "%s %s@%s %s@%s %s@%s\n", instructions[instruction], frames[result_frame], result,
                frames[symbol1_frame], symbol1,
                frames[symbol2_frame], symbol2);
    }
}

void generate_jump(char *label) {
    fprintf(fd, "JUMP %s\n", label);
}

void
generate_conditional_jump(bool is_equal, char *label, frames_t frame, char *symbol1, frames_t frame2, char *symbol2) {
    fprintf(fd, "%s %s %s@%s %s@%s\n", is_equal ? "JUMPIFEQ" : "JUMPIFNEQ", label, frames[frame], symbol1,
            frames[frame2], symbol2);
}

void generate_header() {
    fprintf(fd, ".IFJcode22\n");
}

void generate_exit(int exit_code) {
    fprintf(fd, "EXIT int@%d\n", exit_code);
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

void generate_conversion_base(char *label, char *process_variable, char *type_variable) {
    generate_label(label);
    generate_create_frame();
    generate_push_frame();

    generate_declaration(CODE_GENERATOR_LOCAL_FRAME, process_variable);
    generate_pop_from_top(CODE_GENERATOR_LOCAL_FRAME, process_variable);

    generate_declaration(CODE_GENERATOR_LOCAL_FRAME, type_variable);
    generate_type(CODE_GENERATOR_LOCAL_FRAME, type_variable, CODE_GENERATOR_LOCAL_FRAME, process_variable);
}

void generate_number_conversion_functions() {
    int functions_count = 2;
    string_t *functions[functions_count];
    functions[0] = string_init("floatval");
    functions[1] = string_init("intval");

    char *types[functions_count];
    types[0] = "float";
    types[1] = "int";

    char *process_variable = "$process_var";
    char *type_variable = "$type_var";

    for (int i = 0; i < functions_count; i++) {
        string_t *current_function = functions[i];
        char *current_type = types[i];
        char *conversion_type = i == 0 ? "int" : "float";
        instructions_t conversion_instruction =
                i == 0 ? CODE_GEN_INT2FLOAT_INSTRUCTION : CODE_GEN_FLOAT2INT_INSTRUCTION;

        string_t *end_label = string_init(current_function->value);
        string_append_string(end_label, "_end");

        string_t *conversion_label = string_init(current_function->value);
        string_append_string(conversion_label, "_%s", conversion_type);

        generate_conversion_base(current_function->value, process_variable, type_variable);

        generate_conditional_jump(true, end_label->value, CODE_GENERATOR_LOCAL_FRAME, type_variable,
                                  CODE_GENERATOR_STRING_CONSTANT, current_type);
        generate_conditional_jump(true, conversion_label->value, CODE_GENERATOR_LOCAL_FRAME, type_variable,
                                  CODE_GENERATOR_STRING_CONSTANT, conversion_type);
        generate_jump(end_label->value);

        generate_label(conversion_label->value);
        generate_operation(conversion_instruction, CODE_GENERATOR_LOCAL_FRAME, process_variable,
                           CODE_GENERATOR_LOCAL_FRAME, process_variable, 0, NULL);

        generate_label(end_label->value);
        generate_add_on_top(CODE_GENERATOR_LOCAL_FRAME, process_variable);
        generate_end();

        string_free(end_label);
        string_free(conversion_label);
    }

    for (int i = 0; i < functions_count; i++) {
        string_free(functions[i]);
    }
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

void generate_variable_inline_cast(syntax_abstract_tree_t *tree, data_type cast_to) {
    string_t *casted_string_name = string_init("");
    string_append_string(casted_string_name, "__%s_%s", tree->value->value,
                         cast_to == TYPE_INT ? "i" : cast_to == TYPE_FLOAT ? "f" : "s");

    insert_token(casted_string_name->value);
    tree_node_t *casted_variable = find_token(casted_string_name->value);
    casted_variable->defined = true;
    casted_variable->type = cast_to;

    generate_declaration(CODE_GENERATOR_GLOBAL_FRAME, casted_string_name->value);
    generate_move(CODE_GENERATOR_GLOBAL_FRAME, casted_string_name->value, CODE_GENERATOR_GLOBAL_FRAME,
                  tree->value->value);
    generate_add_on_top(CODE_GENERATOR_GLOBAL_FRAME, casted_string_name->value);
    generate_call(cast_to == TYPE_INT ? "intval" : cast_to == TYPE_FLOAT ? "floatval" : "strval");
    generate_pop_from_top(CODE_GENERATOR_GLOBAL_FRAME, casted_string_name->value);

    tree->value = casted_string_name;
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
        string_t *new_str = string_init("");
        for (int i = 1; i < tree->value->length - 1; i++) {
            char c = tree->value->value[i];

            if (c >= 0 && c <= 32 || c == 35 || c == 92)
                string_append_string(new_str, "\\%03d", c);
            else
                string_append_char(new_str, c);
        }

        string_free(tree->value);
        tree->value = new_str;
    } else if (tree->type & SYN_NODE_KEYWORD_NULL) {
        if (tree->value == NULL)
            tree->value = string_init("nil");
        else
            string_replace(tree->value, "nil");
    } else if (tree->type & (SYN_NODE_ADD | SYN_NODE_SUB | SYN_NODE_MUL | SYN_NODE_DIV)) {
        parse_expression(tree, NULL);
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
        case SYN_NODE_KEYWORD_NULL:
            return CODE_GENERATOR_NULL_CONSTANT;
        case SYN_NODE_IDENTIFIER: {
            // TODO: get frame from symbol table
            return CODE_GENERATOR_GLOBAL_FRAME;
        }
        default:
            return -1;
    }
}

void parse_expression(syntax_abstract_tree_t *tree, string_t *result) {
    if (!tree || (tree->type & (SYN_NODE_INTEGER | SYN_NODE_FLOAT | SYN_NODE_STRING | SYN_NODE_IDENTIFIER))) return;

    bool is_simple = check_tree_using(tree, is_simple_expression);

    bool is_left_const = tree->left &&
                         (tree->left->type &
                          (SYN_NODE_INTEGER | SYN_NODE_FLOAT | SYN_NODE_STRING | SYN_NODE_IDENTIFIER));
    bool is_right_const = tree->right &&
                          (tree->right->type & (SYN_NODE_INTEGER | SYN_NODE_FLOAT | SYN_NODE_STRING |
                                                SYN_NODE_IDENTIFIER));

    bool is_left_simple = check_tree_using(tree->left, is_simple_expression);
    bool is_right_simple = check_tree_using(tree->right, is_simple_expression);

    if (is_left_simple && !is_left_const) {
        string_t *left_var_name = string_init(tmp_var_name);
        string_append_string(left_var_name, "%d", ++code_generator_parameters->tmp_var_counter);
        if (!(is_simple && result))
            generate_declaration(CODE_GENERATOR_GLOBAL_FRAME, left_var_name->value);
        parse_expression(tree->left, is_simple && result ? result : left_var_name);
    } else {
        parse_expression(tree->left, NULL);
    }

    if (is_right_simple && !is_right_const) {
        string_t *right_var_name = string_init(tmp_var_name);
        string_append_string(right_var_name, "%d", ++code_generator_parameters->tmp_var_counter);
        if (!(is_simple && result))
            generate_declaration(CODE_GENERATOR_GLOBAL_FRAME, right_var_name->value);
        parse_expression(tree->right, is_simple && result ? result : right_var_name);
    } else {
        parse_expression(tree->right, NULL);
    }

    if (!(tree->type & (SYN_NODE_ADD | SYN_NODE_SUB | SYN_NODE_MUL | SYN_NODE_DIV | SYN_NODE_CONCAT)))
        return;

    instructions_t instruction =
            tree->type == SYN_NODE_ADD ? CODE_GEN_ADD_INSTRUCTION :
            tree->type == SYN_NODE_SUB ? CODE_GEN_SUB_INSTRUCTION :
            tree->type == SYN_NODE_MUL ? CODE_GEN_MUL_INSTRUCTION :
            tree->type == SYN_NODE_DIV ? CODE_GEN_DIV_INSTRUCTION :
            tree->type == SYN_NODE_CONCAT ? CODE_GEN_CONCAT_INSTRUCTION : -1;

    string_t *operation_var_name = result ? result : string_init(tmp_var_name);
    if (!result)
        string_append_string(operation_var_name, "%d", ++code_generator_parameters->tmp_var_counter);

    insert_token(operation_var_name->value);
    tree_node_t *operation_var = find_token(operation_var_name->value);
    operation_var->defined = true;
    operation_var->type = get_data_type(tree);

    data_type left_type = get_data_type(tree->left);
    data_type right_type = get_data_type(tree->right);

    data_type cast_type_to = left_type > right_type ? left_type : right_type;

    bool need_inline_left_cast = left_type != cast_type_to && tree->left->type == SYN_NODE_IDENTIFIER;
    bool need_inline_right_cast = right_type != cast_type_to && tree->right->type == SYN_NODE_IDENTIFIER;

    if (need_inline_left_cast)
        generate_variable_inline_cast(tree->left, cast_type_to);

    if (need_inline_right_cast)
        generate_variable_inline_cast(tree->right, cast_type_to);

    tree->value = operation_var_name;
    tree->type = SYN_NODE_IDENTIFIER;

    frames_t left_frame = get_node_frame(tree->left);
    frames_t right_frame = get_node_frame(tree->right);

    process_node_value(tree->left);
    process_node_value(tree->right);

    if (!result)
        generate_declaration(CODE_GENERATOR_GLOBAL_FRAME, operation_var_name->value);
    generate_operation(instruction,
                       CODE_GENERATOR_GLOBAL_FRAME,
                       operation_var_name->value,
                       left_frame,
                       tree->left->value->value,
                       right_frame,
                       tree->right->value->value);
}

void parse_relational_expression(syntax_abstract_tree_t *tree, string_t *result) {
    if (tree->type & (SYN_NODE_ADD | SYN_NODE_SUB | SYN_NODE_MUL | SYN_NODE_DIV))
        parse_expression(tree, NULL);

    if (tree->type != SYN_NODE_LESS && tree->type != SYN_NODE_LESS_EQUAL && tree->type != SYN_NODE_GREATER &&
        tree->type != SYN_NODE_GREATER_EQUAL && tree->type != SYN_NODE_EQUAL && tree->type != SYN_NODE_NOT_EQUAL &&
        tree->type != SYN_NODE_NOT && tree->type != SYN_NODE_AND && tree->type != SYN_NODE_OR)
        return;

    instructions_t instruction = tree->type == SYN_NODE_LESS ? CODE_GEN_LT_INSTRUCTION :
                                 tree->type == SYN_NODE_GREATER ? CODE_GEN_GT_INSTRUCTION :
                                 tree->type == SYN_NODE_EQUAL ? CODE_GEN_EQ_INSTRUCTION :
                                 tree->type == SYN_NODE_OR ? CODE_GEN_OR_INSTRUCTION :
                                 tree->type == SYN_NODE_AND ? CODE_GEN_AND_INSTRUCTION :
                                 tree->type == SYN_NODE_NOT ? CODE_GEN_NOT_INSTRUCTION :
                                 CODE_GEN_NOTEQ_INSTRUCTION;

    bool is_left_const = tree->left->type == SYN_NODE_INTEGER || tree->left->type == SYN_NODE_FLOAT ||
                         tree->left->type == SYN_NODE_STRING || tree->left->type == SYN_NODE_IDENTIFIER ||
                         tree->left->type == SYN_NODE_KEYWORD_NULL;
    bool is_right_const = tree->right->type == SYN_NODE_INTEGER || tree->right->type == SYN_NODE_FLOAT ||
                          tree->right->type == SYN_NODE_STRING || tree->right->type == SYN_NODE_IDENTIFIER ||
                          tree->right->type == SYN_NODE_KEYWORD_NULL;

    if (is_left_const && is_right_const) {
        string_t *operation_var_name = result ? result : string_init(tmp_var_name);
        if (!result) {
            string_append_string(operation_var_name, "%d", ++code_generator_parameters->tmp_var_counter);
        }

        insert_token(operation_var_name->value);
        tree_node_t *operation_var = find_token(operation_var_name->value);
        operation_var->defined = true;

        tree->value = operation_var_name;
        tree->type = SYN_NODE_IDENTIFIER;

        frames_t left_frame = get_node_frame(tree->left);
        frames_t right_frame = get_node_frame(tree->right);

        process_node_value(tree->left);
        process_node_value(tree->right);

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

    bool is_constant = tree->right->type == SYN_NODE_INTEGER || tree->right->type == SYN_NODE_FLOAT ||
                       tree->right->type == SYN_NODE_STRING || tree->right->type == SYN_NODE_IDENTIFIER ||
                       tree->right->type == SYN_NODE_CALL;

    if (find_token(tree->left->value->value)->code_generator_defined == false)
        generate_declaration(CODE_GENERATOR_GLOBAL_FRAME, tree->left->value->value);

    find_token(tree->left->value->value)->code_generator_defined = true;

    if (!is_constant) {
        if (is_relational) parse_relational_expression(tree->right, NULL);
        else parse_expression(tree->right, tree->left->value);
    } else {
        process_node_value(tree->right);
        if (tree->right->type == SYN_NODE_CALL) {
            parse_function_call(tree->right);
            generate_pop_from_top(CODE_GENERATOR_GLOBAL_FRAME, tree->left->value->value);
        } else {
            generate_move(CODE_GENERATOR_GLOBAL_FRAME, tree->left->value->value, get_node_frame(tree->right),
                          tree->right->value->value);
        }
    }
}

void parse_function_arg(syntax_abstract_tree_t *tree) {
    if (tree->type != SYN_NODE_ARGS) return;

    process_node_value(tree->left);

    generate_add_on_top(get_node_frame(tree->left), tree->left->value->value);
}

void parse_function_call(syntax_abstract_tree_t *tree) {
    if (tree->type != SYN_NODE_CALL) return;

    bool needed_args_count_push = !strcmp(tree->left->value->value, "write");

    int args_count = 0;
    syntax_abstract_tree_t *arg = tree->right;

    while (arg != NULL) {
        parse_function_arg(arg);
        args_count++;
        arg = arg->right;
    }

    if (needed_args_count_push) {
        string_t *needed_args_count = string_init("");
        string_append_string(needed_args_count, "%d", args_count);
        generate_add_on_top(CODE_GENERATOR_INT_CONSTANT, needed_args_count->value);
        string_free(needed_args_count);
    }

    generate_call(tree->left->value->value);
}

void parse_loop(syntax_abstract_tree_t *tree) {
    if (tree->type != SYN_NODE_KEYWORD_WHILE) return;

    string_t *loop_label = string_init(loop_label_name);
    string_append_string(loop_label, "%d", ++code_generator_parameters->loop_counter);

    string_t *loop_cond_var = string_init(loop_label->value);
    string_append_string(loop_cond_var, "_cond");

    string_t *loop_start_label = string_init(loop_label->value);
    string_append_string(loop_start_label, "_start");

    string_t *loop_end_label = string_init(loop_label->value);
    string_append_string(loop_end_label, "_end");

    syntax_tree_node_type loop_type = tree->left->type;

    generate_declaration(CODE_GENERATOR_GLOBAL_FRAME, loop_cond_var->value);

    parse_relational_expression(tree->left, loop_cond_var);

    generate_label(loop_start_label->value);
    generate_conditional_jump(true, loop_end_label->value, CODE_GENERATOR_GLOBAL_FRAME, tree->left->value->value,
                              CODE_GENERATOR_BOOL_CONSTANT, "false");
    parse_tree(tree->right);
    tree->left->type = loop_type;
    parse_relational_expression(tree->left, loop_cond_var);
    generate_jump(loop_start_label->value);

    generate_label(loop_end_label->value);
}

void parse_condition(syntax_abstract_tree_t *tree) {
    if (tree->type != SYN_NODE_KEYWORD_IF) return;

    string_t *cond_label = string_init(condition_label_name);
    string_append_string(cond_label, "%d", ++code_generator_parameters->condition_counter);

    string_t *condition_var = string_init(cond_label->value);
    string_append_string(condition_var, "_cond");

    string_t *condition_body_label = string_init(cond_label->value);
    string_append_string(condition_body_label, "_body");

    string_t *condition_else_label = string_init(cond_label->value);
    string_append_string(condition_else_label, "_else");

    string_t *condition_end_label = string_init(cond_label->value);
    string_append_string(condition_end_label, "_end");

    bool has_else = tree->right != NULL && tree->right->right != NULL;

    generate_declaration(CODE_GENERATOR_GLOBAL_FRAME, condition_var->value);
    parse_relational_expression(tree->left, condition_var);

    if (has_else) {
        generate_conditional_jump(true, condition_body_label->value, CODE_GENERATOR_GLOBAL_FRAME, condition_var->value,
                                  CODE_GENERATOR_BOOL_CONSTANT, "true");
        generate_conditional_jump(true, condition_else_label->value, CODE_GENERATOR_GLOBAL_FRAME, condition_var->value,
                                  CODE_GENERATOR_BOOL_CONSTANT, "false");
    } else {
        generate_conditional_jump(true, condition_end_label->value, CODE_GENERATOR_GLOBAL_FRAME, condition_var->value,
                                  CODE_GENERATOR_BOOL_CONSTANT, "false");
    }

    generate_label(condition_body_label->value);
    parse_tree(tree->middle);
    generate_jump(condition_end_label->value);

    if (has_else) {
        generate_label(condition_else_label->value);
        parse_tree(tree->right);
        generate_jump(condition_end_label->value);
    }

    generate_label(condition_end_label->value);
}

void parse_tree(syntax_abstract_tree_t *tree) {
    if (tree->left) parse_tree(tree->left);

    if (tree->type != SYN_NODE_SEQUENCE) return;

    if (tree->right) {
        switch (tree->right->type) {
            case SYN_NODE_ASSIGN:
                parse_assign(tree->right);
                break;
            case SYN_NODE_CALL:
                parse_function_call(tree->right);
                break;
            case SYN_NODE_KEYWORD_WHILE:
                parse_loop(tree->right);
                break;
            case SYN_NODE_KEYWORD_IF:
                parse_condition(tree->right);
                break;
            default:
                break;
        }
    }
}

void code_generator_init() {
    code_generator_parameters = (code_generator_parameters_t *) malloc(sizeof(code_generator_parameters_t));

    code_generator_parameters->tmp_var_counter = 0;
    code_generator_parameters->condition_counter = 0;
    code_generator_parameters->loop_counter = 0;
    code_generator_parameters->current_callee_instruction = (instructions_t) -1;
    code_generator_parameters->current_callee_result = NULL;
}