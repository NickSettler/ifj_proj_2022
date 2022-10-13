#ifndef IFJ_PROJ_2022_CODE_GENERATOR_H
#define IFJ_PROJ_2022_CODE_GENERATOR_H

FILE* fd; //file for testing

void generate_move(char *parameter_frame, char *parameter, char *variable_frame, char *variable);
void generate_create_frame();
void generate_push_frame();
void generate_pop_frame();
void generate_declaration(char* variable_frame, char* variable);
void generate_call(char *label);
void generate_push(char* variable_frame, char* variable);
void generate_add(char* result, char* symbol1, char* symbol2);
void generate_sub (char* result, char* symbol1, char* symbol2);
void generate_mul (char* result, char* symbol1, char* symbol2);
void generate_idiv (char* result, char* symbol1, char* symbol2);
void generate_lt (char* result, char* symbol1, char* symbol2);
void generate_gt (char* result, char* symbol1, char* symbol2);
void generate_eq (char* result, char* symbol1, char* symbol2);

#endif //IFJ_PROJ_2022_CODE_GENERATOR_H
