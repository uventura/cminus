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
%%

{digit} {debug_print("[Digit] %s\n", yytext); return DIGIT;}

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
