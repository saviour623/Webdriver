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

/* Meta-data */
typedef struct {
  size_t cap; /* Capacity */
  size_t used; /* Total used */
  size_t dtype; /* sizeof data Type */
  size_t tmp;
} VEC_metaheader;

enum {
      VEC_MIN_SIZE         = 0x100,
      VEC_ALLOC_SIZE       = 0x01,
      VEC_MEMFILL          = 0x01,
      VEC_REM_OPTMZ        = USHRT_MAX
};



/***********************************************************

 * Methods: MACRO functions

************************************************************/

__STATIC_FORCE_INLINE_F __NONNULL__ _cvtindex size_t(const void *v, size_t i, size_t lt) {
  register size_t _i;

  _i = lt ? (ssize_t)i + VEC_size(v) ? i;
  assert( ((_i > 0) && _i < VEC_size(v)) );

  return _i;
}

__STATIC_FORCE_INLINE_F __WARN_UNUSED__ vec_t VEC_create(const size_t size, const size_t dtype) {
  vec_t vec;

  assert ( dtype );

  mvpgAlloc(&vec, safeMulAdd(dtype, size, GLB_metadtSz), 0);
  VEC_mv2MainBlk(vec);
  VEC_size(vec)  = size;
  VEC_dtype(vec) = dtype;

  return vec;
}
__STATIC_FORCE_INLINE_F __NONNULL__ __WARN_UNUSED__ void *VEC_append(void *va, void *vb) {
  PASS;
}
__STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_shrinkInternal(void *v, size_t shrinkto) {
  void *p;

  /**
     Shrink/Expand the capacity of Vector
   */
  assert((p = mvpgAlloc(&p, shrinkto)) == NULL) /* Unable to Shrink */;

  VEC_size(v) = shrinkto;
  if (shrinkto < VEC_used(v))
    VEC_used(v) = shrinkto;

  __bMulOverflow(VEC_dtype(v), shrinkto, &shrinkto);
  memcpy(p, v, shrinkto);
  VEC_destroy(v);

  return p;
}

__STATIC_FORCE_INLINE_F __NONNULL__ __WARN_UNUSED__ void *VEC_sliceInternal(void **v, size_t b, size_t e) {
  /* Slice items from index b to e, returning a new vector of sliced items */
  void *new __MB_UNUSED__;
  size_t sz __MB_UNUSED__;

  if(! ((b < VEC_used(*v)) && (e < VEC_used(*v)) && (e > b)) )
    return NULL;

  new = VEC_newFrmSize((e - b), VEC_dtype(*v));

  __bMulOverflow(VEC_dtype(*v), (e - b), &sz);
  memcpy(p, (*v + b), e);

  memove(*v + b, *v + e, &sz);
  VEC_used(v) -= (e - b);
  return p;
}

__NONNULL__ void VEC_delInternal(vec_t *vec, ssize_t i) {

  size_t mvby = (VEC_size(*vec) - i - 1) * VEC_dtype(*vec);
  mvby ? memmove(*vec + i, (*vec + i) + 1, mvby) /* Shift memory to left */
    : ((*vec)[i] = (void *)MEMCHAR); /* Last index: reusable */
  VEC_used(*vec)--;
}



/**
 * VEC_NEW
 *
 * (macro: alias -> VEC_create)
 *
 */
#define VEC_new(_size, _type, ...)						\
  VEC_create(MACR_DO_ELSE(_size, 32, _size), MACR_DO_ELSE(sizeof(_type), 0, _type))

#define VEC_newFrmSize(_size, _dsize, ...)					\
  VEC_create(MACR_DO_ELSE(_size, 32, _size), MACR_DO_ELSE(_dsize, 0, _dsize))

#define VEC_type(T) T*

#define VEC_push(V, N)							\
  (									\
  assert((V != NULL) && (VEC_dtype == sizeof(N)),				\
   ((VEC_size(V) < 1) || (VEC_size(V) == VEC_used(V)) ) && VEC_resize(V, 1), \
   ((V)[++VEC_used(V) - 2] = (N))
  )

#define VEC_pop_ni(V)				\
  (\
   assert((V) != NULL && VEC_size(V) > 0),\
   (V)[--VEC_used((V))]		  \
  )

#define VEC_pop_i(V, N, I)					\
  (								\
   (VEC_tmp(V) = (V)[_cvtindex(V, I, (I) < 0)]),		\
    VEC_del(V, VEC_iabs(I)),					\
   VEC_tmp(V)						\
  )

#define VEC_pop(V, ...)\
   MACR_DO_ELSE(VEC_pop_i(V, __VA_ARGS__), VEC_pop_i(V), __VA_ARGS__)

#define VEC_insert(V, N, I)\
  (void)((V)[_cvtindex(V, I, (I) < 0)] = (N))

#define VEC_size(V)\
  ((VEC_metaheader *)VEC_peekblkst(vec))->cap

#define VEC_used(V)\
  ((VEC_metaheader *)VEC_peekblkst(vec))->used

#define VEC_free(V)\
  (VEC_size(V) - VEC_used(V))

#define VEC_del(V, I) VEC_delInternal(V, I)

#define VEC_append(V1, V2)\
   VEC_appendInternal(V1, V2)

#define VEC_slice(V, S, E)\
   VEC_sliceInternal(&V, S, E)

#define VEC_shrink(V, ...) (void)			\
  (\
   (V != NULL) && (\
		   (V) = VEC_shrinkInternal(V, MACR_DO_ELSE(__VA_ARGS__, VEC_used(V), __VA_ARGS__))\
		   )							\
   )

#define VEC_destroy(V)\
  (void)(V != NULL ? free(VEC_mv2blkst(V)), (V = NULL) : (void)0)

#define VEC_foreach(V, ...)\
   /* NOT IMPLEMENTED */


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

  *ptr = nalignedPtr = NULL;
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

#ifdef EXIT_ON_MEM_ERR
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
#define __bMulOverflow(a, b, c) !( !(((a) >>(LONG_BIT>>1)) || ((b) >> (LONG_BIT>>1))) && ((*(c) = a * b), true)
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
