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

void freeTree(TreeNode *node) {
    if (!node) return;
    
    for (int i = 0; i < MAX_CHILDREN; i++) {
        freeTree(node->children[i]);
    }
    freeTree(node->sibling);
    
    if (node->type == NIdentifier && node->attribute.name) {
        free(node->attribute.name);
    }
    free(node);
}

TreeNode *newBinaryNode(NodeType type, TreeNode *left, TreeNode *right, int lineno) {
    TreeNode *node = newTreeNode(type, lineno);
    if (node) {
        node->children[0] = left;
        node->children[1] = right;
    }
    return node;
}


// TreeNode * newTreeNode(NodeType type, int lineno);
// extern TreeNode *rootNode;  // Global syntax tree root

TreeNode *rootNode = NULL;  // Definition of global root node