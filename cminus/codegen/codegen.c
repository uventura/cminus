#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "../semantic/symbol-table.h"


static char codeText[MAX_CODE][LINE_SIZE];
static int emitLoc = 0;
static int tmpOffset = 0; // Offset para variáveis temporárias na pilha, relativo a MP
static SymbolTable *symtab = NULL; // Tabela de símbolos para lookup de variáveis

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
    char op[8]; int r, d_old, s;
    char comment_part[LINE_SIZE];

    const char *line = codeText[loc];
    if (sscanf(line + 5, "%7s %d,%d(%d)", op, &r, &d_old, &s) != 4) {
        fprintf(stderr, "Error parsing instruction at line %d for patching: %s\n", loc, line);
        exit(1);
    }

    if (COMMENTS_ENABLED) {
        const char *comment_start_ptr = strstr(line, " \t* ");
        if (comment_start_ptr) {
            strncpy(comment_part, comment_start_ptr + 4, LINE_SIZE - 1);
            comment_part[LINE_SIZE - 1] = '\0';
            char *existing_patched_suffix = strstr(comment_part, " patched");
            if (existing_patched_suffix) {
                *existing_patched_suffix = '\0';
            }
        } else {
            comment_part[0] = '\0';
        }
    } else {
        comment_part[0] = '\0';
    }

    snprintf(
        codeText[loc], LINE_SIZE,
        "%3d:  %s  %d,%d(%d)%s%s%s",
        loc, op, r, n, s,
        COMMENTS_ENABLED ? " \t* " : " ",
        COMMENTS_ENABLED ? comment_part : "",
        COMMENTS_ENABLED ? " patched" : "patched"
    );
}

int getMemLoc(const char *name) {
    Symbol *s = lookup(symtab, name);
    if (!s) { fprintf(stderr,"Var '%s' not found in symbol table.\n", name); exit(1); }
    return s->offset;
}

static void cGen(TreeNode *t) {
    if (!t) return;

    switch (t->type) {

    case NDeclaration: break;

    case NAssign:{
        cGen(t->children[1]);
        int loc = getMemLoc(t->children[0]->attribute.name);
        emitRM("ST",AC,loc,GP,"store variable");
        break;
    }

    case NIdentifier:{
        int loc = getMemLoc(t->attribute.name);
        emitRM("LD",AC,loc,GP,"load identifier");
        break;
    }

    case NNumber:{
        emitRM("LDC",AC,t->attribute.value,0,"load constant");
        break;
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
    case NNotEqual:{
        cGen(t->children[0]);

        --tmpOffset;
        if (abs(tmpOffset) >= 1 && abs(tmpOffset) <= 3) {
            int targetReg = abs(tmpOffset) + 1; 
            emitRM("LDA", targetReg, 0, AC, "push temp to register");
        } else {
            emitRM("ST", AC, tmpOffset, MP, "push temp to memory (fallback)");
        }

        cGen(t->children[1]);


        if (abs(tmpOffset) >= 1 && abs(tmpOffset) <= 3) { 
            int sourceReg = abs(tmpOffset) + 1;
            emitRM("LDA", AC1, 0, sourceReg, "pop temp from register to AC1");
        } else {
            emitRM("LD", AC1, tmpOffset, MP, "pop temp from memory to AC1 (fallback)");
        }

        tmpOffset++;

        switch (t->type) {
            case NPlus:         emitRO("ADD", AC, AC1, AC, "add"); break;
            case NMinus:        emitRO("SUB", AC, AC1, AC, "subtract"); break;
            case NMultiply:     emitRO("MUL", AC, AC1, AC, "multiply"); break;
            case NDivide:       emitRO("DIV", AC, AC1, AC, "divide"); break;
            case NLess:
            case NLessEqual:
            case NGreater:
            case NGreaterEqual:
            case NEqual:
            case NNotEqual:     emitRO("SUB", AC, AC1, AC, "compare (LHS - RHS)"); break;
            default: break;
        }
        break;
    }

    case NIfStmt: {
        NodeType rel = t->children[0]->type;
        cGen(t->children[0]);

        int jFalse = emitLoc;
        switch (rel) {
            case NLess:         emitRM("JGE",AC,0,PC,"if false (LHS < RHS)"); break;
            case NLessEqual:    emitRM("JGT",AC,0,PC,"if false (LHS <= RHS)"); break;
            case NGreater:      emitRM("JLE",AC,0,PC,"if false (LHS > RHS)"); break;
            case NGreaterEqual: emitRM("JLT",AC,0,PC,"if false (LHS >= RHS)"); break;
            case NEqual:        emitRM("JNE",AC,0,PC,"if false (LHS == RHS)"); break;
            case NNotEqual:     emitRM("JEQ",AC,0,PC,"if false (LHS != RHS)"); break;
            default:            emitRM("JEQ",AC,0,PC,"if false (default)"); break;
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
            case NLess:         emitRM("JGE",AC,0,PC,"if false (LHS < RHS) -> else"); break;
            case NLessEqual:    emitRM("JGT",AC,0,PC,"if false (LHS <= RHS) -> else"); break;
            case NGreater:      emitRM("JLE",AC,0,PC,"if false (LHS > RHS) -> else"); break;
            case NGreaterEqual: emitRM("JLT",AC,0,PC,"if false (LHS >= RHS) -> else"); break;
            case NEqual:        emitRM("JNE",AC,0,PC,"if false (LHS == RHS) -> else"); break;
            case NNotEqual:     emitRM("JEQ",AC,0,PC,"if false (LHS != RHS) -> else"); break;
            default:            emitRM("JEQ",AC,0,PC,"if false (default) -> else"); break;
        }

        cGen(t->children[1]);

        int jmpEnd = emitLoc;
        emitRM("LDA",PC,0,PC,"jump over else block");

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


    emitRM("LDC", GP, 0, 0, "Initialize GP to 0 (base for static data)");

    emitRM("LDC", MP, TM_MAX_MEM - 1, 0, "Initialize MP to top of memory (stack base, grows down)");

    emitRM("LDC", AC, 0, 0, "Clear AC (accumulator)");

    emitRM("ST", AC, 0, 0, "Clear mem[0] (or initialize global 0 constant)");

    cGen(syntaxTree);

    emitRO("HALT",0,0,0,"finish");

    codeToFile(filename);
}