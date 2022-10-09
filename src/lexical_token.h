#ifndef IFJ_PROC_LEXICAL_TOKEN_H
#define IFJ_PROC_LEXICAL_TOKEN_H

#include <stdbool.h>
#include "lexical_fsm.h"
#include "errors.h"

#define LEXICAL_TOKEN_STACK_INITIAL_SIZE 1

typedef struct lexical_token {
    LEXICAL_FSM_TOKENS type;
    char *value;
} lexical_token_t;

typedef struct lexical_token_stack_item lexical_token_stack_item_t;

struct lexical_token_stack_item {
    lexical_token_t *token;
    lexical_token_stack_item_t *next;
};

typedef struct lexical_token_stack {
    lexical_token_t *items;
    int top;
    int capacity;
} lexical_token_stack_t;

lexical_token_stack_t *lexical_token_stack_init();

void lexical_token_stack_destroy(lexical_token_stack_t *stack);

void lexical_token_stack_increase(lexical_token_stack_t *stack);

bool lexical_token_stack_empty(lexical_token_stack_t *stack);

void lexical_token_stack_push(lexical_token_stack_t *stack, lexical_token_t token);

lexical_token_t lexical_token_stack_pop(lexical_token_stack_t *stack);

int lexical_token_stack_size(lexical_token_stack_t *stack);


#endif //IFJ_PROC_LEXICAL_TOKEN_H
