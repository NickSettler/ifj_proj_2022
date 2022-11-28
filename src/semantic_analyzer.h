/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *   xpasyn00, Nikita Pasynkov
 *   xmaroc00, Elena Marochkina
 *
 * @file semantic_analyzer.h
 * @brief Semantic analyzer
 * @date 05.10.2022
 */

#ifndef IFJ_PROJ_SEMANTIC_ANALYZER_H
#define IFJ_PROJ_SEMANTIC_ANALYZER_H

#include <stdbool.h>
#include "str.h"
#include "symtable.h"

typedef enum {
    SEMANTIC_READI = 1 << 0,
    SEMANTIC_READF = 1 << 1,
    SEMANTIC_READS = 1 << 2,
    SEMANTIC_WRITE = 1 << 3,
    SEMANTIC_STRLEN = 1 << 4,
    SEMANTIC_ORD = 1 << 5,
    SEMANTIC_CHR = 1 << 6,
    SEMANTIC_SUBSTRING = 1 << 7,
    SEMANTIC_INTVAL = 1 << 8,
    SEMANTIC_FLOATVAL = 1 << 9,
} semantic_internal_functions;

typedef struct semantic_analyzer {
    bool FUNCTION_SCOPE;
    char *function_name;
    int argument_count;
    tree_node_t *symtable_ptr;
    semantic_internal_functions used_functions;
} semantic_analyzer_t;

typedef struct syntax_abstract_tree syntax_abstract_tree_t;

/**
 * Runs semantic analyzer on a tree
 * @param tree Abstract syntax tree
 */
void semantic_tree_check(syntax_abstract_tree_t *tree);

/**
 * Initializes semantic analyzer
 * @return Pointer to semantic analyzer
 */
semantic_analyzer_t *init_semantic_state();

/**
 * Return semantic state
 * @return semantic state
 */
semantic_analyzer_t *get_semantic_state();

/**
 * Runs semantic analyzer on all nodes in the syntax tree
 * @param tree Abstract syntax tree
 */
void process_tree(syntax_abstract_tree_t *tree);

/**
 * Runs semantic analyzer on if/while node
 * @param tree Abstract syntax tree
 */
void process_if_while(syntax_abstract_tree_t *tree);

/**
 * Runs semantic analyzer on assign node
 * @param tree Abstract syntax tree
 */
void process_assign(syntax_abstract_tree_t *tree);

void process_function_declaration(syntax_abstract_tree_t *tree);

/**
 * Checks if function return type is correct
 * @param tree Abstract syntax tree
 */
void check_for_return_value(syntax_abstract_tree_t *tree);

/**
 * Runs semantic analyzer on function call
 * @param tree Abstract syntax tree
 */
void process_call(syntax_abstract_tree_t *tree);

/**
 * Checks types of nodes and returns type of the node
 * @param tree Abstract syntax tree
 * @return Type of the node
 */
data_type get_data_type(syntax_abstract_tree_t *tree);

/**
 * Checks tree for float nodes
 * @param tree Abstract syntax tree
 */
void check_tree_for_float(syntax_abstract_tree_t *tree);

/**
 * Replaces an int node to float
 * @param tree Abstract syntax tree
 */
void replace_node_int_to_float(syntax_abstract_tree_t *tree);

/**
 * Checks tree for string nodes
 * @param tree Abstract syntax tree
 */
void check_tree_for_string(syntax_abstract_tree_t *tree);

/**
 * Replaces int or float nodes to string
 * @param tree Abstract syntax tree
 */
void replace_node_to_string(syntax_abstract_tree_t *tree);

/**
 * Checks types of two nodes
 * @param type_1 Type of first node
 * @param type_2 Type of second node
 * @return Type of both nodes
 */
data_type type_check(data_type type1, data_type type2);

/**
 * Checks if node is int
 * @param tree Abstract syntax tree
 * @return True if node is int, false otherwise
 */
bool is_node_an_int(syntax_abstract_tree_t *tree);

/**
 * Checks if node contains only int or float nodes
 * @param tree Abstract syntax tree
 * @return True if node is only numbers, false otherwise
 */
bool is_only_numbers(syntax_abstract_tree_t *tree);

/**
 * Checks if node is defined, if not, throws an error
 * @param tree Abstract syntax tree
 */
bool check_defined(syntax_abstract_tree_t *tree);

/**
 * Checks if node is defined
 * @param tree Abstract syntax tree
 */
bool is_defined(syntax_abstract_tree_t *tree);

/**
 * Insert return_type of function to symtable
 * @param tree Abstract syntax tree
 */
void set_return_type(syntax_abstract_tree_t *tree);

/**
 * Insert arguments of function to symtable
 * @param tree Abstract syntax tree
 */
void insert_arguments(syntax_abstract_tree_t *tree);

/**
 * Counts number of arguments in function declaration
 * @param tree Abstract syntax tree
 */
int count_arguments(syntax_abstract_tree_t *tree);

/**
 * Creates an array of arguments from function declaration
 */
void create_args_array();

/**
 * Checks if arguments in function call are the same as in function declaration
 * @param tree Abstract syntax tree
 * @param arg_array_ptr Array of function arguments
 * @param counter Counter
 */
void compare_arguments(syntax_abstract_tree_t *tree, data_type *arg_array_ptr, int counter);

/**
 * Checks if symtable_ptr is in function scope
 * @return
 */
void semantic_state_ptr();

#endif //IFJ_PROJ_SEMANTIC_ANALYZER_H
