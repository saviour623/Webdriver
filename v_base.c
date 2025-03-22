#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "v_base.h"

/**
 *                       Vector
 *
 * <[ptr] --------------------+
 *                            |
 *                            +
 * [size]<->[meta-data]<->[Block 1]...[Block N]
 * |              |           |
 * +> BLOCK-START |           +> [Block 1] - meta_data_size --> meta-data
 *                |
 *                +> [meta-data] & 0x0f --> BLOCK-START
 *
 *
 * <[size]
 *      min: 1B, max: sizeof (size_t) or 8
 *
 * <[meta-data: 1B]
 *
 *      1      1      1      1      1      1      1      1
 *      |      |      |      |______|      |_____________|
 *      |      |      |         |                 |
 *    (type)   |  ( misc. bits) |                 |
 *             |                |                 |
 *         (member type)     (misc. bits)     (sizeof size: 0x0f)
 *
 * <[sizeof size] -> byte count.
 *                   It doesn't interpret to be the exact number of bytes that can fit size,
 *                   rather it is the number of shifts of 4 (bits), that can fit the value of size.
 *
 * MIN: 0x01/0b001, MAX: 0x0f/0b111
 *
 * IN BITS: 4 << 1  -> 8B  (1 byte)
 *        4 << 10 -> 16B (2 byte)
 *        4 << 11 -> 32B (4 byte)
 *
 * IN BYTES: 1 << (N - 1)
 *                  ...
 *
 * <[member type] -> this is only set if vector's type is an object.
 *
 */
/* Support for vector-remove optimization */
#define VEC_TRACE_DEL
/* Allow size-bound check on request */
#define VEC_SAFE_REQUEST 1
/* Allow neccessary warnings */
#define VEC_ALW_WARNING 1

/* NULL */
#define VEC_EOV (char *)(long) 0xff

/**
 * Types
 */
typedef uint64_t VEC_szType;
typedef void ** vec_t;

/**
 * Prototypes
 */
static __NONNULL__ vec_t VEC_append(vec_t *, void *, size_t);
__STATIC_FORCE_INLINE_F bool VEC_free(void *);

/* Meta-data */
typedef struct {
  uint8_t vcapSize;
#ifdef VEC_TRACE_DEL
  void *vcache;
  uint8_t vcacheSize;
#endif
} vecMetaDataHeader;

/* feature flags for vector */
typedef struct {
  uint8_t native; /* Default */
  uint8_t readOnly;
  uint8_t memReadOnly;
  uint8_t recurseSet; /* parent setting is inherited by members */
  uint8_t ntrackSize;
  uint8_t optimize;
} VEC_set;

enum {
	  VEC_ARRAY          = 0,
	  VEC_LEAST_SZ       = 0x01,
	  VEC_ALLOC_SZ       = 0x01,
	  VEC_NON_NATIVE     = 0x18,
	  VEC_NO_TRACK_SIZE  = 0x20,
	  VEC_READONLY       = 0x08,
	  VEC_VECTOR         = 0x80,
	  VEC_EROUT_OF_BOUND = 0x3b
};

/**
 * Global Variables
 */
const uint8_t vecDefFlagLoc         = 1;
const uint8_t vecGblMetaDataSize  = sizeof (vecMetaDataHeader) + vecDefFlagLoc;
const uint8_t vecGblDataBlockSize = sizeof (vec_t);

/**
 * MACR_NON_EMPTY,
 * MACR_DO_ELSE
 *
 * Safely assert if a macro parameter is given an argument
 * Return: provided argument(s)/zero/nothing
 */
#define MACR_EMPTY_PARAM(...) 0, 0
#define MACR_INDIRECT_EVAL(a, ...) MACR_ACCEPT_FIRST(__VA_ARGS__)
#define MACR_ACCEPT_FIRST(a, ...) a
#define MACR_IGNORE_FIRST(...) MACR_INDIRECT_EVAL(__VA_ARGS__)
#define MACR_NON_EMPTY(...) MACR_IGNORE_FIRST(MACR_EMPTY_PARAM  __VA_ARGS__(), 1)
# /* empty */
#define MACR_IF_EMPTY_0(a, ...) __VA_ARGS__
# /* also empty */
#define MACR_IF_EMPTY_(a, ...) __VA_ARGS__
# /* non-empty */
#define MACR_IF_EMPTY_1(a, ...) a
#
#define MACR_CAT(A, A1) MACR_INDIRECT_CAT(A, A1)
#define MACR_INDIRECT_CAT(A, A1) A ## A1
#define MACR_DO_ELSE(_true, _false, ...) MACR_CAT(MACR_IF_EMPTY_, MACR_NON_EMPTY(__VA_ARGS__))(_true, _false)
#
# /* end */
#

/**
 * VEC_getMetaData (lvalue)
 *
 */
#define VEC_getMetaData(vec, ...) (MACR_DO_ELSE(__VA_ARGS__ =,,__VA_ARGS__) (VEC_ACCESS(vec) - vecDefFlagLoc)[0])

#define VEC_peekBlockStart(vec)   (void *)(VEC_ACCESS(vec) - vecGblMetaDataSize)
#define VEC_getMagnitude(vec)     ((vecMetaDataHeader *)VEC_peekBlockStart(vec))->vcapSize
#define VEC_moveToMainBlock(vec)  ((vec) = VEC_ACCESS(vec) + vecGblMetaDataSize)
#define VEC_moveToBlockStart(vec) ((vec) = VEC_peekBlockStart(vec))
#
#define ignoreExprReturn(...) (void) (__VA_ARGS__)
#
/* reinterpret as pointer of bytes */
#define VEC_ACCESS(_addr) ((uint8_t *)(void *)(_addr))

/**
 * Tools (functions)
 */
static __inline__ __FORCE_INLINE__ intmax_t VEC_abs(intmax_t a) {
  return a < 0 ? -a : a;
}

static __inline__ __FORCE_INLINE__ uint8_t VEC_ssizeof(uint64_t sz) {
  return ( sz > UINT8_MAX ? sz > UINT16_MAX ?
		   sz > UINT32_MAX ? sz > UINT64_MAX ?
		   0x05 : 0x04 : 0x03 : 0x02 : 0x01 );
}
__STATIC_FORCE_INLINE_F void VEC_adtCache(const void *vec, size_t at) {
  vec_t slot, cacheLoc;
  vecMetaDataHeader *cacheInfo __MAY_ALIAS__;

  cacheInfo = (vecMetaDataHeader *)VEC_peekBlockStart(vec);
  cacheLoc = &(cacheInfo->vcache);

  /* TODO: size-bound check (use request) */
  slot = (vec_t)vec + at;
  /* cache only slot, discard its content */
  (*slot != NULL) && VEC_free(*slot);
  *slot = *cacheLoc, *cacheLoc = slot;

  cacheInfo->vcacheSize += 1;
  cacheInfo->vcapSize -= 1;
}

__STATIC_FORCE_INLINE_F void VEC_rmfCache(void *vec, size_t at) {
  vec_t cacheLoc, cachePrevLoc;

  cacheLoc = ((vecMetaDataHeader *)VEC_peekBlockStart(vec))->vcache;
  while (cacheLoc && ((cacheLoc - (vec_t)vec) != at))
	cachePrevLoc = cacheLoc, cacheLoc = *cacheLoc;
  cacheLoc && (*cachePrevLoc = *cacheLoc);
}

/**
 * vector_new (macro: alias -> VEC_create)
 */
#define VEC_new(size_t_sz, ...)											\
  MACR_DO_ELSE(VEC_create(size_t_sz, MACR_DO_ELSE((__VA_ARGS__), NULL, __VA_ARGS__)), (throwError(NULL)), size_t_sz)

/**
 * vector_create
 */
static vec_t VEC_create(size_t sz, const VEC_set *vnpf) {
  void *vec;
  uint8_t _vecMetaData;

  if ( !sz )
	sz = VEC_LEAST_SZ;
  _vecMetaData = 0x04; /* default -> native */
  if (vnpf && !vnpf->native) {
	/*
	  TODO: NOTRACKSIZE SHOULD HAVE THE SAME EFFECT AS READONLY
	*/
	_vecMetaData = vnpf->optimize && /* not */ vnpf->readOnly ? 0x04 /* maximum supported bit size */ : VEC_ssizeof(sz) + 1;
	_vecMetaData |= (uint8_t)!!vnpf->readOnly << 4 | !(uint8_t)!vnpf->ntrackSize << 4;
  }

  if (! (vec = calloc(sizeof (uint8_t), (vecGblDataBlockSize * sz) + vecGblMetaDataSize)))
	return (vec_t)NULL;
  ((vecMetaDataHeader *)((uint8_t *)vec))->vcapSize = sz;
  VEC_moveToMainBlock(vec);
  ((uint8_t *)vec - vecDefFlagLoc)[0] = VEC_VECTOR | _vecMetaData;

  /*  */
  *(void **)vec = VEC_EOV;
  return vec;
}

/**
 * vector_expand: expands to vector/resize vector
 */
static __inline__  __NONNULL__ vec_t VEC_expand(vec_t *vec, void *new, VEC_szType sz, VEC_szType reqAt) {
  void *tmpAlloc;
  register uintmax_t magnitude;

  if (sz > reqAt)
	return VEC_append(vec, new, reqAt);

  /* TODO: magnitude may overflow */
  magnitude = (vecGblDataBlockSize * (reqAt + VEC_ALLOC_SZ)) + vecGblMetaDataSize;
  /* resizing */
  if ( !(tmpAlloc = *vec && !(VEC_getMetaData(*vec) & VEC_READONLY)
		 ? realloc(VEC_moveToBlockStart(*vec), magnitude) : NULL) )
	return (vec_t)NULL;

  magnitude = reqAt - sz; /* reuse of variable ’magnitude’ */
  (magnitude > 1) /* false? no need to call memset */
	&&  memset(tmpAlloc + sz, 0, vecGblDataBlockSize * magnitude);
  *vec = VEC_moveToMainBlock(tmpAlloc);
  VEC_getMagnitude(tmpAlloc) = reqAt + 1;
  (*vec)[reqAt] = new;

  return tmpAlloc;
}

/**
 * vec_append: add/append new data to vector
 */
static __inline__  __NONNULL__ vec_t VEC_append(vec_t *vec, void *new, size_t reqAt) {
  register size_t magnitude;
  vec_t loc __MB_UNUSED__;

  magnitude = VEC_getMagnitude(*vec);

  if (magnitude > reqAt) {
	loc = *vec + reqAt;
#ifdef VEC_TRACE_DEL
	VEC_rmfCache(*vec, reqAt);
#else
	(*loc != VEC_EOV) && VEC_free(*loc);
#endif
	*loc = new;
  }
  else if (! (!(VEC_getMetaData(*vec) & VEC_READONLY) && VEC_expand(vec, new, magnitude, reqAt)) ) {
	return (vec_t)NULL;
  }
  return *vec;
}

/**
 * VEC_add
 */
static  __NONNULL__ void *VEC_add(vec_t *vec, void *new, size_t bytes,  size_t reqAt) {
  void *_new;

 if (!*vec || !bytes || !(_new = malloc(bytes + vecDefFlagLoc)))
	return NULL;

  _new = (uint8_t *)_new + vecDefFlagLoc;
  (VEC_ACCESS(_new) - vecDefFlagLoc)[0] = VEC_ARRAY;
  memcpy(_new, new, bytes);

  return VEC_append(vec, _new, reqAt);
}

/**
 * VEC_request
 */
static __NONNULL__ __inline__ __attribute__((always_inline, pure)) void *VEC_request(vec_t vec, ssize_t reqAt) {
  size_t magnitude;

  magnitude = VEC_getMagnitude(vec);
  reqAt = reqAt < 0 ? magnitude + reqAt : reqAt;

  if (reqAt >= magnitude || reqAt < 0) {
	/* out of bound */
	return NULL;
  }
  return vec[reqAt];
}

/**
 * VEC_requestAt
 */
__STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_requestAt(vec_t *vec, ssize_t reqAt, ssize_t at) {
  void *itemAt;

  itemAt = VEC_request(*vec, at);
  return itemAt ? VEC_request(&itemAt, reqAt) : NULL;
}
__STATIC_FORCE_INLINE_F __NONNULL__ uint8_t VEC_getLevel(void *vec) {
  /*
   * Non vector if; v & VEC_VECTOR is 0
   */
  return VEC_getMetaData(vec) & VEC_VECTOR;
}

/**
 * VEC_free
 */
__STATIC_FORCE_INLINE_F bool VEC_free(void *rt) {
  free(VEC_getLevel(rt) ? VEC_moveToBlockStart(rt) : (VEC_ACCESS(rt) - vecDefFlagLoc));

  return true;
}

/**
 * VEC_remove
 */
static __NONNULL__ void *VEC_remove(vec_t *vec, ssize_t reqAt) {

#ifdef VEC_TRACE_DEL
  /* cache empty vector slot */
  VEC_adtCache(*vec, reqAt);
#else
  /* NOT IMPLEMENTED */
#endif

  return NULL;
}

/**
 * VEC_shrink
 */
__STATIC_FORCE_INLINE_F void *VEC_shrink(void *ptr) {

  /* NOT IMPLEMENTED */

  return NULL;
}

/**
 *  VEC_delete
 */
static __NONNULL__ void *VEC_delete(vec_t *vec) {

  /* NOT IMPLEMENTED */

  return NULL;
}

int main(void) {
  size_t x;
  int *intArray;
  int **vec = (void *)VEC_new(6);

  int data[][6] = {
				   {0, 2, 4, 6, 8, 10}, {1, 3, 5, 7, 9, 11},
				   {2, 3, 5, 7, 11, 13}, {4, 16, 32, 64, 128},
  };
  VEC_add((void *)&vec, data[0], sizeof(int), 0);
  VEC_add((void *)&vec, data[1], sizeof(int), 1);
  VEC_add((void *)&vec, data[2], sizeof(int), 2);

  intArray = VEC_request((void *)vec, 0);
  //puti(*(intArray + 0));

  intArray = VEC_request((void *)vec, 1);
  //puti(*(intArray + 0));

  VEC_remove((void *)&vec, 1);
  intArray = VEC_request((void *)vec, 1);
  puti(*(intArray + 0));

  VEC_delete((void *)&vec);
  return 0;
}
