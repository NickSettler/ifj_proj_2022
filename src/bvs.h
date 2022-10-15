#ifndef IFJ_PROJ_2022_BVS_H
#define IFJ_PROJ_2022_BVS_H

#endif //IFJ_PROJ_2022_BVS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct tree_node {
    int value;
    struct tree_node *left;
    struct tree_node *right;
} tree_node;

/**
 * Creates tree node with value
 * @param value value in the node
 * @return pointer to the tree node
 */
tree_node *create_node(int value);

/**
 * Compare value with value in the node
 * @param root pointer to the node
 * @param value value to compare
 * @return 0 if values are equal, -1 if value in the node is greater, 1 if value in the node is smaller
 */
int comparator(tree_node *root, int value);

/**
 * Inserts value into the tree
 * @param rootptr pointer to the pointer to the root of the tree
 * @param value value to insert
 * @return true if value was inserted, false if value already exists in the tree
 */
bool insert_number(tree_node **rootptr, int value);

/**
 * Finds value in the tree
 * @param root pointer to the root of the tree
 * @param value value to find
 * @return true if value was found, false if value was not found
 */
bool find_number(tree_node *root, int value);

/**
 * Deletes value from the tree
 * @param rootptr pointer to the pointer to the root of the tree
 * @param value value to delete
 * @return true if value was deleted, false if value was not found
 */
bool delete_number(tree_node **rootptr, int value);


