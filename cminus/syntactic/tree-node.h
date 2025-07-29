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
    NNotEqual,
    NRead, NWrite,
    NWhile
} NodeType;

const char* nodeTypeToString(NodeType type);

typedef enum {
    TYPE_INT,   
    TYPE_VOID,
    TYPE_ERROR  // 2 (for invalid types)
} DataType;

const char* typeToString(DataType type);


typedef struct attribute {
    union
    {
        char *name;
        int value;
        char op;
    } val;
    DataType dataType; 
    int lineno;
    int wasUsed;
} attribute;


typedef struct treeNode {
    NodeType type;
    struct treeNode* children[MAX_CHILDREN];
    struct treeNode* sibling;
    union
    {
        char *name;
        int value;
        char op;
    } attribute;
    DataType dataType;
    int lineno;
    int wasUsed;
} TreeNode;



TreeNode * newTreeNode(NodeType type, int lineno);
TreeNode *newBinaryNode(NodeType type, TreeNode *left, TreeNode *right, int lineno);
void freeTree(TreeNode *node);
void printTree(const TreeNode *t);
extern TreeNode *rootNode;  // Global syntax tree root

#endif