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
  TK_RETURN,   // return
  TK_IF,       // if
  TK_FOR,      // for
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
  ND_RETURN, // return
  ND_IF,     // if
  ND_FOR,    // for
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;        // left
  Node *rhs;        // right
  int val;          // use only kind=ND_NUM
  int offset;       // use only kind=ND_LVAR
  Node *cond;       // use kind=ND_IF, ND_FOR
  Node *block;      // use kind=ND_IF, ND_FOR
  Node *else_block; // use kind=ND_IF
  Node *init;       // use kind=ND_FOR
  Node *change;     // use kind=ND_FOR
};

typedef struct LVar LVar;
struct LVar {
  LVar *next; // next var or null
  char *name; // var name
  int len;    // name length
  int offset; // offset from rbp
};

extern char *user_input;
extern Token *token;
extern Node *code[100];
extern LVar *locals;

void tokenize(char *p);
void parse();
void gen();

void error_at(char *loc, char *fmt, ...);
