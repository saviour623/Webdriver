#define VEC_INTERNAL_IMPLEMENTATION
#include "../v_base.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
  size_t x;
  int *intArray;
  VEC_set config = {.type = 1 , .memfill = 0};
  int **vec;


  vec = (void *)VEC_create(2456, config);
  return 0;
}
