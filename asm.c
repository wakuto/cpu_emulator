#include<stdio.h>
#include<stdlib.h>
#include"asm.h"

bool is_cmd(char *str) {
  u32 cmd_count = 2;
  char *cmd[] = {
    "add",
    "sub"
  };

  u32 i = 0;
  for(i = 0; i < cmd_count; i++) {
    char *s = cmd[i];
    u32 idx = 0;
    bool flag = 1;
    while(s[idx]) {
      if(s[idx] != str[idx]) {
        flag = 0;
        break;
      }
    }
    if(flag) {
      return idx;
    }
  }
  return 0;
}

bool is_digit(char *str) {
  
}

Token *new_token(Token_type type, Token *cur, char *str) {
  Token *tok = (Token*)calloc(1, sizeof(Token));
  tok->type = type;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token *tokenize(char *str) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*str) {
    // スペースの読み飛ばし
    if(*str == ' ')
      str++;

    u32 len = 0;
    if(len = is_cmd(str)) {
      cur = new_token(TK_INST, cur, str);
      str += len;
    }
  }
  return head.next;
}

// 行を単語ごとに分割して返す
strarr split_words(char *str) {
  while(*str) {
    char *tok = str;
    while(*str && 1);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "input error.\n");
    exit(1);
  }
  FILE *fp = fopen(argv[1], "rb");

  char buf[512];
  while(fgets(buf, 512, fp) != NULL) {
    char *str = buf;
    printf("%s", buf);
  }
  return 0;
}
