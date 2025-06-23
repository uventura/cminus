#ifndef _TREE_H_
#define _TREE_H_

#define MAX_CHILDREN 3

typedef enum {
    NProgram, NCmdList, NCmd, //1
     NAssignStmt,     NIdentifier, NExpr,  //2
    NOperator, NNumber, NIfElseStmt, //3
      NReturnStmt, NCompoundStmt,  NType,  //4
     NStatement,      NDeclaration, NCall, //5
      NBlock,       NAssign //6
} NodeType;

typedef struct treeNode {
    NodeType type;
    struct treeNode* children[MAX_CHILDREN];
    struct treeNode* sibling;
    union {
        char *name;
        int value;
        char op;
        int dataType;   // For type information (INT/VOID)
    } attribute;
    int lineno;    
} TreeNode;

TreeNode * newTreeNode(NodeType type, int lineno);
void freeTree(TreeNode *node);
extern TreeNode *rootNode;  // Global syntax tree root

#endif