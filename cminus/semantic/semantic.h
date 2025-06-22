#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../syntactic/tree-node.h"
#include "symbol-table.h"

void semanticAnalysis(TreeNode *syntaxTree);
void checkNode(TreeNode *node, SymbolTable *table);
void checkVariable(TreeNode *node, SymbolTable *table);
void checkFunction(TreeNode *node, SymbolTable *table);
void checkExpression(TreeNode *node, SymbolTable *table);
void checkStatement(TreeNode *node, SymbolTable *table);

#endif