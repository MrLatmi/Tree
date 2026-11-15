#define FOLDER_NAME_LENGTH 50
#define FILE_PATH_LENGTH 100
#define SMALL_STR_LENGTH 32
#define GRAPH_FILE_NAME 200
#define COMMAND_LENGTH 512
dump_file debugFileCreate(void)
{
    time_t myTime;
    time(&myTime);

    tm* localTime = localtime(&myTime);

    char folderName[FOLDER_NAME_LENGTH] = {};

    snprintf(folderName, sizeof(folderName), "log_%02d.%02d.%d_%02d-%02d-%02d",
            localTime->tm_mday,
            localTime->tm_mon + 1,
            localTime->tm_year + 1900,
            localTime->tm_hour,
            localTime->tm_min,
            localTime->tm_sec);

    int mkDirChecker = _mkdir(folderName);

    if (mkDirChecker != 0)
    {
        RED_PRINTF("Problem with creating debug folder");
        dump_file errorStruct = {};
        return errorStruct;
    }

    char filePath[FILE_PATH_LENGTH] = {};
    snprintf(filePath, sizeof(filePath), "%s/LogFile.htm", folderName);

    if (!CopyFile(DUMP_HEADER_PATH, filePath, FALSE))
    {
        printf("LIST DUMP: Sample copy error");
    }

    FILE* debugFile = fopen(filePath, "ab");

    if (debugFile == NULL)
    {
        RED_PRINTF("Problem with creating debug file");
        dump_file errorStruct = {};
        return errorStruct;
    }

    dump_file debugFileStruct  = {};
    debugFileStruct.file       = debugFile;

    strncpy(debugFileStruct.folderPath, folderName, sizeof(debugFileStruct.folderPath) - 1);
    debugFileStruct.folderPath[sizeof(debugFileStruct.folderPath) - 1] = '\0';

    debugFileStruct.graphCount = 0;

    return debugFileStruct;
}

#define TREE_ELEM_FMT "%s"

static void dump_tree_nodes(const tree_node_t* const node, FILE* const graphFile)
{
    assert(node);
    assert(graphFile);

    bool is_leaf            = (node->left == NULL && node->right == NULL);
    const char* fillColor   = is_leaf ? "#4c036e" : "#27512e";
    const char* borderColor = is_leaf ? "#31112b" : "#092604";
    const char* fontColor   = is_leaf ? "#f9f9f7" : "#65B7B7";

    char parent_str [SMALL_STR_LENGTH] = "NIL";
    char left_str   [SMALL_STR_LENGTH] = "NIL";
    char right_str  [SMALL_STR_LENGTH] = "NIL";

    if (node->parent != NULL)
    {
        snprintf(parent_str, sizeof(parent_str), "%p", node->parent);
    }
    if (node->left != NULL)
    {
        snprintf(left_str, sizeof(left_str), "%p", node->left);
    }
    if (node->right != NULL)
    {
        snprintf(right_str, sizeof(right_str), "%p", node->right);
    }

    fprintf(graphFile,
            " node_%p [label=\"{ addr (%p) | parent (%s) | data (" TREE_ELEM_FMT ") | { <left> L (%s) | <right> R (%s) } }\", "
            "fontsize=10, fillcolor=\"%s\", color=\"%s\", fontcolor=\"%s\"];\n",
            node, node, parent_str,
            node->data, left_str, right_str,
            fillColor, borderColor, fontColor);

    if (node->parent != NULL)
    {
        fprintf(graphFile,
            "  node_%p -> node_%p [color=\"#6c757d\", style=dashed, constraint=false, arrowhead=odot];\n",
            node, node->parent);
    }

    if (node->left != NULL || node->right != NULL)
    {
        if (node->left != NULL)
        {
            fprintf(graphFile, "  node_%p -> node_%p [color=\"#65B7B7\"];\n",
                    node, node->left);
            dump_tree_nodes(node->left, graphFile);
        }

        if (node->right != NULL)
        {
            fprintf(graphFile, "  node_%p -> node_%p [color=\"#e06c75\"];\n", node, node->right);
            dump_tree_nodes(node->right, graphFile);
        }
    }
}

void tree_graph_dump(const tree_node_t* const node, dump_file* const dumpFile)
{
    if (node == NULL || dumpFile == NULL || dumpFile->file == NULL)
    {
        RED_PRINTF("Dump: bad tree node or debug file");
        return;
    }

    char graphFileName[GRAPH_FILE_NAME] = "";
    snprintf(graphFileName, sizeof(graphFileName), "%s/graph_%zu.dot",
             dumpFile->folderPath, dumpFile->graphCount);

    FILE* graphFile = fopen(graphFileName, "w");
    if (graphFile == NULL)
    {
        RED_PRINTF("Dump: failed to open graph file %s", graphFileName);
        return;
    }

    fprintf(graphFile,
        "digraph G {\n"
        "  rankdir=TB;\n"
        "  bgcolor=\"#0a0c10\";\n"
        "  splines=polyline;\n"
        "  nodesep=0.8;\n"
        "  ranksep=0.7;\n"
        "  node[shape=Mrecord, style=filled, fillcolor=\"#b82222\", color=\"#4c0000\", fontname=Monospace, fontcolor=\"#65B7B7\"];\n"
        "  edge[color=\"#f9f9f7\", arrowhead=normal];\n\n");

    fprintf(graphFile,
        "  root_info [label=\"Tree Dump #%zu | root \\<%p\\>\", fillcolor=\"#1ca8a3\", color=\"#192832\", fontcolor=\"#131515\"];\n"
        "  root_info -> node_%p [color=\"#1ca8a3\"];\n\n",
        dumpFile->graphCount, node, node);

    dump_tree_nodes(node, graphFile);

    fprintf(graphFile, "}\n");
    fclose(graphFile);

    char command[COMMAND_LENGTH] = "";
    snprintf(command, sizeof(command), "dot -Tsvg %s/graph_%zu.dot -o %s/graph_%zu.svg",
             dumpFile->folderPath, dumpFile->graphCount, dumpFile->folderPath, dumpFile->graphCount);
    system(command);

    time_t nowTime = time(NULL);
    char   timeStr[SMALL_STR_LENGTH] = "";
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", localtime(&nowTime));

    fprintf(dumpFile->file, "<div class=\"section\">"
                            "<div class=\"info\">"
                            "<div><b>Time:</b> %s</div>"
                            "</div>"
                            "<div class=\"graph-box\"><img src=\"graph_%zu.svg\"></div>"
                            "</div>\n",
                            timeStr, dumpFile->graphCount);

    fflush(dumpFile->file);
    dumpFile->graphCount++;
}
