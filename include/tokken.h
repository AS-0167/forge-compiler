#ifndef TOKKEN_H
#define TOKKEN_H

typedef enum {
  T_INT,
  T_FLOAT,
  T_STRING,
  T_CHAR,
  T_BOOL,
  T_VOID,
  T_STRINGLIT,
  T_CHARLIT,
  T_INTLIT,
  T_FLOATLIT,
  T_BOOLLIT,
  T_NULL,
  T_IDENTIFIER,
  T_FUNCTION,
  T_RETURN,
  T_ASSIGN,
  T_PLUS,
  T_MINUS,
  T_MULT,
  T_DIV,
  T_MOD,
  T_EQ,
  T_NEQ,
  T_LT,
  T_GT,
  T_LE,
  T_GE,
  T_AND,
  T_OR,
  T_NOT,
  T_LPAREN,
  T_RPAREN,
  T_LBRACKET,
  T_RBRACKET,
  T_LBRACE,
  T_RBRACE,
  T_SEMICOLON,
  T_COMMA,
  T_IF,
  T_ELSE,
  T_WHILE,
  T_DO,
  T_FOR,
  T_PRINT,
  T_COMMENT,
  T_QUOTE,
  T_SPACE,
  T_EOF
} TokenType;

typedef struct {
  TokenType type;
  char *lexeme;
  int line;
  int col;
} Token;

Token *create_token(TokenType type, const char *lexeme, int line, int col);
void destroy_token(Token *token);
void print_token(Token *token);
const char *token_type_to_string(TokenType type);


#endif
