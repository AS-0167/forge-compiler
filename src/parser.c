#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/utils.h" 
#include "../include/tokken.h"
#include "../include/lexer.h"

static void advance(Parser *p) {
    // fprintf(stderr, "Freeing token %s @ %p\n", p->cur, (void*)p->cur);

    if (p->cur) destroy_token(p->cur);
    p->cur = p->peek;
    do {p->peek = nextToken(p->lex);} while (p->peek->type == T_COMMENT);

    // printf("Advanced to token: %s\n", p->cur ? p->cur->lexeme : "NULL");
    // printf("Peek token: %s\n", p->peek ? p->peek->lexeme : "NULL");
}

static int check(Parser *p, TokenType t) {
    return p->cur && p->cur->type == t;
}

static int match(Parser *p, TokenType t) {
    if (check(p, t)) { advance(p); return 1; }
    return 0;
}

void parser_init(Parser *p, Lexer *l, const char *filename, const char *src) {
    p->lex = l;
    p->filename = filename;
    p->src = src;
    do {p->cur = nextToken(l);} while (p->cur->type == T_COMMENT); 
    do {p->peek = nextToken(l);} while (p->peek->type == T_COMMENT);
    // printf("Initialized parser with first token: %s\n", p->cur->lexeme);
    // printf("Peek token: %s\n", p->peek->lexeme);
}

void parser_free(Parser *p) {
    // fprintf(stderr, "Freeing token %s @ %p\n", p->cur, (void*)p->cur);

    if (p->cur) destroy_token(p->cur);
    if (p->peek) destroy_token(p->peek);
}

/* Forward declarations */
static Node *parse_declaration(Parser *p, ParseError *err);
static Node *parse_statement(Parser *p, ParseError *err);
static Node *parse_block(Parser *p, ParseError *err);
static Node *parse_expression(Parser *p, int rbp, ParseError *err);
static Node *parse_primary(Parser *p, ParseError *err);

/* Binding power table (higher = tighter) */
static int bp(TokenType t) {
    switch (t) {
        case T_LPAREN:    return 90; /* for call - handled specially */
        case T_PLUSPLUS: case T_MINUSMINUS: return 80;
        case T_MULT: case T_DIV: case T_MOD: return 70;
        case T_PLUS: case T_MINUS: return 60;
        case T_LSHIFT: case T_RSHIFT: return 55;
        case T_LT: case T_LE: case T_GT: case T_GE: return 50;
        case T_EQ: case T_NEQ: return 45;
        case T_BITAND: return 40;
        case T_BITXOR: return 39;
        case T_BITOR: return 38;
        case T_AND: return 35; /* && */
        case T_OR: return 34;  /* || */
        case T_QUESTION: return 20; /* ternary - special */
        case T_ASSIGN:
        case T_PLUSASS: case T_MINUSASS: case T_MULTASS: case T_DIVASS:
        case T_MODASS: case T_BITANDASS: case T_BITORASS: case T_BITXORASS:
        case T_LSHIFTASS: case T_RSHIFTASS:
            return 10; /* assignments (right-assoc) */
        default: return 0;
    }
}

/* Helpers to check for type tokens used for declarations */
static int is_type_token(TokenType t) {
    return t == T_INT || t == T_FLOAT || t == T_STRING || t == T_CHAR || t == T_BOOL || t == T_VOID;
}

/* Parse identifier list for function params (simple: identifiers only) */
static char **collect_params(Parser *p, int *out_count, char ***param_type, ParseError *err) {
    // printf("Collecting function parameters\n");
    char **params = NULL;
    char **param_types = NULL;
    int count = 0;
    if (check(p, T_RPAREN)) { *out_count = 0; return NULL; }
    while (1) {
        if (!check(p, T_INT) && !check(p, T_FLOAT) && !check(p, T_STRING) && !check(p, T_CHAR) && !check(p, T_BOOL) && !check(p, T_VOID)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected parameter type");
            if (err) *err = PERR_ExpectedTypeToken;
            // attempt to continue
            return params;
        }
        param_types = realloc(param_types, sizeof(char*) * (count + 1));
        param_types[count] = strdup(token_type_to_string(p->cur->type));
        advance(p); /* consume type */
        if (!check(p, T_IDENTIFIER)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected parameter name");
            if (err) *err = PERR_ExpectedIdentifier;
            // attempt to continue
            return params;
        }
        params = realloc(params, sizeof(char*) * (count + 1));
        params[count] = strdup(p->cur->lexeme);
        count++;
        // printf("Collected param: %s\n", params[count-1]);

        advance(p);
        // printf("Collected param: %s\n", params[count-1]);
        if (check(p, T_COMMA)) { advance(p); continue; }
        break;
    }
    *out_count = count;
    *param_type = param_types;
    return params;
}

/* Primary: literals, identifiers, parenthesized, function calls (postfix) */
static Node *parse_primary(Parser *p, ParseError *err) {
    Token *t = p->cur;
    if (!t) { if (err) *err = PERR_UnexpectedEOF; return NULL; }

    switch (t->type) {
        case T_INTLIT: {
            Node *n = ast_new(NODE_INT_LIT, t->line, t->col);
            n->as.int_lit.value = strtol(t->lexeme, NULL, 10);
            advance(p);
            return n;
        }
        case T_FLOATLIT: {
            Node *n = ast_new(NODE_FLOAT_LIT, t->line, t->col);
            n->as.float_lit.value = strtod(t->lexeme, NULL);
            advance(p);
            return n;
        }
        case T_STRINGLIT: {
            Node *n = ast_new(NODE_STRING_LIT, t->line, t->col);
            n->as.string_lit.value = strdup(t->lexeme);
            advance(p);
            return n;
        }
        case T_CHARLIT: {
            Node *n = ast_new(NODE_CHAR_LIT, t->line, t->col);
            n->as.char_lit.value = t->lexeme[0];
            advance(p);
            return n;
        }
        case T_BOOLLIT: {
            Node *n = ast_new(NODE_BOOL_LIT, t->line, t->col);
            n->as.bool_lit.value = (strcmp(t->lexeme, "true") == 0) ? 1 : 0;
            advance(p);
            return n;
        }
        case T_IDENTIFIER: {
            Node *n = ast_new(NODE_IDENTIFIER, t->line, t->col);
            n->as.ident.name = strdup(t->lexeme);
            advance(p);
            /* postfix: function call */
            if (check(p, T_LPAREN)) {
                /* build call node */
                advance(p); /* consume '(' */
                Node **args = NULL;
                int arg_count = 0;
                if (!check(p, T_RPAREN)) {
                    while (1) {
                        Node *arg = parse_expression(p, 0, err);
                        if (!arg) { /* error already reported */ break; }
                        args = realloc(args, sizeof(Node*) * (arg_count + 1));
                        args[arg_count++] = arg;
                        if (check(p, T_COMMA)) { advance(p); continue; }
                        break;
                    }
                }
                if (!match(p, T_RPAREN)) {
                    report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected ')'");
                    if (err) *err = PERR_UnexpectedToken;
                }
                Node *call = ast_new(NODE_CALL, t->line, t->col);
                call->as.call.callee = n;
                call->as.call.args = args;
                call->as.call.arg_count = arg_count;
                return call;
            }
            return n;
        }
        case T_LPAREN: {
            advance(p);
            Node *expr = parse_expression(p, 0, err);
            if (!match(p, T_RPAREN)) {
                report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected ')'");
                if (err) *err = PERR_UnexpectedToken;
            }
            return expr;
        }
        default:
            report_error(p->filename, p->src, t->line, t->col, "Expected expression");
            if (err) *err = PERR_ExpectedExpr;
            return NULL;
    }
}

/* Pratt-style expression parser */
static Node *parse_expression(Parser *p, int rbp, ParseError *err) {
    // printf("Parsing expression with rbp=%d at token: %s\n", rbp, p->cur ? p->cur->lexeme : "NULL");
    Node *left = NULL;
    /* prefix/unary */
    if (check(p, T_MINUS) || check(p, T_NOT) || check(p, T_BITNOT) || check(p, T_PLUS)) {
        Token *op = p->cur;
        advance(p);
        Node *rhs = parse_expression(p, 80, err); /* unary binds tightly */
        Node *un = ast_new(NODE_UNARY, op->line, op->col);
        un->as.unary.op = op->type;
        un->as.unary.rhs = rhs;
        left = un;
    } else {
        left = parse_primary(p, err);
        if (!left) return NULL;
    }

    while (p->cur && p->cur->type != T_SEMICOLON && p->cur->type != T_RPAREN && rbp < bp(p->cur->type)) {
        Token *op = create_token(p->cur->type, p->cur->lexeme, p->cur->line, p->cur->col);

        /* Calls are handled in primary (postfix). Here handle binary and assignment */
        /* T_QUESTION (ternary) not implemented (could be added) */
        if (bp(op->type) == 0) break;

        /* consume operator */
        advance(p);

        /* assignment (right-assoc) */
        int right_bp = bp(op->type);
        if (op->type == T_ASSIGN ||
            op->type == T_PLUSASS || op->type == T_MINUSASS || op->type == T_MULTASS ||
            op->type == T_DIVASS || op->type == T_MODASS || op->type == T_BITANDASS ||
            op->type == T_BITORASS || op->type == T_BITXORASS || op->type == T_LSHIFTASS ||
            op->type == T_RSHIFTASS) {
            /* right associative: parse with (right_bp - 1) or special */
            Node *right = parse_expression(p, right_bp - 1, err);
            Node *assign = ast_new(NODE_ASSIGN, op->line, op->col);
            assign->as.assign.left = left;
            assign->as.assign.op = op->type;
            assign->as.assign.right = right;
            left = assign;
            continue;
        }

        /* binary (left assoc): parse rhs with binding power */
        Node *right = parse_expression(p, right_bp, err);
        Node *bin = ast_new(NODE_BINARY, op->line, op->col);
        bin->as.binary.left = left;
        bin->as.binary.op = op->type;
        bin->as.binary.right = right;
        left = bin;
    }

    return left;
}

/* Parse a block: { ... } */
static Node *parse_block(Parser *p, ParseError *err) {
    Node *blk = ast_new(NODE_BLOCK, p->cur->line, p->cur->col);
    blk->as.block.stmts = NULL;
    blk->as.block.stmt_count = 0;

    /* assume '{' has been matched by caller */
    while (!check(p, T_RBRACE) && !check(p, T_EOF)) {
        Node *decl = parse_declaration(p, err);
        if (!decl) {
            /* attempt to recover: skip token */
            if (p->cur && p->cur->type != T_EOF) advance(p);
            else break;
        } else {
            blk->as.block.stmts = realloc(blk->as.block.stmts, sizeof(Node*) * (blk->as.block.stmt_count + 1));
            blk->as.block.stmts[blk->as.block.stmt_count++] = decl;
        }
    }
    if (!match(p, T_RBRACE)) {
        report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected '}'");
        if (err) *err = PERR_UnexpectedToken;
    }
    return blk;
}

/* Parse statements and declarations */
static Node *parse_statement(Parser *p, ParseError *err) {
    /* return */
    if (match(p, T_RETURN)) {
        Node *n = ast_new(NODE_RETURN, p->cur->line, p->cur->col);
        if (!check(p, T_SEMICOLON)) n->as.ret.expr = parse_expression(p, 0, err);
        else n->as.ret.expr = NULL;
        match(p, T_SEMICOLON);
        return n;
    }

    /* if */
    if (match(p, T_IF)) {
        if (!match(p, T_LPAREN)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected '(' after if");
            if (err) *err = PERR_UnexpectedToken;
        }
        Node *cond = parse_expression(p, 0, err);
        if (!match(p, T_RPAREN)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected ')'");
            if (err) *err = PERR_UnexpectedToken;
        }
        Node *then_branch = NULL;
        if (match(p, T_LBRACE)) {
            then_branch = parse_block(p, err);
        } else {
            then_branch = parse_statement(p, err);
        }
        Node *else_branch = NULL;
        if (match(p, T_ELSE)) {
            if (match(p, T_LBRACE)) else_branch = parse_block(p, err);
            else else_branch = parse_statement(p, err);
        }
        Node *n = ast_new(NODE_IF, p->cur->line, p->cur->col);
        n->as.ifstmt.cond = cond;
        n->as.ifstmt.then_branch = then_branch;
        n->as.ifstmt.else_branch = else_branch;
        return n;
    }

    /* while */
    if (match(p, T_WHILE)) {
        if (!match(p, T_LPAREN)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected '(' after while");
            if (err) *err = PERR_UnexpectedToken;
        }
        Node *cond = parse_expression(p, 0, err);
        if (!match(p, T_RPAREN)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected ')'");
            if (err) *err = PERR_UnexpectedToken;
        }
        Node *body = NULL;
        if (match(p, T_LBRACE)) body = parse_block(p, err);
        else body = parse_statement(p, err);
        Node *n = ast_new(NODE_WHILE, p->cur->line, p->cur->col);
        n->as.whilestmt.cond = cond;
        n->as.whilestmt.body = body;
        return n;
    }

    /* for ( init; cond; update ) stmt */
    if (match(p, T_FOR)) {
        if (!match(p, T_LPAREN)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected '(' after for");
            if (err) *err = PERR_UnexpectedToken;
        }
        Node *init = NULL;
        if (!check(p, T_SEMICOLON)) {
            /* allow var decl or expr */
            if (is_type_token(p->cur->type)) init = parse_declaration(p, err);
            else {
                init = parse_expression(p, 0, err);
                match(p, T_SEMICOLON);
            }
        } else {
            match(p, T_SEMICOLON);
        }

        Node *cond = NULL;
        if (!check(p, T_SEMICOLON)) cond = parse_expression(p, 0, err);
        match(p, T_SEMICOLON);

        Node *update = NULL;
        if (!check(p, T_RPAREN)) update = parse_expression(p, 0, err);
        if (!match(p, T_RPAREN)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected ')'");
            if (err) *err = PERR_UnexpectedToken;
        }

        Node *body = NULL;
        if (match(p, T_LBRACE)) body = parse_block(p, err);
        else body = parse_statement(p, err);

        Node *n = ast_new(NODE_FOR, p->cur->line, p->cur->col);
        n->as.forstmt.init = init;
        n->as.forstmt.cond = cond;
        n->as.forstmt.update = update;
        n->as.forstmt.body = body;
        return n;
    }

    /* print */
    if (match(p, T_PRINT)) {
        Node *n = ast_new(NODE_PRINT, p->cur->line, p->cur->col);
        if (match(p, T_LPAREN)) {
            n->as.printstmt.expr = parse_expression(p, 0, err);
            match(p, T_RPAREN);
        } else {
            n->as.printstmt.expr = NULL;
        }
        match(p, T_SEMICOLON);
        return n;
    }

    /* block */
    if (match(p, T_LBRACE)) {
        return parse_block(p, err);
    }

    /* expression statement or assignment */
    if (is_type_token(p->cur->type)) {
        /* typed var declaration: <type> identifier [= expr] ; */
        TokenType tt = p->cur->type;
        char *type_name = strdup(token_type_to_string(tt));
        advance(p);
        if (!check(p, T_IDENTIFIER)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected identifier after type");
            if (err) *err = PERR_ExpectedIdentifier;
            return NULL;
        }
        char *name = strdup(p->cur->lexeme);
        advance(p);
        Node *init = NULL;
        if (match(p, T_ASSIGN)) {
            init = parse_expression(p, 0, err);
        }
        match(p, T_SEMICOLON);
        Node *n = ast_new(NODE_VAR_DECL, p->cur->line, p->cur->col);
        n->as.var_decl.type_name = type_name;
        n->as.var_decl.name = name;
        n->as.var_decl.init = init;
        return n;
    }

    /* function declaration: fn IDENT ( params ) block */
    if (match(p, T_FUNCTION)) {
        // printf("Parsing function declaration\n");
        if (!check(p, T_INT) && !check(p, T_FLOAT) && !check(p, T_STRING) && !check(p, T_CHAR) && !check(p, T_BOOL) && !check(p, T_VOID)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected return type after 'fn'");
            if (err) *err = PERR_ExpectedTypeToken;
            return NULL;
        }
        char *type_name = strdup(token_type_to_string(p->cur->type));
        advance(p); /* consume return type */
        if (!check(p, T_IDENTIFIER)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected function name after 'fn'");
            if (err) *err = PERR_ExpectedIdentifier;
            return NULL;
        }
        char *fname = strdup(p->cur->lexeme);
        advance(p);
        if (!check(p, T_LPAREN)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected '(' after function name");
            if (err) *err = PERR_UnexpectedToken;
        }
        int param_count = 0;
        char **param_type = NULL;
        char **params = NULL;
        advance(p); /* consume '(' */
        // printf("Function name: %s\n", fname);
        

        if (!check(p, T_RPAREN)) {
            params = collect_params(p, &param_count, &param_type, err);
        }
        // printf("Function %s has %d params\n", fname, param_count);

        if (!match(p, T_RPAREN)) {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected ')'");
            if (err) *err = PERR_UnexpectedToken;
        }
        // printf("Function %s has %d params\n", fname, param_count);
        Node *body = NULL;
        if (match(p, T_LBRACE)) body = parse_block(p, err);
        else {
            report_error(p->filename, p->src, p->cur->line, p->cur->col, "Expected function body");
            if (err) *err = PERR_UnexpectedToken;
        }
        Node *n = ast_new(NODE_FN_DECL, p->cur->line, p->cur->col);
        n->as.fn_decl.type_name = type_name;
        n->as.fn_decl.name = fname;
        n->as.fn_decl.param_type = param_type;
        n->as.fn_decl.param_names = params;
        n->as.fn_decl.param_count = param_count;
        n->as.fn_decl.body = body;
        return n;
    }

    /* default: expression statement */
    {
        Node *expr = parse_expression(p, 0, err);
        Node *n = ast_new(NODE_EXPR_STMT, p->cur ? p->cur->line : 0, p->cur ? p->cur->col : 0);
        n->as.expr_stmt.expr = expr;
        match(p, T_SEMICOLON);
        return n;
    }
}

/* Top-level program parse */
Node *parse_program(Parser *p, ParseError *err) {
    Node *prog = ast_new(NODE_BLOCK, 0, 0);
    prog->as.block.stmts = NULL;
    prog->as.block.stmt_count = 0;

    while (p->cur && p->cur->type != T_EOF) {
        Node *decl = parse_declaration(p, err);
        if (!decl) {
            /* try to skip to next semicolon or EOF to recover */
            while (p->cur && p->cur->type != T_SEMICOLON && p->cur->type != T_EOF) advance(p);
            if (p->cur && p->cur->type == T_SEMICOLON) advance(p);
            continue;
        }
        prog->as.block.stmts = realloc(prog->as.block.stmts, sizeof(Node*) * (prog->as.block.stmt_count + 1));
        prog->as.block.stmts[prog->as.block.stmt_count++] = decl;
    }
    return prog;
}

/* A declaration can be a top-level function, or typed variable, or statement */
static Node *parse_declaration(Parser *p, ParseError *err) {
    /* function */
    if (check(p, T_FUNCTION) || check(p, T_INT) || check(p, T_FLOAT) || check(p, T_STRING) || check(p, T_CHAR) || check(p, T_BOOL) || check(p, T_VOID)) {
        /* if function starts with 'fn' */
        if (check(p, T_FUNCTION)) {
            // printf("Parsing function declaration\n");
            return parse_statement(p, err); /* parse function as statement (we implemented fn in stmt path) */
        }
        /* typed global variable */
        if (is_type_token(p->cur->type)) {
            return parse_statement(p, err); /* variable decl handled in statement path */
        }
    }
    /* otherwise a statement */
    return parse_statement(p, err);
}
