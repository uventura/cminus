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
    printf("Semantic analysis completed successfully!\n");
}

void checkProgram(TreeNode *node, SymbolTable *table) {
    printf("  DEBUG: checkProgram() called with node type %d \n", node->type);

    if (!node ) {
        semanticError(node ? node->lineno : 0, "Expected program node");
        return;
    }

    enterScope(table);
    
    // Check all declarations and statements
    TreeNode *child = node->children[0]; // First child is typically declarations
    while (child) {
        switch (child->type) {
            case NProgram:
                semanticError(child->lineno, "Nested program found");
                break;
            case NCmdList:
                // Command list - check all commands
                {
                    TreeNode *cmd = child->children[0];
                    while (cmd) {
                        checkProgram(cmd, table);
                        cmd = cmd->sibling;
                    }
                }
                break;
            case NCmd:
                // Command - could be declaration or statement
                if (child->children[0]) {
                    checkProgram(child->children[0], table);
                }
                break;
            case NDeclaration:
                checkDeclaration(child, table);
                break;
            case NStatement:
                checkStatement(child, table);
                break;
            case NBlock:
                enterScope(table);
                checkBlock(child, table);
                exitScope(table);
                break;
            case NAssignStmt:
                checkAssignment(child, table);
                break;
            case NIdentifier:
                if (!lookup(table, child->attribute.name)) {
                    semanticError(child->lineno, "Undeclared identifier '%s'", child->attribute.name);
                }
                break;
            case NExpr:
                checkExpression(child, table);
                break;
            case NOperator:
                if (child->children[0]) checkExpression(child->children[0], table);
                if (child->children[1]) checkExpression(child->children[1], table);
                break;
            case NNumber:
                break;
            case NIfStmt:
                checkIfStatement(child, table);
                break;
            case NWhileStmt:
                checkWhileStatement(child, table);
                break;
            case NReturnStmt:
                checkReturnStatement(child, table);
                break;
            case NCompoundStmt:
                enterScope(table);
                checkStatement(child, table);
                exitScope(table);
                break;
            case NCall:
                checkFunctionCall(child, table);
                break;
            case NAssign:
                checkAssignment(child, table);
                break;
            default:
                semanticError(child->lineno, "Unexpected node type %d in program", child->type);
                break;
        }
        child = child->sibling;
    }
    
    exitScope(table);
}

void checkBlock(TreeNode *node, SymbolTable *table) {
    if (!node || node->type != NBlock) {
        semanticError(node ? node->lineno : 0, "Expected block node");
        return;
    }

    TreeNode *stmt = node->children[0]; // stmt_list
    while (stmt) {
        checkProgram(stmt, table);
        stmt = stmt->sibling;
    }
}

void checkAssignment(TreeNode *node, SymbolTable *table) {
    if (!node || (node->type != NAssign && node->type != NAssignStmt)) {
        semanticError(node ? node->lineno : 0, "Expected assignment node");
        return;
    }

    TreeNode *lhs = node->children[0];
    TreeNode *rhs = node->children[1];

    if (!lhs || lhs->type != NIdentifier) {
        semanticError(node->lineno, "Assignment left-hand side must be an identifier");
        return;
    }
    Symbol *sym = lookup(table, lhs->attribute.name);
    if (!sym) {
        semanticError(lhs->lineno, "Undeclared variable '%s'", lhs->attribute.name);
        return;
    }
    if (rhs) {
        checkExpression(rhs, table);
    
        int lhsType = sym->dataType;
        int rhsType = getExpressionType(rhs, table);
        checkTypeCompatibility(lhsType, rhsType, node->lineno, "assignment");
    }
}

void checkIfStatement(TreeNode *node, SymbolTable *table) {
    if (!node || node->type != NIfStmt) {
        semanticError(node ? node->lineno : 0, "Expected if statement node");
        return;
    }


    TreeNode *cond = node->children[0];
    if (cond) {
        checkExpression(cond, table);
        int condType = getExpressionType(cond, table);
        if (condType != INT) {
            semanticError(cond->lineno, "If condition must be of type int");
        }
    }

    TreeNode *thenClause = node->children[1];
    if (thenClause) {
        enterScope(table);
        checkProgram(thenClause, table);
        exitScope(table);
    }

    TreeNode *elseClause = node->children[2];
    if (elseClause) {
        enterScope(table);
        checkProgram(elseClause, table);
        exitScope(table);
    }
}

void checkWhileStatement(TreeNode *node, SymbolTable *table) {
    if (!node || node->type != NWhileStmt) {
        semanticError(node ? node->lineno : 0, "Expected while statement node");
        return;
    }

    TreeNode *cond = node->children[0];
    if (cond) {
        checkExpression(cond, table);
        int condType = getExpressionType(cond, table);
        if (condType != INT) {
            semanticError(cond->lineno, "While condition must be of type int");
        }
    }

    TreeNode *body = node->children[1];
    if (body) {
        enterScope(table);
        checkProgram(body, table);
        exitScope(table);
    }
}

void checkReturnStatement(TreeNode *node, SymbolTable *table) {
    if (!node || node->type != NReturnStmt) {
        semanticError(node ? node->lineno : 0, "Expected return statement node");
        return;
    }

    TreeNode *expr = node->children[0];
    if (expr) {
        checkExpression(expr, table);
    }
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