#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tree.h"

#define MAX_LEN 256

//TODO: подписи у стрелочек
//TODO: хранить вопросы без ?
//TODO: 5 пункn - записать дерево
//TODO: ifdef для режима записи каждой операции

static void play_akinator(tree_node_t* const root_node, FILE* const tree_file)
{
#ifdef TREE_DEBUG_MODE
    tree_errors_t problem = tree_verify_node(root_node);
    if (problem != TREE_NO_PROBLEM)
    {
        RED_PRINTF("You have a problem %d", problem);
        return;
    }
#endif
    tree_node_t* current_node = root_node;
    char answer[MAX_LEN] = {};

    while (current_node->left != NULL && current_node->right != NULL)
    {
        printf("%s: ", current_node->data);
        scanf(" %255[^\n]", answer);

        if (strcmp(answer, "yes") == 0)
        {
            if (current_node->left != NULL)
                current_node = current_node->left;
            else
                break;
        }
        else if (strcmp(answer, "no") == 0)
        {
            if (current_node->right != NULL)
                current_node = current_node->right;
            else
                break;
        }
        else
        {
            printf("Write 'yes' or 'no'\n");
        }
    }

    printf("Is it %s?: ", current_node->data);
    scanf(" %255[^\n]", answer);

    if (strcmp(answer, "yes") == 0)
    {
        GREEN_PRINTF("Ura!\n");
        return;
    }

    char new_answer[MAX_LEN]   = {};

    printf("What was this? ");
    scanf(" %255[^\n]", new_answer);

    printf("What different between \"%s\" and \"%s\". He ...): ",
           new_answer, current_node->data);

    tree_node_t* yes_node = new_node(new_answer, current_node);
    tree_node_t* no_node  = new_node(current_node->data, current_node);

    free(current_node->data);

    current_node->data = (char*)calloc(MAX_LEN, sizeof(char));

    scanf(" %255[^\n]", current_node->data);

    current_node->left  = yes_node;
    current_node->right = no_node;

    GREEN_PRINTF("Ok!\n");

    freopen("treeRead.tr", "w", tree_file);
    print_tree(root_node, tree_file);
    fflush(tree_file);
}

static void print_word_path(const tree_node_t* const node)
{
    if (node == NULL)
        return;

    if (node->parent == NULL)
    {
        printf("%s\n", node->data);
        return;
    }

    if (node->parent->right == node)
        printf("%s <- not ", node->data);
    else
        printf("%s <- ", node->data);

    print_word_path(node->parent);
}

static void compare_words(const tree_node_t*const root)
{
    char word1[MAX_LEN] = {};
    printf("Write first word: ");
    scanf(" %255[^\n]", word1);
    const tree_node_t* node1 = find_node(root, word1);

    if (!node1)
    {
        RED_PRINTF("Word not found\n");
        return;
    }



    char word2[MAX_LEN] = {};
    printf("Write second word: ");
    scanf(" %255[^\n]", word2);
    const tree_node_t* node2 = find_node(root, word2);

    if (!node2)
    {
        RED_PRINTF("Word not found\n");
        return;
    }

    const tree_node_t* path1[MAX_LEN] = {};
    const tree_node_t* path2[MAX_LEN] = {};
    int len1 = 0, len2 = 0;

    for (const tree_node_t* cur = node1; cur != NULL; cur = cur->parent)
        path1[len1++] = cur;

    for (const tree_node_t* cur = node2; cur != NULL; cur = cur->parent)
        path2[len2++] = cur;

    int i = len1 - 1;
    int j = len2 - 1;

    printf("Common: ");
    while (i > 0 && j > 0 && path1[i - 1] == path2[j - 1])
    {
        printf("%s ", path1[i]->data);
        i--;
        j--;
    }
    printf("%s\n", path1[i]->data);

    printf("Differences:\n");

    printf("  %s: ", word1);
    for (int k = i - 1; k >= 0; k--)
        printf("<%s>", path1[k]->data);
    printf("\n");

    printf("  %s: ", word2);
    for (int k = j - 1; k >= 0; k--)
        printf("<%s> ", path2[k]->data);
    printf("\n");
}

int main(void)
{
    dump_file dumpFile = debugFileCreate();

    FILE* fileRead = fopen("treeRead.tr", "r+");
    if (!fileRead)
    {
        printf("Cannot open tree read file\n");
        return 0;
    }

    tree_node_t* tree = new_tree_from_file(fileRead, NULL);

    char command[MAX_LEN] = {};

    while (1)
    {
        printf("\nChoose something:\n");
        printf("1 - Akinator\n");
        printf("2 - make graphviz dump\n");
        printf("3 - find word and his way\n");
        printf("4 - compare 2 words\n");
        printf("stop - finish programm \n");

        scanf(" %255s", command);

        if (strcmp(command, "stop") == 0)
        {
            RED_PRINTF("Stoping program");
            break;
        }
        else if (strcmp(command, "1") == 0)
        {
            play_akinator(tree, fileRead);
        }
        else if (strcmp(command, "2") == 0)
        {
            tree_graph_dump(tree, &dumpFile);
            GREEN_PRINTF("Creating dump file\n");
        }
        else if (strcmp(command, "3") == 0)
        {
            printf("Write word, which you want to find: ");
            char word[MAX_LEN] = {};
            scanf(" %255[^\n]", word);

            tree_node_t* found_node = find_node(tree, word);
            if (found_node != NULL)
            {
                printf("Your word was found!\n");
                print_word_path(found_node);
            }
            else
            {
                printf("Word not found.\n");
            }
        }
        else if (strcmp(command, "4") == 0)
        {
            compare_words(tree);
        }
        else
        {
            printf("Cannot find command. Write 1, 2, 3, 4 or stop.\n");
        }
    }

    freopen("treeRead.tr", "w", fileRead);
    print_tree(tree, fileRead);

    delete_node(tree);
    fclose(dumpFile.file);
    fclose(fileRead);

    return 0;
}
