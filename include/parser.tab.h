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

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
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
    T_IDENTIFIER = 258,            /* T_IDENTIFIER  */
    T_INTLIT = 259,                /* T_INTLIT  */
    T_FLOATLIT = 260,              /* T_FLOATLIT  */
    T_STRINGLIT = 261,             /* T_STRINGLIT  */
    T_CHARLIT = 262,               /* T_CHARLIT  */
    T_BOOLLIT = 263,               /* T_BOOLLIT  */
    T_INT = 264,                   /* T_INT  */
    T_FLOAT = 265,                 /* T_FLOAT  */
    T_STRING = 266,                /* T_STRING  */
    T_CHAR = 267,                  /* T_CHAR  */
    T_BOOL = 268,                  /* T_BOOL  */
    T_VOID = 269,                  /* T_VOID  */
    T_FUNCTION = 270,              /* T_FUNCTION  */
    T_RETURN = 271,                /* T_RETURN  */
    T_IF = 272,                    /* T_IF  */
    T_ELSE = 273,                  /* T_ELSE  */
    T_WHILE = 274,                 /* T_WHILE  */
    T_FOR = 275,                   /* T_FOR  */
    T_PRINT = 276,                 /* T_PRINT  */
    T_PLUS = 277,                  /* T_PLUS  */
    T_MINUS = 278,                 /* T_MINUS  */
    T_MULT = 279,                  /* T_MULT  */
    T_DIV = 280,                   /* T_DIV  */
    T_MOD = 281,                   /* T_MOD  */
    T_ASSIGN = 282,                /* T_ASSIGN  */
    T_PLUSASS = 283,               /* T_PLUSASS  */
    T_MINUSASS = 284,              /* T_MINUSASS  */
    T_MULTASS = 285,               /* T_MULTASS  */
    T_DIVASS = 286,                /* T_DIVASS  */
    T_MODASS = 287,                /* T_MODASS  */
    T_BITAND = 288,                /* T_BITAND  */
    T_BITOR = 289,                 /* T_BITOR  */
    T_BITXOR = 290,                /* T_BITXOR  */
    T_BITNOT = 291,                /* T_BITNOT  */
    T_LSHIFT = 292,                /* T_LSHIFT  */
    T_RSHIFT = 293,                /* T_RSHIFT  */
    T_AND = 294,                   /* T_AND  */
    T_OR = 295,                    /* T_OR  */
    T_NOT = 296,                   /* T_NOT  */
    T_LT = 297,                    /* T_LT  */
    T_GT = 298,                    /* T_GT  */
    T_LE = 299,                    /* T_LE  */
    T_GE = 300,                    /* T_GE  */
    T_EQ = 301,                    /* T_EQ  */
    T_NEQ = 302,                   /* T_NEQ  */
    T_LPAREN = 303,                /* T_LPAREN  */
    T_RPAREN = 304,                /* T_RPAREN  */
    T_LBRACE = 305,                /* T_LBRACE  */
    T_RBRACE = 306,                /* T_RBRACE  */
    T_COMMA = 307,                 /* T_COMMA  */
    T_SEMICOLON = 308,             /* T_SEMICOLON  */
    T_EOF = 309                    /* T_EOF  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 33 "parser.y"

    Node *node;   /* for AST nodes */
    char *str;    /* for identifiers, type names, string literals */
    long ival;    /* for integer literals */
    double fval;  /* for float literals */
    char cval;    /* for char literals */
    int bval;     /* for boolean literals */

#line 127 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
