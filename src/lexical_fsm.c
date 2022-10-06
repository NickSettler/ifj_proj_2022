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
    const char *current_char = test;

    while (*current_char != '\0') {
        switch (state) {
            case START:
                string_clear(token);

                switch (*current_char) {
                    case ' ':
                        test++;
                        break;
                    case '?':
                        state = KEYWORD_STATE;
                        string_append_char(token, *current_char);
                        break;
                    case '$':
                        state = IDENTIFIER_START_STATE;
                        string_append_char(token, *current_char);
                        break;
                    case '=':
                        state = EQUAL_STATE;
                        string_append_char(token, *current_char);
                        break;
                    case ';':
                        string_append_char(token, *current_char);
                        test++;
                        return SEMICOLON;
                    case '"':
                        state = STRING_STATE;
                        string_append_char(token, *current_char);
                        break;
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                        state = ARITHMETIC_STATE;
                        string_append_char(token, *current_char);
                        break;
                    case ',':
                        string_append_char(token, *current_char);
                        test++;
                        return COMMA;
                    case '(':
                        string_append_char(token, *current_char);
                        test++;
                        return LEFT_PARENTHESIS;
                    case ')':
                        string_append_char(token, *current_char);
                        test++;
                        return RIGHT_PARENTHESIS;
                    case '{':
                        string_append_char(token, *current_char);
                        test++;
                        return LEFT_CURLY_BRACKETS;
                    case '}':
                        string_append_char(token, *current_char);
                        test++;
                        return RIGHT_CURLY_BRACKETS;
                    case '[':
                        string_append_char(token, *current_char);
                        test++;
                        return LEFT_SQUARE_BRACKETS;
                    case ']':
                        string_append_char(token, *current_char);
                        test++;
                        return RIGHT_SQUARE_BRACKETS;
                    case '<':
                    case '>':
                        state = SQUARE_PARENTHESIS_STATE;
                        string_append_char(token, *current_char);
                        break;

                    default:
                        if (isalpha(*current_char)) {
                            state = KEYWORD_STATE;
                            string_append_char(token, *current_char);
                            break;
                        } else if (isdigit(*current_char)) {
                            state = INTEGER_STATE;
                            string_append_char(token, *current_char);
                            if (*(current_char + 1) == '\0') return INTEGER;
                            break;
                        }
                        break;
                }
                break;
            case KEYWORD_STATE:
                if (isalpha(*current_char)) {
                    string_append_char(token, *current_char);
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
                if (isalpha(*current_char) || *current_char == '_') {
                    state = IDENTIFIER_STATE;
                    string_append_char(token, *current_char);
                } else {
                    // TODO: handle Lexical error (identifier must start with letter or underscore)
                    state = START;
                    return END_OF_FILE;
                }
                break;
            case IDENTIFIER_STATE:
                if (isalpha(*current_char) || isdigit(*current_char) || *current_char == '_') {
                    string_append_char(token, *current_char);
                } else {
                    state = START;
                    test += token->length;
                    return IDENTIFIER;
                }
                break;
            case EQUAL_STATE:
                state = START;
                if (*current_char == '=')
                    string_append_char(token, *current_char);
                test += token->length;
                return *current_char == '=' ? EQUAL : ASSIGN;
            case ARITHMETIC_STATE:
                if (*current_char == '=' ||
                    (strcmp(token->value, "+") == 0 && *current_char == '+') ||
                    (strcmp(token->value, "-") == 0 && *current_char == '-')) {
                    string_append_char(token, *current_char);
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
                if (*current_char == '=' ||
                    (strcmp(token->value, "<") == 0 && *current_char == '>')) {
                    string_append_char(token, *current_char);
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
                if (isdigit(*current_char)) {
                    string_append_char(token, *current_char);
                } else if (*current_char == '.') {
                    state = FLOAT_STATE;
                    string_append_char(token, *current_char);
                } else {
                    // TODO handle Lexical error (integer must not contain any other characters)
                    state = START;
                    test += token->length;
                    return INTEGER;
                }
                break;
            case FLOAT_STATE:
                if (isdigit(*current_char)) {
                    string_append_char(token, *current_char);
                } else {
                    // TODO handle Lexical error (float must not contain any other characters)
                    state = START;
                    test += token->length;
                    return FLOAT;
                }
                break;
            case STRING_STATE:
                if (*current_char == '"') {
                    state = START;
                    string_append_char(token, *current_char);
                    test += token->length;
                    return STRING;
                } else if (*current_char == '\\') {
                    state = STRING_ESCAPE_STATE;
                    string_append_char(token, *current_char);
                } else {
                    state = STRING_STATE;
                    string_append_char(token, *current_char);
                }
                break;
            case STRING_ESCAPE_STATE:
                string_append_char(token, *current_char);
                state = STRING_STATE;
                break;
            default:
                break;
        }

        ++current_char;
    }

    return END_OF_FILE;
}
