//
// Created by Елена Марочкина on 08.10.2022.
//

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

bool insert_number(tree_node **rootptr, int value){
    tree_node *root = *rootptr;
    if (root == NULL){
        //tree empty
        (*rootptr) = create_node(value);
        return true;
    }
    if (root->value == value){
        return false;
    }
    if (value < root->value){
        return insert_number(&(root->left), value);
    }
    if (value > root->value){
        return insert_number(&(root->right), value);
    }
}

bool find_number(tree_node *root, int value){
    if (root == NULL) {
        return false;
    }
    if (value == root->value){
        return true;
    }
    if (value < root->value){
        return find_number(root->left, value);
    }
    if (value > root->value){
        return find_number(root->right, value);
    }
}
