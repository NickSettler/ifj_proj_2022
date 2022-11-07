/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *   xmaroc00, Elena Marochkina
 *
 * @file errors.h
 * @brief Error handling
 * @date 24.09.2022
 */

#ifndef IFJ_PROJ_ERRORS_H
#define IFJ_PROJ_ERRORS_H

/* Lexical analyse error */
#define LEXICAL_ERROR_CODE 1
/* Syntax analyse error */
#define SYNTAX_ERROR_CODE 2
/* Use of undefined function. Function redefinition*/
#define SEMANTIC_FUNC_UNDEF_ERROR_CODE 3
/* Wrong arguments type/number in function call. Wrong return type from function */
#define SEMANTIC_FUNC_ARG_ERROR_CODE 4
/* Use of undefined variable */
#define SEMANTIC_UNDEF_VAR_ERROR_CODE 5
/* Wrong or missing function return expression */
#define SEMANTIC_FUNC_RET_ERROR_CODE 6
/* Wrong type of operand in expression */
#define SEMANTIC_TYPE_COMPAT_ERROR_CODE 7
/* Other semantic errors */
#define SEMANTIC_OTHER_ERROR_CODE 8
/* Internal error */
#define INTERNAL_ERROR_CODE 99

#define ERROR(code, ...) \
    fprintf(stderr, __VA_ARGS__); \
    exit(code);

#define LEXICAL_ERROR(...) ERROR(LEXICAL_ERROR_CODE, "[LEXICAL ERROR] " __VA_ARGS__)

#define SYNTAX_ERROR(...) ERROR(SYNTAX_ERROR_CODE, "[SYNTAX ERROR] " __VA_ARGS__)

#define INTERNAL_ERROR(...) ERROR(INTERNAL_ERROR_CODE, "[INTERNAL ERROR] " __VA_ARGS__)

#define SEMANTIC_UNDEF_FUNC_ERROR(...) ERROR(SEMANTIC_FUNC_UNDEF_ERROR_CODE, "[SEMANTIC UNDEF FUNC ERROR] " __VA_ARGS__)

#define SEMANTIC_UNDEF_VAR_ERROR(...) ERROR(SEMANTIC_UNDEF_VAR_ERROR_CODE, "[SEMANTIC UNDEF VAR ERROR] " __VA_ARGS__)

/**
 * Get readable error symbol
 * @param string Error symbol
 * @return Readable error symbol
 */
char *get_readable_error_char(char *string);

#endif //IFJ_PROJ_ERRORS_H
