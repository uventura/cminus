%{
#include <stdio.h>

#include "lexer/tokens.h"
#include "utils/debug.h"
%}

letter      [a-zA-Z]
id  {letter}+
digit       [0-9]
number      {digit}+
whitespace  [ \n\t]

%option noyywrap
%%

{number} {debug_print("[Digit] %s\n", yytext);}
{id} {debug_print("[Identifier] %s\n", yytext);}

%%

#ifdef LEXER_MODE
int main(int argc, char** argv)
{
    ++argv, --argc;
    if ( argc > 0 ) yyin = fopen( argv[0], "r" );
    else yyin = stdin;

    yylex();
}
#endif
