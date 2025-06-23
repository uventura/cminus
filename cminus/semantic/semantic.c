#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "semantic.h"
#include "../lexer/tokens.h"

// Global flag to track if any semantic errors were found
static int hasSemanticErrors = 0;

void semanticError(int lineno, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    fprintf(stderr, "Semantic Error (line %d): ", lineno);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    va_end(args);
    hasSemanticErrors = 1;
}

void semanticAnalysis(TreeNode *syntaxTree) {
    if (!syntaxTree) {
        fprintf(stderr, "Error: Empty syntax tree\n");
        exit(1);
    }

    SymbolTable *table = createSymbolTable();
    checkProgram(syntaxTree, table);
    
    if (hasSemanticErrors) {
        fprintf(stderr, "Compilation failed due to semantic errors\n");
        exit(1);
    }
    
    freeSymbolTable(table);
}

void checkProgram(TreeNode *node, SymbolTable *table) {
    if (!node || node->type != NProgram) {
        semanticError(node ? node->lineno : 0, "Expected program node");
        return;
    }

    enterScope(table);
    
    // Check all declarations and statements
    TreeNode *child = node->children[0]; // First child is typically declarations
    while (child) {
        switch (child->type) {
            case NDeclaration:
                checkDeclaration(child, table);
                break;
            case NStatement:
                checkStatement(child, table);
                break;
            default:
                semanticError(child->lineno, "Unexpected node type in program");
                break;
        }
        child = child->sibling;
    }
    
    exitScope(table);
}

void checkDeclaration(TreeNode *node, SymbolTable *table) {
    if (!node || node->type != NDeclaration) {
        semanticError(node ? node->lineno : 0, "Expected declaration node");
        return;
    }

    TreeNode *typeNode = node->children[0];
    TreeNode *idNode = node->children[1];
    
    if (!typeNode || !idNode) {
        semanticError(node->lineno, "Incomplete declaration");
        return;
    }

    int dataType = typeNode->attribute.dataType;
    const char *name = idNode->attribute.name;

    // Check if identifier is already declared in current scope
    Symbol *existing = lookup(table, name);
    if (existing && existing->scope == table->currentScope) {
        semanticError(idNode->lineno, "Redeclaration of '%s'", name);
        return;
    }

    // Insert into symbol table
    SymbolType symType = (node->children[2]) ? FUNC : VAR; // Function if has params/body
    Symbol *symbol = insert(table, name, symType, dataType);
    if (!symbol) {
        semanticError(idNode->lineno, "Failed to insert '%s' into symbol table", name);
        return;
    }

    // Check function parameters and body if this is a function declaration
    if (symType == FUNC) {
        enterScope(table);
        
        // Check parameters
        TreeNode *params = node->children[2];
        if (params) {
            TreeNode *param = params->children[0];
            while (param) {
                checkDeclaration(param, table);
                param = param->sibling;
            }
        }
        
        // Check function body
        TreeNode *body = node->children[3];
        if (body) {
            checkStatement(body, table);
        }
        
        exitScope(table);
    }
}

void checkStatement(TreeNode *node, SymbolTable *table) {
    if (!node) return;

    switch (node->type) {
        case NCompoundStmt:
            enterScope(table);
            // Check all statements in compound
            TreeNode *stmt = node->children[0];
            while (stmt) {
                checkStatement(stmt, table);
                stmt = stmt->sibling;
            }
            exitScope(table);
            break;
            
        case NAssignStmt:
            checkExpression(node->children[0], table); // LHS
            checkExpression(node->children[1], table); // RHS
            break;
            
        case NIfStmt:
            checkExpression(node->children[0], table); // Condition
            checkStatement(node->children[1], table);  // Then
            if (node->children[2]) {
                checkStatement(node->children[2], table); // Else
            }
            break;
            
        case NWhileStmt:
            checkExpression(node->children[0], table); // Condition
            checkStatement(node->children[1], table);  // Body
            break;
            
        case NReturnStmt:
            if (node->children[0]) {
                checkExpression(node->children[0], table);
            }
            break;
            
        default:
            semanticError(node->lineno, "Unknown statement type");
            break;
    }
}

void checkExpression(TreeNode *node, SymbolTable *table) {
    if (!node) return;

    switch (node->type) {
        case NIdentifier:
            // Check if variable/function is declared
            if (!lookup(table, node->attribute.name)) {
                semanticError(node->lineno, "Undeclared identifier '%s'", node->attribute.name);
            }
            break;
            
        case NCall:
            checkFunctionCall(node, table);
            break;
            
        case NOperator:
            // Check operands
            if (node->children[0]) checkExpression(node->children[0], table);
            if (node->children[1]) checkExpression(node->children[1], table);
            
            // TODO: Add type checking for operations
            break;
            
        case NNumber:
            // Numbers don't need checking
            break;
            
        default:
            semanticError(node->lineno, "Unknown expression type");
            break;
    }
}

void checkFunctionCall(TreeNode *node, SymbolTable *table) {
    const char *funcName = node->children[0]->attribute.name;
    Symbol *funcSymbol = lookup(table, funcName);
    
    if (!funcSymbol) {
        semanticError(node->lineno, "Call to undeclared function '%s'", funcName);
        return;
    }
    
    if (funcSymbol->type != FUNC) {
        semanticError(node->lineno, "'%s' is not a function", funcName);
        return;
    }
    
    // Check arguments
    TreeNode *args = node->children[1];
    if (args) {
        TreeNode *arg = args->children[0];
        while (arg) {
            checkExpression(arg, table);
            arg = arg->sibling;
        }
    }
}

int getExpressionType(TreeNode *expr, SymbolTable *table) {
    if (!expr) return VOID;
    
    switch (expr->type) {
        case NNumber:
            return INT;
            
        case NIdentifier: {
            Symbol *sym = lookup(table, expr->attribute.name);
            return sym ? sym->dataType : VOID;
        }
            
        case NOperator:
            // For simplicity, assume all operations return INT
            // In a real compiler, you'd need to check operator types
            return INT;
            
        default:
            return VOID;
    }
}

void checkTypeCompatibility(int expected, int actual, int lineno, const char *context) {
    if (expected != actual) {
        semanticError(lineno, "Type mismatch in %s: expected %s, got %s",
                     context,
                     expected == INT ? "int" : "void",
                     actual == INT ? "int" : "void");
    }
}