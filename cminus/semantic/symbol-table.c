#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol-table.h"

SymbolTable* createSymbolTable() {
    SymbolTable *table = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!table) return NULL;
    
    table->head = NULL;
    table->currentScope = 0;
    return table;
}

void enterScope(SymbolTable *table) {
    if (table) {
        table->currentScope++;
    }
}

void exitScope(SymbolTable *table) {
    if (!table) return;
    
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
    // printf("\tDEBUG LOOKING UP: %s at scope %d\n", name, table->currentScope);   //# DEBUG

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
    // printf("INSERTING: %s at scope %d\n", name, table->currentScope);  //# DEBUG

    if (!table || !name) return NULL;
    
    // Check for redeclaration in current scope!
    Symbol *existing = lookup(table, name);
    if (existing && existing->scope == table->currentScope) {
        return NULL;
    }
    
    Symbol *symbol = (Symbol*)malloc(sizeof(Symbol));
    if (!symbol) return NULL;
    
    symbol->name = strdup(name);
    if (!symbol->name) {
        free(symbol);
        return NULL;
    }
    
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