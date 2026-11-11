#include <stdio.h>
#include "tree.h"

int main(void)
{
    FILE* file1         = fopen("treeWrite.tr", "w");
    FILE* file2         = fopen("treeRead.tr", "r");

    tree_node_t* tree2  = new_tree_from_file(file2, NULL);

    print_tree(tree2, file1);
}
