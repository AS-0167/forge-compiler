#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"
#include "../include/utils.h"

Lexer initLexer(const char *filename, const char *src) {
  Lexer l;
  l.filename = filename;
  l.src = src;
  l.pos = 0;
  l.line = 1;
  l.col = 1;
  return l;
}

Token *nextToken(Lexer *l) {
  Token *tok = malloc(sizeof(Token));
  while (l->pos < strlen(l->src)) {

    // Skip whitespace and empty lines
    if (isSpace(l->src[l->pos])) {
      // printf("Skipping whitespace character: '%c'\n", l->src[l->pos]);
      if (l->src[l->pos] == '\n') {
        l->line++;
        l->col = 1;
      } else {
        l->col++;
      }
      l->pos++;
      continue;
    }

    // Identifying comments
    if (l->src[l->pos] == '/') {
      int col = l->col;
      if (l->src[l->pos + 1] == '/') {
        while (l->pos < strlen(l->src) && l->src[l->pos] != '\n') {
          l->pos++;
          l->col++;
        }
        tok->type = T_COMMENT;
        tok->lexeme = "//";
        tok->line = l->line;
        tok->col = col;
        return tok;
      } else if (l->src[l->pos + 1] == '*') {
        l->pos += 2;
        l->col += 2;
        int comment_closed = 0;
        while (l->pos < strlen(l->src) && !(l->src[l->pos] == '*' && l->src[l->pos + 1] == '/')) {
          if (l->src[l->pos] == '*' && l->src[l->pos + 1] == '/') {
            comment_closed = 1;
            break;
          }
          if (l->src[l->pos] == '\n') {
            l->line++;
            l->col = 1;
          } else {
            l->col++;
          }
          l->pos++;
        }
        if (l->pos < strlen(l->src)) {
          l->pos += 1;
          l->col += 1;
        }
        if (!comment_closed) {
          report_error(l->filename, l->src, l->line, l->col, "Unterminated comment");
        }
        tok->type = T_COMMENT;
        tok->lexeme = "/*";
        tok->line = l->line;
        tok->col = col;
        return tok;
      }
    }

    // Identifying string literals
    if (l->src[l->pos] == '"') {
      int start_col = l->col;
      int start_line = l->line;

      

      char *buffer = malloc(sizeof(char) * 4096);
      int buf_index = 0;

      buffer[buf_index++] = '"';

      l->pos++;
      l->col++;

      int closed = 0;
      while (l->pos < strlen(l->src)) {
        char c = l->src[l->pos];

        if (c == '"') {
            buffer[buf_index++] = '"';
            closed = 1;
            l->pos++;
            l->col++;
            break;
        }

        if (c == '\n' || c == '\r') {
          report_error(l->filename, l->src, start_line, start_col,
                      "unterminated string literal");
        }

        if (c == '\\') {
            l->pos++;
            l->col++;
            if (l->pos >= strlen(l->src)) {
              report_error(l->filename, l->src, start_line, start_col,
                          "unterminated string literal");
            }
            char esc = l->src[l->pos];
            switch (esc) {
                case 'n':
                  buffer[buf_index++] = '\n'; 
                  break;
                case 't': 
                  buffer[buf_index++] = '\t'; 
                  break;
                case 'r': 
                  buffer[buf_index++] = '\r'; 
                  break;
                case '\\': 
                  buffer[buf_index++] = '\\'; 
                  break;
                case '"': 
                  buffer[buf_index++] = '"'; 
                  break;
                case '0': 
                  buffer[buf_index++] = '\0'; 
                  break;
                default:
                  report_error(l->filename, l->src, l->line, l->col,
                              "invalid escape sequence in string literal");
            }
        } else {
            buffer[buf_index++] = c;
        }

        l->pos++;
        l->col++;

        if (buf_index >= (int)sizeof(buffer) - 1) {
          char *new_buffer = realloc(buffer, sizeof(char) * (buf_index + 4096));
          if (!new_buffer) {
            free(buffer);
            report_error(l->filename, l->src, start_line, start_col,
                        "memory allocation failed");
          }
          buffer = new_buffer;
        }
      }

      if (!closed) {
          report_error(l->filename, l->src, start_line, start_col,
                      "unterminated string literal");
      }
      // buffer[buf_index++] = '"';

      buffer[buf_index] = '\0';

      char *value = strdup(buffer);
      tok->type = T_STRINGLIT;
      tok->lexeme = value;
      tok->line = start_line;
      tok->col = start_col;
      return tok;
    }

    // Identifying character literals
    if (l->src[l->pos] == '\'') {
      int start_col = l->col;
      int start_line = l->line;
      l->pos++;
      l->col++;
      if (l->pos >= strlen(l->src)) {
        report_error(l->filename, l->src, start_line, start_col,
                    "unterminated character literal");
      }
      char c = l->src[l->pos];
      char value;
      if(c == '\'') {
        report_error(l->filename, l->src, start_line, start_col, "empty character sequence");
      }
      if (c == '\\') {
        l->pos++;
        l->col++;
        if (l->pos >= strlen(l->src)) {
          report_error(l->filename, l->src, start_line, start_col,
                      "unterminated character literal");
        }
        char esc = l->src[l->pos];
        switch (esc) {
          case 'n': 
            value = '\n'; 
            break;
          case 't': 
            value = '\t'; 
            break;
          case 'r': 
            value = '\r'; 
            break;
          case '\\': 
            value = '\\'; 
            break;
          case '\'': 
            value = '\''; 
            break;
          case '0': 
            value = '\0'; 
            break;
          default:
            report_error(l->filename, l->src, l->line, l->col,
                        "invalid escape sequence in character literal");
        }
      } else {
          value = c;
      }
      l->pos++;
      l->col++;
      if (l->pos >= strlen(l->src) || l->src[l->pos] != '\'') {
        report_error(l->filename, l->src, start_line, start_col,
                    "unterminated character literal");
      }
      l->pos++;
      l->col++;
      char *lexeme = malloc(2);
      lexeme[0] = value;
      lexeme[1] = '\0';
      tok->type = T_CHARLIT;
      tok->lexeme = lexeme;
      tok->line = start_line;
      tok->col = start_col;
      return tok;
    }

    // Identifying numbers (integers and floats)
    if (isDigit(l->src[l->pos])) {
      int start_col = l->col;
      int start_line = l->line;
      int start_pos = l->pos;
      int has_dot = 0;
      char *buffer = malloc(sizeof(char) * 64);
      int buf_index = 0;
      while (l->pos < strlen(l->src) && ((isDigit(l->src[l->pos]) || l->src[l->pos] == '.')) && !isSpace(l->src[l->pos])) {
        if (l->src[l->pos] == '.') {
          if (has_dot) {
            report_error(l->filename, l->src, start_line, start_col,
                        "invalid number format");
          }
          has_dot = 1;
        }
        buffer[buf_index++] = l->src[l->pos];
        l->pos++;
        l->col++;
        if (buf_index >= (int)sizeof(buffer) - 1) {
          char *new_buffer = realloc(buffer, sizeof(char) * (buf_index + 64));
          if (!new_buffer) {
            free(buffer);
            report_error(l->filename, l->src, start_line, start_col,
                        "memory allocation failed");
          }
          buffer = new_buffer;
        }
      }
      if (!isSpace(l->src[l->pos])) {
        l->pos = start_pos;
        l->col = start_col;
        l->line = start_line;
      }
      else {
        buffer[buf_index] = '\0';
        char *value = strdup(buffer);
        tok->type = has_dot ? T_FLOATLIT : T_INTLIT;
        tok->lexeme = value;
        tok->line = start_line;
        tok->col = start_col;
        return tok;

      }
      
    }

    //identify identifiers and keywords
    if (isAlpha(l->src[l->pos]) || l->src[l->pos] == '_') {
      int start_col = l->col;
      int start_line = l->line;
      char *buffer = malloc(sizeof(char) * 256);
      int buf_index = 0;
      while (l->pos < strlen(l->src) && (isAlphaNumeric(l->src[l->pos]) || l->src[l->pos] == '_')) {
        buffer[buf_index++] = l->src[l->pos];
        l->pos++;
        l->col++;
        if (buf_index >= (int)sizeof(buffer) - 1) {
          char *new_buffer = realloc(buffer, sizeof(char) * (buf_index + 256));
          if (!new_buffer) {
            free(buffer);
            report_error(l->filename, l->src, start_line, start_col,
                        "memory allocation failed");
          }
          buffer = new_buffer;
        }
      }
      buffer[buf_index] = '\0';
      char *value = strdup(buffer);
      // Check for keywords
      TokenType type;
      if (strcmp(value, "int") == 0) type = T_INT;
      else if (strcmp(value, "float") == 0) type = T_FLOAT;
      else if (strcmp(value, "string") == 0) type = T_STRING;
      else if (strcmp(value, "char") == 0) type = T_CHAR;
      else if (strcmp(value, "bool") == 0) type = T_BOOL;
      else if (strcmp(value, "void") == 0) type = T_VOID;
      else if (strcmp(value, "true") == 0 || strcmp(value, "false") == 0) type = T_BOOLLIT;
      else if (strcmp(value, "null") == 0) type = T_NULL;
      else if (strcmp(value, "fn") == 0) type = T_FUNCTION;
      else if (strcmp(value, "return") == 0) type = T_RETURN;
      else if (strcmp(value, "if") == 0) type = T_IF;
      else if (strcmp(value, "else") == 0) type = T_ELSE;
      else if (strcmp(value, "while") == 0) type = T_WHILE;
      else if (strcmp(value, "do") == 0) type = T_DO;
      else if (strcmp(value, "for") == 0) type = T_FOR;
      else if (strcmp(value, "print") == 0) type = T_PRINT;
      else type = T_IDENTIFIER;
      tok->type = type;
      tok->lexeme = value;
      tok->line = start_line;
      tok->col = start_col;
      return tok;
    }
    
    // Identifying operators and punctuation
    int col = l->col;
    int line = l->line;
    // printf("Current char: '%c'\n", l->src[l->pos]);
    // if(l->src[l->pos] == '"') {
    //   // printf("Found quote character\n");
    //   l->pos++;
    //   l->col++;
    //   tok->type = T_QUOTE;
    //   tok->lexeme = "\"";
    //   tok->line = line;
    //   tok->col = col;
    //   // print_token(tok);
    //   return tok;
    // }
    if (l->src[l->pos] == '=' && l->src[l->pos + 1] == '=') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_EQ;
      tok->lexeme = "==";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '!' && l->src[l->pos + 1] == '=') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_NEQ;
      tok->lexeme = "!=";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '+' && l->src[l->pos + 1] == '=') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_PLUSASS;
      tok->lexeme = "++";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '-' && l->src[l->pos + 1] == '=') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_MINUSASS;
      tok->lexeme = "--";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '*' && l->src[l->pos + 1] == '=') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_MULTASS;
      tok->lexeme = "*=";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '/' && l->src[l->pos + 1] == '=') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_DIVASS;
      tok->lexeme = "/=";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '%' && l->src[l->pos + 1] == '=') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_MODASS;
      tok->lexeme = "%="; 
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '&' && l->src[l->pos + 1] == '=') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_BITANDASS;
      tok->lexeme = "&=";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '|' && l->src[l->pos + 1] == '=') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_BITORASS;
      tok->lexeme = "|=";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '^' && l->src[l->pos + 1] == '=') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_BITXORASS;
      tok->lexeme = "^=";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '<' && l->src[l->pos + 1] == '<' && l->src[l->pos + 2] == '=') {
      l->pos += 3;
      l->col += 3;
      tok->type = T_LSHIFTASS;
      tok->lexeme = "<<=";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '>' && l->src[l->pos + 1] == '>' && l->src[l->pos + 2] == '=') {
      l->pos += 3;
      l->col += 3;  
      tok->type = T_RSHIFTASS;
      tok->lexeme = ">>=";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '<' && l->src[l->pos + 1] == '=') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_LE;
      tok->lexeme = "<=";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '>' && l->src[l->pos + 1] == '=') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_GE;
      tok->lexeme = ">=";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '<' && l->src[l->pos + 1] == '<') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_LSHIFT;
      tok->lexeme = "<<";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '>' && l->src[l->pos + 1] == '>') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_RSHIFT;
      tok->lexeme = ">>";
      tok->line = line;
      tok->col = col; 
      return tok;
    }
    if (l->src[l->pos] == '+' && l->src[l->pos + 1] == '+') {
      l->pos+=2;
      l->col+=2;
      tok->type = T_PLUSASS;
      tok->lexeme = "^";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '-' && l->src[l->pos + 1] == '-') {
      l->pos+=2;
      l->col+=2;
      tok->type = T_MINUSMINUS;
      tok->lexeme = "--";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '+') {
      l->pos++;
      l->col++;
      tok->type = T_PLUS;
      tok->lexeme = "+";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '-') {
      l->pos++;
      l->col++;
      tok->type = T_MINUS;
      tok->lexeme = "-";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '*') {
      l->pos++;
      l->col++;
      tok->type = T_MULT;
      tok->lexeme = "*";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '/') {
      l->pos++;
      l->col++;
      tok->type = T_DIV;
      tok->lexeme = "/";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '%') {
      l->pos++;
      l->col++;
      tok->type = T_MOD;
      tok->lexeme = "%";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '=') {
      l->pos++;
      l->col++;
      tok->type = T_ASSIGN;
      tok->lexeme = "=";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '(') {
      l->pos++;
      l->col++;
      tok->type = T_LPAREN;
      tok->lexeme = "(";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == ')') {
      l->pos++;
      l->col++;
      tok->type = T_RPAREN;
      tok->lexeme = ")";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '[') {
      l->pos++;
      l->col++;
      tok->type = T_LBRACKET;
      tok->lexeme = "[";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == ']') {
      l->pos++;
      l->col++;
      tok->type = T_RBRACKET;
      tok->lexeme = "]";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    // printf("Current char: '%c'\n", l->src[l->pos]);
    if (l->src[l->pos] == '{') {
      l->pos++;
      l->col++;
      tok->type = T_LBRACE;
      tok->lexeme = "{";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '}') {
      l->pos++;
      l->col++;
      tok->type = T_RBRACE;
      tok->lexeme = "}";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == ';') {
      l->pos++;
      l->col++;
      tok->type = T_SEMICOLON;
      tok->lexeme = ";";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == ',') {
      l->pos++;
      l->col++;
      tok->type = T_COMMA;
      tok->lexeme = ",";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '<') {
      l->pos++;
      l->col++;
      tok->type = T_LT;
      tok->lexeme = "<";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '>') {
      l->pos++;
      l->col++;
      tok->type = T_GT;
      tok->lexeme = ">";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '&' && l->src[l->pos + 1] == '&') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_AND;
      tok->lexeme = "&&";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '|' && l->src[l->pos + 1] == '|') {
      l->pos += 2;
      l->col += 2;
      tok->type = T_OR;
      tok->lexeme = "||";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '!') {
      l->pos++;
      l->col++;
      tok->type = T_NOT;
      tok->lexeme = "!";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '&') {
      l->pos++;
      l->col++;
      tok->type = T_BITAND;
      tok->lexeme = "&";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '|') {
      l->pos++;
      l->col++;
      tok->type = T_BITOR;
      tok->lexeme = "|";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '^') {
      l->pos++;
      l->col++;
      tok->type = T_BITXOR;
      tok->lexeme = "^";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '~') {
      l->pos++;
      l->col++;
      tok->type = T_BITNOT;
      tok->lexeme = "~";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '.') {
      l->pos++;
      l->col++;
      tok->type = T_DOT;
      tok->lexeme = ".";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == ':') {
      l->pos++;
      l->col++;
      tok->type = T_COLON;
      tok->lexeme = ":";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    if (l->src[l->pos] == '?') {
      l->pos++;
      l->col++;
      tok->type = T_QUESTION;
      tok->lexeme = "?";
      tok->line = line;
      tok->col = col;
      return tok;
    }
    else report_error(l->filename, l->src, l->line, l->col, "unexpected character");

  }

  tok->type = T_EOF;
  tok->lexeme = "EOF";
  tok->line = l->line;
  tok->col = l->col;
  return tok;

} 
    

