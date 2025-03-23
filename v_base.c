#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
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
//#define VEC_TRACE_DEL
/* Allow size-bound check on request */
#define VEC_SAFE_REQUEST 1
/* Allow neccessary warnings */
#define VEC_ALW_WARNING 1

/* NULL */
#define VEC_EOV (char *)(long) 0

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
 * Some useful informations about the container, may be store conveniently
 * within a single byte of data. In our case, we store frequently accessed
 * data such as container type, permissions, modes, etc within N bytes in memory
 */
const uint8_t vecDefFlagLoc         = 1;

/* Meta-data size */
const uint8_t vecGblMetaDataSize  = sizeof (vecMetaDataHeader) + vecDefFlagLoc;

/* Sizeof allocation block for entries */
const uint8_t vecGblDataBlockSize = sizeof (vec_t);

/**
 * VEC_getMetaData (lvalue)
 * Return the meta-data of a container. If a second argument is passed
 * to the macro as a varadic parameter, it is assigned the value returned
 * by meta-data
 */
#define VEC_getMetaData(vec, ...) (MACR_DO_ELSE(__VA_ARGS__ =,,__VA_ARGS__) (VEC_ACCESS(vec) - vecDefFlagLoc)[0])

/* Temporarily view the starting block of the vector */
#define VEC_peekBlockStart(vec)   (void *)(VEC_ACCESS(vec) - vecGblMetaDataSize)

/* Request or update the size of the container */
#define VEC_getMagnitude(vec)     ((vecMetaDataHeader *)VEC_peekBlockStart(vec))->vcapSize

/* Update the pointer to the start of the wriitable section of the container */
#define VEC_moveToMainBlock(vec)  ((vec) = VEC_ACCESS(vec) + vecGblMetaDataSize)

/* Move the pointer to the start of the contaner’s block */
#define VEC_moveToBlockStart(vec) ((vec) = VEC_peekBlockStart(vec))
#
#define ignoreExprReturn(...) (void) (__VA_ARGS__)
#
/* reinterpret memory block as pointer of single bytes */
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

#ifdef VEC_TRACE_DEL

__STATIC_FORCE_INLINE_F void VEC_cache(const void *vec, size_t at) {
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

__STATIC_FORCE_INLINE_F void VEC_rcache(void *vec, size_t at) {
  vec_t cacheLoc, cachePrevLoc;

  /* retrieve cache list */
  cacheLoc = ((vecMetaDataHeader *)VEC_peekBlockStart(vec))->vcache;
  cachePrevLoc = NULL;

  /*
   * if the difference between the initial memory address of the container and that
   * of the cache pointer equals that of the requested index, we had cached the
   * memory on a previous operation
   */
  while (cacheLoc && (ptrdiff_t)((cacheLoc - (vec_t)vec) != at))
	cachePrevLoc = cacheLoc, cacheLoc = *cacheLoc;

  /*
	cacheloc will never be null assuming no cache miss. remove the found memory from the cache list
   */
  if (cacheLoc) {
	cachePrevLoc && ( *cachePrevLoc = cacheLoc );
	((vecMetaDataHeader *)VEC_peekBlockStart(vec))->vcacheSize -= 1;
  }
}
#endif /**/

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
	VEC_rcache(*vec, reqAt);
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
 __STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_baseRequest(vec_t vec, ssize_t reqAt) {
  size_t magnitude;

  magnitude = VEC_getMagnitude(vec);
  reqAt = reqAt < 0 ? magnitude + reqAt : reqAt;

  if (reqAt >= magnitude || reqAt < 0) {
	throwError(OUT_OF_BOUND);
	exit(EXIT_FAILURE);
  }
  return vec + reqAt;
}
/* retrieve data from container located at @request */
__STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_request(vec_t vec, ssize_t reqAt) {
  return *(vec_t)(VEC_baseRequest(vec, reqAt));
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
  VEC_cache(*vec, reqAt);
#else
  vec_t varReq;

  (varReq = VEC_baseRequest(*vec, reqAt));
  free(*varReq);
  /* TODO: fix the possibility of overflow */
  memmove(varReq + 1, varReq, (VEC_getMagnitude(*vec) - reqAt) * vecGblDataBlockSize);
  /* TODO; shrink container to current size */
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
  puti(*(intArray + 0));

  intArray = VEC_request((void *)vec, 1);
  puti(*(intArray + 0));

  intArray = VEC_request((void *)vec, 2);
  puti(*(intArray + 0));

  VEC_remove((void *)&vec, 0);
  VEC_remove((void *)&vec, 1);
  VEC_remove((void *)&vec, 2);

  VEC_delete((void *)&vec);
  return 0;
}
