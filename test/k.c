#include "../include.h"

#define P(...) MvpgMacro_ignore(MvpgMacro_select((__VA_ARGS__), b, __VA_ARGS__))

#define VEC_vused(...) (POPOP).L
#define VEC_shrink(V, ...)						\
  MvpgMacro_ignore(							\
		   (V != NULL) &&					\
		   (							\
		    (V) = VEC_INTERNAL_shrink(V, MvpgMacro_select((__VA_ARGS__), VEC_vused(V), __VA_ARGS__))) \
									)
int main(void) {
VEC_shrink(V)

}
