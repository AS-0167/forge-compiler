#ifndef IR_H
#define IR_H

#include "ast.h"

// -------------------------------
// IR OPCODES
// -------------------------------
typedef enum {
    IR_NOP,

    // literals & loads
    IR_LOAD_INT,
    IR_LOAD_FLOAT,
    IR_LOAD_BOOL,
    IR_LOAD_STRING,

    // arithmetic
    IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_MOD,
    IR_EQ, IR_NEQ, IR_LT, IR_GT, IR_LE, IR_GE,

    // identifiers
    IR_ASSIGN,

    // function IR
    IR_FUNC_BEGIN,
    IR_FUNC_END,
    IR_PARAM_DEF,   // function parameter definition
    IR_PARAM,       // pushing argument before call
    IR_CALL,
    IR_RETURN,

    // control flow
    IR_LABEL,
    IR_GOTO,
    IR_IF_GOTO
} IROp;


// -------------------------------
// IR Node
// -------------------------------
typedef struct IR {
    IROp op;

    char *dst;     // destination register/temp/var
    char *src1;
    char *src2;

    char *label;   // label for jumps / function name

    int arg_count; // for calls only

    struct IR *next;
} IR;


// -------------------------------
// Function-level IR
// -------------------------------
typedef struct IRFunction {
    char *name;
    IR *head;
    IR *tail;
    struct IRFunction *next;
} IRFunction;


// -------------------------------
// Global IR List (multiple funcs)
// -------------------------------
typedef struct IRList {
    IRFunction *funcs;
    IRFunction *current;   // active function being generated
    int temp_counter;
    int label_counter;
} IRList;


// -------------------------------
// Constructors
// -------------------------------
IRList *ir_new();
char *ir_new_temp(IRList *ir);
char *ir_new_label(IRList *ir);

// append instruction to current function
void ir_add(IRList *ir, IROp op, char *dst, char *src1, char *src2, char *label);

// function start/end
void ir_begin_function(IRList *ir, const char *name);
void ir_end_function(IRList *ir);

// main generation entry
IRList *generate_ir(Node *root);

// internal helpers
char *gen_expr(IRList *ir, Node *n);
void gen_stmt(IRList *ir, Node *n);
void gen_block(IRList *ir, Node *n);
void gen_function(IRList *ir, Node *n);

// printing
void ir_print(IRList *ir);

#endif
