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

tree_node_t *create_node(lexical_token_t *token) {
    tree_node_t *result = (tree_node_t *) malloc(sizeof(tree_node_t));
    if (result == 0) {
        INTERNAL_ERROR("Malloc for BST failed");
    }
    result->left = NULL;
    result->right = NULL;
    result->key = token->value;

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

bool insert_element(tree_node_t **rootptr, lexical_token_t *token) {
    tree_node_t *root = *rootptr;
    if (root == NULL) {
        //tree empty
        (*rootptr) = create_node(token);
        return true;
    }
    switch (comparator(root, token->value)) {
        case 0:
            return false;
        case -1:
            return insert_element(&(root->left), token);
        case 1:
            return insert_element(&(root->right), token);
    }
}

bool insert_token(lexical_token_t *token) {
    insert_element(&symtable, token) ? true : false;
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

bool delete_element(tree_node_t **rootptr, lexical_token_t *token) {
    tree_node_t *root = *rootptr;
    if (root == NULL) {
        return false;
    }
    switch (comparator(root, token->value)) {
        case 0:
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
            token->value = temp->key;
            return delete_element(&(root->right), token);
        case -1:
            return delete_element(&(root->left), token);
        case 1:
            return delete_element(&(root->right), token);
    }
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