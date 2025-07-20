#include <limits.h>
#include <stdio.h>









typedef enum {EN = 1, OP} eop;

typedef struct {int i;} abc;
int main(void) {
  abc k;
  k.i = 5;

  const abc *p = &k;

  p->i = 1;
  int EN, OP;

  1 ? (EN = 5, OP = 10) : 0;

  printf("%d\n", EN);
}
