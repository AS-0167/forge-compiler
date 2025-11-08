#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scope.h"

static Symbol *symbol_new(const char *name, SymbolKind kind, const char *type) {
    Symbol *s = malloc(sizeof(Symbol));
    s->name = strdup(name);
    s->kind = kind;
    s->type = type ? strdup(type) : NULL;
    s->next = NULL;
    return s;
}

Scope *scope_new(Scope *parent) {
    Scope *s = malloc(sizeof(Scope));
    s->parent = parent;
    s->symbols = NULL;
    return s;
}

void scope_free(Scope *s) {
    Symbol *sym = s->symbols;
    while (sym) {
        Symbol *next = sym->next;
        free(sym->name);
        if (sym->type) free(sym->type);
        free(sym);
        sym = next;
    }
    free(s);
}

/* --- Add / Lookup --- */

static Symbol *scope_find_local(Scope *s, const char *name) {
    for (Symbol *sym = s->symbols; sym; sym = sym->next) {
        if (strcmp(sym->name, name) == 0)
            return sym;
    }
    return NULL;
}

Symbol *scope_lookup(Scope *s, const char *name, SymbolKind kind) {
    for (Scope *cur = s; cur; cur = cur->parent) {
        for (Symbol *sym = cur->symbols; sym; sym = sym->next) {
            if (strcmp(sym->name, name) == 0 && sym->kind == kind)
                return sym;
        }
    }
    return NULL;
}

void scope_add_var(Scope *s, const char *name, const char *type, int line) {
    if (scope_find_local(s, name)) {
        fprintf(stderr, "[ScopeError] VariableRedefinition: '%s' at line %d\n", name, line);
        exit(ERR_VariableRedefinition);
    }
    Symbol *sym = symbol_new(name, SYMBOL_VAR, type);
    sym->next = s->symbols;
    s->symbols = sym;
}

void scope_add_fn(Scope *s, const char *name, const char *type, int line) {
    if (scope_find_local(s, name)) {
        fprintf(stderr, "[ScopeError] FunctionPrototypeRedefinition: '%s' at line %d\n", name, line);
        exit(ERR_FunctionPrototypeRedefinition);
    }
    Symbol *sym = symbol_new(name, SYMBOL_FN, type);
    sym->next = s->symbols;
    s->symbols = sym;
}

/* --- Recursive AST traversal --- */

static void analyze_node(Node *n, Scope *scope);

static void analyze_block(Node *block, Scope *scope) {
    Scope *inner = scope_new(scope);
    for (int i = 0; i < block->as.block.stmt_count; i++)
        analyze_node(block->as.block.stmts[i], inner);
    scope_free(inner);
}

static void analyze_node(Node *n, Scope *scope) {
    if (!n) return;

    switch (n->type) {
        case NODE_VAR_DECL:
            scope_add_var(scope, n->as.var_decl.name, n->as.var_decl.type_name, n->line);
            if (n->as.var_decl.init)
                analyze_node(n->as.var_decl.init, scope);
            break;

        case NODE_FN_DECL:
            scope_add_fn(scope, n->as.fn_decl.name, n->as.fn_decl.type_name, n->line);
            {
                Scope *fn_scope = scope_new(scope);
                for (int i = 0; i < n->as.fn_decl.param_count; i++)
                    scope_add_var(fn_scope, n->as.fn_decl.param_names[i], n->as.fn_decl.param_type[i], n->line);
                analyze_node(n->as.fn_decl.body, fn_scope);
                scope_free(fn_scope);
            }
            break;

        case NODE_IDENTIFIER: {
            Symbol *sym = scope_lookup(scope, n->as.ident.name, SYMBOL_VAR);
            if (!sym) {
                fprintf(stderr, "[ScopeError] UndeclaredVariableAccessed: '%s' at line %d\n", n->as.ident.name, n->line);
                exit(ERR_UndeclaredVariableAccessed);
            }
            break;
        }

        case NODE_CALL: {
            if (!scope_lookup(scope, n->as.call.callee->as.ident.name, SYMBOL_FN)) {
                fprintf(stderr, "[ScopeError] UndefinedFunctionCalled: '%s' at line %d\n",
                        n->as.call.callee->as.ident.name, n->line);
                exit(ERR_UndefinedFunctionCalled);
            }
            for (int i = 0; i < n->as.call.arg_count; i++)
                analyze_node(n->as.call.args[i], scope);
            break;
        }

        case NODE_ASSIGN:
        case NODE_BINARY:
        case NODE_UNARY:
            analyze_node(n->as.binary.left, scope);
            analyze_node(n->as.binary.right, scope);
            break;

        case NODE_BLOCK:
            analyze_block(n, scope);
            break;

        default:
            // Recurse through children if they exist
            break;
    }
}

ScopeError analyze_scopes(Node *root) {
    Scope *global = scope_new(NULL);
    analyze_node(root, global);
    scope_free(global);
    return (ScopeError){ .type = ERR_NONE };
}
