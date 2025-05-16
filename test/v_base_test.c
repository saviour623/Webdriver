#include "../v_base.h"
#include <stdio.h>

int main(void) {
  size_t x;
  int *intArray;
  VEC_set config = {.type = 1 , .memfill = 0};
  int **vec;

  vec = (void *)VEC_new(131072, config);

  int data[][6] = {
				   {0, 2, 4, 6, 8, 10}, {1, 3, 5, 7, 9, 11},
				   {2, 3, 5, 7, 11, 13}, {4, 16, 32, 64, 128},
  };

  VEC_delete((void *)&vec);
  return 0;
}
