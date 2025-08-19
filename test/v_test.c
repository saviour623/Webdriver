
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

int main(void) {
  /* VECTOR TEST */

  char buffer[3024] = {0};
  Pp_Setup setup = {0};


  VEC_type(int) v = VEC_new(2046, int);

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

  puti(VEC_size(v));

  setup.Pp_buf = buffer;
  setup.Pp_size  = 1024;
  setup.Pp_fmt = "d";

  //VEC_INTERNAL_repr(v, &setup);

  for (int i = 0; i < 1025; i++) {
    VEC_push(v, i);
  }
  puti(VEC_pop(v));

  //for (i < )
  VEC_Repr(v, &setup);
  puts(buffer);

  puti(setup.Pp_size);
  //VEC_shrink(v);

  VEC_destroy(v);

  return 0;
}
