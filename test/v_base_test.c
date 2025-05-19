#include "../v_base.h"
#include <stdio.h>

int main(void) {
  size_t x;
  int *intArray;
  VEC_set config = {.type = 1 , .memfill = 0};
  int **vec;

  vec = (void *)VEC_new(131072, config);
  VEC_delete((void *)&vec);
  return 0;
}
