#include "lexical_fsm.h"

int state = START;

const char *test = "int $abc = 105;";

int get_next_token(char *token) {
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
                    char new_token[strlen(token) + 1];
                    strcpy(new_token, token);
                    new_token[strlen(token)] = *next_char;
                    token = realloc(token, strlen(token) + 1);
                    strcpy(token, new_token);
                    break;
                }
                break;
            case IDENTIFIER_STATE:
                if(isalpha(*next_char)) {
                    char new_token[strlen(token) + 1];
                    strcpy(new_token, token);
                    new_token[strlen(token)] = *next_char;
                    token = realloc(token, strlen(token) + 1);
                    strcpy(token, new_token);
                    break;
                }
                break;
        }

        ++next_char;
    }


    return 0;
}