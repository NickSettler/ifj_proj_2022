//#include "args.h"
#include "lexical_fsm.h"
#include "lexical_token.h"

int main(int argc, char **argv) {
//    args_t *args = parse_args(argc, argv);
    lexical_token_stack_t *stack = lexical_token_stack_init();

    string_t *token = string_init("");
    LEXICAL_FSM_TOKENS token_type = get_next_token(token);

    while (token_type != END_OF_FILE) {
        string_t *token_value = string_init(token->value);
        lexical_token_stack_push(stack, (lexical_token_t) {token_type, token_value->value});
        token_type = get_next_token(token);
    }

    while (!lexical_token_stack_empty(stack)) {
        lexical_token_t ltoken = lexical_token_stack_pop(stack);
        printf("Token type: %d, value: %s\n", ltoken.type, ltoken.value);
    }

    return 0;
}
