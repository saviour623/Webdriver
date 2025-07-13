#include <limits.h>
#include <stdio.h>
struct abc;
int main(void) {
  int c = 1;
  switch (c) {
  case 1:
    c = 5;
  case 2:
    puts("true");
    break;
  case 8:
    puts("true");
  }
}
