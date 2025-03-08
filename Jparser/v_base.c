#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <stdarg.h>

#if defined(__GNUC__) || defined(__clang__)
#define __FORCE_INLINE__ __attribute__((always_inline))
#elif defined(_MSC_VER) || defined(WIN32)
#define __FORCE_INLINE__ __forceInline
#else
#define __FORCE_INLINE__
#endif
#define __NONNULL__ __attribute__((nonnull))
#define CHOOSE_EXPR(cExpr, tVal, fVal)			\
  __builtin_choose_expr(cExpr, tVal, fVal)
/* fprintf(stderr, "[vector] " "An attempt to add '%p' at reqAt %lu to an object of size %lu failed\n", new, (long)reqAt, (long)sz);*/
#define throwError(...) (void *)0
#define puti(i) printf("%lld\n", (long long int)(i))
/*
 * vector
 */
#if defined(__GNUC__) || defined(__clang__)
#define __MAY_ALIAS__ __attribute__((may_alias))
#define __EXPR_LIKELY__(T, L) __builtin_expect(T, L)
#define __MB_UNUSED__ __attribute__((unused))
#else
#define __MAY_ALIAS__
#define __EXPR_LIKELY__(T, L) T
#define __MB_UNUSED__
#endif
#define __STATIC_FORCE_INLINE_F static __inline__ __FORCE_INLINE__

/**
 *                       Vector Information
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
typedef uint64_t VEC_szType;
typedef void ** vec_t;
typedef uint8_t word0;

typedef union {
  void *nword;
  uintmax_t wordx;
  uint64_t word8;
  uint32_t word4;
  uint16_t word2;
  uint8_t word0;
} _inttype_t;

typedef struct {
  size_t vhcap;
  uint8_t vCapSize;
  uint8_t *vnxtv;
} vecMetaDataHeader;
/*
 * Prototypes
 */
static __NONNULL__ vec_t VEC_append(vec_t *, void *, size_t);

/*
 * Global Variables
 */
const uint8_t vecDefFlagLoc         = 1;
const uint8_t vecGblMetaDataSize  = sizeof (vecMetaDataHeader) + vecDefFlagLoc;
const uint8_t vecGblDataBlockSize = sizeof (vec_t);

/* @MACR_NON_EMPTY, MACR_DO_ELSE
 * Safely assert if a pareameter is given an argument
 * Returns the argument if provided, else implemation defined (Nothing by default)
 */
#define MACR_EMPTY_PARAM(...) 0,
#define MACR_INDIRECT_EVAL(a, ...) MACR_ACCEPT_FIRST(__VA_ARGS__)
#define MACR_ACCEPT_FIRST(a, ...) a
#define MACR_IGNORE_FIRST(...) MACR_INDIRECT_EVAL(__VA_ARGS__)
#define MACR_NON_EMPTY(...) MACR_IGNORE_FIRST(MACR_EMPTY_PARAM  __VA_ARGS__(), 1)
# /* MACR_IF_EMPTY_0 -> empty */
#define MACR_IF_EMPTY_0(a, ...) __VA_ARGS__
# /* or */
#define MACR_IF_EMPTY_(a, ...) __VA_ARGS__
#
#define MACR_IF_EMPTY_1(a, ...) a
#
#define MACR_CAT(A, A1) MACR_INDIRECT_CAT(A, A1)
#define MACR_INDIRECT_CAT(A, A1) A ## A1
#define MACR_DO_ELSE(_true, _false, ...) MACR_CAT(MACR_IF_EMPTY_, MACR_NON_EMPTY(__VA_ARGS__))(_true, _false)
#
# /* end */
#

/**
 * VEC_GETMETADATA: Returns the metadata of a vector.
 * if an lvalue is provided as a second argument, it is assigned this metadata
 */
#define VEC_GETMETADATA(vec, ...) (MACR_DO_ELSE(__VA_ARGS__ =,,__VA_ARGS__) (VEC_ACCESS(vec) - vecDefFlagLoc)[0])

#define VEC_peekBlockStart(vec)   (void *)(VEC_ACCESS(vec) - vecGblMetaDataSize)
#define VEC_getMagnitude(vec)     ((vecMetaDataHeader *)VEC_peekBlockStart(vec))->vCapSize
#define VEC_moveToMainBlock(vec)  ((vec) = VEC_ACCESS(vec) + vecGblMetaDataSize)
#define VEC_moveToBlockStart(vec) ((vec) = VEC_peekBlockStart(vec))

#define VEC_DATA_BLOCK_SZ (sizeof (vec_t))
/*vector_byte_size and 1 byte for other meta-info */
#define VEC_META_DATA_SZ(_fl) ( (1 << ( ((_fl) & 0x0f) - 1 )) + 1)
#define VEC_LEAST_SZ 1
#define VEC_ALLOC_SZ 1
#define VEC_VECTOR 0x80
#define VEC_ARRAY 0
#define VEC_SAFE_REQAT_CHECK 1
#define VEC_ALW_WARNING 1
#define VEC_EROUT_OF_BOUND 0x3b
#define VEC_READONLY 0x08
#define VEC_NON_NATIVE 0x18
#define VEC_NO_TRACK_SIZE 0x20
#define VEC_EOV (char *)(long) 0xff
  #
#define ignoreExprReturn(...) (void) (__VA_ARGS__)
#
/* reinterpret as pointer of bytes */
#define VEC_ACCESS(_addr) ((uint8_t *)(void *)(_addr))
#
#define VEC_SZEOF(_SZ) VEC_ssizeof(_SZ)
#
#define VEC_BLOCK_START(vec, fl)				\
  (VEC_ACCESS(vec) - VEC_META_DATA_SZ(fl))
#
#define VEC_MOVTO_DATA_START(vec, fl)							\
  ((vec) = (void *)(VEC_ACCESS(vec) + VEC_META_DATA_SZ(fl)))

static __inline__ __FORCE_INLINE__ intmax_t VEC_abs(intmax_t a) {
  return a < 0 ? -a : a;
}

static __inline__ __FORCE_INLINE__ uint8_t VEC_putSize(void *d, void *s, uint8_t fl) {
  _inttype_t *_d __MAY_ALIAS__, *_s __MAY_ALIAS__;

  _d = d, _s = s;
  switch (fl & 0x0f) {
  case 0x01: _d->word0 = _s->word0; break;
  case 0x02: _d->word2 = _s->word2; break;
  case 0x03: _d->word4 = _s->word4; break;
  case 0x04: _d->word8 = _s->word8; break;
  }
  return 0;
}

static __inline__ __FORCE_INLINE__ uint8_t VEC_Incr(void *d, uint8_t fl) {
  switch (fl & 0x0f) {
  case 0x01: ((_inttype_t * __MAY_ALIAS__)(VEC_BLOCK_START(d, fl)))->word0 += 1;
  case 0x02: ((_inttype_t * __MAY_ALIAS__)(VEC_BLOCK_START(d, fl)))->word2 += 1;
  case 0x03: ((_inttype_t * __MAY_ALIAS__)(VEC_BLOCK_START(d, fl)))->word4 += 1;
  case 0x04: ((_inttype_t * __MAY_ALIAS__)(VEC_BLOCK_START(d, fl)))->word8 += 1;
  }
  return 0;
}
static __inline__ __FORCE_INLINE__ uint8_t VEC_ssizeof(uint64_t sz) {
  return ( sz > UINT8_MAX ? sz > UINT16_MAX ?
		   sz > UINT32_MAX ? sz > UINT64_MAX ?
		   0x05 : 0x04 : 0x03 : 0x02 : 0x01 );
}
typedef struct {
  uint8_t native; /* readonly is set only for members, ntracksize: false; optimize: true */
  uint8_t readOnly;
  uint8_t memReadOnly;
  uint8_t recurseSet; /* parent setting is inherited by members */
  uint8_t ntrackSize;
  uint8_t optimize;
} VEC_set;
/*
 * vector_new (alias to VEC_create)
 */

#define VEC_new(size_t_sz, ...)											\
  MACR_DO_ELSE(VEC_create(size_t_sz, MACR_DO_ELSE((__VA_ARGS__), NULL, __VA_ARGS__)), (throwError(NULL)), size_t_sz)

static vec_t VEC_create(size_t sz, const VEC_set *vnpf) {
  void *vec;
  word0 _vecMetaData;

  if ( !sz )
	sz = VEC_LEAST_SZ;
  _vecMetaData = 0x04; /* default -> native */
  if (vnpf && !vnpf->native) {
	/*
	  TODO: NOTRACKSIZE SHOULD HAVE THE SAME EFFECT AS READONLY
	*/
	_vecMetaData = vnpf->optimize && /* not */ vnpf->readOnly ? 0x04 /* maximum supported bit size */ : VEC_SZEOF(sz) + 1;
	_vecMetaData |= (uint8_t)!!vnpf->readOnly << 4 | !(uint8_t)!vnpf->ntrackSize << 4;
  }

  if (! (vec = calloc(sizeof (uint8_t), (vecGblDataBlockSize * sz) + vecGblMetaDataSize)))
	return (vec_t)NULL;
  ((vecMetaDataHeader *)((uint8_t *)vec))->vCapSize = sz;
  VEC_moveToMainBlock(vec);
  ((uint8_t *)vec - vecDefFlagLoc)[0] = VEC_VECTOR | _vecMetaData;

  /* clear vector */
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
  if ( !(tmpAlloc = *vec && !(VEC_GETMETADATA(*vec) & VEC_READONLY)
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
  register VEC_szType magnitude;
;
  magnitude = VEC_getMagnitude(*vec);

  if (magnitude > reqAt)
	(*vec)[reqAt] = new;
  else if (! (!(VEC_GETMETADATA(*vec) & VEC_READONLY) && VEC_expand(vec, new, magnitude, reqAt)) ) {
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
 * VEC_getVectorItem
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
__STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_requestAt(vec_t *vec, ssize_t reqAt, ssize_t at) {
  void *itemAt;

  itemAt = VEC_request(*vec, at);
  return itemAt ? VEC_request(&itemAt, reqAt) : NULL;
}
__STATIC_FORCE_INLINE_F __NONNULL__ uint8_t VEC_getType(void *vec) {
  /*
   * Non vector if; v & VEC_VECTOR is 0
   */
  return VEC_GETMETADATA(vec) & VEC_VECTOR;
}
static __NONNULL__ void *VEC_remove(vec_t *vec, ssize_t reqAt) {

  /* NOT IMPLEMENTED*/

  return NULL;
}
__STATIC_FORCE_INLINE_F bool VEC_free(void *ptr) {
  free(ptr);
  return 0;
}

#define NEXT_MEMB_OF(ve) (++*(vec_t *)&(ve))[0]
#define VEC_preserveAndAssign(tmp, _1, _2) (((tmp) = (_1)), ((_1) = (_2)))

#define VEC_CURR_STATE 0x10
#define VEC_PREV_STATE 0x20

#define VEC_DEF_NWSTATE_R(fmr, nw, _cur)			\
  ((fmr) = (nw) |= _cur | (((fmr) & _cur) << 1))
#define VEC_DEF_NWSTATE(fmr, nw)				\
  VEC_DEF_NWSTATE_R(fmr, nw, VEC_CURR_STATE)
#define VEC_REM_CURSTATE(fmr) ((fmr) &= ~VEC_CURR_STATE)

__STATIC_FORCE_INLINE_F __NONNULL__ bool VEC_deleteInit(vec_t vec, vec_t curr, vec_t currTmp, vec_t descdant, size_t *sz) {
  uint8_t _vecMetaData;

  vec && VEC_putSize(sz, (VEC_GETMETADATA(vec, _vecMetaData), VEC_BLOCK_START(vec, _vecMetaData)), _vecMetaData);
  if (__EXPR_LIKELY__(!!sz, 1) ) {
	*curr = *currTmp = vec;
	*descdant = *vec, *vec = VEC_EOV;
	return 1;
  }
  free(vec);
  return 0;
}

static __NONNULL__ void *VEC_internalDelete(vec_t *vec){
  void *lCurrt, *lTmp, *lNext;
  size_t sz __MB_UNUSED__;
  register uint8_t _vecMetaData, *ul;

  if (! VEC_deleteInit(*vec, &lCurrt, &lTmp, &lNext, &sz))
	return 0;
  _vecMetaData = (VEC_ACCESS(lCurrt) - 1)[0] |= VEC_CURR_STATE;
  return 0;
  while  (! (sz < 0) ) {
	if (  ! sz-- ) {
	  do {
		lCurrt = (( vec_t )lTmp)[0];
		free(VEC_BLOCK_START(lTmp, _vecMetaData));
		if (lCurrt == VEC_EOV)
		  goto _del_end;
		lTmp = _vecMetaData & VEC_PREV_STATE ? lCurrt : (( vec_t )lCurrt)[0];
		(sz = 0) | VEC_putSize(&sz, VEC_BLOCK_START(lTmp, VEC_GETMETADATA(lTmp)), VEC_GETMETADATA(lTmp));
	  } while ( !sz-- );
	  _vecMetaData = VEC_REM_CURSTATE((VEC_ACCESS(lTmp) - 1)[0]);
	  lNext = NEXT_MEMB_OF(lCurrt);
	}
	if (lNext && ((ul = (VEC_ACCESS(lNext) - 1))[0] & VEC_VECTOR)) {
	  VEC_putSize(VEC_BLOCK_START(lTmp, _vecMetaData), &sz, _vecMetaData);
	  (sz = 0) | VEC_putSize(&sz, VEC_BLOCK_START(lNext, VEC_GETMETADATA(lNext)), VEC_GETMETADATA(lNext));

	  if (!(_vecMetaData & VEC_CURR_STATE)) {
		(( vec_t )lCurrt)[0] = lTmp;
	  }
	  VEC_preserveAndAssign(lTmp, (( vec_t )lNext)[0], lCurrt);
	  VEC_preserveAndAssign(lCurrt, lNext, lTmp);
	  lTmp = lCurrt;

	  VEC_DEF_NWSTATE(_vecMetaData, *ul);
	  continue;
	}
	lNext && VEC_free(VEC_BLOCK_START(lNext, ul[0]));
	sz && (lNext = NEXT_MEMB_OF(lCurrt));
	VEC_REM_CURSTATE(_vecMetaData);
  }
 _del_end:
  vec = NULL;
}

static __NONNULL__ void *VEC_delete(vec_t *vec) {
  return VEC_internalDelete(vec);
}

int main(void) {
  size_t x;
  int *intArray;
  int **vec = VEC_new(6);

  int data[][6] = {
				   {0, 2, 4, 6, 8, 10}, {1, 3, 5, 7, 9, 11},
				   {2, 3, 5, 7, 11, 13}, {4, 16, 32, 64, 128},
  };
  VEC_add(&vec, data[0], sizeof(int), 0);
  VEC_add(&vec, data[1], sizeof(int), 1);
  VEC_add(&vec, data[2], sizeof(int), 2);

  intArray = VEC_request(vec, 0);
  puti(*(intArray + 0));

  intArray = VEC_request(vec, 1);
  puti(*(intArray + 0));

  intArray = VEC_request(vec, 2);
  puti(*(intArray + 0));

  /* VEC_delete(&vec); */
  return 0;
}
