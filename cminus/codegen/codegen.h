#ifndef CODEGEN_H
#define CODEGEN_H

#include "../syntactic/tree-node.h"
#include "../semantic/symbol-table.h"

#define COMMENTS_ENABLED 1
#define MAX_CODE 4096
#define LINE_SIZE 128
#define AC  0
#define AC1 1
#define GP  5
#define MP  6
#define PC  7

// --- Constantes para Registradores ---
// Seus registradores AC (0), AC1 (1), MP (5), GP (6), PC (7) são "especiais"
// e já têm um uso definido no seu compilador.
// Vamos usar outros registradores de propósito geral (GPRs) para nossas variáveis.
// Assumiremos, para fins de exemplo, que R2, R3, R4 estão disponíveis.
// Adapte os números (2, 3, 4) se sua arquitetura de máquina tiver outros registradores livres.
#define R2_REG 2 // O registrador físico R2
#define R3_REG 3 // O registrador físico R3
#define R4_REG 4 // O registrador físico R4

// Define quantos registradores de propósito geral (GPRs) podemos alocar
#define MAX_ALLOCATABLE_GPR 3 // Temos R2, R3 e R4, então 3.


void generateCode(const char* filename, TreeNode* syntaxTree, SymbolTable *symbolTable);
void free_gpr(int reg_id);


#endif
