#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../syntactic/tree-node.h"
#include "symbol-table.h"

// Main semantic analysis function
void semanticAnalysis(TreeNode *syntaxTree);

// Node checking functions
void checkProgram(TreeNode *node, SymbolTable *table);
void checkDeclaration(TreeNode *node, SymbolTable *table);
void checkStatement(TreeNode *node, SymbolTable *table);
void checkExpression(TreeNode *node, SymbolTable *table);

void checkBlock(TreeNode *node, SymbolTable *table);
void checkAssignment(TreeNode *node, SymbolTable *table);
void checkIfStatement(TreeNode *node, SymbolTable *table);
void checkWhileStatement(TreeNode *node, SymbolTable *table);
void checkReturnStatement(TreeNode *node, SymbolTable *table);

// Type checking functions
DataType getExpressionType(TreeNode *expr, SymbolTable *table);
void checkTypeCompatibility(DataType expected, DataType actual, int lineno, const char *context);

// Utility functions
void checkVariableDeclaration(TreeNode *node, SymbolTable *table);
void checkFunctionDeclaration(TreeNode *node, SymbolTable *table);
void checkFunctionCall(TreeNode *node, SymbolTable *table);
void markSymbolAsUsed(SymbolTable *table, const char *name);


// Error reporting
void semanticError(int lineno, const char *format, ...);

#endif