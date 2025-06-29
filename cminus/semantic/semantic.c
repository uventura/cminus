#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "semantic.h"
#include "../lexer/tokens.h"
// #include "../helpers.h"

// Global flag to track if any semantic errors were found
static int hasSemanticErrors = 0;


static const char* getOperatorString(NodeType type) {
    switch(type) {
        case NPlus: return "+";
        case NMinus: return "-";
        case NMultiply: return "*";
        case NDivide: return "/";
        case NLess: return "<";
        case NLessEqual: return "<=";
        case NGreater: return ">";
        case NGreaterEqual: return ">=";
        case NEqual: return "==";
        case NNotEqual: return "!=";
        default: return "unknown";
    }
}


void printSymbolTable(SymbolTable *table) {
    printf("\tDEBUG: Symbol Table (scope %d):\n", table->currentScope);
    Symbol *current = table->head;
    while (current) {
        printf("\t  %s: %s (scope %d, type: %s)\n", 
            current->name, 
            current->type == VAR ? "var" : "func",
            current->scope,
            typeToString(current->dataType));
        current = current->next;
    }
}
    

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
    printf("Program is semantically correct!\n");
}

void checkProgram(TreeNode *node, SymbolTable *table) {
    if (!node) {
        semanticError(node ? node->lineno : 0, "Expected program node");
        return;
    }

    enterScope(table);

    switch (node->type) {
        case NProgram:
        case NCmdList:
        case NCmd:
        case NBlock:
        case NCompoundStmt:
            break;
        case NDeclaration:
            printf("\tDEBUG: case NDeclaration\n");
            checkDeclaration(node, table);
            return;
        case NAssignStmt:
        case NAssign:
            printf("\tDEBUG: case NAssignStmt / NAssign\n");
            checkAssignment(node, table);
            return;
        case NIdentifier:
            printf("\tDEBUG: case NIdentifier\n");
            if (!lookup(table, node->attribute.name)) {
                semanticError(node->lineno, "Undeclared identifier '%s'", node->attribute.name);
            }
            markSymbolAsUsed(table, node->attribute.name); 
            return;
        case NStatement:
            checkStatement(node, table);
            return;
        default:
            printf("\tunexpected node type, likely an ERROR\n");
            break;
    }

    TreeNode *child = node->children[0];
    while (child) {
        switch (child->type) {
            case NProgram:
                printf("\tDEBUG: case NProgram\n");
                semanticError(child->lineno, "Nested program found");
                break;
            case NCmdList:
                printf("\tDEBUG: case NCmdList\n");
                {
                    TreeNode *cmd = child->children[0];
                    while (cmd) {
                        checkProgram(cmd, table);
                        cmd = cmd->sibling;
                    }
                }
                break;
            case NCmd:
                printf("\tDEBUG: case NCmd\n");
                if (child->children[0]) {
                    checkProgram(child->children[0], table);
                }
                break;
            case NDeclaration:
                printf("\tDEBUG: case NDeclaration\n");
                checkDeclaration(child, table);
                break;
            case NStatement:
                printf("\tDEBUG: case NStatement\n");
                checkStatement(child, table);
                break;
            case NBlock:
                printf("\tDEBUG: case NBlock\n");
                enterScope(table);
                checkBlock(child, table);
                exitScope(table);
                break;
            case NAssignStmt:
                printf("\tDEBUG: case NAssignStmt\n");
                checkAssignment(child, table);
                break;
            case NIdentifier:
                printf("\tDEBUG: case NIdentifier\n");
                if (!lookup(table, child->attribute.name)) {
                    semanticError(child->lineno, "Undeclared identifier '%s'", child->attribute.name);
                    printSymbolTable(table);
                }
                break;
            case NExpr:
                printf("\tDEBUG: case NExpr\n");
                checkExpression(child, table);
                break;
            case NOperator:
                printf("\tDEBUG: case NOperator\n");
                if (child->children[0]) checkExpression(child->children[0], table);
                if (child->children[1]) checkExpression(child->children[1], table);
                break;
            case NNumber:
                printf("\tDEBUG: case NNumber\n");
                break;
            case NIfElseStmt:
                printf("\tDEBUG: case NIfElseStmt\n");
                checkIfStatement(child, table);
                break;
            case NReturnStmt:
                printf("\tDEBUG: case NReturnStmt\n");
                checkReturnStatement(child, table);
                break;
            case NCompoundStmt:
                printf("\tDEBUG: case NCompoundStmt\n");
                enterScope(table);
                checkStatement(child, table);
                exitScope(table);
                break;
            case NCall:
                printf("\tDEBUG: case NCall\n");
                checkFunctionCall(child, table);
                break;
            case NAssign:
                printf("\tDEBUG: case NAssign\n");
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

    TreeNode *stmt = node->children[0];
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
    
        DataType lhsType = sym->dataType;
        DataType rhsType = getExpressionType(rhs, table);
        
        printf("DEBUG: Assignment type check - lhs: %s (%s), rhs: %s\n",
               lhs->attribute.name, 
               typeToString(lhsType),
               typeToString(rhsType));
        
        if (lhsType != rhsType) {
            semanticError(node->lineno, 
                "Type mismatch in assignment to '%s': expected %s, got %s",
                lhs->attribute.name,
                typeToString(lhsType),
                typeToString(rhsType));
        }
    }
}

void checkIfStatement(TreeNode *node, SymbolTable *table) {
    if (!node || node->type != NIfElseStmt) {
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
    printf("\tDEBUG: Checking declaration, name: %s\n", 
           node->children[1]->attribute.name);

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

    int dataType = typeNode->dataType;
    const char *name = idNode->attribute.name;

    // Propagate type to identifier node
    idNode->dataType = dataType;
    printf("DEBUG: Setting type %d for identifier %s\n", dataType, name);

    Symbol *existing = lookup(table, name);
    if (existing && existing->scope == table->currentScope) {
        semanticError(idNode->lineno, "Redeclaration of '%s'", name);
        return;
    }

    Symbol *symbol = insert(table, name, VAR, dataType);
    if (!symbol) {
        semanticError(idNode->lineno, "Failed to insert '%s' into symbol table", name);
        return;
    }
    
    printf("DEBUG: Added to symbol table - %s: %d\n", name, dataType);
}

void checkStatement(TreeNode *node, SymbolTable *table) {
    if (!node) return;

    switch (node->type) {
        case NCompoundStmt:
            enterScope(table);
            TreeNode *stmt = node->children[0];
            while (stmt) {
                checkStatement(stmt, table);
                stmt = stmt->sibling;
            }
            exitScope(table);
            break;
            
        case NAssignStmt:
            checkExpression(node->children[0], table);
            checkExpression(node->children[1], table);
            break;
            
        case NIfElseStmt:
            checkExpression(node->children[0], table);
            checkStatement(node->children[1], table);
            if (node->children[2]) {
                checkStatement(node->children[2], table);
            }
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
            if (!lookup(table, node->attribute.name)) {
                semanticError(node->lineno, "Undeclared identifier '%s'", node->attribute.name);
            }
            break;
            
        case NNumber:
            // Numbers are always valid
            break;
            
        case NPlus:
        case NMinus:
        case NMultiply:
        case NDivide:
        case NLess:
        case NLessEqual:
        case NGreater:
        case NGreaterEqual:
        case NEqual:
        case NNotEqual:
            // Check both operands
            if (node->children[0]) checkExpression(node->children[0], table);
            if (node->children[1]) checkExpression(node->children[1], table);
            
            // Verify operand types are compatible
            if (node->children[0] && node->children[1]) {
                DataType leftType = getExpressionType(node->children[0], table);
                DataType rightType = getExpressionType(node->children[1], table);
                
                if (leftType != TYPE_INT || rightType != TYPE_INT) {
                    semanticError(node->lineno, 
                        "Operands must be integers for operator '%s'",
                        getOperatorString(node->type));
                }
            }
            break;
            
        default:
            semanticError(node->lineno, "Unknown expression type %d", node->type);
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
    
    TreeNode *args = node->children[1];
    if (args) {
        TreeNode *arg = args->children[0];
        while (arg) {
            checkExpression(arg, table);
            arg = arg->sibling;
        }
    }
}

DataType getExpressionType(TreeNode *expr, SymbolTable *table) {
    if (!expr) {
        printf("DEBUG: getExpressionType called with NULL expression\n");
        return TYPE_ERROR;
    }
    
    switch (expr->type) {
        case NNumber:
            return TYPE_INT;
            
        case NIdentifier: {
            Symbol *sym = lookup(table, expr->attribute.name);
            return sym ? sym->dataType : TYPE_ERROR;
        }
            
        case NPlus:
        case NMinus:
        case NMultiply:
        case NDivide:
            return TYPE_INT;
            
        default:
            return TYPE_ERROR;
    }
}


void checkTypeCompatibility(DataType expected, DataType actual, int lineno, const char *context) {
    if (expected != actual) {
        semanticError(lineno, 
            "Type mismatch in %s: expected %s, got %s",
            context,
            typeToString(expected),
            typeToString(actual));
    }
}

