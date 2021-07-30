#define MMIO 0x100000

void putch(char c) {
  volatile char *out = (char *)MMIO;
  *out = c;
}

void putstr(char *str) {
  int i = 0;
  for(i = 0; str[i] != 0; i++) {
    putch(str[i]);
  }
}

int main(void) {
  putstr("Hello World!\n");
  for(;;) {}
  return 0;
}
