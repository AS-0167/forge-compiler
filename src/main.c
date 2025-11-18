#include <stdio.h>
#include "../include/utils.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/ast.h"
#include "../include/scope.h"
#include "../include/typechk.h"

int main(int argc, char **argv) {
    // const char *src = "fn add(a, b) { return a + b; }\nint x = add(2, 3);\nprint(x);";
    char *filename = "example/test.c";
    char *src;
    printf("Reading file: %s\n", filename);
    src = read_file(filename);
    printf("File content:\n%s\n", src);
    Lexer l = initLexer("<test>", src);

    Parser p;
    parser_init(&p, &l, "<test>", src);
    ParseError err;
    Node *prog = parse_program(&p, &err);
    ast_print(prog, 0);
    printf("\n=== Scope Analysis ===\n");
    Scope *global = NULL;
    ScopeError sc_err = analyze_scopes(prog, &global);
    if (sc_err.type == ERR_NONE)
        printf("Scope analysis passed \n");
    
    TypeChkReport rep = type_check(prog, global);
    if (rep.error_count > 0) {
        printf("Found %d type errors\n", rep.error_count);
    } else {
        printf("No type errors\n");
    }
    ast_free(prog);
    parser_free(&p);
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
