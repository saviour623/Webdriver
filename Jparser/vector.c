#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#if defined(__GNUC__) || defined(__clang__) 
#define __FORCE_INLINE__ __attribute__((always_inline))
#elif defined(_MS_VER) || defined(WIN32)
#define __FORCE_INLINE__ __ForceInline
#else
#define __FORCE_INLINE__
#endif
#define __NONNULL__ __attribute__((nonnull))
#define CHOOSE_EXPR(cExpr, tVal, fVal)                \
    __builtin_choose_expr(cExpr, tVal, fVal)
/* fprintf(stderr, "[vector] " "An attempt to add '%p' at index %lu to an object of size %lu failed\n", vd, (long)index, (long)sz);*/
#define throwError(...) (void *)0
/*
 * vector
 */
#ifdef __GNUC__
#define __MAY_ALIAS__ __attribute__((may_alias))
#define __EXPR_LIKELY__(T, L) __builtin_expect(T, L)
#else
#define __MAY_ALIAS__
#define __EXPR_LIKELY__(T, L) T
#endif
#define __STATIC_FORCE_INLINE_F static __inline__ __FORCE_INLINE__

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

#define VEC_DATA_BLOCK_SZ (sizeof (void *))
#define VEC_META_DATA_SZ(_pre_sz) ((_pre_sz & 0x0f) + 1) /*vector_byte_size and 1 byte for other meta-info */
#define VEC_LEAST_SZ 1
#define VEC_ALWYS_PREALLOC 0x40
#define VEC_ALLOC_SZ 1
#define VEC_APPEND 0xde
#define VEC_VECTOR 0x80
#define VEC_ARRAY 0
#define VEC_SAFE_INDEX_CHECK 1
#define VEC_ALW_WARNING 1
#define VEC_EROUT_OF_BOUND 0x3b

/* remove the type alignment of bytes so that each block can be addressed like a byte array */
#define VEC_ACCESS(_addr) ((word0 *)(void *)(_addr))
#define VCAST(type, addr) *(volatile type *)(addr)

#define VEC_SZ_INCR(sz, fl)					\
    switch ((fl) & 0x0f) {					\
    case 0x01: VCAST(word0, sz) += 1; break;		\
    case 0x02: VCAST(uint16_t, sz) += 1; break;		\
    case 0x03: VCAST(uint32_t, sz) += 1; break;		\
    case 0x04: VCAST(uint64_t, sz) += 1; break;		\
    } (void)0

#define VEC_SZEOF(_SZ) VEC_ssizeof(_SZ)
#define VEC_BLOCK_START(vec, fl)\
    (VEC_ACCESS(vec) - (((fl) & 0x0f) + 1))

#define VEC_MOVTO_DATA_START(vec, fl)				\
    ((vec) = (void *)(VEC_ACCESS(vec) + VEC_META_DATA_SZ(fl)))

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
static __inline__ __FORCE_INLINE__ uint8_t VEC_ssizeof(uint64_t sz) {
    return ( sz > UINT8_MAX ? sz > UINT16_MAX ?
	    sz > UINT32_MAX ? sz > UINT64_MAX ?
	    0x05 : 0x04 : 0x03 : 0x02 : 0x01 );
}
/*
 * vector_create
 */
static void **VEC_create(size_t vecSize) {
    void *vec;
    word0 mSz;

    if ( !vecSize )
	vecSize = VEC_LEAST_SZ;
    /* meta-data size */
    mSz = VEC_META_DATA_SZ(VEC_SZEOF(vecSize));

    if (! (vec = malloc((VEC_DATA_BLOCK_SZ * vecSize) + mSz)))
	return NULL;
    /* initialize size to zero */
    memset(vec, 0, --mSz);

    /* update meta-data: prealloc | type | n bytes allocated for sz ([1100 0001] for size == 1) */
    VEC_ACCESS(vec)[mSz] = ((word0)(vecSize > VEC_LEAST_SZ) << 6) | VEC_VECTOR | mSz;
    /* mov ahead meta-data block (main) */
    VEC_MOVTO_DATA_START(vec, mSz);
/* initialize the first block (main) to 0 [vector is empty] */
    *(void **)vec = 0;

    return vec;
}

/*
 * vector_append: add to last; realloc/copy vector if neccessary
 */
static __inline__  __NONNULL__ void **VEC_append(void ***vec, void *newdt, VEC_szType sz, word0 meta) {
    void *v0;
    register word0 sb, pb;
    register uintmax_t memtb;

    sb = false; /* True if size reqires an additional block */
    pb = meta & 0x0f; /* n bytes of current size */;

    /* shifting pb bytes should result to 0 if (sz + 1) <= the maximum number pb bytes can represent */
    if ((sz + 1) >> (4ul << pb)) {
	(VEC_ACCESS(*vec) - 1)[0] = (meta & ~pb) | (pb + 1);
	sb = true;
    }
       /* resize vector || allocate an entire new block */
    memtb = (VEC_DATA_BLOCK_SZ * (sz + VEC_ALLOC_SZ)) + VEC_META_DATA_SZ(meta);

    v0 = *vec ? sb == false ? realloc(VEC_BLOCK_START(*vec, meta), memtb) : malloc(memtb + 1) : NULL;

    if (v0 == NULL)
	return (void **)NULL;

    if (sb) {
	/* We are here because we want to pad a new block for size */
	memcpy(v0, &sz, pb);
	(VEC_ACCESS(v0) + pb)[0] = meta;

	/* (local realloc)
	 * This is a trade-off of performance and may be changed in future review.
	 */
	memcpy(v0 + meta + 1, *vec, sz * VEC_DATA_BLOCK_SZ);
	free(VEC_BLOCK_START(*vec, meta));
    }
    /* ++sz */
    VEC_SZ_INCR(v0, meta);

    *vec = (void *)(VEC_ACCESS(v0) + pb + 1);
    (*vec)[sz] = newdt; /* implicit [sz - 1] */

    return *vec;
}

/*
 * vec_expand: add new data to vector
 */
static __inline__  __NONNULL__ void **VEC_expand(void ***vec, void *vd, size_t index, size_t vflag) {
    VEC_szType sz, fl;

    (sz = 0) | VEC_getSize(*vec, &sz);

    fl = (VEC_ACCESS(*vec) - 1)[0];

    if (! (*vec)[0] ) {
#if VEC_SAFE_INDEX_CHECK || VEC_ALW_WARNING
	if ((index != 0) || (vflag != VEC_APPEND)) {
	    /* throw out-of-bound error */
	    goto err_outOfBound;
	}
#endif
	/* ignore index || index is 0 */
	(*vec)[index] = vd;
	/* ++sz */
	VEC_SZ_INCR(VEC_BLOCK_START(*vec, fl), fl);
    }
    else if (index < sz) {
	(*vec)[index] = vd;
    }
    else if (! ((vflag & VEC_APPEND) && VEC_append(vec, vd, sz, fl))) {
	/* out of bound // insufficient memory */
    err_outOfBound:
#if VEC_ALW_WARNING
	vflag & VEC_APPEND && throwError(EROUT_OF_BOUND, vec, vd, index + 1, sz);
#endif
	return NULL;
    }
    return *vec;
}

static  __NONNULL__ void *VEC_add(void ***vec, void *vd, size_t bytesz, size_t sz, size_t index, word0 type) {
    void *newdt /* ptr to memory block */, **newvec /* ptr to vector */;
    word0 fl;
    size_t nalloc;

    newvec = (void *)1; /* prevent unsed newvec folding to 0 */

    fl = VEC_ARRAY | VEC_SZEOF(sz);
    nalloc = bytesz * sz; /* sizeof(vd) * sz */

    if (*vec == NULL || nalloc < 1
	|| /* block alloc  */ !(newdt = malloc(nalloc + VEC_META_DATA_SZ(fl)))) {
	return NULL;
    }
    memcpy(newdt, &sz, fl & 0x0f);
    VEC_MOVTO_DATA_START(newdt, fl);
    (VEC_ACCESS(newdt) - 1)[0] = fl;
    /* copy data to memory including its meta-data */
    memcpy(newdt, vd, nalloc);

    /* if the vec_vector type is specified, create a new vector */
    if ((type & VEC_VECTOR) && (newvec = VEC_create(VEC_LEAST_SZ))) {
	/* initialize its first member with the data */
	*newvec = newdt;
	fl = (VEC_ACCESS(newvec) - 1)[0];
	VEC_SZ_INCR(VEC_BLOCK_START(newvec, fl), fl);
	/* reuse newdt to retain a generic referencing for both non-vector or vector using a void ptr */
	newdt = newvec;
    }
    return newvec && VEC_expand(vec, newdt, index, VEC_APPEND) ? newdt : NULL;
}
static __NONNULL__ __inline__ __attribute__((always_inline, pure)) void *VEC_getVectorItem(void **vec, ssize_t index) {
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
__STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_getVectorItemAt(void ***vec, ssize_t index, ssize_t at) {
    void *itemAt;

    itemAt = VEC_getVectorItem(*vec, at);
    return itemAt ? VEC_getVectorItem(&itemAt, index) : NULL;
}
 __STATIC_FORCE_INLINE_F __NONNULL__ uint8_t VEC_getType(void *vec) {
    /* v & VEC_VECTOR == 0 (array) */
    return ( (VEC_ACCESS(vec) - 1)[0] & VEC_VECTOR );
}
static __NONNULL__ void *VEC_remove(void ***vec, ssize_t index) {
}


#define NEXT_MEMB_OF(ve) (++*(vec_t *)&(ve))[0]
#define VEC_preserveAndAssign(tmp, _1, _2) (((tmp) = (_1)), ((_1) = (_2)))

#define VEC_CURR_STATE 0x10
#define VEC_PREV_STATE 0x20
#define VEC_EOV (char *)(long) 0xff

#define VEC_DEF_NWSTATE_R(fmr, nw, _cur)		\
    ((fmr) = (nw) |= _cur | (((fmr) & _cur) << 1))
#define VEC_DEF_NWSTATE(fmr, nw)\
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
    void *lCurrt, *lTmp;
    void *lNext;
    size_t sz;
    register uint8_t *fl, *ul;

    if (! VEC_deleteInit(*vec, &lCurrt, &lTmp, &lNext, &sz))
	return 0;
    *(fl = (VEC_ACCESS(lCurrt) - 1)) |= VEC_CURR_STATE;

    while  (! (sz < 0) ) {
	if (  ! sz-- ) {
	    do {
		lCurrt = (( vec_t )lTmp)[0];
		free(VEC_BLOCK_START(lTmp, *fl));
		if (lCurrt == VEC_EOV)
		    goto _del_end;
		lTmp = *fl & VEC_PREV_STATE ? lCurrt : (( vec_t )lCurrt)[0];
		(sz = 0) | VEC_getSize(lTmp, &sz);
	    } while ( !sz-- );
	    fl = (VEC_ACCESS(lTmp) - 1);
	    lNext = NEXT_MEMB_OF(lCurrt);
	}
	if (lNext && ((ul = (VEC_ACCESS(lNext) - 1))[0] & VEC_VECTOR)) {
	    VEC_putSize(VEC_BLOCK_START(lTmp, *fl), &sz, *fl);
	    (sz = 0) | VEC_getSize(lNext, &sz);

	    if (!(*fl & VEC_CURR_STATE)) {
		puts("forbidden");
		(( vec_t )lCurrt)[0] = lTmp;
	    }
	    printf("(...) lTmp %p, lCurrt %p\n", lTmp, lCurrt);
	    VEC_preserveAndAssign(lTmp, (( vec_t )lNext)[0], lCurrt);
	    VEC_preserveAndAssign(lCurrt, lNext, lTmp);
	    lTmp = lCurrt;
	    printf("(old) fl %u, ul %u\n", *fl, *ul);
	    VEC_DEF_NWSTATE(*fl, *ul);
	    printf("(new) fl %u, ul %u\n", *fl, *ul);
	    continue;
	}
	lNext ? free(VEC_BLOCK_START(lNext, ul[0])) : ( void )0;
	lNext = NEXT_MEMB_OF(lCurrt);
	VEC_REM_CURSTATE(*fl);
    }
 _del_end:
    vec = NULL;
}

static __NONNULL__ void *VEC_delete(void ***vec) {
    return VEC_internalDelete(vec);
}

int main(void) {
    int num[1024] = {0};
    int p = 0;
    VEC_szType x;
    void **vec = VEC_create(1);
    void **new;
    int data[][6] = {
	{0, 2, 4, 6, 8, 10}, {1, 3, 5, 7, 9, 11},
	{2, 3, 5, 7, 11, 13}, {4, 16, 32, 64, 128},
	{3, 6, 9, 12, 15, 18}, {7, 14, 21, 28, 35, 42},
	{10, 20, 30, 40, 50, 60}, {15, 45, 60, 75, 90, 105},
	{20, 40, 60, 80, 100, 120}, {50, 100, 150, 200, 250, 300},
	{0, 0, 0, 0, 0, 0}, {1, 1, 1, 1, 1, 1}
    };
    VEC_add(&vec, data[0], sizeof(int), 6, 0, VEC_VECTOR);
    VEC_add(&vec, data[1], sizeof(int), 6, 1, VEC_ARRAY);
    //   new = VEC_add(&vec, data[2], sizeof(int), 6, 2, VEC_VECTOR);
    //VEC_add(&new, data[3], sizeof(int), 6, 1, VEC_ARRAY);
    //VEC_add(&new, data[4], sizeof(int), 6, 2, VEC_VECTOR);
    //
    //VEC_add(&new, data[5], sizeof(int), 6, 1, VEC_ARRAY);
    
    p =  (VEC_ACCESS(vec) - 1)[0] & 0x0f;
    x = 0;

    memcpy(&x, VEC_BLOCK_START(vec, p), p);
    printf("%lu\n", x);

    p = 3;
    VEC_SZ_INCR(&p, sizeof p);
    VEC_delete(&vec);
    //printf("%d\n", new == 0);
}

/*
  AX +
     +
     A1X +
         +
	 A2X
     A2X[0] --> A1X
     A2X ptr --> A2X[0]
 */
