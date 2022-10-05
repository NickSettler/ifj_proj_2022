//#include "args.h"
#include "lexical_fsm.h"

int main(int argc, char** argv) {
//    args_t *args = parse_args(argc, argv);
    char *token = malloc(sizeof(char));
    get_next_token(token);

    printf("Token: %s", token);

    return 0;
}
