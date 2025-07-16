
#include <stdio.h>
#include <time.h>
#include <limits.h>

#include "../include.h"
#include "../v_base.h"

VEC_type(int) vecUsageFuncAdd(VEC_type(int) v, int i) {
  VEC_push(v, i);

  return v;

}

VEC_refType(int) vecUsageFuncRefAdd(VEC_refType(int) v, int i) {
  VEC_push(*v, i);

  return v;
}

typedef struct {
  long int z;
  long long int y;
  int x;
} dtype;

typedef struct {
  union {
    int x1;
  };
  union {
    int x;
  };
} dtype;

int main(void) {
  /* VECTOR TEST */

  exit(0);
  VEC_type(int) v = VEC_new(32, int);

  puti(VEC_size(v));

  puti(VEC_sizeof(v));

  puti(VEC_used(v));

  puti(VEC_back(v));

  VEC_push(v, 5);

  puti(VEC_back(v));

  vecUsageFuncAdd(v, 6);

  puti(VEC_back(v));

  vecUsageFuncRefAdd(VEC_base(v), 12);

  puti(VEC_back(v));

  VEC_pop(v);

  puti(VEC_back(v));

  VEC_shrink(v);

  puti(VEC_size(v));

  VEC_destroy(v);

  return 0;
}
