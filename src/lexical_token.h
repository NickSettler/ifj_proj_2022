#ifndef IFJ_PROC_LEXICAL_TOKEN_H
#define IFJ_PROC_LEXICAL_TOKEN_H

#include <stdbool.h>
#include "lexical_fsm.h"
#include "errors.h"

#define LEXICAL_TOKEN_STACK_INITIAL_SIZE 1

/**
 * @struct lexical_token_t
 * Lexical token structure
 *
 * @var lexical_token_t::type
 * Lexical token type
 *
 * @var lexical_token_t::value
 * Lexical token value
 */
typedef struct lexical_token {
    LEXICAL_FSM_TOKENS type;
    char *value;
} lexical_token_t;

/**
 * @struct lexical_token_stack_t
 * Lexical token stack structure
 *
 * @var lexical_token_stack_t::items
 * Lexical token stack items
 *
 * @var lexical_token_stack_t::top
 * Pointer to top of lexical token stack
 *
 * @var lexical_token_stack_t::capacity
 * Lexical token stack capacity
 */
typedef struct lexical_token_stack {
    lexical_token_t *items;
    int top;
    int capacity;
} lexical_token_stack_t;

/**
 * Initializes lexical token stack
 * @return pointer to lexical token stack
 */
lexical_token_stack_t *lexical_token_stack_init();

/**
 * Destroys lexical token stack
 * @param stack pointer to lexical token stack
 */
void lexical_token_stack_destroy(lexical_token_stack_t *stack);

/**
 * Increases lexical token stack capacity twice
 * @param stack
 */
void lexical_token_stack_increase(lexical_token_stack_t *stack);

/**
 * Checks if lexical token stack is empty
 * @param stack pointer to lexical token stack
 * @return true if stack is empty, false otherwise
 */
bool lexical_token_stack_empty(lexical_token_stack_t *stack);

/**
 * Pushes lexical token to stack
 * @param stack pointer to lexical token stack
 * @param token lexical token
 */
void lexical_token_stack_push(lexical_token_stack_t *stack, lexical_token_t token);

/**
 * Pops lexical token from stack
 * @param stack pointer to lexical token stack
 * @return lexical token
 */
lexical_token_t lexical_token_stack_pop(lexical_token_stack_t *stack);

/**
 * Returns lexical token stack size
 * @param stack pointer to lexical token stack
 * @return size of lexical token stack
 */
int lexical_token_stack_size(lexical_token_stack_t *stack);


#endif //IFJ_PROC_LEXICAL_TOKEN_H
