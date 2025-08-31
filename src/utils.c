#include <stdio.h>
#include <stdlib.h>
#include "../include/utils.h"

bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isDigit(char c) {
  return c >= '0' && c <= '9';
}

bool isAlphaNumeric(char c) {
  return isAlpha(c) || isDigit(c);
}

bool isSpace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

char *read_file(const char *filename) {
  char *buffer;
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Could not open file");
    exit(EXIT_FAILURE);
  }
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  printf("File length: %ld bytes\n", length);
  rewind(file);
  buffer = malloc(sizeof(char) * (length + 1));
  fread(buffer, 1, length, file);
  printf("Read %ld bytes from file in buffer %s\n", length, buffer);
  buffer[length] = '\0';
  printf("File content loaded into buffer\n");
  fclose(file);
  return buffer;
}

void report_error(const char *filename, const char *src,
                  int line, int col, const char *msg) {
    // Print error header
    fprintf(stderr, "%s:%d:%d: error: %s\n",
            filename ? filename : "<stdin>", line, col, msg);

    // Find line boundaries in source
    int pos = 0, curr_line = 1;
    while (curr_line < line && src[pos] != '\0') {
        if (src[pos] == '\n') curr_line++;
        pos++;
    }

    int line_start = pos;
    while (src[pos] != '\0' && src[pos] != '\n') pos++;
    int line_end = pos;

    // Print the actual source line
    fwrite(src + line_start, 1, line_end - line_start, stderr);
    fputc('\n', stderr);

    // Print caret at column
    for (int i = 1; i < col; i++) fputc(' ', stderr);
    fputc('^', stderr);
    fputc('\n', stderr);

    exit(EXIT_FAILURE); // stop after error
}
