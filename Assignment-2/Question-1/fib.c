/*
 * No changes are allowed in this file
 */
#include <stdio.h>
int fib(int n) {
  if (n < 2)
    return n;
  else
    return fib(n - 1) + fib(n - 2);
}

int main() {
  int val = fib(60);
  printf("%d", val);

  return 0;
}