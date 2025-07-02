#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "semantic.h"
#include "../lexer/tokens.h"
#include "../utils/debug.h"

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
    debug_print("\tdebug Semantic: Symbol Table (scope %d):\n", table->currentScope);
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
    fprintf(stderr, "Symbol table \n");
    printSymbolTable(table);
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
            checkDeclaration(node, table);
            return;
        case NAssignStmt:
        case NAssign:
            checkAssignment(node, table);
            return;
        case NIdentifier:
            if (!lookup(table, node->attribute.name)) {
                semanticError(node->lineno, "Undeclared identifier '%s'", node->attribute.name);
            }
            markSymbolAsUsed(table, node->attribute.name); 
            return;
        case NStatement:
            checkStatement(node, table);
            return;
        case NIfElseStmt: 
            checkIfStatement(node, table);
            return;
        default:
            printf("\tunexpected node type, likely an ERROR\n");
            break;
    }

    TreeNode *child = node->children[0];
    while (child) {
        switch (child->type) {
            case NProgram:
                semanticError(child->lineno, "Nested program found");
                break;
            case NCmdList:
                {
                    TreeNode *cmd = child->children[0];
                    while (cmd) {
                        checkProgram(cmd, table);
                        cmd = cmd->sibling;
                    }
                }
                break;
            case NCmd:
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
                    printSymbolTable(table);
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
            case NIfElseStmt:
                checkIfStatement(child, table);
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
                semanticError(child->lineno, "Unexpected node type %s (%d) in program", getNodeTypeName(child->type), child->type);
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

    debug_print("\tdebug Semantic: Entering block at line %d\n", node->lineno);
    enterScope(table);
    
    TreeNode *stmt = node->children[0];
    while (stmt) {
        checkProgram(stmt, table);
        stmt = stmt->sibling;
    }
    
    exitScope(table);
    debug_print("\tdebug Semantic: Exiting block at line %d\n", node->lineno);
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
        semanticError(lhs->lineno, "Undeclared variable '%s'. Variables in scope", lhs->attribute.name);
        printSymbolTable(table);
        return;
    }

    if (rhs) {
        checkExpression(rhs, table);
    
        DataType lhsType = sym->dataType;
        DataType rhsType = getExpressionType(rhs, table);
        
        debug_print("\tdebug Semantic: Assignment type check - lhs: %s (%s), rhs: %s\n",
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
    if (!node || (node->type != NIfElseStmt && node->type != NIfStmt)) {
        semanticError(node ? node->lineno : 0, "Expected if statement node");
        return;
    }

    // Check condition
    TreeNode *cond = node->children[0];
    if (cond) {
        checkExpression(cond, table);
        DataType condType = getExpressionType(cond, table);
        
        if (condType == TYPE_VOID) {
            semanticError(cond->lineno, "If condition cannot be void");
        }
    }

    // Check then branch with new scope
    TreeNode *thenBranch = node->children[1];
    if (thenBranch) {
        enterScope(table);
        checkProgram(thenBranch, table);
        exitScope(table);
    }

    // Check else branch if it exists (for NIfElseStmt)
    if (node->type == NIfElseStmt) {
        TreeNode *elseBranch = node->children[2];
        if (elseBranch) {
            enterScope(table);
            checkProgram(elseBranch, table);
            exitScope(table);
        }
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
    debug_print("\tdebug Semantic: Checking declaration, name: %s\n", 
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

    debug_print("\tdebug Semantic: Declaring %s with type %d\n", 
           idNode->attribute.name, typeNode->dataType);

    int dataType = typeNode->dataType;
    const char *name = idNode->attribute.name;

    // Propagate type to identifier node
    idNode->dataType = dataType;
    debug_print("\tdebug Semantic: Setting type %d for identifier %s\n", dataType, name);

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
    
    debug_print("\tdebug Semantic: Added to symbol table - %s: %d\n", name, dataType);
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
            
        case NIfStmt:
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
        debug_print("\tdebug Semantic: getExpressionType called with NULL expression\n");
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
        case NLess:
        case NLessEqual:
        case NGreater:
        case NGreaterEqual:
        case NEqual:
        case NNotEqual:
            // this compiler only supports INT
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

