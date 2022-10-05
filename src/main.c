//#include "args.h"
#include "lexical_fsm.h"

int main(int argc, char** argv) {
//    args_t *args = parse_args(argc, argv);
    char *token = malloc(sizeof(char));
    LEXICAL_FSM_TOKENS token_type = get_next_token(token);

    printf("Token: %s [Type: %d]\n", token, token_type);

    return 0;
}
