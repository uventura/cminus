#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "../lexer/tokens.h"

void semanticAnalysis(TreeNode *syntaxTree) {
    SymbolTable *table = createSymbolTable();
    checkNode(syntaxTree, table);
    freeSymbolTable(table);
}

void checkNode(TreeNode *node, SymbolTable *table) {
    if (!node) return;
    
    switch (node->type) {
        case NProgram:
            enterScope(table);
            // Check all children (statements)
            for (int i = 0; i < MAX_CHILDREN && node->children[i]; i++) {
                checkNode(node->children[i], table);
            }
            exitScope(table);
            break;
            
        case NAssign:
            checkVariable(node->children[0], table); // Check LHS
            checkExpression(node->children[1], table); // Check RHS
            break;
            
        case NConditional:
            checkExpression(node->children[0], table); // Check condition
            checkNode(node->children[1], table); // Check then branch
            if (node->children[2]) {
                checkNode(node->children[2], table); // Check else branch
            }
            break;
            
        case NExpr:
            checkExpression(node, table);
            break;
            
        default:
            // Check all children by default
            for (int i = 0; i < MAX_CHILDREN && node->children[i]; i++) {
                checkNode(node->children[i], table);
            }
            break;
    }
    
    // Check siblings
    if (node->sibling) {
        checkNode(node->sibling, table);
    }
}

void checkExpression(TreeNode *node, SymbolTable *table) {
    if (!node) return;
    
    switch (node->type) {
        case NIdentifier:
            checkVariable(node, table);
            break;
            
        case NNumber:
            // Numbers don't need semantic checking
            break;
            
        case NOperator:
            // Check operands
            if (node->children[0]) checkExpression(node->children[0], table);
            if (node->children[1]) checkExpression(node->children[1], table);
            break;
            
        default:
            // For other expression types, check all children
            for (int i = 0; i < MAX_CHILDREN && node->children[i]; i++) {
                checkExpression(node->children[i], table);
            }
            break;
    }
}

void checkVariable(TreeNode *node, SymbolTable *table) {
    // if (!node || node->type != NIdentifier) return;
    
    // Symbol *symbol = lookup(table, node->attribute.name);
    // if (!symbol) {
    //     fprintf(stderr, "Error: Variable '%s' not declared (line %d)\n", 
    //             node->attribute.name, node->lineno);
    //     exit(1);
    // }
    
    // if (symbol->type != VAR) {
    //     fprintf(stderr, "Error: '%s' is not a variable (line %d)\n", 
    //             node->attribute.name, node->lineno);
    //     exit(1);
    // }

    void checkVariable(TreeNode *node, SymbolTable *table) {
        if (!node || node->type != NIdentifier) return;
        
        Symbol *symbol = lookup(table, node->attribute.name);
        if (!symbol) {
            fprintf(stderr, "Semantic Error: Variable '%s' not declared (line %d)\n", 
                    node->attribute.name, node->lineno);
            exit(1);
        }
        
        if (symbol->type != VAR) {
            fprintf(stderr, "Semantic Error: '%s' is not a variable (line %d)\n", 
                    node->attribute.name, node->lineno);
            exit(1);
        }
        else {
            printf("Semantic OK: Variable '%s' properly declared\n", node->attribute.name);
        }
    }
}

void checkFunction(TreeNode *node, SymbolTable *table) {
    if (!node || node->type != NIdentifier) return;
    
    Symbol *symbol = lookup(table, node->attribute.name);
    if (!symbol) {
        fprintf(stderr, "Error: Function '%s' not declared (line %d)\n", 
                node->attribute.name, node->lineno);
        exit(1);
    }
    
    if (symbol->type != FUNC) {
        fprintf(stderr, "Error: '%s' is not a function (line %d)\n", 
                node->attribute.name, node->lineno);
        exit(1);
    }
}

void checkStatement(TreeNode *node, SymbolTable *table) {
    if (!node) return;
    
    switch (node->type) {
        case NAssign:
            checkVariable(node->children[0], table);
            checkExpression(node->children[1], table);
            break;
            
        case NConditional:
            checkExpression(node->children[0], table);
            checkNode(node->children[1], table);
            if (node->children[2]) {
                checkNode(node->children[2], table);
            }
            break;
            
        default:
            checkNode(node, table);
            break;
    }
}

SymbolTable* createSymbolTable() {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    if (!table) return NULL;
    table->head = NULL;
    table->currentScope = 0;
    return table;
}

void enterScope(SymbolTable *table) {
    if (table) table->currentScope++;
}

void exitScope(SymbolTable *table) {
    if (!table) return;
    
    // Remove all symbols in current scope
    Symbol **ptr = &table->head;
    while (*ptr) {
        if ((*ptr)->scope == table->currentScope) {
            Symbol *toRemove = *ptr;
            *ptr = toRemove->next;
            free(toRemove->name);
            free(toRemove);
        } else {
            ptr = &(*ptr)->next;
        }
    }
    table->currentScope--;
}

Symbol* lookup(SymbolTable *table, const char *name) {
    if (!table || !name) return NULL;
    
    Symbol *current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

Symbol* insert(SymbolTable *table, const char *name, SymbolType type, int dataType) {
    if (!table || !name) return NULL;
    
    Symbol *existing = lookup(table, name);
    if (existing && existing->scope == table->currentScope) {
        return NULL; // Already declared in this scope
    }
    
    Symbol *symbol = malloc(sizeof(Symbol));
    if (!symbol) return NULL;
    
    symbol->name = malloc(strlen(name) + 1);
    if (!symbol->name) {
        free(symbol);
        return NULL;
    }
    strcpy(symbol->name, name);
    
    symbol->type = type;
    symbol->dataType = dataType;
    symbol->scope = table->currentScope;
    symbol->next = table->head;
    table->head = symbol;
    return symbol;
}

void freeSymbolTable(SymbolTable *table) {
    if (!table) return;
    
    Symbol *current = table->head;
    while (current) {
        Symbol *next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    free(table);
}