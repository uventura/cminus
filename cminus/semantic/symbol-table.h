#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "tree-node.h"

typedef enum {
    VAR,
    FUNC
} SymbolType;

typedef struct Symbol {
    char *name;
    SymbolType type;
    int dataType; // INT or VOID from tokens.h
    int scope;
    struct Symbol *next;
} Symbol;

typedef struct SymbolTable {
    Symbol *head;
    int currentScope;
} SymbolTable;

SymbolTable* createSymbolTable();
void enterScope(SymbolTable *table);
void exitScope(SymbolTable *table);
Symbol* lookup(SymbolTable *table, const char *name);
Symbol* insert(SymbolTable *table, const char *name, SymbolType type, int dataType);
void freeSymbolTable(SymbolTable *table);

#endif