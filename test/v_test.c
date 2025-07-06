
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include "../v_base.h"

int main(void) {
  VEC_type(int) v = VEC_new(32, int);

  puti(VEC_size(v));

  puti(VEC_sizeof(v));

  puti(VEC_used(v));

  VEC_push(v, 5);

  /* printf("value at (<object: %p> <size: %ld) -> %d\n", vec, (long int)VEC_getsize((void *)vec),*v); */
  return 0;
}
