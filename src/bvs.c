/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmaroc00, Elena Marochkina
 *
 * @file bvs.c
 * @brief Binary search tree
 * @date 21.10.2022
 */

#include "bvs.h"

tree_node *create_node(int value) {
    tree_node *result = (tree_node *) malloc(sizeof(tree_node));
    if (result != 0) {
        result->left = NULL;
        result->right = NULL;
        result->value = value;
    }
    return result;
}

int comparator(tree_node *root, int value) {
    if (root->value == value) {
        return 0;
    }
    if (root->value > value) {
        return -1;
    }
    return 1;
}

bool insert_number(tree_node **rootptr, int value) {
    tree_node *root = *rootptr;
    if (root == NULL) {
        //tree empty
        (*rootptr) = create_node(value);
        return true;
    }
    switch (comparator(root, value)) {
        case 0:
            return false;
        case -1:
            return insert_number(&(root->left), value);
        case 1:
            return insert_number(&(root->right), value);
    }
}

bool find_number(tree_node *root, int value) {
    if (root == NULL) {
        return false;
    }
    switch(comparator(root, value)){
        case 0:
            return true;
        case -1:
            return find_number(root->left, value);
        case 1:
            return find_number(root->right, value);
    }
}

bool delete_number(tree_node **rootptr, int value) {
    tree_node *root = *rootptr;

    if (root == NULL) {
        return false;
    }
    switch (comparator(root, value)) {
        case 0:
            if (root->left == NULL && root->right == NULL) {
                free(root);
                *rootptr = NULL;
                return true;
            }
            if (root->left == NULL) {
                *rootptr = root->right;
                free(root);
                return true;
            }
            if (root->right == NULL) {
                *rootptr = root->left;
                free(root);
                return true;
            }
            tree_node *temp = root->right;
            while (temp->left != NULL) {
                temp = temp->left;
            }
            root->value = temp->value;
            return delete_number(&(root->right), temp->value);
        case -1:
            return delete_number(&(root->left), value);
        case 1:
            return delete_number(&(root->right), value);

    }
}