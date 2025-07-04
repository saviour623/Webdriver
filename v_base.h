#ifndef V_BASE_H
#define V_BASE_H
#define MVPG_ALLOC_MEMALIGN 32

#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdalign.h>
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
#define __STATIC_FORCE_INLINE_F static __inline__ __FORCE_INLINE__

#if defined(__GNUC__) || defined(__clang__)
#define __MAY_ALIAS__ __attribute__((may_alias))
#define __MB_UNUSED__ __attribute__((unused))
#define __WARN_UNUSED__ __attribute__ ((warn_unused_result))
#else
#define __MAY_ALIAS__
#define __MB_UNUSED__
#define __WARN_UNUSED__
#endif

#define throwError(...) puts(__VA_ARGS__ "\n")
#define puti(i) printf("%lld\n", (long long int)(i))
#define PASS (void)0

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
/* If ... is empty do true else do false */
#define MACR_DO_ELSE(_true, _false, ...) MACR_CAT(MACR_IF_EMPTY_, MACR_NON_EMPTY(__VA_ARGS__))(_true, _false)

/***********************************************************

 * PROTOTYPES

************************************************************/

typedef void ** vec_t;
typedef uint8_t byte;

typedef struct {
  uint16_t alignSize;
  uint8_t  native, type, memfill;
} VEC_set;

__WARN_UNUSED__ vec_t  VEC_create(const size_t, const size_t);
__NONNULL__ void   VEC_add(vec_t *, void *, size_t, size_t);
__NONNULL__ size_t VEC_getsize(const vec_t);
__NONNULL__ void   VEC_remove(vec_t *, ssize_t);
__NONNULL__ void * VEC_request(vec_t, ssize_t);
__NONNULL__ void * VEC_delete(vec_t *);
__NONNULL__ bool   VEC_free(void *);
__STATIC_FORCE_INLINE_F __NONNULL__ vec_t VEC_findNextNonEmpty(vec_t, size_t);
__STATIC_FORCE_INLINE_F __NONNULL__ vec_t VEC_get(vec_t, ssize_t);
/***********************************************************

 * MACRO function variants

************************************************************/
__STATIC_FORCE_INLINE_F _assertV size_t(const void *v, size_t i, size_t lt) {
  register size_t _i;

  _i = lt ? (ssize_t)i + VEC_size(v) ? i;
  assert( (v != NULL) && ((_i > 0) && _i < VEC_size(v)) );

  return _i;
}

/**
 * VEC_NEW
 *
 * (macro: alias -> VEC_create)
 *
 */
#define VEC_new(_size, _type)										\
  VEC_create(MACR_DO_ELSE(_size, 32, _size), MACR_DO_ELSE(sizeof(_type), 0, _type))

#define VEC_type(T) T*

#define VEC_push(V, N)							\
  (									\
   assert(V),								\
   (VEC_size(V) < 1 && VEC_resize(V, 1),				\
   ((V)[VEC_size(V) - 1] = (N))						\
									)

#define VEC_pop(V) \
  (\
   assert((V) != NULL && VEC_size(V) > 0),\
   (V)[VEC_size((V))--]			  \
  )

#define VEC_popIndex(V, N, I)					\
  (								\
   assertV(V, I),						\
   (VEC_result(V) = (V)[_assertV(V, I, (I) < 0)]),		\
   VEC_clean((V) + VEC_iabs(I)),				\
   VEC_result(V)						\
  )
#define VEC_insert(V, N, I)



/***********************************************************

 * ALLOCATOR

************************************************************/


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
#define     MOD2(n, m) ((n) & ((m) - 1)) /* n % m (m is a power of 2) */
#define   MODP2(n, p2) mod2(n, 1ULL << p2) /* N % 2^p2 */
#define PRVMULP2(n, m) ((n) - ((n) & ((m) - 1))) /* (multiple of 2^m) < n */
#define   NXTMUL(n, m) (((n) + ((m) - 1)) & ~((m) - 1)) /* {(multiple m) >= n (m is a power of 2)} */
#define NXTMULP2(n, m) ((((n) >> m) + 1) << m) /* {n < (multiple of 2^m) > n} */


#ifdef VEC_INTERNAL_IMPLEMENTATION
/* methods visible only to internal implementation */


#if (_POSIX_C_SOURCE >= 200112L) || (_DEFAULT_SOURCE || _BSD_SOURCE || (XOPEN_SOURCE >= 500))
#define MvpgMalloc(memptr, size) posix_memalign(memptr, MVPG_ALLOC_MEMALIGN, size)
/* C11 introduced a standard aligned_alloc function */
#elif defined(__STDC__) && (__STDC_VERSION >= 201112L)
#if defined(_MSC_VER) || defined(_WIN32)
#define MvpgMalloc(memptr, size) !(*memptr && (memptr = _aligned_malloc(MVPG_ALLOC_MEMALIGN, size))) /* requires malloc.h */
#define free(memptr) _aligned_free(memptr) /* memory can’t be freed with malloc’s free() */
#else
/*__clang__ and __GNUC__ */
#define MvpgMalloc(memptr, size) !(*memptr && (memptr = aligned_alloc(MVPG_ALLOC_MEMALIGN, size))) /* TODO: size must be multiple of alignment  */
#endif
#else
/* MANUAL MEMALIGN */

#if !defined(UINTPTR_MAX)
     typedef unsigned long int uintptr_t
#endif
typedef uint16_t offset_t;
#define OFFSET_SZ 2

/* offset + fault (catering for worst cases where returned memory + offset may be unaligned)*/
#define MAX_ALIGN_OFFSET_SZ  (OFFSET_SZ + (MVPG_ALLOC_MEMALIGN - 1))
/* Round n up to next multiple of boundary */
#define ALIGN_UP_MEMALIGN(n) NXTMUL(n, MVPG_ALLOC_MEMALIGN)
/* offset is stored just before the aligned memory address */
#define MEM_OFFSET_LOC(ptr) ((offset_t *)ptr - 1)
/* Move to the initial unaligned (returned by malloc) memory */
#define MV2_INIT_ALLOC(ptr) (void *)((uintptr_t)ptr - MEM_OFFSET_LOC(ptr)[0])

/**********************************************************************************************/

__WARN_UNUSED__ __NONNULL__ void *NativeAlignedAlloc(void **ptr, size_t size){
  /**
     Align memory to MVPG_ALLOC_MEMALIGN boundary
   */
  void *nalignedPtr;

  *ptr = nalignedptr = NULL;
  if ( (nalignedPtr = malloc(size + MAX_ALIGN_OFFSET_SZ)) ) {
    /* Align address to required boundary, aligning from the address ahead of the offset
     */
    *ptr = (void *)ALIGN_UP_MEMALIGN((uintptr_t)nalignedPtr + MAX_ALIGN_OFFSET_SZ);
    /* store the offset + fault_offset at pre-offset location before aligned memory (header) */
    MEM_OFFSET_LOC(*ptr)[0] = (uintptr_t)*ptr - (uintptr_t)nalignedPtr;
  }
  return *ptr;
}

__NONNULL__ void NativeAlignedFree(void *ptr) {
  /* Free aligned block */

  free(MV2_INIT_ALLOC(ptr));
}

/************************************************************************************************/

/* Use native as fallback */
#define MvpgMalloc(memptr, size) NativeAlignedAlloc(&memptr, MVPG_ALLOC_MEMALIGN, size)
#define free(memptr) NativeAlignedFree(memptr)
#endif

#define MEMCHAR UINT_MAX

__NONNULL__ static void *mvpgAlloc(void *memptr, size_t size, size_t offset) {
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
  memset(*memAllocPtr, MEMCHAR, size); /* clear memory */
  *memAllocPtr += offset; /* offset */

  return *memAllocPtr;
}

/**
 * MEMSET
 * MEMCPY
 */
typedef struct {
  _Alignas(32) unsigned char p[32];
} _InternalImplMemAlignBuf;

#define _InternalMemAlignBufFill(fill) (long)(fill)
#define prvMul32(n) PRVMULP2(n, 5)
#define mod32(n) MOD2(n, 32)

__NONNULL__ static __inline__ __FORCE_INLINE__ void *internalMemZero32Align(void *memptr, size_t size) {
  size_t split32 __MB_UNUSED__;
  size_t overflow32 __MB_UNUSED__;
  _InternalImplMemAlignBuf *ptr __MAY_ALIAS__;

  unsigned char fillChar = (unsigned char)0;

  _InternalImplMemAlignBuf fillBuf = {0};

  if ((uintptr_t)memptr & 31) {
	return memset(memptr, 0, size);
  }
  split32    = (size & 31) ? prvMul32(size) : size >> 5;
  overflow32 = mod32(size);
  ptr        = memptr;

  while ( --split32 ) {
	*ptr++ = fillBuf;
  }
  /* cleanup */
  while (overflow32--)
	*(char *)ptr++ = fillChar;

  return memptr;
}

/***********************************************************

 * SAFE INTEGER ARITHMETIC

************************************************************/


/*
 * SAFE_MUL_ADD
 */
#if defined(__GNUC__) || defined(__clang__)
#define __bMulOverflow(a, b, c) __builtin_mul_overflow(a, b, c)
#define __bAddOverflow(a, b, c) __builtin_add_overflow(a, b, c)
#elif defined(_MSC_VER) || defined(_WIN32)
/* WINDOWS KENRNEL API FOR SAFE ARITHMETIC */
#include <ntintsafe.h>
#define __bAddOverflow(a, b, c) (RtlLongAdd(a, b, c) == STATUS_INTEGER_OVERFLOW)
#define __bMulOverflow(a, b, c) (RtlLongMul(a, b, c) == STATUS_INTEGER_OVERFLOW)
#else
#define __bAddOverflow(a, b, c) !( ((a) < (ULONG_MAX - (b)))) && (*(c) = (a) + (b))
#define __bMulOverflow(a, b, c) !( !(((a) >>(LONG_BIT>>1)) || ((b) >> (LONG_BIT>>1))))
#endif

static __inline__ __FORCE_INLINE__ long safeMulAdd(unsigned long a, unsigned long b, unsigned long c) {
  unsigned long res;

  assert(!__bMulOverflow(a, b, &res) && !__bAddOverflow(res, c, &res)); /* TODO: Handle failed assertion */
  return res;
}


#endif /* VEC_INTERNAL_IMPLEMENTATION */

#endif /* V_BASE_H */

/*

[2, 3, 4, 5, 6, 7]
 rem 1, 4
[2, 3, ]

 */
