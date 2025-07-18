#include <limits.h>
#include <stdio.h>









typedef enum {EN = 1, OP} eop;

typedef struct abc {int i;}
int main(void) {
  const abc k;
  k.i = 0;
  int EN, OP;

  1 ? (EN = 5, OP = 10) : 0;

  printf("%d\n", EN);
}
