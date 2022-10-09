#include "bvs.h"

tree_node *create_node(int value){
    tree_node* result = malloc(sizeof(tree_node));
    if(result != 0){
        result->left = NULL;
        result->right = NULL;
        result->value = value;
    }
    return result;
}

int comparator(tree_node *root, int value){
    if(root->value == value){
        return 0;
    }
    if(root->value > value){
        return -1;
    }
    return 1;
}

bool insert_number(tree_node **rootptr, int value){
    tree_node *root = *rootptr;
    if (root == NULL){
        //tree empty
        (*rootptr) = create_node(value);
        return true;
    }
    if (comparator(root, value) == 0){
        return false;
    }
    if (comparator(root, value) == -1){
        return insert_number(&(root->left), value);
    }
    if (comparator(root, value) == 1){
        return insert_number(&(root->right), value);
    }
}

bool find_number(tree_node *root, int value){
    if (root == NULL) {
        return false;
    }
    if (comparator(root, value) == 0){
        return true;
    }
    if (comparator(root, value) == -1){
        return find_number(root->left, value);
    }
    if (comparator(root, value) == 1){
        return find_number(root->right, value);
    }
}

bool delete_number(tree_node **rootptr, int value) {
    tree_node *root = *rootptr;

    if (root == NULL) {
        return false;
    }
    if (comparator(root, value) == -1) {
        return delete_number(&root->left, value);
    }
    if (comparator(root, value) == 1) {
        return delete_number(&root->right, value);
    }
    if (comparator(root, value) == 0) {
        //node has no children
        if (root->left == NULL && root->right == NULL) {
            free(root);
            *rootptr = NULL;
            return true;
        }
        //node has one child (right)
        if (root->left == NULL) {
            tree_node *temp = root->right;
            free(temp);
            *rootptr = NULL;
            return true;
        } //node has one child (left)
        if (root->right == NULL) {
            tree_node *temp = root->left;
            free(temp);
            *rootptr = NULL;
            return true;
        }
        //node has two children
        if (root->left != NULL && root->right != NULL) {
            tree_node *temp = root->right;
            while (temp->left != NULL) {
                temp = temp->left;
            }
            root->value = temp->value;
            delete_number(&root->right, temp->value);
            return true;
        }
    }
}