//must set $$ in every rule that returns a TreeNode*
%{
#include "tree-node.h"
#include "../semantic/semantic.h" 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 

extern int lineno;
extern int yylineno;
extern TreeNode *rootNode;
extern char* yytext;
int yylex();
void yyerror(char *s);
%}

/* Forward declaration for Bison */
%union {
    struct treeNode* node;
    int num_value;
    char* cadeia;
}


%token <num_value> NUMBER
%token <cadeia> ID
%type <node> expr simple_stmt compound_stmt stmt stmt_list block program declaration type_specifier identifier

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
    $$ = newTreeNode(NProgram, @$.first_line);
    if ($1 == NULL) {
        printf("!! Block is NULL!\n");
    } 
    $$ = newTreeNode(NProgram, yylineno);
    if ($$ == NULL) {
        printf("Failed to create program node!\n");
    }
    $$->children[0] = $1;
    rootNode = $$;
    printf("Root node set to %p\n", (void*)rootNode);
}
;

block: OPEN_BRACES stmt_list CLOSE_BRACES {
    $$ = newTreeNode(NBlock, yylineno);
    $$->children[0] = $2;  // stmt_list
}
;

declaration: type_specifier identifier SEMICOLON {
    $$ = newTreeNode(NDeclaration, yylineno);
    $$->children[0] = $1;  // type (INT/VOID)
    $$->children[1] = $2;  // identifier (e.g., "a")
    // Ensure type is properly set in the identifier
    $2->dataType = $1->dataType;
    printf("DECLARATION: Set type %d for identifier %s\n", 
           $1->dataType, $2->attribute.name);
}

type_specifier:
    INT {
        $$ = newTreeNode(NType, yylineno);
        $$->dataType = TYPE_INT;  // Use consistent type value
    }
  | VOID {
        $$ = newTreeNode(NType, yylineno);
        $$->dataType = TYPE_VOID;
    }
;

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

compound_stmt: 
    IF OPEN_PARENTHESIS expr CLOSE_PARENTHESIS stmt %prec LOWER_THAN_ELSE {
        $$ = newTreeNode(NIfStmt, yylineno);
        $$->children[0] = $3; // condition (expr)
        $$->children[1] = $5; // then-part (stmt)
    }
    | IF OPEN_PARENTHESIS expr CLOSE_PARENTHESIS stmt ELSE stmt {
        $$ = newTreeNode(NIfElseStmt, yylineno);
        $$->children[0] = $3; // condition (expr)
        $$->children[1] = $5; // then-part (stmt)
        $$->children[2] = $7; // else-part (stmt)
    }
    | block {
        $$ = $1; // block already returns a TreeNode*
    }
;

identifier:
    ID {
        printf("PARSER: Creating ID node from '%s' (pointer: %p)\n", $1, $1);  // Debug
        $$ = newTreeNode(NIdentifier, yylineno);
        if ($1 == NULL) {
            yyerror("Identifier name is NULL");
            YYERROR;
        }
        $$->attribute.name = strdup($1);
        if ($$->attribute.name == NULL) {
            yyerror("Memory allocation failed for identifier name");
            YYERROR;
        }
        // Initialize dataType to some default (will be overwritten by declaration)
        $$->dataType = -1; 
        printf("PARSER: Set node name to %p\n", $$->attribute.name);  // Debug
    }
;

expr: 
    expr MULTIPLY expr { $$ = newBinaryNode(NMultiply, $1, $3, yylineno); }
    | expr DIVIDE expr { $$ = newBinaryNode(NDivide, $1, $3, yylineno); }
    | expr PLUS expr { $$ = newBinaryNode(NPlus, $1, $3, yylineno); }
    | expr MINUS expr { $$ = newBinaryNode(NMinus, $1, $3, yylineno); }
    | expr LESS expr { $$ = newBinaryNode(NLess, $1, $3, yylineno); }
    | expr LESS_EQUAL expr { $$ = newBinaryNode(NLessEqual, $1, $3, yylineno); }
    | expr GREATER expr { $$ = newBinaryNode(NGreater, $1, $3, yylineno); }
    | expr GREATER_EQUAL expr { $$ = newBinaryNode(NGreaterEqual, $1, $3, yylineno); }
    | expr EQUAL expr { $$ = newBinaryNode(NEqual, $1, $3, yylineno); }
    | expr NOT_EQUAL expr { $$ = newBinaryNode(NNotEqual, $1, $3, yylineno); }
    | identifier { $$ = $1; }
    | NUMBER { 
        $$ = newTreeNode(NNumber, yylineno); 
        $$->attribute.value = $1;
    }
;

%%

int main() 
{
    rootNode = NULL; 
    
    yyparse();
    
    if (rootNode) {
        printf("Semantic analysis:\n");
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