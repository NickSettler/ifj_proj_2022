#ifndef IFJ_PROJ_SEMANTIC_ANALYZER_H
#define IFJ_PROJ_SEMANTIC_ANALYZER_H

#include <stdbool.h>

typedef struct syntax_abstract_tree syntax_abstract_tree_t;

/**
 * @brief Runs semantic analyzer on a tree
 * @param tree Abstract syntax tree
 */
void semantic_tree_check(syntax_abstract_tree_t *tree);

/**
 * @brief Runs semantic analyzer on all nodes in the syntax tree
 * @param tree Abstract syntax tree
 */
void process_tree(syntax_abstract_tree_t *tree);

/**
 * @brief Runs semantic analyzer on assign node
 * @param tree Abstract syntax tree
 */
void process_assign(syntax_abstract_tree_t *tree);

/**
 * @brief Checks types of nodes and returns type of the node
 * @param tree Abstract syntax tree
 * @return Type of the node
 */
int get_data_type(syntax_abstract_tree_t *tree);

/**
 * @brief Checks tree for float nodes
 * @param tree Abstract syntax tree
 */
void check_tree_for_float(syntax_abstract_tree_t *tree);

/**
 * @brief Replaces an int node to float
 * @param tree Abstract syntax tree
 */
void replace_node_int_to_float(syntax_abstract_tree_t *tree);

/**
 * @brief Checks types of two nodes
 * @param type_1 Type of first node
 * @param type_2 Type of second node
 * @return Type of both nodes
 */
int type_check(int type1, int type2);

/**
 * @brief Checks if node is int
 * @param tree Abstract syntax tree
 * @return True if node is int, false otherwise
 */
bool is_node_an_int(syntax_abstract_tree_t *tree);

/**
 * @brief Checks if node contains only int or float nodes
 * @param tree Abstract syntax tree
 * @return True if node is only numbers, false otherwise
 */
bool is_only_numbers(syntax_abstract_tree_t *tree);

bool is_str_an_int(string_t *str);

bool is_str_a_float(string_t *str);

int string_conversion(syntax_abstract_tree_t *tree);

#endif //IFJ_PROJ_SEMANTIC_ANALYZER_H
