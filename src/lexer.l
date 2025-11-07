/* =====================================================================
   Forge Compiler - Flex Lexer (Standalone Version)
   ---------------------------------------------------------------------
   Works with parser.y (no external includes or token.h needed)
   ===================================================================== */

%{
#include "parser.tab.h"
#include <stdio.h>
#include <string.h>

extern int yylineno;
%}

/* ---------------- Regular Expressions ---------------- */
DIGIT      [0-9]
ID_START   [a-zA-Z_]
ID_CONT    [a-zA-Z0-9_]
INT        {DIGIT}+
FLOAT      {DIGIT}+"."{DIGIT}+
STRING     \"([^\\\"]|\\.)*\"

%%

"int"           { return T_INT; }
"float"         { return T_FLOAT; }
"string"        { return T_STRING; }
"char"          { return T_CHAR; }
"bool"          { return T_BOOL; }
"void"          { return T_VOID; }

"function"      { return T_FUNCTION; }
"return"        { return T_RETURN; }
"if"            { return T_IF; }
"else"          { return T_ELSE; }
"while"         { return T_WHILE; }
"for"           { return T_FOR; }
"print"         { return T_PRINT; }


"="             { return T_ASSIGN; }
"+"             { return T_PLUS; }
"-"             { return T_MINUS; }
"*"             { return T_MULT; }
"/"             { return T_DIV; }
"%"             { return T_MOD; }


"("             { return T_LPAREN; }
")"             { return T_RPAREN; }
"{"             { return T_LBRACE; }
"}"             { return T_RBRACE; }
","             { return T_COMMA; }
";"             { return T_SEMICOLON; }


{FLOAT}         { yylval.fval = atof(yytext); return T_FLOATLIT; }
{INT}           { yylval.ival = atol(yytext); return T_INTLIT; }
{STRING}        { yylval.str = strdup(yytext); return T_STRINGLIT; }

{ID_START}{ID_CONT}*   { yylval.str = strdup(yytext); return T_IDENTIFIER; }

"//".*          { /* single-line comment */ }
"/*"([^*]|\*+[^*/])*\*+"/"  { /* multi-line comment */ }
[ \t\r]+        { /* ignore whitespace */ }
\n              { yylineno++; }

.               { fprintf(stderr, "Unknown token '%s' at line %d\n", yytext, yylineno); }

%%

int yywrap() { return 1; }
