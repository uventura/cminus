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
    if (!table || table == NULL) {
        fprintf(stderr, "Error: Symbol table is NULL\n");
        return NULL;
    } else if (table->currentScope < 0) {
        fprintf(stderr, "Error: Invalid scope %d in symbol table\n", table->currentScope);
        return NULL;
    } else if (lookup(table, name) != NULL) {
        fprintf(stderr, "Error: Symbol '%s' already exists in the current scope %d\n", name, table->currentScope);
        return NULL;
    } else if (name == NULL) {
        fprintf(stderr, "Error: Attempt to insert an empty or NULL name into symbol table\n");
        return NULL;
    } 

    // printf("INSERTING: %s at scope %d\n", name, table->currentScope);  //# DEBUG

    if (name == NULL || strlen(name) == 0) {
        fprintf(stderr, "Error: Attempt to insert NULL name into symbol table\n");
        return NULL;
    }

    Symbol* symbol = malloc(sizeof(Symbol));
    if (!symbol) return NULL;

    symbol->name = strdup(name);  // Now safe since we checked name
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