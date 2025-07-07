#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "../semantic/symbol-table.h"

static char codeText[MAX_CODE][LINE_SIZE];
static int emitLoc = 0;
static int tmpOffset = 0;
static SymbolTable *symtab = NULL;

// Em codegen.c (após os includes e as variáveis estáticas já existentes)

// --- Variáveis de Controle do Estado dos GPRs ---

// `gpr_map`: Um array que guarda o NOME da variável que está em cada GPR.
// O índice 0 corresponde a R2_REG, o 1 a R3_REG, etc.
// Se um elemento for NULL, o GPR correspondente está livre.
static char* gpr_map[MAX_ALLOCATABLE_GPR]; 

// `gpr_busy`: Um array de booleanos (0 ou 1) que indica se um GPR está ocupado (1) ou livre (0).
// Sincronizado com `gpr_map`.
static int gpr_busy[MAX_ALLOCATABLE_GPR];

// --- Funções para Gerenciar GPRs ---

// Função: init_gprs
// Objetivo: Limpar o estado de todos os GPRs, marcando-os como livres.
// Por que é necessário: Cada vez que começamos a gerar código para um novo programa,
//                      queremos que todos os registradores estejam "limpos" e disponíveis.
void init_gprs() {
    for (int i = 0; i < MAX_ALLOCATABLE_GPR; ++i) {
        gpr_busy[i] = 0; // Marca o registrador como livre
        if (gpr_map[i]) { // Se havia um nome de variável associado (de uma execução anterior)
            free(gpr_map[i]); // Libera a memória alocada para o nome
            gpr_map[i] = NULL; // Zera a referência
        }
    }
}

// Função: get_free_gpr
// Objetivo: Tentar encontrar e reservar um registrador GPR livre.
// Retorna: O ID do registrador físico (ex: 2 para R2_REG) se encontrar um, ou -1 se não houver GPRs livres.
// Como funciona: Percorre a lista de GPRs e pega o primeiro que não estiver ocupado.
int get_free_gpr() {
    for (int i = 0; i < MAX_ALLOCATABLE_GPR; ++i) {
        if (!gpr_busy[i]) { // Se o GPR no índice 'i' está livre
            gpr_busy[i] = 1; // Marca ele como ocupado
            gpr_map[i] = NULL; // A princípio, não tem nenhuma variável associada ainda
            return R2_REG + i; // Retorna o ID físico do registrador (ex: 2, 3, 4)
        }
    }
    return -1; // Se o loop terminar e não encontrar nenhum livre, retorna -1
}

// Função: free_gpr
// Objetivo: Liberar um registrador GPR previamente ocupado.
// Parâmetro: `reg_id` é o ID do registrador físico a ser liberado (ex: 2, 3, 4).
// Por que é necessário: Quando uma variável não é mais necessária (ex: sai de escopo),
//                      o registrador que ela ocupava pode ser reutilizado.
void free_gpr(int reg_id) {
    // Verifica se o reg_id fornecido é um dos GPRs gerenciáveis
    if (reg_id >= R2_REG && reg_id < R2_REG + MAX_ALLOCATABLE_GPR) {
        int index = reg_id - R2_REG; // Converte o ID físico para o índice do array (0, 1, 2)
        gpr_busy[index] = 0; // Marca o registrador como livre
        if (gpr_map[index]) { // Se havia um nome de variável associado a ele
            free(gpr_map[index]); // Libera a memória do nome da variável
            gpr_map[index] = NULL; // Zera a referência
        }
    }
}

// Função: occupy_gpr
// Objetivo: Associar um nome de variável a um registrador que foi acabado de alocar.
// Parâmetros: `reg_id` é o ID do registrador físico; `var_name` é o nome da variável.
// Por que é necessário: Quando `get_free_gpr` retorna um registrador, ele apenas marca
//                      como ocupado. `occupy_gpr` efetivamente "coloca" a variável
//                      dentro do registrador virtualmente.
void occupy_gpr(int reg_id, const char* var_name) {
    // Verifica se o reg_id é válido para os nossos GPRs
    if (reg_id >= R2_REG && reg_id < R2_REG + MAX_ALLOCATABLE_GPR) {
        int index = reg_id - R2_REG; // Converte o ID físico para o índice do array
        gpr_busy[index] = 1; // Garante que está marcado como ocupado (redundante, mas seguro)
        if (gpr_map[index]) free(gpr_map[index]); // Libera o nome antigo se o registrador foi reocupado
        gpr_map[index] = strdup(var_name); // Copia o nome da variável para o mapa de registradores
                                          // strdup aloca memória, então precisamos liberar depois.
    }
}

// Função: find_var_in_gpr
// Objetivo: Verificar se uma variável específica já está alocada em algum GPR.
// Retorna: O ID do registrador físico se a variável for encontrada, ou -1 caso contrário.
// Por que é necessário: Antes de carregar uma variável, queremos saber se ela já está em um registrador.
int find_var_in_gpr(const char* var_name) {
    for (int i = 0; i < MAX_ALLOCATABLE_GPR; ++i) {
        // Verifica se o GPR está ocupado, tem um nome associado e se o nome é o da variável que buscamos
        if (gpr_busy[i] && gpr_map[i] && strcmp(gpr_map[i], var_name) == 0) {
            return R2_REG + i; // Retorna o ID físico do registrador
        }
    }
    return -1; // Variável não encontrada em nenhum GPR
}

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

    case NDeclaration: {
        // As declarações não emitem código diretamente.
        // No entanto, é aqui que tentamos atribuir um registrador à variável
        // para que ela possa usá-lo em vez da memória posteriormente.

        TreeNode *idNode = t->children[1]; // O segundo filho de NDeclaration é o NIdentifier (o nome da variável)
        Symbol *s = lookup(symtab, idNode->attribute.name); // Busca o símbolo na tabela de símbolos

        if (s) { // Se o símbolo for encontrado (o que deve ser, após a análise semântica)
            int assigned_reg = get_free_gpr(); // Tenta obter um registrador GPR livre
            
            if (assigned_reg != -1) {
                // Se conseguiu um registrador livre:
                s->assigned_reg = assigned_reg; // Armazena o ID do registrador no símbolo da variável
                occupy_gpr(assigned_reg, s->name); // Marca o registrador como ocupado pelo NOME da variável
                
                // NOTA: Ainda não emitimos nenhum código aqui.
                // A variável só receberá um valor (e será carregada para o registrador)
                // quando ocorrer a primeira ATRIBUIÇÃO ou LEITURA.
                // Isso apenas "reserva" o registrador para essa variável.
            } else {
                // Se não conseguiu um registrador livre:
                s->assigned_reg = -1; // Mantém -1 para indicar que a variável NÃO está em um registrador
                                      // Ela será tratada com acesso à memória, como antes.
            }
        }
        break; // NDeclaration não emite código, apenas gerencia o estado dos registradores
    }
// Em codegen.c, dentro do switch de cGen

    case NAssign: {
        // Primeiro, geramos o código para a expressão do lado direito (RRHS).
        // O resultado dessa expressão, por convenção, será colocado no registrador AC.
        cGen(t->children[1]); 
        
        // Agora, identificamos a variável do lado esquerdo (LHS) da atribuição.
        const char *var_name = t->children[0]->attribute.name;
        Symbol *s = lookup(symtab, var_name); // Busca o símbolo da variável na tabela de símbolos

        if (!s) { // Isso é uma verificação de segurança, não deveria ocorrer após a análise semântica.
            fprintf(stderr, "Semantic Error: Assignment to undeclared variable '%s' at line %d\n", var_name, t->lineno);
            exit(1);
        }

        int current_reg_for_var = s->assigned_reg; // Verifica se esta variável já tem um registrador atribuído

        if (current_reg_for_var != -1) {
            // Caso 1: A variável JÁ TEM um registrador atribuído.
            // Isso é o ideal! Movemos o valor de AC (onde está o resultado da expressão RHS)
            // diretamente para o registrador da variável.
            emitRO("MOV", current_reg_for_var, AC, 0, var_name); // MOV destino, origem (destino=GPR_da_var, origem=AC)
                                                                 // O '0' é um placeholder para o terceiro operando,
                                                                 // pois MOV é uma instrução de 2 operandos.
            
        } else {
            // Caso 2: A variável NÃO TEM um registrador atribuído.
            // Tentamos obter um registrador GPR LIVRE AGORA. Isso pode acontecer se
            // no momento da declaração (NDeclaration) não havia GPRs disponíveis.
            int new_reg_attempt = get_free_gpr(); // Tenta pegar um novo registrador livre
            
            if (new_reg_attempt != -1) {
                // Caso 2.1: Conseguiu um novo registrador livre!
                // Atribui este novo registrador à variável:
                s->assigned_reg = new_reg_attempt; 
                occupy_gpr(new_reg_attempt, var_name); // Marca o GPR como ocupado por esta variável

                // Move o valor de AC para o recém-alocado registrador da variável.
                emitRO("MOV", new_reg_attempt, AC, 0, var_name);

            } else {
                // Caso 2.2: Não conseguiu nenhum registrador livre (todos ocupados).
                // Isso é o que chamamos de "spilling" (derramamento).
                // A variável terá que ir para a memória, como no comportamento original.
                int loc = getMemLoc(var_name); // Obtém o endereço de memória da variável
                emitRM("ST", AC, loc, GP, var_name); // Salva o valor de AC na memória (loc(GP))
            }
        }
        break;
    }

    case NIdentifier: {
        // Quando um identificador é usado, seu valor precisa ser carregado para AC.
        // Primeiro, tentamos carregá-lo do registrador, se ele estiver lá.
        // Se não, carregamos da memória, como antes.

        const char *var_name = t->attribute.name;
        Symbol *s = lookup(symtab, var_name); // Busca o símbolo da variável

        if (!s) { // Verificação de segurança
            fprintf(stderr, "Semantic Error: Use of undeclared identifier '%s' at line %d\n", var_name, t->lineno);
            exit(1);
        }

        int current_reg_for_var = s->assigned_reg; // Verifica se a variável tem um registrador atribuído

        if (current_reg_for_var != -1) {
            // Caso 1: A variável está em um registrador.
            // Movemos o valor desse registrador para AC (o registrador padrão para resultados de expressões).
            emitRO("MOV", AC, current_reg_for_var, 0, var_name); // MOV destino (AC), origem (GPR_da_var)
        } else {
            // Caso 2: A variável NÃO está em um registrador.
            // Carregamos o valor da memória para AC (comportamento original).
            int loc = getMemLoc(var_name); // Obtém o endereço de memória da variável
            emitRM("LD", AC, loc, GP, var_name); // Carrega da memória para AC
        }
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

    init_gprs(); // <--- CHAME AQUI! Garante que todos os GPRs estejam livres antes de começar.

    emitRM("LD",MP,0,0,"load MP <- mem[0]");
    emitRM("ST",AC,0,0,"zero mem[0]");

    cGen(syntaxTree);

    emitRO("HALT",0,0,0,"finish");

    codeToFile(filename);
}
