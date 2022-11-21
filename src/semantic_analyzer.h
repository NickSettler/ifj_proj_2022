#ifndef IFJ_PROJ_SEMANTIC_ANALYZER_H
#define IFJ_PROJ_SEMANTIC_ANALYZER_H

#include <stdbool.h>
#include "str.h"
#include "symtable.h"

typedef struct semantic_analyzer {
    bool FUNCTION_SCOPE;
    char *function_name;
    int argument_count;
} semantic_analyzer_t;

typedef struct syntax_abstract_tree syntax_abstract_tree_t;

/**
 * Runs semantic analyzer on a tree
 * @param tree Abstract syntax tree
 */
void semantic_tree_check(syntax_abstract_tree_t *tree);

semantic_analyzer_t *init_semantic_state();

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
 * Checks if node is defined
 * @param tree Abstract syntax tree
 */
bool check_defined(syntax_abstract_tree_t *tree);

void get_return_type(syntax_abstract_tree_t *tree);

void insert_arguments(syntax_abstract_tree_t *tree);


#endif //IFJ_PROJ_SEMANTIC_ANALYZER_H
