%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node Node;


/* ------------------ Basic AST Node Definition ------------------ */
typedef enum {
    NODE_BLOCK,
    NODE_VAR_DECL,
    NODE_FN_DECL,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_PRINT,
    NODE_RETURN,
    NODE_EXPR_STMT,
    NODE_BINARY,
    NODE_ASSIGN,
    NODE_LITERAL,
    NODE_IDENTIFIER
} NodeType;

typedef struct Node {
    NodeType type;
    char *text;            /* for identifiers or literals */
    struct Node **stmts;   /* for block statements */
    int stmt_count;
    struct Node *left;     /* for binary/assign */
    struct Node *right;
} Node;

/* --- Simple helper functions --- */
Node *make_node(NodeType type) {
    Node *n = malloc(sizeof(Node));
    memset(n, 0, sizeof(Node));
    n->type = type;
    return n;
}

Node *make_binary(NodeType type, Node *l, Node *r) {
    Node *n = make_node(type);
    n->left = l;
    n->right = r;
    return n;
}

Node *make_ident(const char *name) {
    Node *n = make_node(NODE_IDENTIFIER);
    n->text = strdup(name);
    return n;
}

Node *make_literal(const char *val) {
    Node *n = make_node(NODE_LITERAL);
    n->text = strdup(val);
    return n;
}

/* --- Externs --- */
int yylineno = 1;
extern int yylex();
extern int yylineno;
extern char *yytext;
void yyerror(const char *s);

Node *root = NULL;
%}

/* === Bison Type Definitions === */
%union {
    Node *node;   /* now valid, since Node is defined above */
    char *str;
    long ival;
    double fval;
}


/* === Tokens === */
%token <str> T_IDENTIFIER
%token <ival> T_INTLIT
%token <fval> T_FLOATLIT
%token <str> T_STRINGLIT
%token T_INT T_FLOAT T_STRING T_CHAR T_BOOL T_VOID
%token T_FUNCTION T_RETURN T_IF T_ELSE T_WHILE T_FOR T_PRINT
%token T_PLUS T_MINUS T_MULT T_DIV T_MOD
%token T_ASSIGN
%token T_LPAREN T_RPAREN T_LBRACE T_RBRACE
%token T_COMMA T_SEMICOLON

/* === Precedence === */
%right T_ASSIGN
%left T_PLUS T_MINUS
%left T_MULT T_DIV T_MOD

/* === Types === */
%type <node> program decl stmt block expr stmt_list
%type <node> var_decl fn_decl
%type <str> type

%%

/* ---------------- Program Structure ---------------- */

program
    : /* empty */           { root = make_node(NODE_BLOCK); }
    | decl stmt_list        { root = $2; }
    | stmt_list             { root = $1; }
    ;

/* ---------------- Declarations ---------------- */

decl
    : type T_IDENTIFIER T_SEMICOLON
        { $$ = make_node(NODE_VAR_DECL); $$->text = strdup($2); }
    | T_FUNCTION type T_IDENTIFIER T_LPAREN T_RPAREN block
        { $$ = make_node(NODE_FN_DECL); $$->text = strdup($3); $$->right = $6; }
    ;

/* ---------------- Statements ---------------- */

stmt
    : block
    | var_decl
    | expr T_SEMICOLON             { $$ = make_node(NODE_EXPR_STMT); $$->left = $1; }
    | T_PRINT expr T_SEMICOLON     { $$ = make_node(NODE_PRINT); $$->left = $2; }
    | T_RETURN expr T_SEMICOLON    { $$ = make_node(NODE_RETURN); $$->left = $2; }
    | T_IF T_LPAREN expr T_RPAREN stmt
        { $$ = make_node(NODE_IF); $$->left = $3; $$->right = $5; }
    | T_WHILE T_LPAREN expr T_RPAREN stmt
        { $$ = make_node(NODE_WHILE); $$->left = $3; $$->right = $5; }
    | T_FOR T_LPAREN expr T_SEMICOLON expr T_SEMICOLON expr T_RPAREN stmt
        { $$ = make_node(NODE_FOR); $$->left = $3; $$->right = $9; }
    ;

var_decl
    : type T_IDENTIFIER T_ASSIGN expr T_SEMICOLON
        { $$ = make_node(NODE_VAR_DECL); $$->text = strdup($2); $$->right = $4; }
    ;

/* ---------------- Block and Statement List ---------------- */

block
    : T_LBRACE stmt_list T_RBRACE
        { $$ = make_node(NODE_BLOCK);
          $$->stmts = $2->stmts;
          $$->stmt_count = $2->stmt_count; }
    ;

stmt_list
    : stmt
        { $$ = make_node(NODE_BLOCK);
          $$->stmts = malloc(sizeof(Node*));
          $$->stmts[0] = $1;
          $$->stmt_count = 1; }
    | stmt_list stmt
        { $$ = $1;
          $$->stmts = realloc($$->stmts, sizeof(Node*) * (++$$->stmt_count));
          $$->stmts[$$->stmt_count - 1] = $2; }
    ;

/* ---------------- Expressions ---------------- */

expr
    : T_IDENTIFIER
        { $$ = make_ident($1); }
    | T_INTLIT
        { char buf[64]; sprintf(buf, "%ld", $1); $$ = make_literal(buf); }
    | T_FLOATLIT
        { char buf[64]; sprintf(buf, "%f", $1); $$ = make_literal(buf); }
    | expr T_PLUS expr
        { $$ = make_binary(NODE_BINARY, $1, $3); $$->text = strdup("+"); }
    | expr T_MINUS expr
        { $$ = make_binary(NODE_BINARY, $1, $3); $$->text = strdup("-"); }
    | expr T_MULT expr
        { $$ = make_binary(NODE_BINARY, $1, $3); $$->text = strdup("*"); }
    | expr T_DIV expr
        { $$ = make_binary(NODE_BINARY, $1, $3); $$->text = strdup("/"); }
    | expr T_ASSIGN expr
        { $$ = make_binary(NODE_ASSIGN, $1, $3); $$->text = strdup("="); }
    | T_LPAREN expr T_RPAREN
        { $$ = $2; }
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

/* ---------------- Error Handling ---------------- */
void yyerror(const char *s) {
    fprintf(stderr, "Parser error (line %d): %s near '%s'\n", yylineno, s, yytext);
}
