#ifndef LEXER_H
#define LEXER_H

#include "tokken.h"

typedef struct {
  const char *filename;
  const char *src;
  int pos;
  int line;
  int col;
} Lexer;

Lexer initLexer(const char *filename, const char *src);
Token *nextToken(Lexer *l);

#endif
