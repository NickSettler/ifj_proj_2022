#include "lexical_token.h"

lexical_token_stack_t *lexical_token_stack_init() {
    lexical_token_stack_t *stack = (lexical_token_stack_t *) malloc(sizeof(lexical_token_stack_t));
    if (stack == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for lexical token stack");
    }

    lexical_token_t *items = (lexical_token_t *) malloc(sizeof(lexical_token_t) * LEXICAL_TOKEN_STACK_INITIAL_SIZE);
    if (items == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for lexical token stack items");
    }

    stack->items = items;
    stack->top = -1;
    stack->capacity = LEXICAL_TOKEN_STACK_INITIAL_SIZE;

    return stack;
}

void lexical_token_stack_destroy(lexical_token_stack_t *stack) {
    if (stack == NULL) {
        INTERNAL_ERROR("Failed to destroy lexical token stack, stack is NULL");
    }

    free(stack->items);
    free(stack);
}

void lexical_token_stack_increase(lexical_token_stack_t *stack) {
    if (stack == NULL) {
        INTERNAL_ERROR("Failed to increase lexical token stack, stack is NULL");
    }

    int new_capacity = stack->capacity * 2;
    lexical_token_t *new_items = (lexical_token_t *) malloc(sizeof(lexical_token_t) * new_capacity);
    if (new_items == NULL) {
        INTERNAL_ERROR("Failed to increase lexical token stack items");
    }

    for (int i = 0; i <= stack->top; i++) {
        new_items[i] = stack->items[i];
    }

    free(stack->items);
    stack->items = new_items;
    stack->capacity = new_capacity;
}

bool lexical_token_stack_empty(lexical_token_stack_t *stack) {
    if (stack == NULL) {
        INTERNAL_ERROR("Failed to check if lexical token stack is empty, stack is NULL");
    }
    return stack->top < 0;
}

void lexical_token_stack_push(lexical_token_stack_t *stack, lexical_token_t token) {
    if (stack->top == stack->capacity - 1) {
        lexical_token_stack_increase(stack);
    }

    stack->items[++stack->top] = token;
}

lexical_token_t lexical_token_stack_pop(lexical_token_stack_t *stack) {
    if (lexical_token_stack_empty(stack)) {
        INTERNAL_ERROR("Failed to pop lexical token stack, stack is empty");
    }

    return stack->items[stack->top--];
}

int lexical_token_stack_size(lexical_token_stack_t *stack) {
    if (stack == NULL) {
        INTERNAL_ERROR("Failed to get size of lexical token stack, stack is NULL");
    }

    return stack->top + 1;
}