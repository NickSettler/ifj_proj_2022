#include "lexical_token.h"

lexical_token_stack_t *lexical_token_stack_init() {
    lexical_token_stack_t *stack = (lexical_token_stack_t *) malloc(sizeof(lexical_token_stack_t));

    stack->top = NULL;

    return stack;
}

bool lexical_token_stack_empty(lexical_token_stack_t *stack) {
    return stack->top == NULL;
}

void lexical_token_stack_push(lexical_token_stack_t *stack, lexical_token_t token) {
    lexical_token_stack_item_t *item = (lexical_token_stack_item_t *) malloc(sizeof(lexical_token_stack_item_t));

    item->token = token;
    item->next = stack->top;

    stack->top = item;
}

lexical_token_t lexical_token_stack_pop(lexical_token_stack_t *stack) {
    if (lexical_token_stack_empty(stack)) {
        // TODO handle error (maybe)
    }

    lexical_token_stack_item_t *item = stack->top;
    lexical_token_t token = item->token;

    stack->top = item->next;

    free(item);

    return token;
}
