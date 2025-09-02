#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tokken.h"

Token *create_token(TokenType type, const char *lexeme, int line, int col) {
  Token *tok = malloc(sizeof(Token));
  tok->type = type;
  tok->lexeme = lexeme;
  tok->line = line;
  tok->col = col;
  return tok;
}

void destroy_token(Token *tok) {
  if (tok) {
    free(tok->lexeme);
    free(tok);
  }
}

void print_token(Token *tok) {
  if (tok->type == T_INTLIT || tok->type == T_FLOATLIT || 
      tok->type == T_STRINGLIT || tok->type == T_CHARLIT || 
      tok->type == T_IDENTIFIER || tok->type == T_COMMENT) {
    printf("%s, (\"%s\")\n", token_type_to_string(tok->type), tok->lexeme);
  } else {
    printf("%s\n", token_type_to_string(tok->type));
  }
}

const char *token_type_to_string(TokenType type) {
  printf("Converting token type %d to string\n", type);
  
  switch (type) {

    case T_INT: return "T_INT";
    case T_FLOAT: return "T_FLOAT";
    case T_STRING: return "T_STRING";
    case T_CHAR: return "T_CHAR";
    case T_BOOL: return "T_BOOL";
    case T_VOID: return "T_VOID";
    case T_STRINGLIT: return "T_STRINGLIT";
    case T_CHARLIT: return "T_CHARLIT";
    case T_INTLIT: return "T_INTLIT";
    case T_FLOATLIT: return "T_FLOATLIT";
    case T_BOOLLIT: return "T_BOOLLIT";
    case T_NULL: return "T_NULL";
    case T_IDENTIFIER: return "T_IDENTIFIER";
    case T_FUNCTION: return "T_FUNCTION";
    case T_RETURN: return "T_RETURN";
    case T_ASSIGN: return "T_ASSIGN";
    case T_PLUS: return "T_PLUS";
    case T_MINUS: return "T_MINUS";
    case T_MULT: return "T_MULT";
    case T_DIV: return "T_DIV";
    case T_MOD: return "T_MOD";
    case T_EQ: return "T_EQ";
    case T_NEQ: return "T_NEQ";
    case T_LT: return "T_LT";
    case T_GT: return "T_GT";
    case T_LE: return "T_LE";
    case T_GE: return "T_GE";
    case T_AND: return "T_AND";
    case T_OR: return "T_OR";
    case T_NOT: return "T_NOT";
    case T_LPAREN: return "T_LPAREN";
    case T_RPAREN: return "T_RPAREN";
    case T_LBRACKET: return "T_LBRACKET";
    case T_RBRACKET: return "T_RBRACKET";
    case T_LBRACE: return "T_LBRACE";
    case T_RBRACE: return "T_RBRACE";
    case T_SEMICOLON: return "T_SEMICOLON";
    case T_COMMA: return "T_COMMA";
    case T_IF: return "T_IF";
    case T_ELSE: return "T_ELSE";
    case T_WHILE: return "T_WHILE";
    case T_DO: return "T_DO";
    case T_FOR: return "T_FOR";
    case T_PRINT: return "T_PRINT";
    case T_COMMENT: return "T_COMMENT";
    case T_DOT: return "T_DOT";
    case T_COLON: return "T_COLON";
    case T_QUESTION: return "T_QUESTION";
    case T_SPACE: return "T_SPACE";
    case T_BITAND: return "T_BITAND";
    case T_BITOR: return "T_BITOR";
    case T_BITXOR: return "T_BITXOR";
    case T_BITNOT: return "T_BITNOT";
    case T_LSHIFT: return "T_LSHIFT";
    case T_RSHIFT: return "T_RSHIFT";
    case T_PLUSPLUS: return "T_PLUSPLUS";
    case T_MINUSMINUS: return "T_MINUSMINUS";
    case T_PLUSASS: return "T_PLUSASS";
    case T_MINUSASS: return "T_MINUSASS";
    case T_MULTASS: return "T_MULTASS";
    case T_DIVASS: return "T_DIVASS";
    case T_MODASS: return "T_MODASS";
    case T_BITANDASS: return "T_BITANDASS";
    case T_BITORASS: return "T_BITORASS";
    case T_BITXORASS: return "T_BITXORASS";
    case T_LSHIFTASS: return "T_LSHIFTASS";
    case T_RSHIFTASS: return "T_RSHIFTASS";
    // case T_SPACE: return "T_SPACE";
    case T_EOF: return "T_EOF";
    // default: return "UNKNOWN";
  }
}
