#ifndef SCOPE_H
#define SCOPE_H

#include "ast.h"

typedef enum {
    SYMBOL_VAR,
    SYMBOL_FN
} SymbolKind;

typedef struct Symbol {
    char *name;
    SymbolKind kind;
    char *type;   // For now, type as string (can be extended later)
    struct Symbol *next; // For linked list inside a scope
} Symbol;

typedef struct Scope {
    struct Scope *parent;
    Symbol *symbols;
} Scope;

typedef enum {
    ERR_NONE = 0,
    ERR_UndeclaredVariableAccessed,
    ERR_UndefinedFunctionCalled,
    ERR_VariableRedefinition,
    ERR_FunctionPrototypeRedefinition
} ScopeErrorType;

typedef struct {
    ScopeErrorType type;
    const char *message;
    const char *name;
    int line;
} ScopeError;

/* Scope API */
Scope *scope_new(Scope *parent);
void scope_free(Scope *s);

void scope_add_var(Scope *s, const char *name, const char *type, int line);
void scope_add_fn(Scope *s, const char *name, const char *type, int line);

Symbol *scope_lookup(Scope *s, const char *name, SymbolKind kind);

/* Analysis */
ScopeError analyze_scopes(Node *root);

#endif
