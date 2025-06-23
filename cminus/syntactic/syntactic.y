//must set $$ in every rule that returns a TreeNode*
%{
#include <stdio.h> 
#include <stdlib.h>
#include "tree-node.h"
#include <string.h> 
#include "../semantic/semantic.h" 

#define YYSTYPE TreeNode*

extern int lineno;
extern int yylineno;
extern TreeNode *rootNode;
extern char* yytext;
int yylex();
void yyerror(char *s);
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

input:
    /* empty */
  | input line
  | error { yyerror("Invalid input"); yyclearin; }
;


line: program { printf("Program syntax is correct!\n"); }
;

program: block { 
    printf("Program node creation\n");
    if ($1 == NULL) {
        printf("Block is NULL!\n");
    } else {
        printf("Block type: %d, line: %d\n", $1->type, $1->lineno);
    }
    $$ = newTreeNode(NProgram, yylineno);
    if ($$ == NULL) {
        printf("Failed to create program node!\n");
    }
    $$->children[0] = $1;
    rootNode = $$;
    printf("Root node set to %p\n", (void*)rootNode);
}

block: OPEN_BRACES stmt_list CLOSE_BRACES {
    $$ = newTreeNode(NBlock, yylineno);
    $$->children[0] = $2;  // stmt_list
}

declaration: type_specifier identifier SEMICOLON {
    $$ = newTreeNode(NDeclaration, yylineno);
    $$->children[0] = $1;  // type (INT/VOID)
    $$->children[1] = $2;  // identifier (e.g., "a")
};

type_specifier: INT | VOID {
    $$ = newTreeNode(NType, yylineno);
    $$->attribute.dataType = ($1 == INT) ? INT : VOID;
};


stmt_list:
    stmt stmt_list {
        if ($1 == NULL) {
            $$ = $2;
        } else {
            TreeNode* t = $1;
            while (t->sibling) t = t->sibling;
            t->sibling = $2;
            $$ = $1;
        }
    }
  | /* empty */ {
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
  | declaration 
;

simple_stmt:
    identifier ASSIGN expr {
        $$ = newTreeNode(NAssign, yylineno);
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
;

compound_stmt: IF OPEN_PARENTHESIS expr CLOSE_PARENTHESIS stmt %prec LOWER_THAN_ELSE
             | IF OPEN_PARENTHESIS expr CLOSE_PARENTHESIS stmt ELSE stmt
             | block
;

identifier:
    ID {
        $$ = newTreeNode(NIdentifier, yylineno);
        $$->attribute.name = strdup(yytext);
    }
;


expr: expr MULTIPLY expr
    | expr DIVIDE expr
    | expr PLUS expr
    | expr MINUS expr
    | expr LESS expr
    | expr LESS_EQUAL expr
    | expr GREATER expr
    | expr GREATER_EQUAL expr
    | expr EQUAL expr
    | expr NOT_EQUAL expr
    | identifier
    | NUMBER
;

%%

int main() 
{

    printf("===> MAIN <===\n");
    rootNode = NULL; 
    
    yyparse();
    printf("Root node after parse: %p\n", (void*)rootNode);
    
    if (rootNode) {
        semanticAnalysis(rootNode);
        freeTree(rootNode);
    } else {
        fprintf(stderr, "Error: No syntax tree generated\n");
        return 1;
    }

    return 0;
}


void yyerror(char *s)
{
    extern char* yytext;
    printf("Problema com a analise sintatica! Erro proximo de: '%s'\n", yytext);
}
