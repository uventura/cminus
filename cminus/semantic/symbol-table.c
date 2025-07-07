#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol-table.h"
#include "../codegen/codegen.h"

static int nextOffset = 0;

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

void checkUnusedVariables(SymbolTable *table, int currentScope) {
    Symbol *current = table->head;
    while (current) {
        if (current->scope == currentScope && 
            current->type == VAR && 
            !current->wasUsed) {
            fprintf(stderr, "Warning (line %d): Variable '%s' of type %s declared but never used\n", 
                   current->scope, current->name, typeToString(current->dataType));
        }
        current = current->next;
    }
}

// Em semantic/symbol-table.c, na função exitScope

void exitScope(SymbolTable *table) {
    if (!table) return;
    
    // Warns of unused variables in the current scope
    checkUnusedVariables(table, table->currentScope);
    
    Symbol **ptr = &table->head;
    while (*ptr) {
        if ((*ptr)->scope == table->currentScope) {
            Symbol *toRemove = *ptr;
            *ptr = toRemove->next;
            
            // --- NOVO TRECHO DE CÓDIGO AQUI ---
            // Se o símbolo que está sendo removido é uma VARIÁVEL
            // E ela tinha um registrador atribuído (assigned_reg != -1)
            if (toRemove->type == VAR && toRemove->assigned_reg != -1) {
                // Chame a função de `codegen.c` para liberar esse registrador físico.
                free_gpr(toRemove->assigned_reg); 
            }
            // --- FIM DO NOVO TRECHO ---

            free(toRemove->name); // Libera a memória do nome da variável
            free(toRemove);       // Libera a memória do próprio símbolo
        } else {
            ptr = &(*ptr)->next; // Move para o próximo símbolo na lista
        }
    }
    
    table->currentScope--; // Decrementa o escopo atual
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

void markSymbolAsUsed(SymbolTable *table, const char *name) {
    if (!table || !name) return;
    
    Symbol *current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            current->wasUsed = 1;
            return;
        }
        current = current->next;
    }
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

    symbol->assigned_reg = -1; // Indica que a variável ainda não está em um registrador

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
    symbol->wasUsed = 0;  // unused
    if (type == VAR) symbol->offset = nextOffset++;
    
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