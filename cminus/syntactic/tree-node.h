#ifndef _TREE_H_
#define _TREE_H_
#define MAX_CHILDREN 3

typedef enum {
    NProgram, NCmdList, NCmd, 
    NAssignStmt, NIdentifier, NExpr,  
    NOperator, NNumber, NIfElseStmt, 
    NReturnStmt, NCompoundStmt, NType,  
    NStatement, NDeclaration, NCall, 
    NBlock, NAssign, NIfStmt, 
    NMultiply, NDivide, NPlus, 
    NMinus,NLess,NLessEqual, 
    NGreater,NGreaterEqual,NEqual, 
    NNotEqual
} NodeType;


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
extern TreeNode *rootNode;  // Global syntax tree root

// helpers
const char* getNodeTypeName(NodeType type);

#endif