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
    case NDivide:
    case NLess:
    case NLessEqual:
    case NGreater:
    case NGreaterEqual:
    case NEqual:
    case NNotEqual:{
  // 1. Gera código para o operando esquerdo. O resultado estará em AC.
        cGen(t->children[0]);

        // 2. Decrementa tmpOffset para alocar um novo slot para o temporário.
        --tmpOffset;

        // 3. Decide se o temporário vai para um registrador ou para a memória.
        //    Registradores disponíveis para temporários: AC1 (1), R2 (2), R3 (3), R4 (4).
        //    Pool de registradores temporários: de 1 a 4.
        //    Se tmpOffset for -1, abs(tmpOffset) = 1 (AC1)
        //    Se tmpOffset for -2, abs(tmpOffset) = 2 (R2)
        //    Se tmpOffset for -3, abs(tmpOffset) = 3 (R3)
        //    Se tmpOffset for -4, abs(tmpOffset) = 4 (R4)
        if (abs(tmpOffset) >= 1 && abs(tmpOffset) <= 4) {
            int targetReg = abs(tmpOffset); // Registrador destino (1 a 4)
            // Usa LDA como MOV: move o valor de AC para o registrador temporário
            emitRM("LDA", targetReg, 0, AC, "push temp to register");
        } else {
            // Pool de registradores esgotada ou tmpOffset fora do range, volta para a memória
            emitRM("ST", AC, tmpOffset, MP, "push temp to memory");
        }

        // 4. Gera código para o operando direito. O resultado estará em AC.
        cGen(t->children[1]);

        // 5. Carrega o operando esquerdo de volta para AC1 (ou R0, se preferir, mas AC1 é o padrão).
        //    O valor de tmpOffset ainda aponta para o slot onde o temporário foi salvo.
        if (abs(tmpOffset) >= 1 && abs(tmpOffset) <= 4) {
            int sourceReg = abs(tmpOffset); // Registrador de onde o temp foi salvo
            // Usa LDA como MOV: move o valor do registrador temporário para AC1
            emitRM("LDA", AC1, 0, sourceReg, "pop temp from register to AC1");
        } else {
            // Temporário foi salvo na memória, carrega de lá para AC1
            emitRM("LD", AC1, tmpOffset, MP, "pop temp from memory to AC1");
        }

        // 6. Incrementa tmpOffset para liberar o slot temporário.
        tmpOffset++;

        // 7. Emite a instrução da operação real.
        //    Neste ponto, o operando esquerdo está em AC1, e o operando direito está em AC.
        switch (t->type) {
            case NPlus:         emitRO("ADD", AC, AC1, AC, "+"); break;
            case NMinus:        emitRO("SUB", AC, AC1, AC, "-"); break;
            case NMultiply:     emitRO("MUL", AC, AC1, AC, "*"); break;
            case NDivide:       emitRO("DIV", AC, AC1, AC, "/"); break;
            // Para operações relacionais, o resultado da comparação (L-R) já está em AC
            case NLess:
            case NLessEqual:
            case NGreater:
            case NGreaterEqual:
            case NEqual:
            case NNotEqual:     emitRO("SUB", AC, AC1, AC, "compare (L-R)"); break;
            default: break; // Caso inesperado
        }
        break;
    }
    // --- FIM DA SEÇÃO CRÍTICA PARA A OTIMIZAÇÃO ---
    

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
