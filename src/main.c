/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *
 * @file main.c
 * @brief Main file
 * @date 24.09.2022
 */
#include "symtable.h"
#include "syntax_analyzer.h"
#include "semantic_analyzer.h"
#include "optimiser.h"
#include "code_generator.h"

int main(int argc, char **argv) {
    FILE *input = stdin;

    syntax_abstract_tree_t *tree = load_syntax_tree(input);

    semantic_tree_check(tree);

    optimize_tree(tree);

    set_code_gen_output(stdout);
    code_generator_init();
    generate_header();
    parse_tree(tree);
    generate_exit(0);

    parse_func_dec(tree);

    semantic_analyzer_t *current_semantic_state = get_semantic_state();

    if (current_semantic_state->used_functions & (SEMANTIC_INTVAL | SEMANTIC_FLOATVAL))
        generate_number_conversion_functions();
    if (current_semantic_state->used_functions & (SEMANTIC_STRVAL))
        generate_strval();
    if (current_semantic_state->used_functions & SEMANTIC_SUBSTRING)
        generate_substr();
    if (current_semantic_state->used_functions & SEMANTIC_ORD)
        generate_ord();

    dispose_symtable();
//    free_syntax_tree(tree);

    return 0;
}
