#include <stdio.h>
#include <stdlib.h>
#include "tree-node.h"

extern int lineno;

TreeNode* newTreeNode(NodeType type, int lineno) {
    TreeNode* node = (TreeNode*) malloc(sizeof(TreeNode));
    if (!node) return NULL;

    node->type = type;
    node->lineno = lineno;
    node->sibling = NULL;

    for (int i = 0; i < MAX_CHILDREN; i++) {
        node->children[i] = NULL;
    }

    // Initialize union to avoid uninitialized access
    node->attribute.name = NULL; // Use only one, since it's a union

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
    if (!node) return NULL;

    node->children[0] = left;
    node->children[1] = right;

    return node;
}

const char* typeToString(DataType type) {
    switch(type) {
        case TYPE_INT:  return "int";
        case TYPE_VOID: return "void";
        case TYPE_ERROR: return "error";
        default: return "unknown";
    }
}

// TreeNode * newTreeNode(NodeType type, int lineno);
// extern TreeNode *rootNode;  // Global syntax tree root

TreeNode *rootNode = NULL;  // Definition of global root node