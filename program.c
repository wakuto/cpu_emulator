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
  volatile char *in = (volatile char *)(UART | 0x000);
  char c = *in;
  return c;
}

void getstr(char *buf, int len) {
  int i = 0;
  while(i < len-1) {
    buf[i] = getch();
    switch(buf[i]) {
      case '\n':
      case '\r': {
        putch('\n');
        buf[i] = '\0';
        return;
      }
      case 0x7F:
      case '\b': {
        if(i < 1) i -= 1;
        else {
          i -= 2;
          putstr("\b \b");
        }
        break;
      }
      default: putch(buf[i]); break;
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
