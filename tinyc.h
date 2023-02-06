#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TK_RESERVED, // Symbol(+, -)
  TK_IDENT,    // Identifier
  TK_NUM,      // Number
  TK_EOF,      // Enf Of File
} TokenKind;

typedef struct Token Token;
struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

typedef enum {
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // -
  ND_EQ,     // ==
  ND_NE,     // !=
  ND_LT,     // <
  ND_LE,     // <=
  ND_NUM,    // Number
  ND_ASSIGN, // =
  ND_LVAR,   // Local variant
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;  // left
  Node *rhs;  // right
  int val;    // use only kind=ND_NUM
  int offset; // use only kind=ND_LVAR
};

extern char *user_input;
extern Token *token;
extern Node *code[100];

void tokenize(char *p);
void parse();
void gen(Node *node);

void error_at(char *loc, char *fmt, ...);