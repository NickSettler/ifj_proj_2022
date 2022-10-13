#include "code_generator.h"

#include <stdio.h>
#include <stdbool.h>

// Assign a value to a variable
void generate_move(char *parameter_frame, char *parameter, char *variable_frame, char *variable) {
    fprintf(fd, "MOVE %s%s %s%s\n", parameter_frame, parameter, variable_frame, variable);
}

// Create a new temporary frame
void generate_create_frame() {
    fprintf(fd, "CREATEFRAME\n");
}

// Push a frame to the stack
void generate_push_frame() {
    fprintf(fd, "PUSHFRAME\n");
}

// Pop a frame from the stack
void generate_pop_frame() {
    fprintf(fd, "POPFRAME\n");
}

// Define a new variable in the frame
void generate_declaration(char *variable_frame, char *variable) {
    fprintf(fd, "DEFVAR %s%s\n", variable_frame, variable);
}

// Jump on a sign with return support
void generate_call(char *label) {
    fprintf(fd, "CALL $%s\n", label);
}

// Put the value on the top of the stack
void generate_push(char *variable_frame, char *variable) {
    fprintf(fd, "PUSHS %s%s\n", variable_frame, variable);
}

// Sum of two numerical values
void generate_add(char *result, char *symbol1, char *symbol2) {
    fprintf(fd, "ADD LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
}

// Subtraction of two numerical values
void generate_sub(char *result, char *symbol1, char *symbol2) {
    fprintf(fd, "SUB LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
}

// Multiplication of two numerical values
void generate_mul(char *result, char *symbol1, char *symbol2) {
    fprintf(fd, "MUL LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
}

// Division of two numerical values
void generate_idiv(char *result, char *symbol1, char *symbol2) {
    fprintf(fd, "IDIV LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
}

// Compare two values for lower that
void generate_lt(char *result, char *symbol1, char *symbol2) {
    fprintf(fd, "LT LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
}

// Compare two values for greater than
void generate_gt(char *result, char *symbol1, char *symbol2) {
    fprintf(fd, "GT LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
}

// Compare two values for equality
void generate_eq(char *result, char *symbol1, char *symbol2) {
    fprintf(fd, "EQ LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
}

// Compare two values with logical AND
void generate_and(char *result, char *symbol1, char *symbol2) {
    fprintf(fd, "AND LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
}

//  Compare two values with logical OR
void generate_or(char *result, char *symbol1, char *symbol2) {
    fprintf(fd, "OR LF@%s LF@%s LF@%s\n", result, symbol1, symbol2);
}

// Compare two values with logical NOT
void generate_not(char *result, char *symbol) {
    fprintf(fd, "NOT LF@%s LF@%s\n", result, symbol);
}

void generate_end() {
    printf("\nPOPFRAME");
    printf("\nRETURN");
}

int test_code_generator() {
    fd = fopen("test.txt", "w");
    generate_move("GF", "a", "GF", "b");
    generate_create_frame();
    generate_push_frame();
    generate_pop_frame();
    generate_declaration("GF", "c");
    generate_call("test");
    generate_push("GF", "a");
    generate_add("c", "a", "b");
    generate_sub("c", "a", "b");
    generate_mul("c", "a", "b");
    generate_idiv("c", "a", "b");
    generate_lt("c", "a", "b");
    generate_gt("c", "a", "b");
    generate_eq("c", "a", "b");
    generate_and("c", "a", "b");
    generate_or("c", "a", "b");
    generate_not("c", "a");
    fclose(fd);
    return 0;
}





