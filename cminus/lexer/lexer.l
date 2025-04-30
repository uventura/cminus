%{
#include "lexer/tokens.h"
#include "utils/debug.h"
%}

%option noyywrap

WHITESPACE_T  [ \t]
LETTER_T      [a-zA-Z]
ID_T  {LETTER_T}+
DIGIT_EXPR       [0-9]
NUMBER_T      {DIGIT_EXPR}+
KEYWORDS_T    else|if|int|return|void|while
ANY_EXPR .|\n
SPECIAL_T "+"|"-"|"*"|"/"|"<"|"<="|">"|">="|"=="|"!="|"="|";"|","|"("|")"|"["|"]"|"{"|"}"

%x COMMENT_STATE
START_COMMENT   "/*"
END_COMMENT    "*/"

%x LEXING_ERROR

%%

{START_COMMENT} {
    BEGIN(COMMENT_STATE);
}
<COMMENT_STATE>{
    {END_COMMENT} {
        debug_comment(yytext, 0);
        BEGIN(INITIAL);
    }
    {ANY_EXPR} {
        debug_comment(yytext, 1);
    }
}

{NUMBER_T} {debug_print("[ %s \033[0;33m<Number>\033[0m ]", yytext);}
{KEYWORDS_T} {debug_print("[ %s \033[0;33m<Keyword>\033[0m ]", yytext);}
{ID_T} {debug_print("[ %s \033[0;33m<Identifier>\033[0m ]", yytext);}
{WHITESPACE_T}   {debug_print("[ %s \033[0;33m<WhiteSpace>\033[0m ]", yytext);}
{SPECIAL_T} {debug_print("[ %s \033[0;33m<Special>\033[0m ]", yytext);}

[^\n]   { debug_print("[ %s \033[0;31m<undefined>\033[0m ]", yytext);}
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
