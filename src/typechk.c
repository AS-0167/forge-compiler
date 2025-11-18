// typechk.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/typechk.h"

/* Helper: simple canonicalization of type strings */
static const char *canon_type(const char *t) {

    // printf("Canonicalizing type: %s\n", t ? t : "NULL");
    if (!t) return NULL;
    if (strcmp(t, "T_INT") == 0) return "int";
    if (strcmp(t, "T_FLOAT") == 0) return "float";
    if (strcmp(t, "T_BOOL") == 0) return "bool";
    if (strcmp(t, "T_STRING") == 0) return "string";
    if (strcmp(t, "T_CHAR") == 0) return "char";
    if (strcmp(t, "T_VOID") == 0) return "void";
    return t;
}

/* result of typing an expression: simple string pointer to canonical type name */
typedef struct {
    const char *type; /* e.g. "int","float","bool","string","char","void", or NULL for error */
} TypeRes;

static int errors = 0;
static Scope *current_scope = NULL;

/* forward */
static TypeRes type_expr(Node *n);

static void report(TypeChkError tc, const char *msg, int line) {
    errors++;
    const char *ename = "TypeError";
    printf("[%s] (%d): %s\n", ename, (int)line, msg);
}

/* Helper: check if numeric */
static int is_numeric(const char *t) {
    if (!t) return 0;
    return (strcmp(t, "int") == 0) || (strcmp(t, "float") == 0);
}
static int is_int(const char *t) {
    return t && strcmp(t, "int") == 0;
}
static int is_bool(const char *t) {
    return t && strcmp(t, "bool") == 0;
}

/* Type a binary operator node: returns type or NULL on error */
static TypeRes type_binary(Node *n) {
    TypeRes L = type_expr(n->as.binary.left);
    TypeRes R = type_expr(n->as.binary.right);
    const char *op = NULL;
    if (n->as.binary.op == T_PLUS) op = "+";
    else if (n->as.binary.op == T_MINUS) op = "-";
    else if (n->as.binary.op == T_MULT) op = "*";
    else if (n->as.binary.op == T_DIV) op = "/";
    else if (n->as.binary.op == T_LT || n->as.binary.op == T_GT ||
             n->as.binary.op == T_LE || n->as.binary.op == T_GE ||
             n->as.binary.op == T_EQ || n->as.binary.op == T_NEQ)
                op = "cmp";
    /* assignment: treated via NODE_ASSIGN branch */
    if (!L.type || !R.type) {
        report(TC_ExpressionTypeMismatch, "Empty expression or previous error in binary operands", n->line);
        return (TypeRes){ .type = NULL };
    }

    /* Arithmetic: require numeric */
    if (op && (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 || strcmp(op, "/") == 0)) {
        if (!is_numeric(L.type) || !is_numeric(R.type)) {
            report(TC_AttemptedAddOpOnNonNumeric, "Arithmetic operator on non-numeric operands", n->line);
            return (TypeRes){ .type = NULL };
        }
        /* promote to float if either is float */
        if (strcmp(L.type, "float") == 0 || strcmp(R.type, "float") == 0) return (TypeRes){ .type = "float" };
        return (TypeRes){ .type = "int" };
    }

    /* comparison -> bool */
    if (strcmp(op, "cmp") == 0) {
        /* allow comparison among numeric or among same types (simple) */
        if (is_numeric(L.type) && is_numeric(R.type)) return (TypeRes){ .type = "bool" };
        if (strcmp(L.type, R.type) == 0) return (TypeRes){ .type = "bool" };
        report(TC_ExpressionTypeMismatch, "Comparison between incompatible types", n->line);
        return (TypeRes){ .type = NULL };
    }

    /* default */
    return (TypeRes){ .type = NULL };
}

static TypeRes type_assign(Node *n) {
    /* left must be an identifier or field (we treat identifier) */
    if (n->as.assign.left->type != NODE_IDENTIFIER) {
        report(TC_ExpressionTypeMismatch, "Left-hand side of assignment must be an identifier", n->line);
        return (TypeRes){ .type = NULL };
    }
    const char *varname = n->as.assign.left->as.ident.name;
    Symbol *sym = scope_lookup(current_scope, varname, SYMBOL_VAR);
    if (!sym) {
        char buf[256];
        snprintf(buf, sizeof(buf), "Assignment to undeclared variable '%s'", varname);
        report(TC_ErroneousVarDecl, buf, n->line);
        return (TypeRes){ .type = NULL };
    }
    TypeRes r = type_expr(n->as.assign.right);
    if (!r.type) return r;
    const char *decl_t = canon_type(sym->type);
    const char *rhs_t = canon_type(r.type);

    if (!decl_t || !rhs_t) {
        report(TC_ExpressionTypeMismatch, "Unknown type in assignment", n->line);
        return (TypeRes){ .type = NULL };
    }
    /* simple compatibility: allow int->float? Here we require exact match except int->float accepted */
    if (strcmp(decl_t, rhs_t) == 0) return (TypeRes){ .type = decl_t };
    if (strcmp(decl_t, "float") == 0 && strcmp(rhs_t, "int") == 0) return (TypeRes){ .type = "float" }; /* widen */
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "Type mismatch in assignment: '%s' := '%s'", decl_t, rhs_t);
        report(TC_ExpressionTypeMismatch, buf, n->line);
    }
    return (TypeRes){ .type = NULL };
}

/* Expression typing dispatcher */
static TypeRes type_expr(Node *n) {
    if (!n) { report(TC_EmptyExpression, "Empty expression encountered", 0); return (TypeRes){ .type = NULL }; }

    printf("Typing expression node at line %d, type %d\n", n->line, n->type);

    switch (n->type) {
        case NODE_INT_LIT: {
            return (TypeRes){ .type = "int" };
        }
        case NODE_FLOAT_LIT: {
            return (TypeRes){ .type = "float" };
        }
        case NODE_BOOL_LIT: {
            return (TypeRes){ .type = "bool" };
        }
        case NODE_STRING_LIT: {
            return (TypeRes){ .type = "string" };
        }
        case NODE_CHAR_LIT: {
            return (TypeRes){ .type = "char" };
        }
        case NODE_IDENTIFIER: {
            Symbol *s = scope_lookup(current_scope, n->as.ident.name, SYMBOL_VAR);
            if (!s) {
                char buf[256];
                snprintf(buf, sizeof(buf), "Use of undeclared variable '%s'", n->as.ident.name);
                report(TC_ErroneousVarDecl, buf, n->line);
                return (TypeRes){ .type = NULL };
            }
            return (TypeRes){ .type = canon_type(s->type) };
        }
        case NODE_BINARY:
            return type_binary(n);
        case NODE_ASSIGN:
            return type_assign(n);
        case NODE_CALL: {
            /* callee must be identifier node */
            if (n->as.call.callee->type != NODE_IDENTIFIER) {
                report(TC_FnCallParamCount, "Function callee is not a simple identifier", n->line);
                return (TypeRes){ .type = NULL };
            }
            const char *fname = n->as.call.callee->as.ident.name;
            Symbol *fsym = scope_lookup(current_scope, fname, SYMBOL_FN);
            if (!fsym) {
                char buf[256];
                snprintf(buf, sizeof(buf), "Call to undefined function '%s'", fname);
                report(TC_FnCallParamCount, buf, n->line);
                return (TypeRes){ .type = NULL };
            }
            /* param count check */
            if (fsym->param_count != n->as.call.arg_count) {
                char buf[256];
                snprintf(buf, sizeof(buf), "Function '%s' expects %d params but %d were passed",
                         fname, fsym->param_count, n->as.call.arg_count);
                report(TC_FnCallParamCount, buf, n->line);
                /* continue to type arguments anyway */
            }
            for (int i = 0; i < n->as.call.arg_count; ++i) {
                TypeRes argt = type_expr(n->as.call.args[i]);
                const char *expected = (i < fsym->param_count) ? canon_type(fsym->param_types[i]) : NULL;
                if (!argt.type || !expected) continue;
                /* allow int->float widening */
                if (strcmp(argt.type, expected) != 0) {
                    if (!(strcmp(expected, "float") == 0 && strcmp(argt.type, "int") == 0)) {
                        char buf[256];
                        snprintf(buf, sizeof(buf), "Parameter %d of '%s' expects '%s' but got '%s'",
                                 i+1, fname, expected, argt.type);
                        report(TC_FnCallParamType, buf, n->line);
                    }
                }
            }
            return (TypeRes){ .type = canon_type(fsym->type) }; /* function return type */
        }
        default:
            report(TC_ExpressionTypeMismatch, "Unhandled expression node in typechecker", n->line);
            return (TypeRes){ .type = NULL };
    }
}

/* Walk statements and functions, check returns etc. */
static void tc_stmt(Node *stmt);

static void tc_block(Node *block) {
    if (!block) return;
    // printf("Type checking block at line %d with %d statements\n", block->line, block->as.block.stmt_count);
    for (int i = 0; i < block->as.block.stmt_count; ++i)
        tc_stmt(block->as.block.stmts[i]);
}

static void tc_stmt(Node *stmt) {
    printf("Type checking statement at line %d, type %d\n", stmt ? stmt->line : -1, stmt ? stmt->type : -1);
    if (!stmt) return;
    switch (stmt->type) {
        case NODE_VAR_DECL:
            /* check initializer type against var declared type */
            if (stmt->as.var_decl.init) {
                TypeRes rv = type_expr(stmt->as.var_decl.init);
                const char *decl_t = canon_type(stmt->as.var_decl.type_name);
                if (rv.type && decl_t) {
                    if (strcmp(decl_t, rv.type) != 0) {
                        if (!(strcmp(decl_t, "float")==0 && strcmp(rv.type, "int")==0)) {
                            char buf[256];
                            snprintf(buf, sizeof(buf), "Variable declaration '%s' type '%s' incompatible with initializer type '%s'",
                                     stmt->as.var_decl.name, decl_t, rv.type);
                            report(TC_ErroneousVarDecl, buf, stmt->line);
                        }
                    }
                }
            }
            break;

        case NODE_EXPR_STMT:
            type_expr(stmt->as.expr_stmt.expr);
            break;

        case NODE_PRINT:
            type_expr(stmt->as.printstmt.expr);
            break;

        case NODE_RETURN: {
            /* find containing function's declared return type by walking scopes (we stored fn symbol) */
            /* for simplicity we assume the top-level parse gives us function nodes to inspect: */
            /* The check of return correctness is done when visiting functions below (see type_check) */
            if (stmt->as.ret.expr) type_expr(stmt->as.ret.expr);
            break;
        }

        case NODE_IF:
            {
                TypeRes cond = type_expr(stmt->as.ifstmt.cond);
                if (!is_bool(cond.type)) {
                    report(TC_NonBooleanCondStmt, "If condition must be boolean", stmt->line);
                }
                tc_stmt(stmt->as.ifstmt.then_branch);
                if (stmt->as.ifstmt.else_branch) tc_stmt(stmt->as.ifstmt.else_branch);
            }
            break;

        case NODE_WHILE:
            {
                TypeRes cond = type_expr(stmt->as.whilestmt.cond);
                if (!is_bool(cond.type)) {
                    report(TC_NonBooleanCondStmt, "While condition must be boolean", stmt->line);
                }
                tc_stmt(stmt->as.whilestmt.body);
            }
            break;

        case NODE_FOR:
            /* basic checking: type expressions inside */
            if (stmt->as.forstmt.init) type_expr(stmt->as.forstmt.init);
            if (stmt->as.forstmt.cond) {
                TypeRes cond = type_expr(stmt->as.forstmt.cond);
                if (!is_bool(cond.type)) report(TC_NonBooleanCondStmt, "For condition must be boolean", stmt->line);
            }
            if (stmt->as.forstmt.update) type_expr(stmt->as.forstmt.update);
            tc_stmt(stmt->as.forstmt.body);
            break;

        case NODE_BLOCK:
            tc_block(stmt);
            break;

        case NODE_FN_DECL:
            /* handled in top-level type_check to be able to check return type */
            break;

        default:
            /* other nodes handled in expression typing paths */
            break;
    }
}

/* For each function, ensure all returns are compatible with declared return type.
   We'll do a simple traversal collecting returns inside the body. */
static void check_function_returns(Node *fn) {
    if (!fn) return;
    const char *ret_type = canon_type(fn->as.fn_decl.type_name);
    /* traverse the body statements and check return statements */
    /* simplistic recursion that checks returns anywhere in body */
    /* helper lambda-like */
    struct ctx {
        const char *decl_ret;
    } c = { .decl_ret = ret_type };

    /* local recursive fn */
    void walk(Node *n){
        if (!n) return;
        if (n->type == NODE_RETURN) {
            if (c.decl_ret == NULL) {
                report(TC_ErroneousReturnType, "Function has declared unknown return type", n->line);
            } else if (n->as.ret.expr) {
                TypeRes tr = type_expr(n->as.ret.expr);
                if (tr.type == NULL) return;
                if (strcmp(c.decl_ret, tr.type) != 0) {
                    if (!(strcmp(c.decl_ret,"float")==0 && strcmp(tr.type,"int")==0)) {
                        char buf[256];
                        snprintf(buf, sizeof(buf), "Return type mismatch: function declared '%s' but returned '%s'",
                                 c.decl_ret, tr.type);
                        report(TC_ErroneousReturnType, buf, n->line);
                    }
                }
            } else {
                /* return with no expr: allowed only if declared void */
                if (strcmp(c.decl_ret, "void") != 0) {
                    char buf[256];
                    snprintf(buf, sizeof(buf), "Return without value in function declared '%s'", c.decl_ret);
                    report(TC_ErroneousReturnType, buf, n->line);
                }
            }
            return;
        }
        /* descend into children */
        switch (n->type) {
            case NODE_BLOCK:
                for (int i = 0; i < n->as.block.stmt_count; ++i) walk(n->as.block.stmts[i]);
                break;
            case NODE_IF:
                walk(n->as.ifstmt.then_branch);
                if (n->as.ifstmt.else_branch) walk(n->as.ifstmt.else_branch);
                break;
            case NODE_WHILE:
                walk(n->as.whilestmt.body); break;
            case NODE_FOR:
                walk(n->as.forstmt.body); break;
            default:
                /* nothing */
                break;
        }
    }
    walk(fn->as.fn_decl.body);
}

/* Top-level entrypoint */
TypeChkReport type_check(Node *root, Scope *global) {
    errors = 0;
    current_scope = global;
    if (!root) return (TypeChkReport){ .error_count = 0 };

    printf("Global Scope Symbols:\n");
    for (Symbol *s = global->symbols; s; s = s->next) {
        printf("  Symbol: %s kind=%d type=%s\n", s->name, s->kind, s->type);
    }

    /* 1) Walk top-level (global) statements: register usage checks and type-check statements */
    if (root->type == NODE_BLOCK) {
        for (int i = 0; i < root->as.block.stmt_count; ++i) {
            Node *st = root->as.block.stmts[i];
            if (!st) continue;
            if (st->type == NODE_FN_DECL) {
                /* we'll check bodies after we've set current_scope and such */
                continue;
            } else {
                tc_stmt(st);
            }
        }
        /* 2) Now check each function's returns and body with function-local scope:
           For each fn symbol in global scope, create a local scope with params bound and run checks */
        for (Symbol *s = global->symbols; s; s = s->next) {
            printf("Checking symbol: %s kind=%d\n", s->name, s->kind);
            if (s->kind == SYMBOL_FN && s->fn_node) {
                /* create function-local scope: push a new scope that chains to global */
                Scope *fnscope = scope_new(global);
                printf("  Function '%s' with %d params\n", s->name, s->param_count);
                for (int i = 0; i < s->param_count; ++i) {
                    scope_add_var(fnscope, s->fn_node->as.fn_decl.param_names[i], s->param_types[i], s->fn_node->line);
                }
                /* set current_scope to fnscope for correct variable lookups */
                Scope *prev = current_scope;
                current_scope = fnscope;
                /* type-check function body statements */
                tc_block(s->fn_node->as.fn_decl.body);
                /* check returns conform to declared return type */
                check_function_returns(s->fn_node);
                current_scope = prev;
                scope_free(fnscope);
            }
        }
    } else {
        printf("Root is not a block node!\n");
        tc_stmt(root);
    }

    TypeChkReport r = { .error_count = errors };
    if (errors == 0) {
        printf("Type checking passed: 0 errors\n");
    } else {
        printf("Type checking finished: %d error(s)\n", errors);
    }
    return r;
}
