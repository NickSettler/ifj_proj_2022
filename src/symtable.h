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

typedef struct syntax_abstract_tree syntax_abstract_tree_t;

/**
 * @enum Data_type_t
 * Data type of variable
 * @var DATA_TYPE_INT
 * @var DATA_TYPE_FLOAT
 * @var DATA_TYPE_STRING
 */
typedef enum {
    TYPE_NULL = 1 << 0,
    TYPE_INT = 1 << 1,
    TYPE_FLOAT = 1 << 2,
    TYPE_STRING = 1 << 3,
} data_type;

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
 * Key is identifier
 *
 * @bool tree_node_t::defined
 * Is variable defined
 *
 * @bool tree_node_t::global
 * Is variable global
 */
typedef struct tree_node {
    struct tree_node *function_tree;
    data_type type;
    data_type argument_type;
    int argument_count;
    bool defined;
    bool global;
    bool code_generator_defined;
    bool local;
    bool is_function;
    char *key;
    struct tree_node *left;
    struct tree_node *right;
} tree_node_t;

/**
 * @brief Initialize symbol table with built-in functions
 */
void init_symtable();

/**
 * @brief Initialize function tree
 */
void init_tree();

/**
 * @brief insert function into symtable
 * @param key
 * @param return_type
 * @param global
 * @param defined
 */
void insert_function(char *key);

/**
 * @brief insert return type into function
 * @param key
 * @param type
 */
void insert_return_type(char *key, data_type type);

/**
 * @brief insert function args into symtable
 * @param key
 * @param type
 */
void insert_args(char *key, data_type type);

/**
 * Creates tree node with key
 * @param pointer to the token
 * @return pointer to the tree node
 */
tree_node_t *create_node(char *key);

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
bool insert_element(tree_node_t **rootptr, char *key);

/**
 * Inserts token into the symtable
 * @param token token value
 * @return true if token was inserted, false otherwise
 */
bool insert_token(char *key);

/**
 * Finds value in the tree
 * @param root pointer to the root of the tree
 * @param pointer to the token, which value is searched
 * @return  if key was found
 */
tree_node_t *find_element(tree_node_t *root, char *key);

/**
 * Finds token in the symtable
 * @param key token value
 * @return pointer to the node if token was found, NULL otherwise
 */
tree_node_t *find_token(char *key);

/**
 * Deletes tree by the key
 * @param rootptr pointer to the pointer to the root of the tree
 * @param key key of the node to be deleted
 * @return true if key was deleted, false otherwise
 */
bool delete_element(tree_node_t **rootptr, char *key);

/**
 * Deletes token from the symtable
 * @param pointer token value to be deleted
 * @return true if token was deleted, false if value was not found
 */
bool delete_token(char *key);

/**
 * Deletes tree
 * @param root pointer to the root of the tree
 */
void dispose_tree(tree_node_t **root);

/**
 * Deletes symtable
 */
void dispose_symtable();

/**
 * changes data type of the variable
 * @param root pointer to the root of the tree
 *
 * @key key of the node, which data type will be changed
 *
 * @type new data type
 */
void change_data_type(tree_node_t *tree, data_type type);

/**
 * Prints tabs
 * @param numtabs Number of tabs to be printed
 */
void printtabs(int numtabs);

/**
 * Prints tree
 * @param root pointer to the root of the tree
 * @param level Number of tabs to be printed
 */
void print_tree(tree_node_t *root, int level);

/**
 * Prints symtable
 */
void print_symtable();

/**
 * Creates global token in symtable
 * @param tree Node that contains token
 */
void create_global_token(syntax_abstract_tree_t *tree);

void create_local_token(syntax_abstract_tree_t *tree, char *function_name);

#endif //IFJ_PROJ_2022_SYMTABLE_H