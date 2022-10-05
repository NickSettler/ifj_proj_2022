#include "lexical_fsm.h"

int state = START;

char *test = "int $abc = 105;";

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
                        string_append_char(token, *next_char);
                        test++;
                        return ASSIGN;
                    case ';':
                        string_append_char(token, *next_char);
                        test++;
                        return SEMICOLON;
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
                    if (strcmp(token->value, "int") != 0) return INTEGER;
                    else if (strcmp(token->value, "float") != 0) return FLOAT;
                    else if (strcmp(token->value, "string") != 0) return STRING;
                }
                break;
            case IDENTIFIER_START_STATE:
                if (isalpha(*next_char) || *next_char == '_') {
                    state = IDENTIFIER_STATE;
                    string_append_char(token, *next_char);
                } else {
                    return -1;
                    // TODO: handle Lexical error (identifier must start with letter or underscore)
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
            case INTEGER_STATE:
                if (isdigit(*next_char)) {
                    string_append_char(token, *next_char);
                } else if (*next_char == '.') {
                    state = START;
                    test += token->length;
                    return INTEGER;
                } else {
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
            default:
                break;
        }

        ++next_char;
    }

    return END_OF_FILE;
}
