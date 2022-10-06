#include <gtest/gtest.h>
#include <stdlib.h>
extern "C" {
    #include "../src/lexical_fsm.h"
    #include "../src/lexical_fsm.c"
    #include "../src/lexical_token.h"
    #include "../src/lexical_token.c"
}

TEST(LexicalAnalyzer, TypedIntVariableDeclaration) {
    lexical_token_stack_t *stack = lexical_token_stack_init();

    string_t *token = string_init("");
    LEXICAL_FSM_TOKENS token_type = get_next_token(token);

    while (token_type != END_OF_FILE) {
        string_t *token_value = string_init(token->value);
        lexical_token_stack_push(stack, (lexical_token_t) {token_type, token_value->value});
        token_type = get_next_token(token);
    }

    lexical_token_t l_token_1 = lexical_token_stack_pop(stack);
    lexical_token_t l_token_2 = lexical_token_stack_pop(stack);
    lexical_token_t l_token_3 = lexical_token_stack_pop(stack);
    lexical_token_t l_token_4 = lexical_token_stack_pop(stack);
    lexical_token_t l_token_5 = lexical_token_stack_pop(stack);

    EXPECT_STREQ(l_token_1.value, ";");
    EXPECT_STREQ(l_token_2.value, "105");
    EXPECT_STREQ(l_token_3.value, "=");
    EXPECT_STREQ(l_token_4.value, "$abc");
    EXPECT_STREQ(l_token_5.value, "int");
}
