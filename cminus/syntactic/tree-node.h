#ifndef _TREE_H_
#define _TREE_H_

#define MAX_CHILDREN 3

typedef enum {
    NProgram, NCmdList, NCmd, //1
    NAssignStmt, NIdentifier, NExpr,  //2
    NOperator, NNumber, NIfElseStmt, //3
    NReturnStmt, NCompoundStmt, NType,  //4
    NStatement, NDeclaration, NCall, //5
    NBlock, NAssign, NIfStmt, //6 
    NMultiply, NDivide, NPlus, //7
    NMinus,NLess,NLessEqual, //8
    NGreater,NGreaterEqual,NEqual, //9 
    NNotEqual
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
TreeNode *newBinaryNode(NodeType type, TreeNode *left, TreeNode *right, int lineno);
void freeTree(TreeNode *node);
extern TreeNode *rootNode;  // Global syntax tree root

#endif