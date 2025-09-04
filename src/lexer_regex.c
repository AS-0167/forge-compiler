#include "../include/lexer_regex.h"

Rule rules[] = {
  {"^[ \t\n\r]+", T_SPACE},
  {"^//[^\n]*", T_COMMENT},
  {"^/\\*([^*]|\\*+[^*/])*\\*+/", T_COMMENT},
//   {"^[0-9]+\\.[0-9]*(?=$|[ \t\n\r;:,(){}\\[\\]\\+\\-\\*/%<>=!&|^])", T_FLOATLIT},
//   {"^[0-9]+(?=$|[ \t\n\r;:,(){}\\[\\]\\+\\-\\*/%<>=!&|^])", T_INTLIT},

//   {"^[0-9]*\\.[0-9]+", T_FLOATLIT},
  {"^[0-9]*\\.[0-9]+([eE][+-]?[0-9]+)?", T_FLOATLIT},
  {"^\\.[0-9]+([eE][+-]?[0-9]+)?", T_FLOATLIT},
  {"^[0-9]+\\.([eE][+-]?[0-9]+)", T_FLOATLIT},
  {"^[0-9]+([eE][+-]?[0-9]+)", T_FLOATLIT},


  {"^[0-9]+", T_INTLIT},

  {"^\"([^\"\\\\]|\\\\[ntr\"\\\\])*\"", T_STRINGLIT},
//   {"^'(?:[^'\\\\]|\\\\[ntr\"'\\\\])'", T_CHARLIT},
  {"^'([^'\\\\]|\\\\[ntr\"'\\\\])'", T_CHARLIT},

  
  {"^int", T_INT},
  {"^float", T_FLOAT},
  {"^string", T_STRING},
  {"^char", T_CHAR},
  {"^bool", T_BOOL},
  {"^void", T_VOID},
  {"^true", T_BOOLLIT},
  {"^false", T_BOOLLIT},
  {"^null", T_NULL},
  {"^fn", T_FUNCTION},
  {"^return", T_RETURN},
  {"^if", T_IF},
  {"^else", T_ELSE},
  {"^while", T_WHILE},
  {"^do", T_DO},
  {"^for", T_FOR},
  {"^print", T_PRINT},
  {"^[A-Za-z_][A-Za-z0-9_]*", T_IDENTIFIER},
  {"^==", T_EQ},
  {"^!=", T_NEQ},
  {"^\\+=", T_PLUSASS},
  {"^-=", T_MINUSASS},
  {"^\\*=", T_MULTASS},
  {"^/=", T_DIVASS},
  {"^%=", T_MODASS},
  {"^&=", T_BITANDASS},
  {"^\\|=", T_BITORASS},
  {"^\\^=", T_BITXORASS},
  {"^<<=", T_LSHIFTASS},
  {"^>>=", T_RSHIFTASS},
  {"^\\+\\+", T_PLUSPLUS},
  {"^--", T_MINUSMINUS},
  {"^<<", T_LSHIFT},
  {"^>>", T_RSHIFT},
  {"^<=", T_LE},
  {"^>=", T_GE},
  {"^&&", T_AND},
  {"^\\|\\|", T_OR},
  {"^&", T_BITAND},
  {"^\\|", T_BITOR},
  {"^\\^", T_BITXOR},
  {"^~", T_BITNOT},
  {"^=", T_ASSIGN},
  {"^<", T_LT},
  {"^>", T_GT},
  {"^!", T_NOT},
  {"^\\+", T_PLUS},
  {"^-", T_MINUS},
  {"^\\*", T_MULT},
  {"^/", T_DIV},
  {"^%", T_MOD},
  {"^\\(", T_LPAREN},
  {"^\\)", T_RPAREN},
  {"^\\[", T_LBRACKET},
  {"^\\]", T_RBRACKET},
  {"^\\{", T_LBRACE},
  {"^\\}", T_RBRACE},
  {"^;", T_SEMICOLON},
  {"^,", T_COMMA},
  {"^\\.", T_DOT},
  {"^:", T_COLON},
  {"^\\?", T_QUESTION},
  {NULL, T_EOF}
};

RegexLexer initRegexLexer(const char *filename, const char *src) {
    RegexLexer l;
    l.filename = filename;
    l.src = src;
    l.pos = 0;
    l.line = 1;
    l.col = 1;
    return l;
}

Token *nextRegexToken(RegexLexer *l) {
    if (l->pos >= strlen(l->src)) {
        Token *eof_token = create_token(T_EOF, "", l->line, l->col);
        return eof_token;
    }

    const char *remaining_src = l->src + l->pos;

    for (int i = 0; rules[i].pattern != NULL; i++) {
        regex_t regex;
        if (regcomp(&regex, rules[i].pattern, REG_EXTENDED) != 0) {
            fprintf(stderr, "Could not compile regex: %s\n", rules[i].pattern);
            exit(EXIT_FAILURE);
        }

        regmatch_t match;
        if (regexec(&regex, remaining_src, 1, &match, 0) == 0) {
            int match_length = match.rm_eo - match.rm_so;
            char *lexeme = strndup(remaining_src + match.rm_so, match_length);

            TokenType type = rules[i].type;
            if(type == T_FLOATLIT || type == T_INTLIT) {
                // Remove trailing dot if present
                if(l->src[l->pos + match_length] == '\0' || isSpace(l->src[l->pos + match_length]) || strchr(";,)}]+-*/%<>=!&|^", l->src[l->pos + match_length])) {
                    
                }
                else {
                    report_error(l->filename, l->src, l->line, l->col, "Invalid numeric literal");
                    free(lexeme);
                    regfree(&regex);
                    return NULL;
                }
            }

            // Update lexer position
            for (int j = 0; j < match_length; j++) {
                if (remaining_src[j] == '\n') {
                    l->line++;
                    l->col = 1;
                } else {
                    l->col++;
                }
            }
            l->pos += match_length;

            regfree(&regex);

            // Skip whitespace tokens
            if (type == T_SPACE) {
                free(lexeme);
                return nextRegexToken(l);
            }

            Token *token = create_token(type, lexeme, l->line, l->col - match_length);
            // free(lexeme);
            return token;
        }

        regfree(&regex);
    }

    // If no rule matched, report an error
    report_error(l->filename, l->src, l->line, l->col, "Unrecognized token");
    return NULL; // This line will never be reached due to exit in report_error
}

    
