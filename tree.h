#ifndef _TREE_
#define _TREE_

#include <stdio.h>

typedef int tree_element_t;

typedef struct tree_node_t
{
    tree_element_t      data;
    struct tree_node_t* left;
    struct tree_node_t* right;
    struct tree_node_t* parent;
} tree_node_t;

tree_node_t* new_node           (const tree_element_t element, tree_node_t* parent_node);
void         delete_node        (      tree_node_t* node);
void         print_tree         (const tree_node_t* node, FILE* writeFile);
tree_node_t* new_tree_from_file (FILE* readFile, tree_node_t* parent);

#endif
