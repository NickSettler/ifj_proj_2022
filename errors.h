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

#endif //IFJ_PROJ_ERRORS_H
