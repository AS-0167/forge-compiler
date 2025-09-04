#ifndef LEXER_H
#define LEXER_H

#include "tokken.h"

typedef struct {
  const char *filename;  // Source file name (for error reporting)
  const char *src;       // Source code string
  int pos;               // Current position in source (current character)
  int line;              // Current line number
  int col;               // Current column number
} Lexer;

Lexer initLexer(const char *filename, const char *src);
Token *nextToken(Lexer *l);

#endif
