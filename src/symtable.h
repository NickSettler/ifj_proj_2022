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

#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lexical_fsm.h"

/**
 * @struct tree_node_t
 * Binary search tree node
 *
 * @var tree_node_t::left
 * Left child
 *
 * @var tree_node_t::right
 * Right child
 *
 * @var tree_node_t::key
 * Key
 */

typedef enum {
    TYPE_STRING,    // Data type is string
    TYPE_INT,        // Data type is integer
    TYPE_FLOAT,    // Data type is float
} Data_type;

typedef struct tree_node {
    Data_type type; // Data type of symbol / return type of function
    bool defined; // Defined if current function was defined
    bool global;  // Global if current symbol is global
    char *key;  // Key is identifier
    struct tree_node *left;
    struct tree_node *right;
} tree_node_t;

tree_node_t *symtable;

/**
 * Creates tree node with key
 * @param pointer to the token
 * @return pointer to the tree node
 */
tree_node_t *create_node(lexical_token_t *token);

/**
 * Compare token value with key in the node
 * @param root pointer to the node
 * @param pointer to the token, which value is compared with key
 * @return 0 if values are equal, -1 if value in the node is greater, 1 if value in the node is smaller
 */
int comparator(tree_node_t *root, char *key);

/**
 * Inserts value into the tree
 * @param rootptr pointer to the pointer to the root of the tree
 * @param pointer to the token, which value will be inserted
 * @return true if key was inserted, false if key already exists in the tree
 */
bool insert_element(tree_node_t **rootptr, lexical_token_t *token);

/**
 * Initialise tree, insert tokens in the course of lexical analysis
 * @param token pointer to the token
 * @return true if token was inserted, false otherwise
 */
bool insert_token(lexical_token_t *token);

/**
 * Finds value in the tree
 * @param root pointer to the root of the tree
 * @param pointer to the token, which value is searched
 * @return  if key was found
 */
tree_node_t *find_element(tree_node_t *root, char *key);

/**
 * Deletes value from the tree
 * @param rootptr pointer to the pointer to the root of the tree
 * @param pointer to the token, which value will be deleted
 * @return true if value was deleted, false if value was not found
 */
bool delete_element(tree_node_t **rootptr, lexical_token_t *token);

//next 2 functions only for testing
void printtabs(int numtabs);

void print_tree(tree_node_t *root, int level);

#endif //IFJ_PROJ_2022_SYMTABLE_H