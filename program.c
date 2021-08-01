#define UART 0x10000000

void putch(char c);
void putstr(char *str);
char getch();
void getstr(char *buf, int len);

void putch(char c) {
  volatile char *out = (volatile char *)UART;
  *out = c;
}

char getch() {
  volatile char *in = (volatile char *)(UART | 0x004);
  char c = *in;
  if(c == '\b' || c == 0x7F) {
    putch('\b');
    putch(' ');
    putch('\b');
    c = '\b';
  } else putch(c);
  return c;
}

void getstr(char *buf, int len) {
  int i = 0;
  while(i < len-1) {
    buf[i] = getch();
    switch(buf[i]) {
      case '\n':
      case '\r': {
        buf[i] = '\0';
        return;
      }
      case '\b': {
        i -= 2;
        break;
      }
      default: break;
    }
    i++;
  }
  buf[len-1] = '\0';
}

void putstr(char *str) {
  int i = 0;
  for(i = 0; str[i] != 0; i++) {
    putch(str[i]);
  }
}

void putstrn(char *str) {
  putstr(str);
  putch('\n');
}

int main(void) {
  unsigned int len = 256;
  char buf[len];
  putstr("what's your name? >>> ");
  getstr(buf, len);
  putstr("Hi ");
  putstr(buf);
  putstrn("!");
  for(;;) {}
  return 0;
}
