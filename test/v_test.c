
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include "../v_base.h"

VEC_type(int) vecUsageFuncAdd(VEC_type(int) v, int i) {
  VEC_push(v, i);

  return v;

}

VEC_refType(int) vecUsageFuncRefAdd(VEC_refType(int) v, int i) {
  VEC_push(*v, i);

  return v;
}

int main(void) {
  /* VECTOR TEST */

  VEC_type(int) v = VEC_new(32, int);

  puti(VEC_size(v));

  puti(VEC_sizeof(v));

  puti(VEC_used(v));

  VEC_push(v, 5);

  puti(v[0]);

  vecUsageFuncAdd(v, 6);

  puti(v[1]);

  vecUsageFuncRefAdd(VEC_base(v), 12);

  puti(v[2]);

  VEC_pop(v);

  puti(VEC_back(v));

  VEC_destroy(v);

  return 0;
}
