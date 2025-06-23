#include <stdio.h>
#include <stdlib.h>
#include "tree-node.h"

extern int lineno;

TreeNode * newTreeNode(NodeType type, int lineno) {
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed for new tree node\n");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->lineno = lineno;
    for (int i = 0; i < MAX_CHILDREN; i++) {
        node->children[i] = NULL;
    }
    node->sibling = NULL;
    return node;
}

TreeNode *rootNode = NULL;  // Definition of global root node