#include "../include/ir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --------------------------------------------
// Helpers
// --------------------------------------------
static char *strdup_safe(const char *s) {
    if (!s) return NULL;
    char *out = malloc(strlen(s) + 1);
    strcpy(out, s);
    return out;
}

// --------------------------------------------
// IR LIST INIT
// --------------------------------------------
IRList *ir_new() {
    IRList *ir = malloc(sizeof(IRList));
    ir->funcs = NULL;
    ir->current = NULL;
    ir->temp_counter = 0;
    ir->label_counter = 0;
    return ir;
}

char *ir_new_temp(IRList *ir) {
    char buf[32];
    sprintf(buf, "t%d", ir->temp_counter++);
    return strdup_safe(buf);
}

char *ir_new_label(IRList *ir) {
    char buf[32];
    sprintf(buf, "L%d", ir->label_counter++);
    return strdup_safe(buf);
}

// --------------------------------------------
// Add IR to current function
// --------------------------------------------
void ir_add(IRList *ir, IROp op, char *dst, char *src1, char *src2, char *label) {
    if (!ir->current) {
        printf("ERROR: ir_add called with no active function.\n");
        exit(1);
    }

    IR *i = malloc(sizeof(IR));
    i->op = op;
    i->dst = dst;
    i->src1 = src1;
    i->src2 = src2;
    i->label = label;
    i->arg_count = 0;
    i->next = NULL;

    if (!ir->current->head) {
        ir->current->head = ir->current->tail = i;
    } else {
        ir->current->tail->next = i;
        ir->current->tail = i;
    }
}

// --------------------------------------------
// Function Begin/End
// --------------------------------------------
void ir_begin_function(IRList *ir, const char *name) {
    IRFunction *fn = malloc(sizeof(IRFunction));
    fn->name = strdup_safe(name);
    fn->head = fn->tail = NULL;
    fn->next = ir->funcs;

    ir->funcs = fn;
    ir->current = fn;

    // emit IR_FUNC_BEGIN
    ir_add(ir, IR_FUNC_BEGIN, strdup_safe(name), NULL, NULL, NULL);
}

void ir_end_function(IRList *ir) {
    ir_add(ir, IR_FUNC_END, NULL, NULL, NULL, NULL);
    ir->current = NULL;
}

// --------------------------------------------
// Expression Generation
// --------------------------------------------
char *gen_expr(IRList *ir, Node *n) {
    switch (n->type) {

        case NODE_INT_LIT: {
            char *t = ir_new_temp(ir);
            char buf[32];
            sprintf(buf, "%ld", n->as.int_lit.value);
            ir_add(ir, IR_LOAD_INT, t, strdup_safe(buf), NULL, NULL);
            return t;
        }

        case NODE_BOOL_LIT: {
            char *t = ir_new_temp(ir);
            char buf[8];
            sprintf(buf, "%d", n->as.bool_lit.value);
            ir_add(ir, IR_LOAD_BOOL, t, strdup_safe(buf), NULL, NULL);
            return t;
        }

        case NODE_FLOAT_LIT: {
            char *t = ir_new_temp(ir);
            char buf[64];
            sprintf(buf, "%f", n->as.float_lit.value);
            ir_add(ir, IR_LOAD_FLOAT, t, strdup_safe(buf), NULL, NULL);
            return t;
        }

        case NODE_STRING_LIT: {
            char *t = ir_new_temp(ir);
            ir_add(ir, IR_LOAD_STRING, t, strdup_safe(n->as.string_lit.value), NULL, NULL);
            return t;
        }

        case NODE_IDENTIFIER:
            return strdup_safe(n->as.ident.name);

        case NODE_UNARY: {
            char *rhs = gen_expr(ir, n->as.unary.rhs);
            char *t = ir_new_temp(ir);
            ir_add(ir, IR_SUB, t, strdup_safe("0"), rhs, NULL);
            return t;
        }

        case NODE_BINARY: {
            char *l = gen_expr(ir, n->as.binary.left);
            char *r = gen_expr(ir, n->as.binary.right);
            char *t = ir_new_temp(ir);

            IROp op = IR_NOP;
            switch (n->as.binary.op) {
                case T_PLUS: op = IR_ADD; break;
                case T_MINUS: op = IR_SUB; break;
                case T_MULT: op = IR_MUL; break;
                case T_DIV: op = IR_DIV; break;
                case T_MOD: op = IR_MOD; break;
                case T_EQ: op = IR_EQ; break;
                case T_NEQ: op = IR_NEQ; break;
                case T_LT: op = IR_LT; break;
                case T_GT: op = IR_GT; break;
                case T_LE: op = IR_LE; break;
                case T_GE: op = IR_GE; break;
            }

            ir_add(ir, op, t, l, r, NULL);
            return t;
        }

        case NODE_CALL: {
            for (int i = 0; i < n->as.call.arg_count; i++) {
                char *a = gen_expr(ir, n->as.call.args[i]);
                ir_add(ir, IR_PARAM, NULL, a, NULL, NULL);
            }

            char *dst = ir_new_temp(ir);
            IR *call = malloc(sizeof(IR));

            call->op = IR_CALL;
            call->dst = dst;
            call->src1 = strdup_safe(n->as.call.callee->as.ident.name);
            call->src2 = NULL;
            call->label = NULL;
            call->arg_count = n->as.call.arg_count;
            call->next = NULL;

            if (!ir->current->head) ir->current->head = ir->current->tail = call;
            else {
                ir->current->tail->next = call;
                ir->current->tail = call;
            }

            return dst;
        }

        case NODE_ASSIGN: {
            char *rhs = gen_expr(ir, n->as.assign.right);
            char *lhs = strdup_safe(n->as.assign.left->as.ident.name);
            ir_add(ir, IR_ASSIGN, lhs, rhs, NULL, NULL);
            return lhs;
        }

        default:
            printf("Unhandled expr node: %d\n", n->type);
            exit(1);
    }
}


// --------------------------------------------
// Statement Generation
// --------------------------------------------
void gen_stmt(IRList *ir, Node *n) {
    switch (n->type) {

        case NODE_VAR_DECL:
            if (n->as.var_decl.init) {
                char *rhs = gen_expr(ir, n->as.var_decl.init);
                ir_add(ir, IR_ASSIGN, strdup_safe(n->as.var_decl.name), rhs, NULL, NULL);
            }
            return;

        case NODE_EXPR_STMT:
            gen_expr(ir, n->as.expr_stmt.expr);
            return;

        case NODE_PRINT: {
            char *t = gen_expr(ir, n->as.printstmt.expr);
            ir_add(ir, IR_CALL, NULL, strdup_safe("print"), t, NULL);
            return;
        }

        case NODE_RETURN: {
            char *t = gen_expr(ir, n->as.ret.expr);
            ir_add(ir, IR_RETURN, t, NULL, NULL, NULL);
            return;
        }

        case NODE_IF: {
            char *cond = gen_expr(ir, n->as.ifstmt.cond);
            char *lbl_else = ir_new_label(ir);
            char *lbl_end = ir_new_label(ir);

            ir_add(ir, IR_IF_GOTO, NULL, cond, NULL, lbl_else);
            gen_stmt(ir, n->as.ifstmt.then_branch);
            ir_add(ir, IR_GOTO, NULL, NULL, NULL, lbl_end);

            ir_add(ir, IR_LABEL, NULL, NULL, NULL, lbl_else);
            if (n->as.ifstmt.else_branch)
                gen_stmt(ir, n->as.ifstmt.else_branch);

            ir_add(ir, IR_LABEL, NULL, NULL, NULL, lbl_end);
            return;
        }

        case NODE_WHILE: {
            char *lbl_start = ir_new_label(ir);
            char *lbl_end = ir_new_label(ir);

            ir_add(ir, IR_LABEL, NULL, NULL, NULL, lbl_start);
            char *cond = gen_expr(ir, n->as.whilestmt.cond);
            ir_add(ir, IR_IF_GOTO, NULL, cond, NULL, lbl_end);

            gen_stmt(ir, n->as.whilestmt.body);
            ir_add(ir, IR_GOTO, NULL, NULL, NULL, lbl_start);
            ir_add(ir, IR_LABEL, NULL, NULL, NULL, lbl_end);
            return;
        }

        case NODE_BLOCK:
            gen_block(ir, n);
            return;

        default:
            printf("Unhandled stmt: %d\n", n->type);
            exit(1);
    }
}

void gen_block(IRList *ir, Node *n) {
    for (int i = 0; i < n->as.block.stmt_count; i++)
        gen_stmt(ir, n->as.block.stmts[i]);
}

// --------------------------------------------
// FUNCTION DECL
// --------------------------------------------
void gen_function(IRList *ir, Node *n) {
    ir_begin_function(ir, n->as.fn_decl.name);

    // define parameters
    for (int i = 0; i < n->as.fn_decl.param_count; i++) {
        ir_add(ir,
            IR_PARAM_DEF,
            strdup_safe(n->as.fn_decl.param_names[i]),
            NULL,
            NULL,
            NULL
        );
    }

    gen_block(ir, n->as.fn_decl.body);

    ir_end_function(ir);
}


// --------------------------------------------
// TOP-LEVEL IR GENERATION
// --------------------------------------------
IRList *generate_ir(Node *root) {
    IRList *ir = ir_new();

    // PROGRAM is always a block
    for (int i = 0; i < root->as.block.stmt_count; i++) {
        Node *stmt = root->as.block.stmts[i];

        if (stmt->type == NODE_FN_DECL)
            gen_function(ir, stmt);
        else {
            // Wrap top-level code in an implicit _global function
            if (!ir->current)
                ir_begin_function(ir, "_global");

            gen_stmt(ir, stmt);
        }
    }

    if (ir->current)
        ir_end_function(ir);

    return ir;
}


// --------------------------------------------
// IR PRINT
// --------------------------------------------
void ir_print(IRList *ir) {
    for (IRFunction *f = ir->funcs; f; f = f->next) {
        printf("=== FUNCTION %s ===\n", f->name);

        for (IR *i = f->head; i; i = i->next) {
            switch (i->op) {

                case IR_FUNC_BEGIN:
                    printf("func %s begin\n", i->dst);
                    break;

                case IR_FUNC_END:
                    printf("func end\n");
                    break;

                case IR_PARAM_DEF:
                    printf("param %s\n", i->dst);
                    break;

                case IR_LOAD_INT:
                    printf("%s = %s\n", i->dst, i->src1);
                    break;

                case IR_ASSIGN:
                    printf("%s = %s\n", i->dst, i->src1);
                    break;

                case IR_ADD: printf("%s = %s + %s\n", i->dst, i->src1, i->src2); break;
                case IR_SUB: printf("%s = %s - %s\n", i->dst, i->src1, i->src2); break;
                case IR_MUL: printf("%s = %s * %s\n", i->dst, i->src1, i->src2); break;

                case IR_LABEL: printf("%s:\n", i->label); break;
                case IR_GOTO: printf("goto %s\n", i->label); break;
                case IR_IF_GOTO: printf("if %s goto %s\n", i->src1, i->label); break;

                case IR_PARAM:
                    printf("param %s\n", i->src1);
                    break;

                case IR_CALL:
                    printf("%s = call %s (%d args)\n",
                           i->dst ? i->dst : "_",
                           i->src1,
                           i->arg_count);
                    break;

                case IR_RETURN:
                    printf("return %s\n", i->dst);
                    break;

                default:
                    printf("; unknown IR\n");
            }
        }

        printf("\n");
    }
}
