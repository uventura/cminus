#ifndef CODEGEN_H
#define CODEGEN_H

#include "../syntactic/tree-node.h"
#include "../semantic/symbol-table.h"

#define COMMENTS_ENABLED 1
#define MAX_CODE 4096
#define LINE_SIZE 128
#define AC  0
#define AC1 1
#define GP  5
#define MP  6
#define PC  7
#define TM_MAX_MEM 1024 // Total memory size for the Tiny Machine (example value, adjust as needed)


void generateCode(const char* filename, TreeNode* syntaxTree, SymbolTable *symbolTable);

#endif
