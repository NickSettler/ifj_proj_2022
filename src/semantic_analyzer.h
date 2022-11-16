#ifndef IFJ_PROJ_SEMANTIC_ANALYZER_H
#define IFJ_PROJ_SEMANTIC_ANALYZER_H

#include "syntax_analyzer.h"

/**
 * @brief Prints syntax tree in inorder
 * @param tree Syntax tree
 */
void printInorder(syntax_abstract_tree_t *tree);

/**
 * @brief Checks types of nodes and returns type of the node
 * @param tree
 * @return
 */
int get_data_type(syntax_abstract_tree_t *tree);

/**
 * @brief Checks types of two nodes
 * @param type_1 Type of first node
 * @param type_2 Type of second node
 * @return Type of both nodes
 */
int type_check(int type1, int type2);

void defined(syntax_abstract_tree_t *tree);

void create_global_id_node(syntax_abstract_tree_t *tree);

void tree_traversal(syntax_abstract_tree_t *tree);

#endif //IFJ_PROJ_SEMANTIC_ANALYZER_H
