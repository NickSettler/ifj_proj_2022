#include "lexical_fsm.h"

int state = START;

const char *test = "int $abc = 105;";

LEXICAL_FSM_TOKENS get_next_token(char *token) {
    const char *next_char = test;

    while (*next_char != '\0') {
        switch (state) {
            case START:
                if (*next_char == '\n' || *next_char == ' ') {
                    break;
                }
                if (*next_char == '?') {
                    state = KEYWORD_STATE;
                    strcat(token, next_char);
                    break;
                }
                if (*next_char == '$') {
                    state = IDENTIFIER_STATE;
                    string_add_char(token, *next_char);
                    break;
                }
                break;
            case IDENTIFIER_STATE:
                if (isalpha(*next_char) || isdigit(*next_char) || *next_char == '_') {
                    string_add_char(token, *next_char);
                    break;
                } else {
                    state = START;
                    return IDENTIFIER;
                }
            default:
                break;
        }

        ++next_char;
    }

    return 0;
}