#ifndef V_BASE_H
#define V_BASE_H
#define MVPG_ALLOC_MEMALIGN sizeof (void *)

#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <assert.h>

#if defined(__GNUC__) || defined(__clang__)
#define __FORCE_INLINE__ __attribute__((always_inline))
#elif defined(_MSC_VER) || defined(_WIN32) || defined(_win32)
#define __FORCE_INLINE__ __forceinline
#else
#define __FORCE_INLINE__
#endif
#define __NONNULL__ __attribute__((nonnull))
#define CHOOSE_EXPR(cExpr, tVal, fVal) __builtin_choose_expr(cExpr, tVal, fVal)
#define __STATIC_FORCE_INLINE_F static __inline__ __FORCE_INLINE__

#if defined(__GNUC__) || defined(__clang__)
#define __EXPR_LIKELY__(T, L) __builtin_expect(T, L)
#define __MAY_ALIAS__ __attribute__((may_alias))
#define __MB_UNUSED__ __attribute__((unused))
#else
#define __MAY_ALIAS__
#define __EXPR_LIKELY__(T, L) T
#define __MB_UNUSED__
#endif

#define throwError(...) puts(__VA_ARGS__ "\n")
#define puti(i) printf("%lld\n", (long long int)(i))

/**
 * MACR_NON_EMPTY, MACR_DO_ELSE
 *
 * checks if a macro parameter is given an argument, returns argument if non-emppty
 */
#define MACR_EMPTY_PARAM(...) 0, 0
#define MACR_INDIRECT_EVAL(a, ...) MACR_ACCEPT_FIRST(__VA_ARGS__)
#define MACR_ACCEPT_FIRST(a, ...) a
#define MACR_IGNORE_FIRST(...) MACR_INDIRECT_EVAL(__VA_ARGS__)
#define MACR_NON_EMPTY(...) MACR_IGNORE_FIRST(MACR_EMPTY_PARAM  __VA_ARGS__(), 1)
/* empty */
#define MACR_IF_EMPTY_0(a, ...) __VA_ARGS__
/* also empty */
#define MACR_IF_EMPTY_(a, ...) __VA_ARGS__
/* non-empty */
#define MACR_IF_EMPTY_1(a, ...) a

#define MACR_CAT(A, A1) MACR_INDIRECT_CAT(A, A1)
#define MACR_INDIRECT_CAT(A, A1) A ## A1
#define MACR_DO_ELSE(_true, _false, ...) MACR_CAT(MACR_IF_EMPTY_, MACR_NON_EMPTY(__VA_ARGS__))(_true, _false)

/**
 *  PROTOTYPES (func/types)
 */
typedef void ** vec_t;
typedef struct {
  /* feature flags */
  uint16_t alignSize;
  uint8_t  native, type, memfill;
} VEC_set;

vec_t VEC_create(size_t, const VEC_set);
__NONNULL__ void *VEC_add(vec_t *, void *, size_t, size_t);
__NONNULL__ void *VEC_remove(vec_t *, ssize_t);
__NONNULL__ void *VEC_request(vec_t, ssize_t);
__NONNULL__ void *VEC_delete(vec_t *);

/* MACRO variants */
#
#
#
/*
 * VEC_NEW
 *
 * (macro: alias -> VEC_create)
 * Autofill and error-check arguments before call to VEC_create
 */
#define VEC_new(size_t_size, ...)											\
  MACR_DO_ELSE(VEC_create(size_t_size, MACR_DO_ELSE((__VA_ARGS__), 0, __VA_ARGS__)), (throwError(NULL)), size_t_size)

/**
 *  MVPGALLOC - Mvpg Memory Allocator
 *
 * MvpgAlloc actually takes in a ’void **’ memptr due to the prototype of posix_memalign.
 * However, parameter (memptr) is defined as type ’void *’. This is only to avoid the need
 * for an explicit cast to ’void **’ for pointers of different types on calls to the function.
 *
 * A typical to @MvpgAlloc looks like: mvpgAlloc(&memptr, 8)
 */

/**
 * MOD_p2
 * PREVIOUSMULTIPLE_p2
 * NEXTMULTIPLE_p2
 */
#define modp2(n, p2)     ((n) & ((1ULL << (p2)) - 1)) /* N % 2^p2 */
#define mod256(n)        ((n) & 255) /* N % 256 */
#define prvMulp2(n, p2)  ((n) - modp2(n, p2)) /* multiple of 2^p2 less than N */
#define prvMul32(n)      ((n) - ((n) & 31)) /* multiple of 32, < N */
#define prvpMulp2(n, m) ((n) - ((n) & ((m) - 1))) /* multiple of m (power of 2),  < N */
#define nxtMulp2(n, m)   ((((n) >> m) + 1) << m) /* multiple of m (power of 2), > N */

/**
 * INTERNAL API
 */
#ifdef VEC_INTERNAL_IMPLEMENTATION
#if (_POSIX_C_SOURCE >= 200112L) || (_DEFAULT_SOURCE || _BSD_SOURCE || (XOPEN_SOURCE >= 500))
#define MvpgMalloc(memptr, size) posix_memalign(memptr, MVPG_ALLOC_MEMALIGN, size)
/* C11 introduced a standard aligned_alloc function */
#elif defined(__STDC__) && (__STDC_VERSION >= 201112L)
#if defined(_MSC_VER) || defined(_WIN32)
#define MvpgMalloc(memptr, size) !(*memptr && (memptr = _aligned_malloc(MVPG_ALLOC_MEMALIGN, size))) /* requires malloc.h */
#define free(memptr) _aligned_free(memptr) /* memory can’t be freed with malloc’s free()
											  #else */
/*__clang__ and __GNUC__ */
#define MvpgMalloc(memptr, size) !(*memptr && (memptr = aligned_alloc(MVPG_ALLOC_MEMALIGN, size))) /* TODO: size must be multiple of alignment  */
#endif
#else
/* fallback to malloc */
#define MvpgMalloc(*memptr, size) malloc(size) /* TODO: perform manual alignment */
#define free(memptr) free(memptr) /*TODO: perform manual free */
#endif

__NONNULL__ static void *mvpgAlloc(void *memptr, size_t size) {
  void **memAllocPtr;

  memAllocPtr = memptr;

  assert(size != 0);

  if ( MvpgMalloc(memAllocPtr, size) ) {
	/* ERROR */
	fprintf(stderr, "mvpgAlloc: allocation of size %lu failed (%s)\n", (long)size, strerror(errno));

#ifdef EXIT_ON_MEMERR
	exit(EXIT_FAILURE);
#else
	return NULL;
#endif
  }
  /* zero memory */
  memset(*memAllocPtr, 0, size);
  return *memAllocPtr;
}

typedef struct {
#if defined(__STDC__) && (__STDC_VERSION >= 201112L)
	_Alignas(32) char alignv;
#else
  char alignv [32];
#endif
} _internalImplementationMemAlign;
/**
 * MEMSET
 */
__NONNULL__ static void *memset(void *memptr, size_t fill, size_t size) {
}
#endif /* VEC_INTERNAL_IMPLEMENTATION */

#endif
)

