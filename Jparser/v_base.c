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
/* fprintf(stderr, "[vector] " "An attempt to add '%p' at index %lu to an object of size %lu failed\n", new, (long)index, (long)sz);*/
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

/*
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

#define VEC_DATA_BLOCK_SZ (sizeof (vec_t))
#define VEC_META_DATA_SZ(_fl) ( (1 << ( ((_fl) & 0x0f) - 1 )) + 1) /*vector_byte_size and 1 byte for other meta-info */
#define VEC_LEAST_SZ 1
#define VEC_ALLOC_SZ 1
#define VEC_VECTOR 0x80
#define VEC_ARRAY 0
#define VEC_SAFE_INDEX_CHECK 1
#define VEC_ALW_WARNING 1
#define VEC_EROUT_OF_BOUND 0x3b
#define VEC_READONLY 0x08
#define VEC_NON_NATIVE 0x18
#define VEC_NO_TRACK_SIZE 0x20
#define suppress_unused_warning(...) (void) (__VA_ARGS__)

/* remove the type alignment of bytes so that each block can be addressed like a byte array */
#define VEC_ACCESS(_addr) ((word0 *)(void *)(_addr))
#define VCAST(type, addr) *(volatile type *)(addr)

#define VEC_SZ_INCR(sz, fl)						\
  switch ((fl) & 0x0f) {						\
  case 0x01: VCAST(word0, sz) += 1; break;		\
  case 0x02: VCAST(uint16_t, sz) += 1; break;	\
  case 0x03: VCAST(uint32_t, sz) += 1; break;	\
  case 0x04: VCAST(uint64_t, sz) += 1; break;	\
  } (void)0

#define VEC_SZEOF(_SZ) VEC_ssizeof(_SZ)
#define VEC_BLOCK_START(vec, fl)				\
  (VEC_ACCESS(vec) - VEC_META_DATA_SZ(fl))

#define VEC_MOVTO_DATA_START(vec, fl)							\
  ((vec) = (void *)(VEC_ACCESS(vec) + VEC_META_DATA_SZ(fl)))

static __inline__ __FORCE_INLINE__ intmax_t VEC_abs(intmax_t a) {
  return a < 0 ? -a : a;
}
static __inline__ __FORCE_INLINE__ uint8_t VEC_getSize(void *vec, void *to) {
  register word0 *s, *v, i;

  v = vec, s = to;

  i = *--v & 0x0f; /* n bytes of vec size */
  v -= i; /* mov v to block start */

  while ( i-- ) {
	*s++ = *v++;
  }
  return i;
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
 * vector_create
 */
#define VEC_new(size_t_sz, ...) VEC_create(size_t_sz, *(const char *)# __VA_ARGS__ "[")

static void **VEC_create(size_t sz, const uint8_t npf, ...) {
  void *vec;
  word0 fl;

  if ( !sz )
	sz = VEC_LEAST_SZ;

  if (npf != '['){
	va_list _vnpf;
	va_start(_vnpf, npf);
	const VEC_set *vnpf = va_arg(_vnpf, VEC_set *);

	if (! vnpf->native) {
	  /*
		TODO: NOTRACKSIZE SHOULD HAVE THE SAME EFFECT AS READONLY
	  */
	  fl |= (uint8_t)!!vnpf->readOnly << 4 | !(uint8_t)!vnpf->ntrackSize << 4;
	  fl = vnpf->optimize && /* not */ vnpf->readOnly ? 0x04 /* maximum supported bit size */ : VEC_SZEOF(sz) + 1;
	}
  }
  else {
	fl = 0x04;
  }
  if (! (vec = malloc((VEC_DATA_BLOCK_SZ * sz) + /* bytes required by size */ VEC_META_DATA_SZ(fl) + /* 1 byte for meta-data */ 1)))
	return NULL;
  VEC_putSize(vec, &sz, fl);
  
  /* mov ahead meta-data block (main) */
  VEC_MOVTO_DATA_START(vec, fl);
  /* update meta-data: prealloc | type | n bytes allocated for sz ([1100 0001] for size == 1) */
  (VEC_ACCESS(vec) - 1)[0] = VEC_VECTOR | fl;
  /* initialize the first block (main) to 0 [vector is empty] */
  *(void **)vec = 0;

  return vec;
}

/*
 * vector_append: add to last; realloc/copy vector if neccessary
 */
static __inline__  __NONNULL__ void **VEC_append(vec_t *vec, void *_new, VEC_szType sz, VEC_szType index, word0 _vecMetaData) {
  void *v0;
  register _Bool gt, sb;
  register uint8_t pb;
  register uintmax_t memPtr;

  sb = false; /* True if size reqires an additional block */
  pb = _vecMetaData & 0x0f; /* n bytes of current size */;
  gt = index > sz;

  /* shifting pb bytes should result to 0 if (sz + 1) <= the maximum number pb bytes can represent */
  if (!(_vecMetaData & VEC_READONLY) && (index) >> (4ul << pb)) {
	(VEC_ACCESS(*vec) - 1)[0] = pb = (_vecMetaData & ~pb) | VEC_ssizeof(index);
	sb = (_vecMetaData & (VEC_NON_NATIVE | VEC_NO_TRACK_SIZE));
  }
  else {
	/* error: attempt to modify a readonly container */
	puts("error");
	return (void **)NULL;
  }
  /* resize vector || allocate an entire new block */
  memPtr = (VEC_DATA_BLOCK_SZ * (gt ? index : sz + VEC_ALLOC_SZ)) + VEC_META_DATA_SZ(_vecMetaData);

  v0 = *vec ? (sb == false) ? realloc(VEC_BLOCK_START(*vec, _vecMetaData), memPtr) : malloc(memPtr + 1) : NULL;
  suppress_unused_warning( gt && memset(v0 + sz, 0, VEC_DATA_BLOCK_SZ * (index - sz)) );

  if (v0 == NULL)
	return (void **)NULL;

  if (__EXPR_LIKELY__(sb, false)) {
	/* We are here because we want to pad a new block for size */
	memcpy(v0, &sz, pb);
	(VEC_ACCESS(v0) + pb)[0] = _vecMetaData;

	/* (local realloc)
	 * This is a trade-off of performance and may be changed in future review.
	 */
	memcpy(v0 + _vecMetaData + 1, *vec, sz * VEC_DATA_BLOCK_SZ);
	free(VEC_BLOCK_START(*vec, _vecMetaData));
  }
  suppress_unused_warning( gt ? VEC_putSize(v0, &index, _vecMetaData) : VEC_Incr(v0, _vecMetaData) );
  *vec = (void *)(VEC_ACCESS(v0) + pb + 1);
  (*vec)[index - 1] = _new;

  return *vec;
}

/*
 * vec_expand: add new data to vector
 */
static __inline__  __NONNULL__ void **VEC_expand(vec_t *vec, void *new, size_t index) {
  VEC_szType sz, _vecMetaData;

  _vecMetaData = (VEC_ACCESS(*vec) - 1)[0];
  suppress_unused_warning( (sz = 0) | VEC_putSize(&sz, VEC_BLOCK_START(*vec, _vecMetaData), _vecMetaData)  );

  if (sz > index)
	(*vec)[index] = new;
  else if (! (!(_vecMetaData & VEC_READONLY) && VEC_append(vec, new, sz, index, _vecMetaData)) ) {
	/* out of bound // insufficient memory */
  err_outOfBound:
#if VEC_ALW_WARNING
	(_vecMetaData & VEC_READONLY) && throwError(EROUT_OF_BOUND);
#endif
	return NULL;
  }
  return *vec;
}
/*
 * VEC_add
 */
static  __NONNULL__ void *VEC_add(vec_t *vec, void *new, size_t bytesz, size_t sz, size_t index, word0 type) {
  void *_new /* ptr to memory block */, **newvec /* ptr to vector */;
  word0 _vecMetaData;
  size_t nalloc;

  newvec = (void *)1; /* prevent unsed newvec folding to 0 */

  _vecMetaData = VEC_ARRAY | VEC_SZEOF(sz);
  nalloc = bytesz * sz; /* sizeof(new) * sz */

  if ((*vec == NULL) || (nalloc < 1)
	  || !(_new = malloc(nalloc + VEC_META_DATA_SZ(_vecMetaData)))) {
	return NULL;
  }
  VEC_putSize(_new, &sz, _vecMetaData & 0x0f);
  VEC_MOVTO_DATA_START(_new, _vecMetaData);
  (VEC_ACCESS(_new) - 1)[0] = _vecMetaData;
  memcpy(_new, new, nalloc);

  /* if the vec_vector type is specified, create a new vector */
  if ((type & VEC_VECTOR) && (newvec = VEC_new(VEC_LEAST_SZ))) {
	*newvec = _new;
	_vecMetaData = (VEC_ACCESS(newvec) - 1)[0];
	VEC_SZ_INCR(VEC_BLOCK_START(newvec, _vecMetaData), _vecMetaData);
	_new = newvec;
  }
  return newvec && VEC_expand(vec, _new, index) ? _new : NULL;
}
static __NONNULL__ __inline__ __attribute__((always_inline, pure)) void *VEC_getVectorItem(vec_t vec, ssize_t index) {
  size_t sz;

  (sz = 0) | VEC_getSize(*vec, &sz);
  index = sz + ( index < 0 ? index : 0 );

  if (index > sz || index < 0) {
#if VEC_ALW_WARNING
	throwError(ERGOUT_OUT_OF_BOUND, *vec, sz, index);
#endif
	return NULL;
  }
  return vec[index];
}
__STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_getVectorItemAt(vec_t *vec, ssize_t index, ssize_t at) {
  void *itemAt;

  itemAt = VEC_getVectorItem(*vec, at);
  return itemAt ? VEC_getVectorItem(&itemAt, index) : NULL;
}
__STATIC_FORCE_INLINE_F __NONNULL__ uint8_t VEC_getType(void *vec) {
  /* v & VEC_VECTOR == 0 (array) */
  return ( (VEC_ACCESS(vec) - 1)[0] & VEC_VECTOR );
}
static __NONNULL__ void *VEC_remove(vec_t *vec, ssize_t index) {
}
__STATIC_FORCE_INLINE_F bool VEC_free(void *ptr) {
  free(ptr);
  return 0;
}

#define NEXT_MEMB_OF(ve) (++*(vec_t *)&(ve))[0]
#define VEC_preserveAndAssign(tmp, _1, _2) (((tmp) = (_1)), ((_1) = (_2)))

#define VEC_CURR_STATE 0x10
#define VEC_PREV_STATE 0x20
#define VEC_EOV (char *)(long) 0xff

#define VEC_DEF_NWSTATE_R(fmr, nw, _cur)			\
  ((fmr) = (nw) |= _cur | (((fmr) & _cur) << 1))
#define VEC_DEF_NWSTATE(fmr, nw)				\
  VEC_DEF_NWSTATE_R(fmr, nw, VEC_CURR_STATE)
#define VEC_REM_CURSTATE(fmr) ((fmr) &= ~VEC_CURR_STATE)

__STATIC_FORCE_INLINE_F __NONNULL__ bool VEC_deleteInit(vec_t vec, vec_t curr, vec_t currTmp, vec_t descdant, size_t *sz) {
  *sz = 0;
  if (__EXPR_LIKELY__(vec && (VEC_getSize(vec, sz), sz), 1) ) {
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

  while  (! (sz < 0) ) {
	if (  ! sz-- ) {
	  do {
		lCurrt = (( vec_t )lTmp)[0];
		free(VEC_BLOCK_START(lTmp, _vecMetaData));
		if (lCurrt == VEC_EOV)
		  goto _del_end;
		lTmp = _vecMetaData & VEC_PREV_STATE ? lCurrt : (( vec_t )lCurrt)[0];
		(sz = 0) | VEC_getSize(lTmp, &sz);
	  } while ( !sz-- );
	  _vecMetaData = VEC_REM_CURSTATE((VEC_ACCESS(lTmp) - 1)[0]);
	  lNext = NEXT_MEMB_OF(lCurrt);
	}
	if (lNext && ((ul = (VEC_ACCESS(lNext) - 1))[0] & VEC_VECTOR)) {
	  VEC_putSize(VEC_BLOCK_START(lTmp, _vecMetaData), &sz, _vecMetaData);
	  (sz = 0) | VEC_getSize(lNext, &sz);

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
  void *ptr;
  void **vec = VEC_new(0);

  int data[][6] = {
				   {0, 2, 4, 6, 8, 10}, {1, 3, 5, 7, 9, 11},
				   {2, 3, 5, 7, 11, 13}, {4, 16, 32, 64, 128},
  };
  VEC_add(&vec, data[0], sizeof(int), 6, 1, VEC_ARRAY);
  // VEC_add(&vec, data[1], sizeof(int), 6, 1, VEC_ARRAY);

  //ptr = *(vec + 1);
  puti(VEC_META_DATA_SZ(0x03));
  /* int g = 0, k = 5; */

  /* VEC_putSize(&k, &g, 1); */
  /* puti(k); */
  //printf("%d\n", *((int *)ptr + 1) );
  // VEC_delete(&vec);
}

/*
  TODO:
*/
