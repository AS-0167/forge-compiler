#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) fputs("  ", stdout);
}

Node *ast_new(NodeType type, int line, int col) {
    Node *n = malloc(sizeof(Node));
    if (!n) { fprintf(stderr, "Out of memory\n"); exit(1); }
    memset(n, 0, sizeof(Node));
    n->type = type;
    n->line = line;
    n->col = col;
    return n;
}

/* Recursive free (shallow for strings) */
void ast_free(Node *n) {
    if (!n) return;
    switch (n->type) {
        case NODE_BLOCK:
            for (int i = 0; i < n->as.block.stmt_count; ++i) ast_free(n->as.block.stmts[i]);
            free(n->as.block.stmts);
            break;
        case NODE_VAR_DECL:
            free(n->as.var_decl.type_name);
            free(n->as.var_decl.name);
            ast_free(n->as.var_decl.init);
            break;
        case NODE_FN_DECL:
            free(n->as.fn_decl.name);
            for (int i = 0; i < n->as.fn_decl.param_count; ++i) free(n->as.fn_decl.param_names[i]);
            free(n->as.fn_decl.param_names);
            ast_free(n->as.fn_decl.body);
            break;
        case NODE_RETURN:
            ast_free(n->as.ret.expr);
            break;
        case NODE_IF:
            ast_free(n->as.ifstmt.cond);
            ast_free(n->as.ifstmt.then_branch);
            ast_free(n->as.ifstmt.else_branch);
            break;
        case NODE_WHILE:
            ast_free(n->as.whilestmt.cond);
            ast_free(n->as.whilestmt.body);
            break;
        case NODE_FOR:
            ast_free(n->as.forstmt.init);
            ast_free(n->as.forstmt.cond);
            ast_free(n->as.forstmt.update);
            ast_free(n->as.forstmt.body);
            break;
        case NODE_PRINT:
            ast_free(n->as.printstmt.expr);
            break;
        case NODE_EXPR_STMT:
            ast_free(n->as.expr_stmt.expr);
            break;
        case NODE_STRING_LIT:
            free(n->as.string_lit.value);
            break;
        case NODE_IDENTIFIER:
            free(n->as.ident.name);
            break;
        case NODE_UNARY:
            ast_free(n->as.unary.rhs);
            break;
        case NODE_BINARY:
            ast_free(n->as.binary.left);
            ast_free(n->as.binary.right);
            break;
        case NODE_CALL:
            ast_free(n->as.call.callee);
            for (int i = 0; i < n->as.call.arg_count; ++i) ast_free(n->as.call.args[i]);
            free(n->as.call.args);
            break;
        case NODE_ASSIGN:
            ast_free(n->as.assign.left);
            ast_free(n->as.assign.right);
            break;
        default:
            break;
    }
    free(n);
}

/* printing helpers */
static void ast_print_node(Node *n, int indent) {
    if (!n) return;
    print_indent(indent);
    switch (n->type) {
        case NODE_PROGRAM: printf("Program\n"); break;
        case NODE_BLOCK:
            printf("Block (%d stmts)\n", n->as.block.stmt_count);
            for (int i = 0; i < n->as.block.stmt_count; ++i) ast_print(n->as.block.stmts[i], indent + 1);
            return;
        case NODE_VAR_DECL:
            printf("VarDecl type=%s name=%s\n", n->as.var_decl.type_name ? n->as.var_decl.type_name : "<anon>", n->as.var_decl.name);
            if (n->as.var_decl.init) ast_print(n->as.var_decl.init, indent+1);
            return;
        case NODE_FN_DECL:
            printf("FnDecl name=%s return_type=%s (%d params)\n", n->as.fn_decl.name ? n->as.fn_decl.name : "<anon>", n->as.fn_decl.type_name, n->as.fn_decl.param_count);
            for (int i = 0; i < n->as.fn_decl.param_count; ++i) { print_indent(indent+1); printf("param %d: (%s:%s)\n", i, n->as.fn_decl.param_type[i], n->as.fn_decl.param_names[i]); }
            ast_print(n->as.fn_decl.body, indent+1);
            return;
        case NODE_RETURN:
            printf("Return\n"); ast_print(n->as.ret.expr, indent+1); return;
        case NODE_IF:
            printf("If\n"); ast_print(n->as.ifstmt.cond, indent+1); ast_print(n->as.ifstmt.then_branch, indent+1);
            if (n->as.ifstmt.else_branch) { print_indent(indent); printf("Else\n"); ast_print(n->as.ifstmt.else_branch, indent+1); }
            return;
        case NODE_WHILE:
            printf("While\n"); ast_print(n->as.whilestmt.cond, indent+1); ast_print(n->as.whilestmt.body, indent+1); return;
        case NODE_FOR:
            printf("For\n");
            if (n->as.forstmt.init) { print_indent(indent+1); printf("Init:\n"); ast_print(n->as.forstmt.init, indent+2); }
            if (n->as.forstmt.cond) { print_indent(indent+1); printf("Cond:\n"); ast_print(n->as.forstmt.cond, indent+2); }
            if (n->as.forstmt.update) { print_indent(indent+1); printf("Update:\n"); ast_print(n->as.forstmt.update, indent+2); }
            ast_print(n->as.forstmt.body, indent+1);
            return;
        case NODE_PRINT:
            printf("Print\n"); ast_print(n->as.printstmt.expr, indent+1); return;
        case NODE_EXPR_STMT:
            printf("ExprStmt\n"); ast_print(n->as.expr_stmt.expr, indent+1); return;
        case NODE_INT_LIT:
            printf("Int(%ld)\n", n->as.int_lit.value); return;
        case NODE_FLOAT_LIT:
            printf("Float(%f)\n", n->as.float_lit.value); return;
        case NODE_STRING_LIT:
            printf("String(\"%s\")\n", n->as.string_lit.value); return;
        case NODE_CHAR_LIT:
            printf("Char('%c')\n", n->as.char_lit.value); return;
        case NODE_BOOL_LIT:
            printf("Bool(%s)\n", n->as.bool_lit.value ? "true" : "false"); return;
        case NODE_IDENTIFIER:
            printf("Ident(%s)\n", n->as.ident.name); return;
        case NODE_UNARY:
            printf("Unary(%d)\n", n->as.unary.op); ast_print(n->as.unary.rhs, indent+1); return;
        case NODE_BINARY:
            printf("Binary(%s)\n", token_type_to_string(n->as.binary.op)); ast_print(n->as.binary.left, indent+1); ast_print(n->as.binary.right, indent+1); return;
        case NODE_CALL:
            printf("Call\n"); ast_print(n->as.call.callee, indent+1);
            for (int i = 0; i < n->as.call.arg_count; ++i) ast_print(n->as.call.args[i], indent+1);
            return;
        case NODE_ASSIGN:
            printf("Assign(%s)\n", token_type_to_string(n->as.assign.op));
            ast_print(n->as.assign.left, indent+1);
            ast_print(n->as.assign.right, indent+1);
            return;
        default:
            printf("Unknown node %d\n", n->type);
    }
}

void ast_print(Node *n, int indent) {
    ast_print_node(n, indent);
}
