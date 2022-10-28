/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmaroc00, Elena Marochkina
 *
 * @file symtable.h
 * @brief Binary search tree
 * @date 28.10.2022
 */

#ifndef IFJ_PROJ_2022_SYMTABLE_H
#define IFJ_PROJ_2022_SYMTABLE_H

#endif //IFJ_PROJ_2022_SYMTABLE_H
#include "errors.h"
#include "lexical_fsm.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct tree_node {
    char *key;
    struct tree_node *left;
    struct tree_node *right;
} tree_node;

/**
 * Creates tree node with key
 * @param pointer to the token
 * @return pointer to the tree node
 */
tree_node *create_node(lexical_token_t *token);

/**
 * Compare token value with key in the node
 * @param root pointer to the node
 * @param pointer to the token, which value is compared with key
 * @return 0 if values are equal, -1 if value in the node is greater, 1 if value in the node is smaller
 */
int comparator(tree_node *root, lexical_token_t *token);

/**
 * Inserts value into the tree
 * @param rootptr pointer to the pointer to the root of the tree
 * @param pointer to the token, which value will be inserted
 * @return true if key was inserted, false if key already exists in the tree
 */
bool insert_element(tree_node **rootptr, lexical_token_t *token);

/**
 * Finds value in the tree
 * @param root pointer to the root of the tree
 * @param pointer to the token, which value is searched
 * @return  if key was found
 */
tree_node *find_element (tree_node *root, lexical_token_t *token);

/**
 * Deletes value from the tree
 * @param rootptr pointer to the pointer to the root of the tree
 * @param pointer to the token, which value will be deleted
 * @return true if value was deleted, false if value was not found
 */
bool delete_element(tree_node **rootptr, lexical_token_t *token);

//next 2 functions only for testing
void printtabs(int numtabs);

void print_tree(tree_node *root, int level);
