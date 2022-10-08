//#include "args.h"
#include "lexical_fsm.h"
#include "lexical_token.h"

char *test = "$a <> $b;"
             "10.5;"
             "int $a = 5;"
             "int $a = b";
//             "(a + b) * c;"
//             "string $abc = \"Hello World\";"
//             "1 += 1;"
//             "int $b = 123;"
//             "$b++"
//             "$b /= 2;"
//             "$c = $d = $e = 2;"
//             "int $b = \"Hello World\";";

FILE *test_lex_input() {
    FILE *fd = fmemopen(test, strlen(test), "r");
    return fd;
}

int main(int argc, char **argv) {
//    args_t *args = parse_args(argc, argv);
    lexical_token_stack_t *stack = lexical_token_stack_init();
    FILE *fd = test_lex_input();

    string_t *token = string_init("");
    LEXICAL_FSM_TOKENS token_type = get_next_token(fd, token);

    while (token_type != END_OF_FILE) {
        string_t *token_value = string_init(token->value);
        lexical_token_stack_push(stack, (lexical_token_t) {token_type, token_value->value});
        token_type = get_next_token(fd, token);
    }

    while (!lexical_token_stack_empty(stack)) {
        lexical_token_t ltoken = lexical_token_stack_pop(stack);
        printf("Token type: %d, value: %s\n", ltoken.type, ltoken.value);
    }

    return 0;
}
