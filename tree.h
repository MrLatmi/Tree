#ifndef _TREE_
#define _TREE_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "formatingPrint.h"
#include <direct.h>
#include <windows.h>

#include <time.h>

#define DUMP_HEADER_PATH "dumpSample.htm"
#define TREE_DEBUG_MODE
typedef char* tree_element_t;

#define MAX_DATA_LENGTH 256

#ifdef LIST_DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

struct dump_file
{
    FILE*   file            ;
    char    folderPath [100];
    size_t  graphCount      ;
};


struct tree_node_t
{
    tree_element_t      data;
    struct tree_node_t* left;
    struct tree_node_t* right;
    struct tree_node_t* parent;
};

enum tree_errors_t
{
    TREE_NO_PROBLEM,
    THREE_ROOT_PARENT,
    TREE_NULL_NODE,
    TREE_NULL_DATA,
    TREE_ONE_CHILD,
    TREE_BROKEN_LEFT_LINK,
    TREE_BROKEN_RIGHT_LINK,
    TREE_SIMILAR_CHILD_NODE,
};


//TODO: сделать обертку дерева для корня и глубину дерева

#ifdef TREE_DEBUG_MODE
tree_errors_t   tree_verify_node    (const tree_node_t* const node);
#endif

dump_file       debugFileCreate     (void);
void            tree_graph_dump     (const tree_node_t* node, dump_file *const dumpFile);


tree_node_t*    new_node            (const tree_element_t element, tree_node_t* parent_node);
void            delete_node         (      tree_node_t* node);

void            print_tree          (const tree_node_t* node, FILE* writeFile);
tree_node_t*    new_tree_from_file  (FILE* readFile, tree_node_t* parent);
tree_node_t*    find_node           (tree_node_t* node, const char* target);

#endif
