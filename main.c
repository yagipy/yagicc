#include "tinyc.h"

char *user_input;
Token *token;
Node *code[100];
LVar *locals;

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int position = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", position, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

int main(int argc, int **argv) {
  if (argc != 2) {
    fprintf(stderr, "incorrect number of arguments");
    return 1;
  }

  user_input = argv[1];
  tokenize(user_input);
  parse();
  gen();
  return 0;
}
