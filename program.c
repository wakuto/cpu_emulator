int fibonacci(int n) {
  if(n == 0 || n == 1)
    return n;
  else
    return fibonacci(n - 2) + fibonacci(n - 1);
}
int main(void) {
  int *x = (int*)512;
  *x = fibonacci(6);
  for(;;) {}
  return 0;
}
