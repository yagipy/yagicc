#include "yagicc.h"

bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

bool consume_number() {
  if (token->kind != TK_NUM)
    return false;
  token = token->next;
  return true;
}

bool consume_ident() {
  if (token->kind != TK_IDENT)
    return false;
  token = token->next;
  return true;
}

void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "not found '%s'", op);
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "not found number");
  int val = token->val;
  token = token->next;
  return val;
}

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  }
  return NULL;
}

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

Node *new_assign(Node *lhs, Node *rhs) {
  Node *node = new_node(ND_ASSIGN);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_var(int offset) {
  Node *node = new_node(ND_LVAR);
  node->offset = offset;
  return node;
}

bool at_eof() { return token->kind == TK_EOF; }

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();

void parse() { program(); }

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node;
  if (token->kind == TK_RETURN) {
    node = new_node(ND_RETURN);
    token = token->next;
    node->lhs = expr();
    expect(";");
  } else if (token->kind == TK_IF) {
    node = new_node(ND_IF);
    token = token->next;
    expect("(");
    node->cond = expr();
    expect(")");
    node->block = stmt();
    if (consume("else")) {
      node->else_block = stmt();
    }
  } else if (token->kind == TK_FOR) {
    node = new_node(ND_FOR);
    token = token->next;
    expect("(");
    if (!consume(";")) {
      node->init = expr();
      expect(";");
    }
    if (!consume(";")) {
      node->cond = expr();
      expect(";");
    }
    if (!consume(")")) {
      node->change = expr();
      expect(")");
    }
    node->block = stmt();
  } else if (token->kind == TK_WHILE) {
    node = new_node(ND_WHILE);
    token = token->next;
    expect("(");
    node->cond = expr();
    expect(")");
    node->block = stmt();
  } else if (consume("{")) {
    Node *head = new_node(ND_BLOCK);
    head->body = NULL;
    Node *cur = head;

    cur->body = new_node(ND_BLOCK);
    cur = cur->body;

    while (!consume("}")) {
      cur->body = stmt();
      cur = cur->body;
    }
    node = head->body;
  } else {
    node = expr();
    expect(";");
  }
  return node;
}

Node *expr() { return assign(); }

Node *assign() {
  Node *node = equality();
  if (consume("=")) {
    node = new_assign(node, assign());
  }
  return node;
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_binary(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_binary(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_binary(ND_LT, node, add());
    else if (consume("<="))
      node = new_binary(ND_LE, node, add());
    else if (consume(">"))
      node = new_binary(ND_LT, add(), node);
    else if (consume(">="))
      node = new_binary(ND_LE, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_binary(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_binary(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_binary(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_binary(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  if (token->kind == TK_NUM) {
    return new_num(expect_number());
  }

  if (token->kind == TK_IDENT) {
    if (token->next->kind == TK_RESERVED && strlen("(") == token->next->len &&
        memcmp(token->next->str, "(", token->next->len) == 0) {
      Node *node = new_node(ND_CALL);
      node->caller = token->str;
      node->caller_len = token->len;
      token = token->next;
      expect("(");
      expect(")");
      return node;
    } else {
      Node *node = new_node(ND_LVAR);
      LVar *var = find_lvar(token);
      if (var) {
        node->offset = var->offset;
      } else {
        LVar *var = calloc(1, sizeof(LVar));
        var->next = locals;
        var->name = token->str;
        var->len = token->len;
        if (!locals) {
          var->offset = 8;
        } else {
          var->offset = locals->offset + 8;
        }
        node->offset = var->offset;
        locals = var;
      }
      token = token->next;
      return node;
    }
  }

  fprintf(stderr, "expected an expr. kind: %u\n", token->kind);
}

Node *unary() {
  if (consume("+"))
    return unary();
  if (consume("-"))
    return new_binary(ND_SUB, new_num(0), unary());
  return primary();
}
