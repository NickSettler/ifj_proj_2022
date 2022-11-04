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

tree_node_t *symtable;

tree_node_t *create_node(char *key) {
    tree_node_t *result = (tree_node_t *) malloc(sizeof(tree_node_t));
    if (result == 0) {
        INTERNAL_ERROR("Malloc for BST failed");
    }
    result->left = NULL;
    result->right = NULL;
    result->key = key;
    result->defined = false;

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

void dispose_tree(tree_node_t *root) {
    if (root == NULL) return;

    dispose_tree(root->left);
    dispose_tree(root->right);
    free(root);
}

void dispose_symtable() {
    if (symtable == NULL) return;

    dispose_tree(symtable);
    symtable = NULL;
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
    printf("key = %s\n", root->key);
    printtabs(level);
    printf("left\n");
    print_tree(root->left, level + 1);
    printtabs(level);
    printf("right\n");
    print_tree(root->right, level + 1);
    printtabs(level);
}
