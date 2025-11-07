// #ifndef AST_H
// #define AST_H

// #include <stdbool.h>
// #include "tokken.h"
// // #include "parser.tab.h"
// /*
//  * Node types for AST
//  */

// //  typedef enum TokenType TokenType;
//  typedef struct Node Node;


// typedef enum {
//     NODE_PROGRAM,
//     NODE_BLOCK,
//     NODE_VAR_DECL,     // typed var declaration: <type> name (= init)?;
//     NODE_FN_DECL,      // fn name(params) block
//     NODE_RETURN,
//     NODE_IF,
//     NODE_WHILE,
//     NODE_FOR,
//     NODE_PRINT,
//     NODE_EXPR_STMT,

//     /* expressions / literals */
//     NODE_INT_LIT,
//     NODE_FLOAT_LIT,
//     NODE_STRING_LIT,
//     NODE_CHAR_LIT,
//     NODE_BOOL_LIT,
//     NODE_IDENTIFIER,
//     NODE_UNARY,
//     NODE_BINARY,
//     NODE_CALL,
//     NODE_ASSIGN,       // assignment as AST node (a = b)
// } NodeType;

// typedef struct Node Node;

// struct Node {
//     NodeType type;
//     int line;
//     int col;

//     union {
//         /* program / block */
//         struct { Node **stmts; int stmt_count; } block;

//         /* var decl */
//         struct { char *type_name; char *name; Node *init; } var_decl;

//         /* function decl */
//         struct { char* type_name; char *name; char **param_type; char **param_names; int param_count; Node *body; } fn_decl;

//         /* return */
//         struct { Node *expr; } ret;

//         /* if */
//         struct { Node *cond; Node *then_branch; Node *else_branch; } ifstmt;

//         /* while */
//         struct { Node *cond; Node *body; } whilestmt;

//         /* for */
//         struct { Node *init; Node *cond; Node *update; Node *body; } forstmt;

//         /* print */
//         struct { Node *expr; } printstmt;

//         /* expr stmt */
//         struct { Node *expr; } expr_stmt;

//         /* literals */
//         struct { long value; } int_lit;
//         struct { double value; } float_lit;
//         struct { char *value; } string_lit;
//         struct { char value; } char_lit;
//         struct { int value; } bool_lit;

//         /* identifier */
//         struct { char *name; } ident;

//         /* unary */
//         struct { TokenType op; Node *rhs; } unary;

//         /* binary */
//         struct { Node *left; TokenType op; Node *right; } binary;

//         /* call */
//         struct { Node *callee; Node **args; int arg_count; } call;

//         /* assign */
//         struct { Node *left; TokenType op; Node *right; } assign;
//     } as;
// };

// /* allocation / deallocation */
// Node *ast_new(NodeType type, int line, int col);

// Node *ast_int(long v);
// Node *ast_float(double v);
// Node *ast_str(const char *s);
// Node *ast_ident(const char *s);
// Node *ast_binary(NodeType type, int op, Node *left, Node *right);


// void ast_free(Node *n);


// /* pretty print (for debugging) */
// void ast_print(Node *n, int indent);

// #endif






// include/ast.h
#ifndef AST_H
#define AST_H
typedef struct Node Node;
struct Node { int dummy; };
#define NODE_BLOCK 0
#define NODE_VAR_DECL 1
#define NODE_FN_DECL 2
#define NODE_ASSIGN 3
#define NODE_BINARY 4
#define NODE_IDENTIFIER 5
#define NODE_INT_LIT 6
#define NODE_STRING_LIT 7

Node *ast_new(int t, int l, int c);
Node *ast_binary(int t, int op, Node *l, Node *r);
Node *ast_ident(char *s);
Node *ast_int(int v);
Node *ast_str(char *s);
#endif

