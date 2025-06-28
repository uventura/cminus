/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_CMINUS_SYNTACTIC_SYNTACTIC_TAB_H_INCLUDED
# define YY_YY_CMINUS_SYNTACTIC_SYNTACTIC_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NUMBER = 258,                  /* NUMBER  */
    ID = 259,                      /* ID  */
    MULTIPLY = 260,                /* MULTIPLY  */
    DIVIDE = 261,                  /* DIVIDE  */
    SEMICOLON = 262,               /* SEMICOLON  */
    OPEN_BRACES = 263,             /* OPEN_BRACES  */
    CLOSE_BRACES = 264,            /* CLOSE_BRACES  */
    LESS = 265,                    /* LESS  */
    LESS_EQUAL = 266,              /* LESS_EQUAL  */
    GREATER = 267,                 /* GREATER  */
    GREATER_EQUAL = 268,           /* GREATER_EQUAL  */
    EQUAL = 269,                   /* EQUAL  */
    NOT_EQUAL = 270,               /* NOT_EQUAL  */
    COMMA = 271,                   /* COMMA  */
    OPEN_PARENTHESIS = 272,        /* OPEN_PARENTHESIS  */
    CLOSE_PARENTHESIS = 273,       /* CLOSE_PARENTHESIS  */
    OPEN_BRACKET = 274,            /* OPEN_BRACKET  */
    CLOSE_BRACKET = 275,           /* CLOSE_BRACKET  */
    ELSE = 276,                    /* ELSE  */
    IF = 277,                      /* IF  */
    INT = 278,                     /* INT  */
    RETURN = 279,                  /* RETURN  */
    VOID = 280,                    /* VOID  */
    WHILE = 281,                   /* WHILE  */
    PLUS = 282,                    /* PLUS  */
    MINUS = 283,                   /* MINUS  */
    ASSIGN = 284,                  /* ASSIGN  */
    LOWER_THAN_ELSE = 285          /* LOWER_THAN_ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 17 "cminus/syntactic/syntactic.y"

    TreeNode* node;
    int num_value;
    char* str_value;

#line 100 "cminus/syntactic/syntactic.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_CMINUS_SYNTACTIC_SYNTACTIC_TAB_H_INCLUDED  */
