#define VEC_INTERNAL_IMPLEMENTATION
#include "../v_base.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
  size_t x;
  int *intArray;
  VEC_set config = {.type = 0, .memfill = 0};
  int **vec;
  int *data = (int[30]){1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 24, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
  /**
  vec = (void *)VEC_new(2456, void *);

  for (int i = 0; i < 30; i++) {
   	VEC_add((void *)&vec, data + i, sizeof(void *), i);
  }
  /* VEC_remove((void *)&vec, 28);

  int *v = *(vec + 28);

  printf("value at (<object: %p> <size: %ld) -> %d\n", vec, (long int)VEC_getsize((void *)vec),*v);
  return 0;
*/

  (assert(0), 0);

  char p[-1];

  //VEC_type(int) v = VEC_new(4, int);
}

/*
[1, 1, 1, 1, 1, 0]
 */
