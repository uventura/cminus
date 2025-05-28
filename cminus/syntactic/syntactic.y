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

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

input: /* empty */
     | input line
;

line: program { printf("Program syntax is correct!\n"); }
;

program: OPEN_BRACES cmd_list CLOSE_BRACES
;

cmd_list: cmd cmd_list
        | /* empty */
;

cmd: simple_cmd SEMICOLON
    | compound_cmd
;

simple_cmd: identifier ASSIGN expr
;

compound_cmd: IF OPEN_PARENTHESIS cond CLOSE_PARENTHESIS cmd %prec LOWER_THAN_ELSE
            | IF OPEN_PARENTHESIS cond CLOSE_PARENTHESIS cmd ELSE cmd
            | program
;

identifier: ID
;

expr: expr MULTIPLY expr
    | expr DIVIDE expr
    | expr PLUS expr
    | expr MINUS expr
    | identifier
    | NUMBER
;

cond: expr boolean_operator expr
;

boolean_operator: LESS
      | LESS_EQUAL
      | GREATER
      | GREATER_EQUAL
      | EQUAL
      | NOT_EQUAL
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
