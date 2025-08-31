#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"
#include "../include/utils.h"
#include "../include/tokken.h"

int main() {
  printf("Lexer Test\n");
  char *filename = "example/test.c";
  char *src;
  printf("Reading file: %s\n", filename);
  src = read_file(filename);
  printf("File content:\n%s\n", src);
  Lexer l = initLexer(filename, src);
  while (1) {
    Token *tok = nextToken(&l);
    // if (!tok) break;
    if (tok->type == T_EOF) break;
    print_token(tok);
  }
}
