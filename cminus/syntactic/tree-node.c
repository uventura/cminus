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

const char* nodeTypeToString(NodeType type) {
    switch(type) {
        case NProgram:        return "Program";
        case NCmdList:        return "CmdList";
        case NCmd:            return "Cmd";
        case NAssignStmt:     return "AssignStmt";
        case NIdentifier:     return "Identifier";
        case NExpr:           return "Expr";
        case NOperator:       return "Operator";
        case NNumber:         return "Number";
        case NIfElseStmt:     return "IfElse Statement";
        case NReturnStmt:     return "Return Statement";
        case NCompoundStmt:   return "Compound Statement";
        case NType:           return "Type";
        case NStatement:      return "Statement";
        case NDeclaration:    return "Declaration";
        case NCall:           return "Call";
        case NBlock:          return "Block";
        case NAssign:         return "Assign";
        case NIfStmt:         return "If Statement";
        case NMultiply:       return "Multiply";
        case NPlus:           return "Plus";
        case NMinus:          return "Minus";
        case NLess:           return "Less";
        case NLessEqual:      return "LessEqual";
        case NGreater:        return "Greater";
        case NGreaterEqual:   return "GreaterEqual";
        case NEqual:          return "Equal";
        case NNotEqual:       return "Not Equal";
        default:              return "Unknown";
    }
}

const char* typeToString(DataType type) {
    switch(type) {
        case TYPE_INT:  return "int";
        case TYPE_VOID: return "void";
        case TYPE_ERROR: return "error";
        default: return "unknown";
    }
}

static void printTreeRec(const TreeNode *t,int depth){
    if(!t) return;

    for(int i=0;i<(depth*2);i++) putchar(' ');
    printf("%s", nodeTypeToString(t->type));

    switch(t->type){
        case NIdentifier:
            printf("  [name=\"%s\"]", t->attribute.name);
            break;

        case NNumber:
            printf("  [value=%d]", t->attribute.value);
            break;

        case NOperator:
            printf("  [op='%c']",  t->attribute.op);
            break;

        case NType:
            printf("  [dataType=%s]", typeToString(t->dataType));
            break;

        default: break;
    }
    printf("  (line %d)\n", t->lineno);

    for(int i=0;i<MAX_CHILDREN;i++)
        printTreeRec(t->children[i], depth+1);

    printTreeRec(t->sibling, depth);
}

void printTree(const TreeNode *t){
    puts("\n===== Syntactic Tree =====");
    printTreeRec(t,0);
    puts("============================\n");
}

// TreeNode * newTreeNode(NodeType type, int lineno);
// extern TreeNode *rootNode;  // Global syntax tree root

TreeNode *rootNode = NULL;  // Definition of global root node