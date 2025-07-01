#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "../semantic/symbol-table.h"

static char codeText[MAX_CODE][LINE_SIZE];
static int emitLoc = 0;
static int tmpOffset = 0;
static SymbolTable *symtab = NULL;

void emitRO(const char *op, int r, int s, int t, const char *c) {
    snprintf(
        codeText[emitLoc], LINE_SIZE,
        "%3d:  %s  %d,%d,%d%s%s",
        emitLoc, op, r, s, t,
        COMMENTS_ENABLED ? " \t* " : "",
        COMMENTS_ENABLED ? c : ""
    );
    emitLoc++;
}

void emitRM(const char *op, int r, int d, int s, const char *c) {
    snprintf(
        codeText[emitLoc], LINE_SIZE,
        "%3d:  %s  %d,%d(%d)%s%s",
        emitLoc, op, r, d, s,
        COMMENTS_ENABLED ? " \t* " : "",
        COMMENTS_ENABLED ? c : ""
    );
    emitLoc++;
}

int emitSkip(int n) {
    int loc = emitLoc;
    emitLoc += n;
    if (emitLoc >= MAX_CODE) { fprintf(stderr,"codegen: buffer full\n"); exit(1); }
    return loc;
}

static void patchRM(int loc, int n) {
    char op[8]; int r, d, s;
    sscanf(codeText[loc] + 5, "%7s %d,%d(%d)", op, &r, &d, &s);
    snprintf(
        codeText[loc], LINE_SIZE,
        "%3d:  %s  %d,%d(%d)%spatched",
        loc, op, r, n, s,
        COMMENTS_ENABLED ? " \t* " : " "
    );
}

int getMemLoc(const char *name) {
    Symbol *s = lookup(symtab, name);
    if (!s) { fprintf(stderr,"Var '%s' not found\n", name); exit(1); }
    return s->offset;
}

static void cGen(TreeNode *t) {
    if (!t) return;

    switch (t->type) {

    case NDeclaration: break;

    case NAssign:{
        cGen(t->children[1]);
        int loc = getMemLoc(t->children[0]->attribute.name);
        emitRM("ST",AC,loc,GP,"store");
        break;
    }

    case NIdentifier:{
        int loc = getMemLoc(t->attribute.name);
        emitRM("LD",AC,loc,GP,"load");
        break;
    }

    case NNumber:{
        emitRM("LDC",AC,t->attribute.value,0,"const");
        break;
    }

    case NPlus:
    case NMinus:
    case NMultiply:
    case NDivide:{
        cGen(t->children[0]);
        emitRM("ST",AC,--tmpOffset,MP,"push");
        
        cGen(t->children[1]);
        emitRM("LD",AC1,tmpOffset++,MP,"pop");
        
        switch (t->type) {
            case NPlus:     emitRO("ADD",AC,AC1,AC,"+"); break;
            case NMinus:    emitRO("SUB",AC,AC1,AC,"-"); break;
            case NMultiply: emitRO("MUL",AC,AC1,AC,"*"); break;
            case NDivide:   emitRO("DIV",AC,AC1,AC,"/"); break;
            default: break;
        }
        break;
    }

    case NLess:
    case NLessEqual:
    case NGreater:
    case NGreaterEqual:
    case NEqual:
    case NNotEqual:{
        cGen(t->children[0]);
        emitRM("ST",AC,--tmpOffset,MP,"push");

        cGen(t->children[1]);
        emitRM("LD",AC1,tmpOffset++,MP,"pop");

        emitRO("SUB",AC,AC1,AC,"L-R");
        break;
    }

    case NIfStmt: {
        NodeType rel = t->children[0]->type;
        cGen(t->children[0]);

        int jFalse = emitLoc;
        switch (rel) {
            case NLess:         emitRM("JGE",AC,0,PC,"if False"); break;
            case NLessEqual:    emitRM("JGT",AC,0,PC,"if False"); break;
            case NGreater:      emitRM("JLE",AC,0,PC,"if False"); break;
            case NGreaterEqual: emitRM("JLT",AC,0,PC,"if False"); break;
            case NEqual:        emitRM("JNE",AC,0,PC,"if False"); break;
            case NNotEqual:     emitRM("JEQ",AC,0,PC,"if False"); break;
            default:            emitRM("JEQ",AC,0,PC,"if False"); break;
        }

        cGen(t->children[1]);

        int afterThen = emitLoc;
        patchRM(jFalse, afterThen - (jFalse + 1));
        break;
    }

    case NIfElseStmt: {
        NodeType rel = t->children[0]->type;
        cGen(t->children[0]);

        int jFalse = emitLoc;
        switch (rel) {
            case NLess:         emitRM("JGE",AC,0,PC,"if False"); break;
            case NLessEqual:    emitRM("JGT",AC,0,PC,"if False"); break;
            case NGreater:      emitRM("JLE",AC,0,PC,"if False"); break;
            case NGreaterEqual: emitRM("JLT",AC,0,PC,"if False"); break;
            case NEqual:        emitRM("JNE",AC,0,PC,"if False"); break;
            case NNotEqual:     emitRM("JEQ",AC,0,PC,"if False"); break;
            default:            emitRM("JEQ",AC,0,PC,"if False"); break;
        }

        cGen(t->children[1]);

        int jmpEnd = emitLoc;
        emitRM("LDA",PC,0,PC,"goto");

        int elseLoc = emitLoc;
        patchRM(jFalse, elseLoc - (jFalse + 1));

        cGen(t->children[2]);

        int endLoc = emitLoc;
        patchRM(jmpEnd, endLoc - (jmpEnd + 1));
        break;
    }

    default:
        for (int i = 0; i < MAX_CHILDREN; i++)
            cGen(t->children[i]);
        break;
    }
    cGen(t->sibling);
}

void codeToFile(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) { perror(filename); exit(1); }
    
    for (int i = 0; i < emitLoc; i++)
        fprintf(f, "%s\n", codeText[i]);
    
    fclose(f);
}

void generateCode(const char *filename, TreeNode *syntaxTree, SymbolTable *symbolTable) {
    symtab = symbolTable;

    emitRM("LD",MP,0,0,"load MP <- mem[0]");
    emitRM("ST",AC,0,0,"zero mem[0]");

    cGen(syntaxTree);

    emitRO("HALT",0,0,0,"finish");

    codeToFile(filename);
}
