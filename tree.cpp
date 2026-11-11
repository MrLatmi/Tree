#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"

tree_node_t* new_node(const tree_element_t element, tree_node_t*const parent_node)
{
    tree_node_t* node = (tree_node_t*)calloc(1, sizeof(tree_node_t));
    assert(node);

    node->data          = element;
    node->left          = NULL;
    node->right         = NULL;

    if(parent_node)
        node->parent    = parent_node;

    return node;
}

void delete_node(tree_node_t*const node)
{
    assert(node);

    if (node->left)
        delete_node(node->left);

    if (node->right)
        delete_node(node->right);


    if (node->parent)
    {
        if(node->parent->left == node)
            node->parent->left = NULL;
        else if(node->parent->right == node)
            node->parent->right = NULL;
        node->parent = NULL;
    }

    free(node);
}

void print_tree(const tree_node_t*const node, FILE*const writeFile)
{
    assert(node);
    assert(writeFile);

    fprintf(writeFile, "(");
    fprintf(writeFile, "\"%d\" ", node->data);

    if (node->left)
        print_tree(node->left, writeFile);
    else
        fprintf(writeFile, "(nil )");

    if (node->right)
        print_tree(node->right, writeFile);
    else
        fprintf(writeFile, "(nil )");

    fprintf(writeFile, ")");
}

tree_node_t* new_tree_from_file(FILE*const readFile, tree_node_t*const parent)
{
    assert(readFile);

    char bracket = 0;

    if (fscanf(readFile, " %c", &bracket) != 1 || bracket != '(')
        return NULL;

    int data = 0;
    if (fscanf(readFile, " \"%d\"", &data) == 1)
    {
        tree_node_t *node = new_node(data, parent);
        assert(node);

        node->left  = new_tree_from_file(readFile, node);
        node->right = new_tree_from_file(readFile, node);

        fscanf(readFile, " %c", &bracket);

        return node;
    }

    fscanf(readFile, " nil %c", &bracket);
    return NULL;
}
