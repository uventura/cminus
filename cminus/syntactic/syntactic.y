%{
#include <stdio.h> 
#include <stdlib.h>
#include "tree-node.h"

#define YYSTYPE TreeNode*

extern int lineno;
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

input: /* empty */
     | input line
;

line: program { printf("Program syntax is correct!\n"); }
;

program: block
;

block: OPEN_BRACES stmt_list CLOSE_BRACES
;

stmt_list: stmt stmt_list
         | /* empty */
;

stmt: simple_stmt SEMICOLON
    | compound_stmt
;

simple_stmt: identifier ASSIGN expr
;

compound_stmt: IF OPEN_PARENTHESIS expr CLOSE_PARENTHESIS stmt %prec LOWER_THAN_ELSE
             | IF OPEN_PARENTHESIS expr CLOSE_PARENTHESIS stmt ELSE stmt
             | block
;

identifier: ID
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
	yyparse();
    return 0;
}

void yyerror(char *s)
{
    extern char* yytext;
    printf("Problema com a analise sintatica! Erro proximo de: '%s'\n", yytext);
}
