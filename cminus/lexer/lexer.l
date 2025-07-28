%{
#include "syntactic/tree-node.h"
#include "utils/debug.h"
#include "syntactic/syntactic.tab.h"
#include <string.h> 
%}

%option noyywrap
%option yylineno

WHITESPACE_T  [ \t]
LETTER_T      [a-zA-Z]
ID_T  {LETTER_T}+
DIGIT_EXPR       [0-9]
NUMBER_T      {DIGIT_EXPR}+
KEYWORDS_T    "else"|"if"|"int"|"return"|"void"|"while"|"read"|"write"
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

{NUMBER_T} {
#ifdef LEXER_MODE
    debug_print("[ %s \033[0;33m<Number>\033[0m ]", yytext);
#else
    yylval.num_value = atoi(yytext);
    return NUMBER;
#endif
}
{KEYWORDS_T} {
#ifdef LEXER_MODE
    debug_print("[ %s \033[0;33m<Keyword>\033[0m ]", yytext);
#endif
    if (strcmp(yytext, "else") == 0) return ELSE;
    else if (strcmp(yytext, "if") == 0) return IF;
    else if (strcmp(yytext, "int") == 0) return INT;
    else if (strcmp(yytext, "return") == 0) return RETURN;
    else if (strcmp(yytext, "void") == 0) return VOID;
    else if (strcmp(yytext, "while") == 0) return WHILE;
    else if (strcmp(yytext, "read") == 0) return READ;
    else if (strcmp(yytext, "write") == 0) return WRITE;
}

{ID_T} {
#ifdef LEXER_MODE
    debug_print("[ %s \033[0;33m<Identifier>\033[0m ]", yytext);
#endif

    printf("LEXER: Found ID '%s'\n", yytext);  // Debug
    yylval.cadeia = strdup(yytext);  // Set the semantic value
    return ID;
}

{WHITESPACE_T}   {
#ifdef LEXER_MODE
    debug_print("[ %s \033[0;33m<WhiteSpace>\033[0m ]", yytext);
#endif
    // Ignore whitespace
}

{SPECIAL_T} {
#ifdef LEXER_MODE
    debug_print("[ %s \033[0;33m<Special>\033[0m ]", yytext);
#endif
    if (strcmp(yytext, "+") == 0) return PLUS;
    else if (strcmp(yytext, "-") == 0) return MINUS;
    else if (strcmp(yytext, "*") == 0) return MULTIPLY;
    else if (strcmp(yytext, "/") == 0) return DIVIDE;
    else if (strcmp(yytext, "<") == 0) return LESS;
    else if (strcmp(yytext, "<=") == 0) return LESS_EQUAL;
    else if (strcmp(yytext, ">") == 0) return GREATER;
    else if (strcmp(yytext, ">=") == 0) return GREATER_EQUAL;
    else if (strcmp(yytext, "==") == 0) return EQUAL;
    else if (strcmp(yytext, "!=") == 0) return NOT_EQUAL;
    else if (strcmp(yytext, "=") == 0) return ASSIGN;
    else if (strcmp(yytext, ";") == 0) return SEMICOLON;
    else if (strcmp(yytext, ",") == 0) return COMMA;
    else if (strcmp(yytext, "(") == 0) return OPEN_PARENTHESIS;
    else if (strcmp(yytext, ")") == 0) return CLOSE_PARENTHESIS;
    else if (strcmp(yytext, "[") == 0) return OPEN_BRACKET;
    else if (strcmp(yytext, "]") == 0) return CLOSE_BRACKET;
    else if (strcmp(yytext, "{") == 0) return OPEN_BRACES;
    else if (strcmp(yytext, "}") == 0) return CLOSE_BRACES;
}

\n {
#ifdef LEXER_MODE
    debug_print("[ \\n \033[0;33m<Newline>\033[0m ]");
#endif
    // This will be handled automatically by %option yylineno
}

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
