/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmaroc00, Elena Marochkina
 *
 * @file symtable.h
 * @brief Binary search tree
 * @date 21.10.2022
 */

#ifndef IFJ_PROJ_2022_SYMTABLE_H
#define IFJ_PROJ_2022_SYMTABLE_H

#endif //IFJ_PROJ_2022_SYMTABLE_H
#include "errors.h"
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
 * @param key identifier in the node
 * @return pointer to the tree node
 */
tree_node *create_node(char *key);

/**
 * Compare key with key in the node
 * @param root pointer to the node
 * @param key value to compare
 * @return 0 if values are equal, -1 if value in the node is greater, 1 if value in the node is smaller
 */
int comparator(tree_node *root, char *key);

/**
 * Inserts value into the tree
 * @param rootptr pointer to the pointer to the root of the tree
 * @param value value to insert
 * @return true if value was inserted, false if value already exists in the tree
 */
bool insert_element(tree_node **rootptr, char *key);

/**
 * Finds value in the tree
 * @param root pointer to the root of the tree
 * @param value value to find
 * @return true if value was found, false if value was not found
 */
bool find_element(tree_node *root, char *key);

/**
 * Deletes value from the tree
 * @param rootptr pointer to the pointer to the root of the tree
 * @param value value to delete
 * @return true if value was deleted, false if value was not found
 */
bool delete_element(tree_node **rootptr, char *key);

//next 2 functions only for testing
void printtabs(int numtabs);

void print_tree(tree_node *root, int level);
