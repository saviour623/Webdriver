
#include <stdio.h>
#define P 0
#define P1 1
#define T(...) # __VA_ARGS__

/* #define MACR_EMPTY_PARAM(...) 0 */
/* #define MACR_INDIRECT_EVAL(a, ...) MACR_ACCEPT_FIRST(__VA_ARGS__) */
/* #define MACR_ACCEPT_FIRST(a, ...) a */
/* #define MACR_IGNORE_FIRST(...) MACR_INDIRECT_EVAL(__VA_ARGS__) */
/* #define MACR_NON_EMPTY(...) MACR_IGNORE_FIRST(MACR_EMPTY_PARAM  __VA_ARGS__(), __VA_ARGS__) */


#define MACR_EMPTY_PARAM(...) 0,
#define MACR_INDIRECT_EVAL(a, ...) MACR_ACCEPT_FIRST(__VA_ARGS__)
#define MACR_ACCEPT_FIRST(a, ...) a
#define MACR_IGNORE_FIRST(...) MACR_INDIRECT_EVAL(__VA_ARGS__)
#define MACR_NON_EMPTY(...) MACR_IGNORE_FIRST(MACR_EMPTY_PARAM  __VA_ARGS__(), 1)

#define MACR_IF_EMPTY_0(a, ...) __VA_ARGS__
# /* or */
#define MACR_IF_EMPTY_(a, ...) __VA_ARGS__
#
#define MACR_IF_EMPTY_1(a, ...) a

#define MACR_CAT(A, A1) MACR_INDIRECT_CAT(A, A1)
#define MACR_INDIRECT_CAT(A, A1) A ## A1
#define MACR_DO_ELSE(_true, _false, ...) MACR_CAT(MACR_IF_EMPTY_, MACR_NON_EMPTY(__VA_ARGS__))(_true, _false)

#define VEC_GETMETADATA(vec, ...) MACR_DO_ELSE(__VA_ARGS__,,__VA_ARGS__) (VEC_ACCESS(vec) - 1)[0])
#define VEC_new(size_t_sz, ...)\
  MACR_DO_ELSE(VEC_create(size_t_sz, MACR_DO_ELSE((__VA_ARGS__), NULL, __VA_ARGS__)), (throwError(NULL)), size_t_sz)

int main() {
  VEC_new(0, 1)
	//VEC_create(size_t_sz, MACR_DO_ELSE((__VA_ARGS__), NULL, __VA_ARGS__))
	//VEC_GETMETADATA(v)
  #undef MACR_EMPTY_PARAM
	MACR_DO_ELSE((),,0);
#define MACR_EMPTY_PARAM(...) 0, NULL
  MACR_NON_EMPTY(c);
}
