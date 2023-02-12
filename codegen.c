#include "tinyc.h"

void gen_epilogue() {
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    fprintf(stderr, "lvalue is not a var");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen_node(Node *node) {
  switch (node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen_node(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case ND_RETURN:
    gen_node(node->lhs);
    printf("  pop rax\n");
    gen_epilogue();
    return;
  case ND_IF:
    gen_node(node->cond);
    printf(" pop rax\n");
    printf(" cmp rax, 0\n");
    if (node->else_block == NULL) {
      printf("  je .Lend\n");
      gen_node(node->block);
      printf(".Lend:\n");
    } else {
      printf("  je .Lelse\n");
      gen_node(node->block);
      printf("  jmp .Lend\n");
      printf(".Lelse:\n");
      gen_node(node->else_block);
      printf(".Lend:\n");
    }
    return;
  case ND_FOR:
    if (node->init != NULL) {
      gen_node(node->init);
    }
    printf(".Lbegin:\n");
    if (node->cond != NULL) {
      gen_node(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend\n");
    }
    gen_node(node->block);
    if (node->change != NULL) {
      gen_node(node->change);
    }
    printf("  jmp .Lbegin\n");
    printf(".Lend:\n");
    return;
  case ND_WHILE:
    printf(".Lbegin:\n");
    gen_node(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend\n");
    gen_node(node->block);
    printf("  jmp .Lbegin\n");
    printf(".Lend:\n");
    return;
  case ND_BLOCK:
    for (; node->body; node = node->body) {
      gen_node(node->body);
      printf("  pop rax\n");
    }
    return;
  }

  gen_node(node->lhs);
  gen_node(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}

void gen() {
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n"); // 208 = var(8) * alphabet(26)

  for (int i = 0; code[i]; i++) {
    gen_node(code[i]);
    printf("  pop rax\n");
  }

  gen_epilogue();
}
