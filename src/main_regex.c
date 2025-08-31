#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer_regex.h"

int main() {
    printf("Regex Lexer Test\n");
    char *filename = "example/test.c";
    char *src;
    printf("Reading file: %s\n", filename);
    src = read_file(filename);
    printf("File content:\n%s\n", src);
    RegexLexer l = initRegexLexer(filename, src);
    while (1) {
        Token *tok = nextRegexToken(&l);
        // if (!tok) break;
        if (tok->type == T_EOF) break;
        print_token(tok);
        destroy_token(tok);
    }
    free(src);
    return 0;
}