#include "tree.h"
#include "treeGraphDump.cpp"
#include "formatingPrint.h"

//TODO: обертка для функции new_node для универсальности
//TODO: функции работы с деревом для пользователя (создание ноды, удаление)

#ifdef TREE_DEBUG_MODE
tree_errors_t tree_verify_node(const tree_node_t* const node)
{
    if (node == NULL)
        return TREE_NULL_NODE;

    if (node->data == NULL)
    {
        RED_PRINTF("TREE: node %p has NULL data\n", node);
        return TREE_NULL_DATA;
    }

    if ((node->left == NULL) != (node->right == NULL))
    {
        RED_PRINTF("TREE: node %p has one child\n", node);
        return TREE_ONE_CHILD;
    }

    if (node->left != NULL)
    {
        if (node->left == node || node->left->parent != node)
        {
            RED_PRINTF("TREE: broken left link at node %p\n", node);
            return TREE_BROKEN_LEFT_LINK;
        }

        tree_errors_t problemL = tree_verify_node(node->left);
        if (problemL != TREE_NO_PROBLEM)
            return problemL;
    }

    if (node->right != NULL)
    {
        if (node->right == node || node->right->parent != node)
        {
            RED_PRINTF("TREE: broken right link at node %p\n", node);
            return TREE_BROKEN_RIGHT_LINK;
        }

        tree_errors_t problemR = tree_verify_node(node->right);
        if (problemR != TREE_NO_PROBLEM)
            return problemR;
    }

    if (node->left != NULL && node->left == node->right)
    {
        RED_PRINTF("TREE: left and right children are equal at node %p\n", node);
        return TREE_SIMILAR_CHILD_NODE;
    }

    return TREE_NO_PROBLEM;
}
#endif


tree_node_t* new_node(const tree_element_t element, tree_node_t* const parent_node)
{
    assert(element);

    tree_node_t* node = (tree_node_t*)calloc(1, sizeof(tree_node_t));
    assert(node);

    node->data  = strdup(element);
    assert(node->data);

    node->left  = NULL;
    node->right = NULL;

    if (parent_node)
        node->parent = parent_node;

#ifdef TREE_DEBUG_MODE
    tree_errors_t problem = tree_verify_node(node);
    if (problem != TREE_NO_PROBLEM)
    {
        RED_PRINTF("You have a problem %d", problem);
        return NULL;
    }
#endif

    return node;
}


void delete_node(tree_node_t*const node)
{
    assert(node);

#ifdef TREE_DEBUG_MODE
    tree_errors_t problem = tree_verify_node(node);
    if (problem != TREE_NO_PROBLEM)
    {
        RED_PRINTF("You have a problem %d", problem);
        return;
    }
#endif

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
    free(node->data);
    free(node);
}

void print_tree(const tree_node_t*const node, FILE*const writeFile)
{
    assert(node);
    assert(writeFile);

#ifdef TREE_DEBUG_MODE
    tree_errors_t problem = tree_verify_node(node);
    if (problem != TREE_NO_PROBLEM)
    {
        RED_PRINTF("You have a problem %d", problem);
        return;
    }
#endif

    fprintf(writeFile, "(");
    fprintf(writeFile, "\"%s\" ", node->data);

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

tree_node_t* new_tree_from_file(FILE* const readFile, tree_node_t* const parent)
{
    assert(readFile);

    char bracket = 0;

    if (fscanf(readFile, " %c", &bracket) != 1 || bracket != '(')
        return NULL;

    char data[MAX_DATA_LEN] = {};
    if (fscanf(readFile, " \"%255[^\"]\"", data) == 1)
    {
        tree_node_t* node = new_node(data, parent);
        assert(node);

        node->left  = new_tree_from_file(readFile, node);
        node->right = new_tree_from_file(readFile, node);

        fscanf(readFile, " %c", &bracket);
    #ifdef TREE_DEBUG_MODE
        tree_errors_t problem = tree_verify_node(node);
        if (problem != TREE_NO_PROBLEM)
        {
            RED_PRINTF("You have a problem %d", problem);
            return NULL;
        }
    #endif
        return node;
    }

    fscanf(readFile, " nil %c", &bracket);
    return NULL;
}


tree_node_t* find_node(tree_node_t* const node, const char* const target)
{
    assert(target);

    if (node == NULL)
        return NULL;

    if (node->data != NULL && strcmp(node->data, target) == 0)
        return node;

    tree_node_t* found_left = find_node(node->left, target);
    if (found_left != NULL)
        return found_left;

    return find_node(node->right, target);
}
