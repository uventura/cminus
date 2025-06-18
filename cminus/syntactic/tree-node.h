#ifndef _TREE_H_
#define _TREE_H_

#define MAX_CHILDREN 3

typedef enum {
    NProgram, NCmdList, NCmd, NAssign, NIdentifier, NExpr, NOperator, NNumber, NConditional
} NodeType;

typedef struct treeNode {
    NodeType type;
    struct treeNode* children[MAX_CHILDREN];
    struct treeNode* sibling;
    union
    {
        char *name;
        int value;
        char op;
        int dataType;   // For type information (INT/VOID)
    } attribute;
    int lineno;    
} TreeNode;

TreeNode * newTreeNode(NodeType type, int lineno);
extern TreeNode *rootNode;  // Global syntax tree root

#endif