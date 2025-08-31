#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

bool isAlpha(char c);
bool isDigit(char c);
bool isAlphaNumeric(char c);
bool isSpace(char c);
char *read_file(const char *filename);
void report_error(const char *filename, const char *src, int line, int col, const char *msg);

#endif
