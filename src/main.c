// #include <stdio.h>
// #include "../include/utils.h"
// #include "../include/lexer.h"
// #include "../include/parser.h"
// #include "../include/ast.h"

// int main(int argc, char **argv) {
//     // const char *src = "fn add(a, b) { return a + b; }\nint x = add(2, 3);\nprint(x);";
//     char *filename = "example/test.c";
//     char *src;
//     printf("Reading file: %s\n", filename);
//     src = read_file(filename);
//     printf("File content:\n%s\n", src);
//     Lexer l = initLexer("<test>", src);

//     Parser p;
//     parser_init(&p, &l, "<test>", src);
//     ParseError err;
//     Node *prog = parse_program(&p, &err);
//     ast_print(prog, 0);
//     ast_free(prog);
//     parser_free(&p);
//     return 0;
// }

// src/main.c
#include <stdio.h>

int yyparse(void);

int main() {
    printf("Parsing...\n");
    if (yyparse() == 0)
        printf("✅ Parse successful!\n");
    else
        printf("❌ Parse failed!\n");
    return 0;
}





// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "../include/lexer.h"
// #include "../include/utils.h"
// #include "../include/tokken.h"

// int main() {
//   printf("Lexer Test\n");
//   char *filename = "example/test.c";
//   char *src;
//   printf("Reading file: %s\n", filename);
//   src = read_file(filename);
//   printf("File content:\n%s\n", src);
//   Lexer l = initLexer(filename, src);
//   while (1) {
//     Token *tok = nextToken(&l);
//     // if (!tok) break;
//     if (tok->type == T_EOF) break;
//     print_token(tok);
//   }
// }
