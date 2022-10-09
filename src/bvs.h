//
// Created by Елена Марочкина on 08.10.2022.
//

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

//node creation
tree_node *create_node(int value);

//tree initialization
tree_node *create_node(int value);

//comparator
int comparator(tree_node *root, int value);

//insert numbers into the node
bool insert_number(tree_node **rootptr, int value);

//find numbers into the node
bool find_number(tree_node *root, int value);

//delete numbers and reorganise tree
bool delete_number(tree_node **rootptr, int value);


