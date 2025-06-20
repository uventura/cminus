%{
#define _POSIX_C_SOURCE 200809L
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "tree-node.h"

#define YYSTYPE TreeNode*

extern int lineno;
extern char* yytext;
int yylex();
void yyerror(char *s);

TreeNode *root = NULL;
%}

%token ID NUMBER
%token MULTIPLY DIVIDE SEMICOLON
%token OPEN_BRACES CLOSE_BRACES
%token LESS LESS_EQUAL GREATER GREATER_EQUAL
%token EQUAL NOT_EQUAL
%token COMMA
%token OPEN_PARENTHESIS CLOSE_PARENTHESIS
%token OPEN_BRACKET CLOSE_BRACKET
%token ELSE IF INT RETURN VOID WHILE
%left PLUS MINUS
%left MULTIPLY DIVIDE
%right ASSIGN

%nonassoc LESS LESS_EQUAL GREATER GREATER_EQUAL
%nonassoc EQUAL NOT_EQUAL

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

input: /* empty */
     | input line
;

line:
    program {
        root = $1;
        printf("Program syntax is correct!\n");
    }
;

program:
    block {
        $$ = newTreeNode(NProgram, lineno);
        $$->children[0] = $1;
    }
;

block:
    OPEN_BRACES stmt_list CLOSE_BRACES {
        $$ = $2;
    }
;

stmt_list:
    stmt stmt_list {
        $$ = newTreeNode(NCmdList, lineno);
        $$->children[0] = $1;
        $1->sibling = $2;
    }
    | { /* vazio */
        $$ = NULL;
    }
;

stmt:
    simple_stmt SEMICOLON {
        $$ = $1;
    }
    | compound_stmt {
        $$ = $1;
    }
;

simple_stmt:
    identifier ASSIGN expr {
        $$ = newTreeNode(NAssign, lineno);
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
;

compound_stmt:
    IF OPEN_PARENTHESIS expr CLOSE_PARENTHESIS stmt %prec LOWER_THAN_ELSE {
        $$ = newTreeNode(NConditional, lineno);
        $$->children[0] = $3; // condição
        $$->children[1] = $5; // if verdadeiro
    }
    | IF OPEN_PARENTHESIS expr CLOSE_PARENTHESIS stmt ELSE stmt {
        $$ = newTreeNode(NConditional, lineno);
        $$->children[0] = $3; // condição
        $$->children[1] = $5; // if verdadeiro
        $$->children[2] = $7; // else
    }
    | block {
        $$ = $1;
    }
;

identifier:
    ID {
        $$ = newTreeNode(NIdentifier, lineno);
        $$->attribute.name = strdup(yytext);
    }
;

expr:
    expr MULTIPLY expr {
        $$ = newTreeNode(NOperator, lineno);
        $$->attribute.op = '*';
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | expr DIVIDE expr {
        $$ = newTreeNode(NOperator, lineno);
        $$->attribute.op = '/';
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | expr PLUS expr {
        $$ = newTreeNode(NOperator, lineno);
        $$->attribute.op = '+';
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | expr MINUS expr {
        $$ = newTreeNode(NOperator, lineno);
        $$->attribute.op = '-';
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | expr LESS expr {
        $$ = newTreeNode(NOperator, lineno);
        $$->attribute.op = '<';
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | expr LESS_EQUAL expr {
        $$ = newTreeNode(NOperator, lineno);
        $$->attribute.op = 'l'; // custom op
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | expr GREATER expr {
        $$ = newTreeNode(NOperator, lineno);
        $$->attribute.op = '>';
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | expr GREATER_EQUAL expr {
        $$ = newTreeNode(NOperator, lineno);
        $$->attribute.op = 'g'; // custom op
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | expr EQUAL expr {
        $$ = newTreeNode(NOperator, lineno);
        $$->attribute.op = '=';
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | expr NOT_EQUAL expr {
        $$ = newTreeNode(NOperator, lineno);
        $$->attribute.op = '!';
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | identifier {
        $$ = $1;
    }
    | NUMBER {
        $$ = newTreeNode(NNumber, lineno);
        $$->attribute.value = atoi(yytext);
    }
;

%%


/* Begin Printing AST */
void printSpaces(int level) {
    for (int i = 0; i < level; i++)
        printf("  "); // 2 espaços por nível
}

void printAST(TreeNode *node, int level) {
    while (node != NULL) {
        printSpaces(level);

        switch (node->type) {
            case NProgram:
                printf("Program\n");
                break;
            case NCmdList:
                printf("Command List\n");
                break;
            case NCmd:
                printf("Command\n");
                break;
            case NAssign:
                printf("Assignment\n");
                break;
            case NIdentifier:
                printf("Identifier: %s\n", node->attribute.name);
                break;
            case NNumber:
                printf("Number: %d\n", node->attribute.value);
                break;
            case NExpr:
                printf("Expr\n");
                break;
            case NOperator:
                printf("Operator: %c\n", node->attribute.op);
                break;
            case NConditional:
                printf("If/Else Statement\n");
                break;
            default:
                printf("Unknown Node\n");
        }

        // Chama recursivamente nos filhos
        for (int i = 0; i < MAX_CHILDREN; i++) {
            if (node->children[i] != NULL) {
                printAST(node->children[i], level + 1);
            }
        }

        // Irmãos no mesmo nível
        node = node->sibling;
    }
}
/* End Printing AST */

int main() 
{
	yyparse();
    
    /* Printing AST */
    printf("\n===== Syntax Tree =====\n");
    printAST(root, 0);
    
    return 0;
}

void yyerror(char *s)
{
    printf("Problema com a analise sintatica! Erro proximo de: '%s'\n", yytext);
}
