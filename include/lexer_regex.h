#ifndef LEXER_REGEX_H
#define LEXER_REGEX_H

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokken.h"
#include "utils.h"


typedef struct {
  const char *pattern;
  TokenType type;
} Rule;

typedef struct {
    const char *filename;
    const char *src;
    int pos;
    int line;
    int col;
} RegexLexer;


RegexLexer initRegexLexer(const char *filename, const char *src);

Token *nextRegexToken(RegexLexer *l);

    

#endif