#include <limits.h>
#include <stdio.h>









typedef enum {EN = 1, OP} eop;
int main(void) {

  int EN, OP;

  1 ? (EN = 5, OP = 10) : 0;

  printf("%d\n", EN);
}
