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

// helpers
const char* getNodeTypeName(NodeType type) {
    switch (type) {
        case NProgram: return "NProgram";
        case NCmdList: return "NCmdList";
        case NCmd: return "NCmd";
        case NAssignStmt: return "NAssignStmt";
        case NIdentifier: return "NIdentifier";
        case NExpr: return "NExpr";
        case NOperator: return "NOperator";
        case NNumber: return "NNumber";
        case NIfElseStmt: return "NIfElseStmt";
        case NReturnStmt: return "NReturnStmt";
        case NCompoundStmt: return "NCompoundStmt";
        case NType: return "NType";
        case NStatement: return "NStatement";
        case NDeclaration: return "NDeclaration";
        case NCall: return "NCall";
        case NBlock: return "NBlock";
        case NAssign: return "NAssign";
        case NIfStmt: return "NIfStmt";
        case NMultiply: return "NMultiply";
        case NDivide: return "NDivide";
        case NPlus: return "NPlus";
        case NMinus: return "NMinus";
        case NLess: return "NLess";
        case NLessEqual: return "NLessEqual";
        case NGreater: return "NGreater";
        case NGreaterEqual: return "NGreaterEqual";
        case NEqual: return "NEqual";
        case NNotEqual: return "NNotEqual";
        default: return "Unknown NodeType";
    }
}
