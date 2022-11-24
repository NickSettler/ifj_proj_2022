/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmaroc00, Elena Marochkina
 *
 * @file bvs.c
 * @brief Binary search tree
 * @date 28.10.2022
 */

#include "symtable.h"
#include "syntax_analyzer.h"
#include "semantic_analyzer.h"


tree_node_t *symtable;

tree_node_t *init_symtable() {
    tree_node_t *symtable_ptr = init_tree();
    return symtable_ptr;
}

tree_node_t *init_tree() {
    insert_function("readi");
    insert_return_type("readi", (data_type) (TYPE_INT | TYPE_NULL));
    find_token("readi")->argument_count = 0;

    insert_function("reads");
    insert_return_type("reads", (data_type) (TYPE_STRING | TYPE_NULL));
    find_token("reads")->argument_count = 0;

    insert_function("readf");
    insert_return_type("readf", (data_type) (TYPE_FLOAT | TYPE_NULL));
    find_token("readf")->argument_count = 0;

    data_type *type_array = (data_type *) malloc(sizeof(data_type) * 4);
    type_array[0] = TYPE_ALL;
    insert_function("write");
    find_token("write")->argument_count = -1;
    insert_args("write", 1, type_array);
    find_token("write")->argument_type = TYPE_ALL;

    insert_function("strlen");
    insert_return_type("strlen", TYPE_INT);
    find_token("strlen")->argument_count = 1;
    type_array[0] = TYPE_STRING;
    insert_args("strlen", 1, type_array);
    find_token("strlen")->argument_type = TYPE_STRING;

    insert_function("ord");
    insert_return_type("ord", TYPE_INT);
    find_token("ord")->argument_count = 1;
    type_array[0] = TYPE_STRING;
    insert_args("ord", 1, type_array);
    find_token("ord")->argument_type = TYPE_STRING;

    insert_function("chr");
    insert_return_type("chr", TYPE_STRING);
    find_token("chr")->argument_count = 1;
    type_array[0] = TYPE_INT;
    insert_args("chr", 1, type_array);
    find_token("chr")->argument_type = TYPE_INT;

    insert_function("substring");
    insert_return_type("substring", (data_type) (TYPE_STRING | TYPE_NULL));
    find_token("substring")->argument_count = 3;
    type_array[0] = TYPE_STRING;
    type_array[1] = TYPE_INT;
    type_array[2] = TYPE_INT;
    insert_args("substring", 3, type_array);
    find_token("substring")->argument_type = TYPE_STRING;

    return symtable;
}

void insert_function(char *key) {
    insert_token(key);
    tree_node_t *function_ptr = find_element(symtable, key);
    function_ptr->defined = true;
    function_ptr->global = true;
    function_ptr->is_function = true;
}

void insert_return_type(char *key, data_type type) {
    tree_node_t *function_ptr = find_element(symtable, key);
    if (function_ptr->type == 0) {
        function_ptr->type = type;
    } else {
        function_ptr->type = (data_type) (function_ptr->type | type);
    }
}

void insert_args(char *key, int arg_count, data_type *type_array) {
    tree_node_t *function_ptr = find_element(symtable, key);
    data_type *args = (data_type *) malloc(sizeof(data_type) * arg_count);
    find_element(symtable, key)->args_array = args;
    data_type *arg_ptr = find_element(symtable, function_ptr->key)->args_array;
    for (int i = 0; i < arg_count; i++) {
        arg_ptr[i] = type_array[i];
    }
}

tree_node_t *create_node(char *key) {
    tree_node_t *result = (tree_node_t *) malloc(sizeof(tree_node_t));
    if (result == 0) {
        INTERNAL_ERROR("Malloc for BST failed");
    }
    result->left = NULL;
    result->right = NULL;
    result->key = key;
    result->defined = false;
    result->code_generator_defined = false;
    result->global = false;
    result->local = false;
    result->is_function = false;
    result->type = TYPE_NULL;
    result->argument_type = TYPE_NULL;
    result->argument_count = 0;
    result->function_tree = NULL;

    return result;
}

int comparator(tree_node_t *root, char *key) {
    if (strcmp(root->key, key) == 0) {
        return 0;
    }
    if (strcmp(root->key, key) > 0) {
        return -1;
    }
    return 1;

}

bool insert_element(tree_node_t **rootptr, char *key) {
    tree_node_t *root = *rootptr;
    if (root == NULL) {
        //tree empty
        (*rootptr) = create_node(key);
        return true;
    }
    switch (comparator(root, key)) {
        case 0:
            return false;
        case -1:
            return insert_element(&(root->left), key);
        case 1:
            return insert_element(&(root->right), key);
    }
}

bool insert_token(char *key) {
    return insert_element(&symtable, key);
}

tree_node_t *find_element(tree_node_t *root, char *key) {
    if (root == NULL) {
        return NULL;
    }
    switch (comparator(root, key)) {
        case 0:
            return root;
        case -1:
            return find_element(root->left, key);
        case 1:
            return find_element(root->right, key);
    }
}

tree_node_t *find_token(char *key) {
    return find_element(symtable, key);
}

bool delete_element(tree_node_t **rootptr, char *key) {
    tree_node_t *root = *rootptr;
    if (root == NULL) {
        return false;
    }
    switch (comparator(root, key)) {
        case 0: {
            if (root->left == NULL && root->right == NULL) {
                free(root);
                (*rootptr) = NULL;
                return true;
            }
            if (root->left == NULL) {
                (*rootptr) = root->right;
                free(root);
                return true;
            }
            if (root->right == NULL) {
                (*rootptr) = root->left;
                free(root);
                return true;
            }
            tree_node_t *temp = root->right;
            while (temp->left != NULL) {
                temp = temp->left;
            }
            root->key = temp->key;
            return delete_element(&(root->right), key);
        }
        case -1: {
            return delete_element(&(root->left), key);
        }
        case 1: {
            return delete_element(&(root->right), key);
        }
    }
}

bool delete_token(char *key) {
    return delete_element(&symtable, key);
}

void dispose_tree(tree_node_t **root) {
    if (*root == NULL) return;

    dispose_tree(&((*root)->left));
    dispose_tree(&((*root)->right));
    free(*root);
    *root = NULL;
}

void dispose_symtable() {
    if (symtable == NULL) return;

    dispose_tree(&symtable);
    symtable = NULL;
}

void change_data_type(tree_node_t *tree, data_type type) {
    tree->type = type;
}

void printtabs(int numtabs) {
    for (int i = 0; i < numtabs; i++) {
        printf("\t");
    }
}

void print_tree(tree_node_t *root, int level) {
    if (root == NULL) {
        printtabs(level);
        printf("-----\n");
        return;
    }
    printtabs(level);
    printf("key = %s, defined = %d, global = %d, type = %d args_count = %d\n", root->key, root->defined, root->global,
           root->type, root->argument_count);
    printtabs(level);
    printf("left\n");
    print_tree(root->left, level + 1);
    printtabs(level);
    printf("right\n");
    print_tree(root->right, level + 1);
    printtabs(level);
}

void print_symtable() {
    print_tree(symtable, 0);
}

void create_global_token(syntax_abstract_tree_t *tree) {
    insert_token(tree->value->value);
    find_token(tree->value->value)->defined = true;
    find_token(tree->value->value)->global = true;
}

void create_local_token(syntax_abstract_tree_t *tree, char *function_name) {
    tree_node_t *local_sym_table = find_token(function_name)->function_tree;
    char *key = tree->value->value;
    insert_element(&local_sym_table, key);
    tree_node_t *local_token = find_element(local_sym_table, key);
    local_token->local = true;
    local_token->defined = true;
    find_token(function_name)->function_tree = local_sym_table;
}


