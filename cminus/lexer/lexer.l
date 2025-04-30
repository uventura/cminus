%{
#include "lexer/tokens.h"
#include "utils/debug.h"
%}

%option noyywrap

letter      [a-zA-Z]
id  {letter}+
digit       [0-9]
number      {digit}+
whitespace  [ \n\t]
keywords    else|if|int|return|void|while
any .|\n|\t

%x COMMENT_STATE
START_COMMENT   "/*"
END_COMMENT    "*/"

%%

{START_COMMENT} {
    BEGIN(COMMENT_STATE);
}
<COMMENT_STATE>{
    {END_COMMENT} {
        debug_comment(yytext, 0);
        BEGIN(INITIAL);
    }
    {any} {
        debug_comment(yytext, 1);
    }
}

{number} {debug_print("[ %s <Digit> ]", yytext);}
{keywords} {debug_print("[ %s <Keyword> ]", yytext);}
{id} {debug_print("[ %s<Identifier> ]", yytext);}

%%

#ifdef LEXER_MODE
int main(int argc, char** argv)
{
    ++argv, --argc;
    if ( argc > 0 ) yyin = fopen( argv[0], "r" );
    else yyin = stdin;

    yylex();

    printf("\n");
}
#endif
