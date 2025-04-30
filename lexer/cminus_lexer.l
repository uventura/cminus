%{
#include <stdio.h>

#include "utils/debug.h"
%}

DIGIT       [0-9]
NUMBER      {DIGIT}+
LETTER      [a-zA-Z]
ID  {LETTER}+
WHITESPACE  [ \n\t]
%%

{DIGIT} {debug_print("DIGIT");}

%%

int main(int argc, char** argv)
{
    ++argv, --argc;
    if ( argc > 0 ) yyin = fopen( argv[0], "r" );
    else yyin = stdin;

    yylex();
}
