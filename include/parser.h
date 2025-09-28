#ifndef PARSER_H
#define PARSER_H

#include "../include/lexer.h"
#include "ast.h"

typedef enum {
    PERR_OK = 0,
    PERR_UnexpectedEOF,
    PERR_FailedToFindToken,
    PERR_ExpectedTypeToken,
    PERR_ExpectedIdentifier,
    PERR_UnexpectedToken,
    PERR_ExpectedFloatLit,
    PERR_ExpectedIntLit,
    PERR_ExpectedStringLit,
    PERR_ExpectedBoolLit,
    PERR_ExpectedExpr,
} ParseError;

typedef struct {
    Lexer *lex;
    Token *cur;
    Token *peek;
    const char *filename;
    const char *src;
} Parser;

void parser_init(Parser *p, Lexer *l, const char *filename, const char *src);
void parser_free(Parser *p);

/* Entry point */
Node *parse_program(Parser *p, ParseError *err);

#endif
