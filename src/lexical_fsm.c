/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *   xpasyn00, Nikita Pasynkov
 *
 * @file lexical_fsm.c
 * @brief Lexical Analyser FSM
 * @date 05.10.2022
 */

#include "lexical_fsm.h"

int state = START;

LEXICAL_FSM_TOKENS get_next_token(FILE *fd, string_t *token) {
    char current_char = (char) getc(fd);

    string_clear(token);
    while (true) {
        switch (state) {
            case START:

                switch (current_char) {
                    case ' ':
                        break;
                    case '\0':
                    case EOF:
                        return END_OF_FILE;
                    case '?':
                        state = KEYWORD_STATE;
                        string_append_char(token, current_char);
                        break;
                    case '$':
                        state = IDENTIFIER_STATE;
                        string_append_char(token, current_char);
                        break;
                    case '=':
                        state = EQUAL_STATE;
                        string_append_char(token, current_char);
                        break;
                    case ':':
                        string_append_char(token, current_char);
                        return COLON;
                    case ';':
                        string_append_char(token, current_char);
                        return SEMICOLON;
                    case '\'':
                    case '"':
                        state = STRING_STATE;
                        string_append_char(token, current_char);
                        break;
                    case '#' :
                        state = COMMENT_STATE;
                        break;
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                        state = ARITHMETIC_STATE;
                        string_append_char(token, current_char);
                        break;
                    case '&':
                    case '|':
                    case '!':
                        state = LOGICAL_STATE;
                        string_append_char(token, current_char);
                        break;
                    case ',':
                        string_append_char(token, current_char);
                        return COMMA;
                    case '(':
                        string_append_char(token, current_char);
                        return LEFT_PARENTHESIS;
                    case ')':
                        string_append_char(token, current_char);
                        return RIGHT_PARENTHESIS;
                    case '{':
                        string_append_char(token, current_char);
                        return LEFT_CURLY_BRACKETS;
                    case '}':
                        string_append_char(token, current_char);
                        return RIGHT_CURLY_BRACKETS;
                    case '[':
                        string_append_char(token, current_char);
                        return LEFT_SQUARE_BRACKETS;
                    case ']':
                        string_append_char(token, current_char);
                        return RIGHT_SQUARE_BRACKETS;
                    case '.':
                        string_append_char(token, current_char);
                        return CONCATENATION;
                    case '<':
                    case '>':
                        state = SQUARE_PARENTHESIS_STATE;
                        string_append_char(token, current_char);
                        break;

                    default:
                        if (isalpha(current_char)) {
                            state = KEYWORD_STATE;
                            string_append_char(token, current_char);
                            break;
                        } else if (isdigit(current_char)) {
                            state = INTEGER_STATE;
                            string_append_char(token, current_char);
                            break;
                        }
                        break;
                }
                break;
            case KEYWORD_STATE:
                if (isalpha(current_char) || (!strcmp(token->value, "?") && current_char == '>')) {
                    string_append_char(token, current_char);
                } else {
                    int keyword = -1;
                    string_convert_by(token, tolower);  // all keywords are not case-sensitive

                    if (!strcmp(token->value, "int") || !strcmp(token->value, "?int")) keyword = KEYWORD_INTEGER;
                    else if (!strcmp(token->value, "float") || !strcmp(token->value, "?float")) keyword = KEYWORD_FLOAT;
                    else if (!strcmp(token->value, "string") || !strcmp(token->value, "?string"))
                        keyword = KEYWORD_STRING;
                    else if (!strcmp(token->value, "if")) keyword = KEYWORD_IF;
                    else if (!strcmp(token->value, "else")) keyword = KEYWORD_ELSE;
                    else if (!strcmp(token->value, "while")) keyword = KEYWORD_WHILE;
                    else if (!strcmp(token->value, "function")) keyword = KEYWORD_FUNCTION;
                    else if (!strcmp(token->value, "return")) keyword = KEYWORD_RETURN;
                    else if (!strcmp(token->value, "null")) keyword = KEYWORD_NULL;
                    else if (!strcmp(token->value, "void")) keyword = KEYWORD_VOID;
                    else if (!strcmp(token->value, "declare")) keyword = KEYWORD_DECLARE;
                    else if (!strcmp(token->value, "?>")) keyword = CLOSE_PHP_BRACKET;

                    if (keyword != -1) {
                        state = keyword == KEYWORD_DECLARE ? STRICT_TYPES_STATE : START;
                    } else {
                        state = IDENTIFIER_STATE;
                    }
                    ungetc(current_char, fd);
                    if (keyword != -1) return (LEXICAL_FSM_TOKENS) keyword;
                }
                break;
            case IDENTIFIER_STATE:
                if (!strcmp(token->value, "$")) {
                    if (isalpha(current_char) || current_char == '_') {
                        string_append_char(token, current_char);
                    } else {
                        LEXICAL_ERROR("Identifier must start with letter or underscore");
                    }
                } else if (isalpha(current_char) || isdigit(current_char) || current_char == '_') {
                    string_append_char(token, current_char);
                } else {
                    state = START;
                    ungetc(current_char, fd);
                    return IDENTIFIER;
                }
                break;
            case PHP_BRACKET_STATE:
                if (tolower(current_char) == 'p' || tolower(current_char) == 'h') {
                    string_append_char(token, current_char);
                } else {
                    string_convert_by(token, tolower);
                    bool is_php_bracket = !strcmp(token->value, "<?") || !strcmp(token->value, "<?php");

                    if (!is_php_bracket) {
                            LEXICAL_ERROR("Invalid PHP open bracket");
                    }

                    state = START;
                    ungetc(current_char, fd);
                    return OPEN_PHP_BRACKET;
                }
                break;
            case STRICT_TYPES_STATE:
                if (current_char == '(') {
                    string_append_char(token, current_char);
                    return LEFT_PARENTHESIS;
                }
                if (current_char == 's' || current_char == 't' || current_char == 'r' ||
                    current_char == 'c' || current_char == 'e' ||
                    current_char == 'y' || current_char == 'p' || current_char == 'i' || current_char == '_') {
                    string_append_char(token, current_char);
                } else {
                    if (strcmp(token->value, "strict_types")) {
                        LEXICAL_ERROR("Invalid strict_types declaration");
                    }
                    state = START;
                    ungetc(current_char, fd);
                    return KEYWORD_STRICT_TYPES;
                }
                break;
            case EQUAL_STATE:
                if (current_char == '=')
                    string_append_char(token, current_char);
                else {
                    state = START;
                    ungetc(current_char, fd);

                    if (!strcmp(token->value, "=")) return ASSIGN;
                    else if (!strcmp(token->value, "==")) return EQUAL;
                    else if (!strcmp(token->value, "!=")) return NOT_EQUAL;
                    else if (!strcmp(token->value, "===")) return TYPED_EQUAL;
                    else if (!strcmp(token->value, "!==")) return TYPED_NOT_EQUAL;
                }
                break;
            case ARITHMETIC_STATE:
                if (current_char == '=' ||
                    (!strcmp(token->value, "+") && current_char == '+') ||
                    (!strcmp(token->value, "-") && current_char == '-')) {
                    string_append_char(token, current_char);
                } else if (!strcmp(token->value, "/")) {
                    switch (current_char) {
                        case '/':
                        case '*':
                            string_append_char(token, current_char);
                            state = current_char == '/' ? COMMENT_STATE : MULTILINE_COMMENT_STATE;
                            break;
                        default:
                            state = START;
                            ungetc(current_char, fd);
                            return DIVIDE;
                    }
                } else {
                    state = START;

                    if (current_char != '+' && current_char != '-' && current_char != '*' && current_char != '/') {
                        ungetc(current_char, fd);
                    } else {
                        string_append_char(token, current_char);
                    }
                    if (!strcmp(token->value, "+")) return PLUS;
                    else if (!strcmp(token->value, "-")) return MINUS;
                    else if (!strcmp(token->value, "*")) return MULTIPLY;
                    else if (!strcmp(token->value, "/")) return DIVIDE;
                    else if (!strcmp(token->value, "+=")) return PLUS_ASSIGN;
                    else if (!strcmp(token->value, "-=")) return MINUS_ASSIGN;
                    else if (!strcmp(token->value, "*=")) return MULTIPLY_ASSIGN;
                    else if (!strcmp(token->value, "/=")) return DIVIDE_ASSIGN;
                    else if (!strcmp(token->value, "--")) return DECREMENT;
                    else if (!strcmp(token->value, "++")) return INCREMENT;
                    else LEXICAL_ERROR("Invalid arithmetic operator");
                }
                break;
            case LOGICAL_STATE:
                if ((!strcmp(token->value, "&") && current_char == '&') ||
                    (!strcmp(token->value, "|") && current_char == '|')) {
                    string_append_char(token, current_char);
                } else {
                    state = START;
                    ungetc(current_char, fd);

                    if (!strcmp(token->value, "!")) {
                        if (current_char == '=') state = EQUAL_STATE;
                        else return LOGICAL_NOT;
                    } else if (!strcmp(token->value, "&&")) return LOGICAL_AND;
                    else if (!strcmp(token->value, "||")) return LOGICAL_OR;
                }
                break;
            case SQUARE_PARENTHESIS_STATE:
                if (current_char == '=' ||
                    (!strcmp(token->value, "<") && current_char == '?')) {
                    string_append_char(token, current_char);
                } else {
                    state = START;

                    if (!strcmp(token->value, "<")) return LESS;
                    else if (!strcmp(token->value, ">")) return GREATER;
                    else if (!strcmp(token->value, "<=")) return LESS_EQUAL;
                    else if (!strcmp(token->value, ">=")) return GREATER_EQUAL;
                    else if (!strcmp(token->value, "<?")) {
                        state = PHP_BRACKET_STATE;
                        ungetc(current_char, fd);
                    };
                }
                break;
            case INTEGER_STATE:
                if (isdigit(current_char) || (isalpha(current_char) && current_char != 'e' && current_char != 'E')) {
                    string_append_char(token, current_char);
                } else if (current_char == '.' || current_char == 'e' || current_char == 'E') {
                    state = FLOAT_STATE;
                    string_append_char(token, current_char);
                } else {
                    state = START;
                    ungetc(current_char, fd);

                    if (string_check_by(token, isdigit)) return INTEGER;
                    else {
                        LEXICAL_ERROR("Invalid integer number format");
                    }
                }
                break;
            case FLOAT_STATE:
                if (isdigit(current_char) || current_char == 'e' || current_char == 'E' || current_char == '+' ||
                    current_char == '-') {
                    string_append_char(token, current_char);
                } else {
                    state = START;
                    ungetc(current_char, fd);

                    regex_t regex;
                    int regex_comp = regcomp(&regex, "^[0-9]+\\.?[0-9]+([eE][+-]?[0-9]+)?$", REG_EXTENDED);
                    if (regex_comp != 0) {
                        LEXICAL_ERROR("Invalid float regexp");
                    }
                    int return_value = regexec(&regex, token->value, 0, NULL, 0);
                    if (!return_value) {
                        return FLOAT;
                    } else {
                        LEXICAL_ERROR("Invalid float number format");
                    }
                }
                break;
            case STRING_STATE:
                if (token->value[0] == '"' && current_char == '"' || token->value[0] == '\'' && current_char == '\'') {
                    state = START;
                    string_append_char(token, current_char);
                    return STRING;
                } else if (current_char == '\\') {
                    state = STRING_ESCAPE_STATE;
                    string_append_char(token, current_char);
                } else {
                    state = STRING_STATE;
                    string_append_char(token, current_char);
                }
                break;
            case STRING_ESCAPE_STATE:
                string_append_char(token, current_char);
                state = STRING_STATE;
                break;
            case COMMENT_STATE:
                if (current_char == '\n' || current_char == '\0' || current_char == EOF) {
                    string_clear(token);
                    state = START;
                }
                break;
            case MULTILINE_COMMENT_STATE:
                if (current_char == '*') {
                    current_char = (char) getc(fd);
                    if (current_char == '/') {
                        string_clear(token);
                        state = START;
                    } else
                        ungetc(current_char, fd);
                }
                break;
            default:
                break;
        }

        current_char = getc(fd);
    }

    return END_OF_FILE;
}


FILE *test_lex_input(char *input) {
    FILE *fd = fmemopen(input, strlen(input), "r");
    return fd;
}

lexical_token_t *get_token(FILE *fd) {
    string_t *token_string = string_init("");
    LEXICAL_FSM_TOKENS token_type = get_next_token(fd, token_string);

    lexical_token_t *token = (lexical_token_t *) malloc(sizeof(lexical_token_t));
    if (token == NULL) {
        INTERNAL_ERROR("Unable to allocate memory for lexical token");
    }

    token->type = token_type;
    token->value = token_string->value;

    return token;
}

lexical_token_stack_t *lexical_token_stack_init() {
    lexical_token_stack_t *stack = (lexical_token_stack_t *) malloc(sizeof(lexical_token_stack_t));
    if (stack == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for lexical token stack");
    }

    lexical_token_t *items = (lexical_token_t *) malloc(sizeof(lexical_token_t) * LEXICAL_TOKEN_STACK_INITIAL_SIZE);
    if (items == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for lexical token stack items");
    }

    stack->items = items;
    stack->top = -1;
    stack->capacity = LEXICAL_TOKEN_STACK_INITIAL_SIZE;

    return stack;
}

void lexical_token_stack_destroy(lexical_token_stack_t *stack) {
    if (stack == NULL) {
        INTERNAL_ERROR("Failed to destroy lexical token stack, stack is NULL");
    }

    free(stack->items);
    free(stack);
}

void lexical_token_stack_increase(lexical_token_stack_t *stack) {
    if (stack == NULL) {
        INTERNAL_ERROR("Failed to increase lexical token stack, stack is NULL");
    }

    int new_capacity = stack->capacity * 2;
    lexical_token_t *new_items = (lexical_token_t *) malloc(sizeof(lexical_token_t) * new_capacity);
    if (new_items == NULL) {
        INTERNAL_ERROR("Failed to increase lexical token stack items");
    }

    for (int i = 0; i <= stack->top; i++) {
        new_items[i] = stack->items[i];
    }

    free(stack->items);
    stack->items = new_items;
    stack->capacity = new_capacity;
}

bool lexical_token_stack_empty(lexical_token_stack_t *stack) {
    if (stack == NULL) {
        INTERNAL_ERROR("Failed to check if lexical token stack is empty, stack is NULL");
    }
    return stack->top < 0;
}

void lexical_token_stack_push(lexical_token_stack_t *stack, lexical_token_t token) {
    if (stack->top == stack->capacity - 1) {
        lexical_token_stack_increase(stack);
    }

    stack->items[++stack->top] = token;
}

lexical_token_t lexical_token_stack_pop(lexical_token_stack_t *stack) {
    if (lexical_token_stack_empty(stack)) {
        INTERNAL_ERROR("Failed to pop lexical token stack, stack is empty");
    }

    return stack->items[stack->top--];
}

int lexical_token_stack_size(lexical_token_stack_t *stack) {
    if (stack == NULL) {
        INTERNAL_ERROR("Failed to get size of lexical token stack, stack is NULL");
    }

    return stack->top + 1;
}
