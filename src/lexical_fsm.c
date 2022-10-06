#include "lexical_fsm.h"

int state = START;

char *test = "$a <> $b;"
             "10.5;"
             "int $a = 5;"
             "int $a = 5";
//             "(a + b) * c;"
//             "string $abc = \"Hello World\";"
//             "1 += 1;"
//             "int $b = 123;"
//             "$b++"
//             "$b /= 2;"
//             "$c = $d = $e = 2;"
//             "int $b = \"Hello World\";";

LEXICAL_FSM_TOKENS get_next_token(string_t *token) {
    const char *next_char = test;

    while (*next_char != '\0') {
        switch (state) {
            case START:
                string_clear(token);

                switch (*next_char) {
                    case ' ':
                        test++;
                        break;
                    case '?':
                        state = KEYWORD_STATE;
                        string_append_char(token, *next_char);
                        break;
                    case '$':
                        state = IDENTIFIER_START_STATE;
                        string_append_char(token, *next_char);
                        break;
                    case '=':
                        state = EQUAL_STATE;
                        string_append_char(token, *next_char);
                        break;
                    case ';':
                        string_append_char(token, *next_char);
                        test++;
                        return SEMICOLON;
                    case '"':
                        state = STRING_STATE;
                        string_append_char(token, *next_char);
                        break;
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                        state = ARITHMETIC_STATE;
                        string_append_char(token, *next_char);
                        break;
                    case ',':
                        string_append_char(token, *next_char);
                        test++;
                        return COMMA;
                    case '(':
                        string_append_char(token, *next_char);
                        test++;
                        return LEFT_PARENTHESIS;
                    case ')':
                        string_append_char(token, *next_char);
                        test++;
                        return RIGHT_PARENTHESIS;
                    case '{':
                        string_append_char(token, *next_char);
                        test++;
                        return LEFT_CURLY_BRACKETS;
                    case '}':
                        string_append_char(token, *next_char);
                        test++;
                        return RIGHT_CURLY_BRACKETS;
                    case '[':
                        string_append_char(token, *next_char);
                        test++;
                        return LEFT_SQUARE_BRACKETS;
                    case ']':
                        string_append_char(token, *next_char);
                        test++;
                        return RIGHT_SQUARE_BRACKETS;
                    case '<':
                    case '>':
                        state = SQUARE_PARENTHESIS_STATE;
                        string_append_char(token, *next_char);
                        break;

                    default:
                        if (isalpha(*next_char)) {
                            state = KEYWORD_STATE;
                            string_append_char(token, *next_char);
                            break;
                        } else if (isdigit(*next_char)) {
                            state = INTEGER_STATE;
                            string_append_char(token, *next_char);
                            break;
                        }
                        break;
                }
                break;
            case KEYWORD_STATE:
                if (isalpha(*next_char)) {
                    string_append_char(token, *next_char);
                } else {
                    state = START;
                    test += token->length;
                    if (strcmp(token->value, "int") == 0) return INTEGER;
                    else if (strcmp(token->value, "float") == 0) return FLOAT;
                    else if (strcmp(token->value, "string") == 0) return STRING;
                    else if (strcmp(token->value, "if") == 0) return IF;
                    else if (strcmp(token->value, "elseif") == 0) return ELSEIF;
                    else if (strcmp(token->value, "else") == 0) return ELSE;
                    else if (strcmp(token->value, "while") == 0) return WHILE;
                    else if (strcmp(token->value, "for") == 0) return FOR;
                    else if (strcmp(token->value, "do") == 0) return DO;
                    else if (strcmp(token->value, "return") == 0) return RETURN;
                    else return 100;
                }
                break;
            case IDENTIFIER_START_STATE:
                if (isalpha(*next_char) || *next_char == '_') {
                    state = IDENTIFIER_STATE;
                    string_append_char(token, *next_char);
                } else {
                    // TODO: handle Lexical error (identifier must start with letter or underscore)
                    state = START;
                    return END_OF_FILE;
                }
                break;
            case IDENTIFIER_STATE:
                if (isalpha(*next_char) || isdigit(*next_char) || *next_char == '_') {
                    string_append_char(token, *next_char);
                } else {
                    state = START;
                    test += token->length;
                    return IDENTIFIER;
                }
                break;
            case EQUAL_STATE:
                state = START;
                if (*next_char == '=')
                    string_append_char(token, *next_char);
                test += token->length;
                return *next_char == '=' ? EQUAL : ASSIGN;
            case ARITHMETIC_STATE:
                if (*next_char == '=' ||
                    (strcmp(token->value, "+") == 0 && *next_char == '+') ||
                    (strcmp(token->value, "-") == 0 && *next_char == '-')) {
                    string_append_char(token, *next_char);
                } else {
                    state = START;
                    test += token->length;

                    if (strcmp(token->value, "+") == 0) return PLUS;
                    else if (strcmp(token->value, "-") == 0) return MINUS;
                    else if (strcmp(token->value, "*") == 0) return MULTIPLY;
                    else if (strcmp(token->value, "/") == 0) return DIVIDE;
                    else if (strcmp(token->value, "+=") == 0) return PLUS_ASSIGN;
                    else if (strcmp(token->value, "-=") == 0) return MINUS_ASSIGN;
                    else if (strcmp(token->value, "*=") == 0) return MULTIPLY_ASSIGN;
                    else if (strcmp(token->value, "/=") == 0) return DIVIDE_ASSIGN;
                    else if (strcmp(token->value, "++") == 0) return INCREMENT;
                    else if (strcmp(token->value, "--") == 0) return DECREMENT;
                }
                break;
            case SQUARE_PARENTHESIS_STATE:
                if (*next_char == '=' ||
                    (strcmp(token->value, "<") == 0 && *next_char == '>')) {
                    string_append_char(token, *next_char);
                } else {
                    state = START;
                    test += token->length;

                    if (strcmp(token->value, "<") == 0) return LESS;
                    else if (strcmp(token->value, ">") == 0) return GREATER;
                    else if (strcmp(token->value, "<=") == 0) return LESS_EQUAL;
                    else if (strcmp(token->value, ">=") == 0) return GREATER_EQUAL;
                    else if (strcmp(token->value, "<>") == 0) return NOT_EQUAL;
                }
                break;
            case INTEGER_STATE:
                /* TODO: if digit is the last character in code, it is not handled as separate token
                 * for example: "1 == 1" will have 2 tokens: "1" and "==" instead of 3 tokens: "1", "==", "1"
                 */
                if (isdigit(*next_char)) {
                    string_append_char(token, *next_char);
                } else if (*next_char == '.') {
                    state = FLOAT_STATE;
                    string_append_char(token, *next_char);
                }
                else {
                    // TODO handle Lexical error (integer must not contain any other characters)
                    state = START;
                    test += token->length;
                    return INTEGER;
                }
                break;
            case FLOAT_STATE:
                if (isdigit(*next_char)) {
                    string_append_char(token, *next_char);
                } else {
                    // TODO handle Lexical error (float must not contain any other characters)
                    state = START;
                    test += token->length;
                    return FLOAT;
                }
                break;
            case STRING_STATE:
                if (*next_char == '"') {
                    state = START;
                    string_append_char(token, *next_char);
                    test += token->length;
                    return STRING;
                } else if (*next_char == '\\') {
                    state = STRING_ESCAPE_STATE;
                    string_append_char(token, *next_char);
                } else {
                    state = STRING_STATE;
                    string_append_char(token, *next_char);
                }
                break;
            case STRING_ESCAPE_STATE:
                string_append_char(token, *next_char);
                state = STRING_STATE;
                break;
            default:
                break;
        }

        ++next_char;
    }

    return END_OF_FILE;
}
