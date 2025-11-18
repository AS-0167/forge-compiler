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
    char *type;   // for vars: declared type (e.g. "int"); for fns: return type
    /* ---- new fields for functions ---- */
    char **param_types; // array of parameter types (strings) for functions, NULL for vars
    int param_count;    // number of params for functions, 0 for vars
    Node *fn_node;      // pointer to function AST node (optional, for later passes)
    /* ---------------------------------- */
    struct Symbol *next; // linked list chaining inside a scope
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

/* Add var: type string may be NULL if unknown (but recommended). */
void scope_add_var(Scope *s, const char *name, const char *type, int line);

/* Add function: pass function node so scope can store param types and count. */
void scope_add_fn(Scope *s, const char *name, const char *ret_type, Node *fn_node, int line);

/* lookup: return NULL if not found. kind can be SYMBOL_VAR or SYMBOL_FN, or  -1 to ignore kind. */
Symbol *scope_lookup(Scope *s, const char *name, int kind);

/* Analysis - returns pointer to the created global scope (do NOT free it if you want to use it in typechecker) */
ScopeError analyze_scopes(Node *root, Scope **out_global_scope);

#endif




















// #ifndef SCOPE_H
// #define SCOPE_H

// #include "ast.h"

// typedef enum {
//     SYMBOL_VAR,
//     SYMBOL_FN
// } SymbolKind;

// typedef struct Symbol {
//     char *name;
//     SymbolKind kind;
//     char *type;   // For now, type as string (can be extended later)
//     struct Symbol *next; // For linked list inside a scope
// } Symbol;

// typedef struct Scope {
//     struct Scope *parent;
//     Symbol *symbols;
// } Scope;

// typedef enum {
//     ERR_NONE = 0,
//     ERR_UndeclaredVariableAccessed,
//     ERR_UndefinedFunctionCalled,
//     ERR_VariableRedefinition,
//     ERR_FunctionPrototypeRedefinition
// } ScopeErrorType;

// typedef struct {
//     ScopeErrorType type;
//     const char *message;
//     const char *name;
//     int line;
// } ScopeError;

// /* Scope API */
// Scope *scope_new(Scope *parent);
// void scope_free(Scope *s);

// void scope_add_var(Scope *s, const char *name, const char *type, int line);
// void scope_add_fn(Scope *s, const char *name, const char *type, int line);

// Symbol *scope_lookup(Scope *s, const char *name, SymbolKind kind);

// /* Analysis */
// ScopeError analyze_scopes(Node *root);

// #endif
