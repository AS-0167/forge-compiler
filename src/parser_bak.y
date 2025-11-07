/* =====================================================================
   Forge Compiler - Bison Grammar
   ---------------------------------------------------------------------

   GROUP MEMBERS
   --------------------------------
   - Izza Arooj (Roll # Bscs22006)
   - Muhammad Ahmad (Roll # Bscs22026)
   - Talha Sikandar (Roll # Bscs22050)
   - Ayesha Siddiqa (Roll # Bscs22106)

   ===================================================================== */
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ast.h"
#include "../include/utils.h"



extern int yylex();
extern int yylineno;
extern char *yytext;
void yyerror(const char *s);



Node *root; /* final AST root */
%}

/* === Define the semantic value types === */
%union {
    Node *node;   /* for AST nodes */
    char *str;    /* for identifiers, type names, string literals */
    long ival;    /* for integer literals */
    double fval;  /* for float literals */
    char cval;    /* for char literals */
    int bval;     /* for boolean literals */
}

/* === Token Definitions === */
%token <str> T_IDENTIFIER
%token <ival> T_INTLIT
%token <fval> T_FLOATLIT
%token <str> T_STRINGLIT
%token <cval> T_CHARLIT
%token <bval> T_BOOLLIT
%token T_INT T_FLOAT T_STRING T_CHAR T_BOOL T_VOID
%token T_FUNCTION T_RETURN T_IF T_ELSE T_WHILE T_FOR T_PRINT
%token T_PLUS T_MINUS T_MULT T_DIV T_MOD
%token T_ASSIGN T_PLUSASS T_MINUSASS T_MULTASS T_DIVASS T_MODASS
%token T_BITAND T_BITOR T_BITXOR T_BITNOT
%token T_LSHIFT T_RSHIFT
%token T_AND T_OR T_NOT
%token T_LT T_GT T_LE T_GE T_EQ T_NEQ
%token T_LPAREN T_RPAREN T_LBRACE T_RBRACE
%token T_COMMA T_SEMICOLON
%token T_EOF

/* === Operator Precedence === */
%right T_ASSIGN T_PLUSASS T_MINUSASS T_MULTASS T_DIVASS T_MODASS
%left T_OR
%left T_AND
%left T_BITOR
%left T_BITXOR
%left T_BITAND
%left T_EQ T_NEQ
%left T_LT T_LE T_GT T_GE
%left T_LSHIFT T_RSHIFT
%left T_PLUS T_MINUS
%left T_MULT T_DIV T_MOD
%right T_NOT T_BITNOT
%left T_LPAREN T_RPAREN

/* === Nonterminal type declarations === */
%type <node> program top_decl_list top_decl decl stmt block expr
%type <node> var_decl fn_decl if_stmt while_stmt for_stmt print_stmt
%type <node> return_stmt expr_stmt stmt_list stmt_list_opt
%type <node> param_list param_list_opt
%type <str>  type

%%

/* ---------------- Program Structure ---------------- */

program
    : /* empty */                         { root = ast_new(NODE_BLOCK, 0, 0); }
    | top_decl_list                       { root = $1; }
    ;

top_decl_list
    : top_decl                            { $$ = ast_new(NODE_BLOCK, 0, 0);
                                            $$->as.block.stmts = malloc(sizeof(Node*));
                                            $$->as.block.stmts[0] = $1;
                                            $$->as.block.stmt_count = 1; }
    | top_decl_list top_decl              { $$ = $1;
                                            $$->as.block.stmts = realloc($$->as.block.stmts,
                                                sizeof(Node*) * (++$$->as.block.stmt_count));
                                            $$->as.block.stmts[$$->as.block.stmt_count - 1] = $2; }
    ;

top_decl
    : decl
    ;

/* ---------------- Declarations ---------------- */

decl
    : var_decl
    | fn_decl
    ;

/* Variable Declaration */
var_decl
    : type T_IDENTIFIER T_ASSIGN expr T_SEMICOLON
        { $$ = ast_new(NODE_VAR_DECL, yylineno, 0);
          $$->as.var_decl.type_name = strdup($1);
          $$->as.var_decl.name = strdup($2);
          $$->as.var_decl.init = $4; }
    | type T_IDENTIFIER T_SEMICOLON
        { $$ = ast_new(NODE_VAR_DECL, yylineno, 0);
          $$->as.var_decl.type_name = strdup($1);
          $$->as.var_decl.name = strdup($2);
          $$->as.var_decl.init = NULL; }
    ;

/* Function Declaration */
fn_decl
    : T_FUNCTION type T_IDENTIFIER T_LPAREN param_list_opt T_RPAREN block
        { $$ = ast_new(NODE_FN_DECL, yylineno, 0);
          $$->as.fn_decl.type_name = strdup($2);
          $$->as.fn_decl.name = strdup($3);
          $$->as.fn_decl.param_names = $5 ? $5->as.fn_decl.param_names : NULL;
          $$->as.fn_decl.param_type  = $5 ? $5->as.fn_decl.param_type : NULL;
          $$->as.fn_decl.param_count = $5 ? $5->as.fn_decl.param_count : 0;
          $$->as.fn_decl.body = $7; }
    ;

param_list_opt
    : /* empty */ { $$ = NULL; }
    | param_list  { $$ = $1; }
    ;

param_list
    : type T_IDENTIFIER
        { $$ = ast_new(NODE_FN_DECL, yylineno, 0);
          $$->as.fn_decl.param_type = malloc(sizeof(char*));
          $$->as.fn_decl.param_names = malloc(sizeof(char*));
          $$->as.fn_decl.param_type[0] = strdup($1);
          $$->as.fn_decl.param_names[0] = strdup($2);
          $$->as.fn_decl.param_count = 1; }
    | param_list T_COMMA type T_IDENTIFIER
        { $$ = $1;
          $$->as.fn_decl.param_count++;
          $$->as.fn_decl.param_type = realloc($$->as.fn_decl.param_type,
                                  sizeof(char*) * $$->as.fn_decl.param_count);
          $$->as.fn_decl.param_names = realloc($$->as.fn_decl.param_names,
                                  sizeof(char*) * $$->as.fn_decl.param_count);
          $$->as.fn_decl.param_type[$$->as.fn_decl.param_count - 1] = strdup($3);
          $$->as.fn_decl.param_names[$$->as.fn_decl.param_count - 1] = strdup($4); }
    ;

/* ---------------- Statements ---------------- */

stmt
    : block
    | if_stmt
    | while_stmt
    | for_stmt
    | print_stmt
    | return_stmt
    | expr_stmt
    | var_decl
    ;

/* Compound Block */
block
    : T_LBRACE stmt_list_opt T_RBRACE
        { $$ = ast_new(NODE_BLOCK, yylineno, 0);
          $$->as.block.stmts = $2 ? $2->as.block.stmts : NULL;
          $$->as.block.stmt_count = $2 ? $2->as.block.stmt_count : 0; }
    ;

stmt_list_opt
    : /* empty */ { $$ = NULL; }
    | stmt_list   { $$ = $1; }
    ;

stmt_list
    : stmt                             { $$ = ast_new(NODE_BLOCK, yylineno, 0);
                                         $$->as.block.stmts = malloc(sizeof(Node*));
                                         $$->as.block.stmts[0] = $1;
                                         $$->as.block.stmt_count = 1; }
    | stmt_list stmt                   { $$ = $1;
                                         $$->as.block.stmts = realloc($$->as.block.stmts,
                                            sizeof(Node*) * (++$$->as.block.stmt_count));
                                         $$->as.block.stmts[$$->as.block.stmt_count - 1] = $2; }
    ;

/* If Statement */
if_stmt
    : T_IF T_LPAREN expr T_RPAREN stmt
        { $$ = ast_new(NODE_IF, yylineno, 0);
          $$->as.ifstmt.cond = $3;
          $$->as.ifstmt.then_branch = $5;
          $$->as.ifstmt.else_branch = NULL; }
    | T_IF T_LPAREN expr T_RPAREN stmt T_ELSE stmt
        { $$ = ast_new(NODE_IF, yylineno, 0);
          $$->as.ifstmt.cond = $3;
          $$->as.ifstmt.then_branch = $5;
          $$->as.ifstmt.else_branch = $7; }
    ;

/* While Statement */
while_stmt
    : T_WHILE T_LPAREN expr T_RPAREN stmt
        { $$ = ast_new(NODE_WHILE, yylineno, 0);
          $$->as.whilestmt.cond = $3;
          $$->as.whilestmt.body = $5; }
    ;

/* For Statement */
for_stmt
    : T_FOR T_LPAREN var_decl expr T_SEMICOLON expr T_RPAREN stmt
        { $$ = ast_new(NODE_FOR, yylineno, 0);
          $$->as.forstmt.init = $3;
          $$->as.forstmt.cond = $4;
          $$->as.forstmt.update = $6;
          $$->as.forstmt.body = $8; }
    ;

/* Print Statement */
print_stmt
    : T_PRINT T_LPAREN expr T_RPAREN T_SEMICOLON
        { $$ = ast_new(NODE_PRINT, yylineno, 0);
          $$->as.printstmt.expr = $3; }
    ;

/* Return Statement */
return_stmt
    : T_RETURN T_SEMICOLON
        { $$ = ast_new(NODE_RETURN, yylineno, 0);
          $$->as.ret.expr = NULL; }
    | T_RETURN expr T_SEMICOLON
        { $$ = ast_new(NODE_RETURN, yylineno, 0);
          $$->as.ret.expr = $2; }
    ;

/* Expression Statement */
expr_stmt
    : expr T_SEMICOLON
        { $$ = ast_new(NODE_EXPR_STMT, yylineno, 0);
          $$->as.expr_stmt.expr = $1; }
    ;

/* ---------------- Expressions ---------------- */

expr
    : expr T_ASSIGN expr
        { $$ = ast_new(NODE_ASSIGN, yylineno, 0);
          $$->as.assign.left = $1;
          $$->as.assign.op = T_ASSIGN;
          $$->as.assign.right = $3; }
    | expr T_PLUS expr
        { $$ = ast_binary(NODE_BINARY, T_PLUS, $1, $3); }
    | expr T_MINUS expr
        { $$ = ast_binary(NODE_BINARY, T_MINUS, $1, $3); }
    | expr T_MULT expr
        { $$ = ast_binary(NODE_BINARY, T_MULT, $1, $3); }
    | expr T_DIV expr
        { $$ = ast_binary(NODE_BINARY, T_DIV, $1, $3); }
    | expr T_LT expr
        { $$ = ast_binary(NODE_BINARY, T_LT, $1, $3); }
    | expr T_GT expr
        { $$ = ast_binary(NODE_BINARY, T_GT, $1, $3); }
    | T_LPAREN expr T_RPAREN
        { $$ = $2; }
    | T_IDENTIFIER
        { $$ = ast_ident($1); }
    | T_INTLIT
        { $$ = ast_int(atoi(yytext)); }
    | T_FLOATLIT
        { $$ = ast_float(atof(yytext)); }
    | T_STRINGLIT
        { $$ = ast_str(yytext); }
    ;

/* ---------------- Types ---------------- */

type
    : T_INT       { $$ = strdup("int"); }
    | T_FLOAT     { $$ = strdup("float"); }
    | T_STRING    { $$ = strdup("string"); }
    | T_CHAR      { $$ = strdup("char"); }
    | T_BOOL      { $$ = strdup("bool"); }
    | T_VOID      { $$ = strdup("void"); }
    ;

%%






void yyerror(const char *s) {
    fprintf(stderr, "Parser error (line %d): %s near '%s'\n", yylineno, s, yytext);
}
