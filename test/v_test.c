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

  vec = (void *)VEC_new(2456, config);

  for (int i = 0; i < 30; i++) {
	VEC_add((void *)&vec, (int[3]){1, 2, 3}, sizeof(void *), 25);
  }

  int *v = *(vec + 25);

  printf("value at (<object: %p> <size: %ld) -> %d\n", vec, (long int)VEC_getsize((void *)vec),*v);
  return 0;
}
