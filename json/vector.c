#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#define __FORCE_INLINE__ __attribute__((always_inline))
#define __NONNULL__ __attribute__((nonnull))
#define CHOOSE_EXPR(cExpr, tVal, fVal)                \
    __builtin_choose_expr(cExpr, tVal, fVal)

/*
 * vector
 */
typedef uint32_t VEC_szType;
typedef uint8_t word8;
#define VEC_PREALLOC 0x40 /* 2 ^ 7 */

#define VEC_DATA_BLOCK_SZ (sizeof (void *))
#define VEC_META_DATA_SZ(_pre_sz) ((_pre_sz & 0x0f) + 1) /*VEC_BYTE_SZ + 1 */
#define VEC_ALLOC_SZ 1
#define VEC_APPEND 268
#define VEC_VECTOR 0x80
#define VEC_ARRAY 0

/* remove the type alignment of bytes so that each block can be addressed like a byte array */
#define VEC_ACCESS(_addr) ((word8 *)(void *)(_addr))

#define VEC_SZ_INCR(vec, fl)			\
    switch ((fl) & 0x0f) {					\
    case 0x01: *(volatile word8 *)(vec) += 1; break;		\
    case 0x02: *(volatile uint16_t *)(vec) += 1; break;		\
    case 0x03: *(volatile uint32_t *)(vec) += 1; break;		\
    case 0x04: *(volatile uint64_t *)(vec) += 1; break;		\
    } (void)0

#define VEC_SZEOF(_SZ)				\
    ((_SZ) > UINT8_MAX ?			\
     (_SZ) > UINT16_MAX ?			\
     (_SZ) > UINT32_MAX ?			\
     (_SZ) > UINT64_MAX ?			\
     0x05 : 0x04 : 0x03 : 0x02 : 0x01		\
     )
#define VEC_BLOCK_START(vec, fl)\
    (VEC_ACCESS(vec) - (((fl) & 0x0f) + 1))

#define VEC_MOVTO_DATA_START(vec, fl)				\
    ((vec) = (void *)(VEC_ACCESS(vec) + VEC_META_DATA_SZ(fl)))

static __inline__ __FORCE_INLINE__ void VEC_getSize(void *vec, void *to) {
    register word8 *s, *v, i;

    v = vec, s = to;

    i = *--v & 0x0f; /* n bytes of vec size */
    v -= i + 1; /* mov v to block start */

    while ( i-- ) {
	*s++ = *v++;
    }
}

/*
 * vector_create
 */
static __inline__ void **VEC_create(void) {
    void *vec;
    word8 mSz;

    /* meta data size */
    mSz = VEC_META_DATA_SZ(VEC_SZEOF(VEC_ALLOC_SZ));
 
    if (! (vec = malloc((VEC_DATA_BLOCK_SZ * VEC_ALLOC_SZ) + mSz)))
	return NULL;
    /* initial size to zero */
    memset(vec, 0, mSz - 1);

    /* prealloc | n bytes allocated for sz */
    VEC_ACCESS(vec)[mSz - 1] = ((word8)(VEC_ALLOC_SZ > 1) << 6) | (mSz - 1);
    
    /* move pointer to end of the meta-data block, which is the actual ad/dress of vec allowed to store data */
    vec = VEC_ACCESS(vec) + mSz;
/* initializing the first block to 0 (block is empty)*/
    *(void **)vec = 0;

    return vec;
}

/*
 * vector_append: add to last; realloc/copy vector if neccessary
 */
static __inline__  __NONNULL__ void **VEC_append(void ***vec, void *new, VEC_szType sz, word8 meta) {
    void *v0;
    register word8 sb, pb;
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
    (*vec)[sz] = new; /* implicit [sz - 1] */

    return *vec;
}

/*
 * The sum of preallocated block: ((sz / pre_alloc_sz) + (1 or 0))
 * pre_alloc_sz; where the +1 accounts for the effect of the remainder when sz is not a multiple of pre_alloc_sz
 * If sz is lesser than pre_alloc_sz, then there is only a single pre-allocation. if sz falls in the range of pre_alloc_sz and (n * pre_alloc_sz) then n allocations has been done
 */
#define NPREALLOC_FROM_SZ(sz) (((sz / VEC_ALLOC_SZ) + !!(sz % VEC_ALLOC_SZ)) * VEC_ALLOC_SZ)

/*
 * vec_expand: add new data to vector
 */
static __inline__  __NONNULL__ void **VEC_expand(void ***vec, void *vd, size_t index, size_t vflag) {
    
    VEC_szType sz = 0, fl;

    VEC_getSize(*vec, &sz);

    fl = (VEC_ACCESS(*vec) - 1)[0];

    if (! (*vec)[0] ) {
	/* Ignore index and initialize empty vector with new data */
	(*vec)[index] = vd;
	/* ++sz */
	VEC_SZ_INCR(VEC_BLOCK_START(*vec, fl), fl);
    }
    else if (index < sz) {
	(*vec)[index] = vd;
    }
    else if ((fl & VEC_PREALLOC) && (index < (NPREALLOC_FROM_SZ(sz)))) {
	(*vec)[index] = vd;
	/* ++sz */
	VEC_SZ_INCR(VEC_BLOCK_START(*vec, fl), fl);
    }
    else if (! ((vflag & VEC_APPEND) && VEC_append(vec, vd, sz, fl))) {
	return NULL;
    }
    return *vec;
}

static __inline__ __NONNULL__ void *VEC_add(void ***vec, void *vd, size_t bytesz, size_t sz, size_t index, word8 type) {
    void *v0 /* ptr to memory block */, **v00 /* ptr to vector */;
    word8 meta;
    size_t nalloc;

    meta = type | VEC_SZEOF(sz);

    nalloc = bytesz * sz; /* sizeof(vd) * sz */
    if (*vec == NULL || nalloc < 1
	|| /* block alloc  */ !(v0 = malloc(nalloc + VEC_META_DATA_SZ(meta)))) {
	return NULL;
    }
    memcpy(v0, &sz, meta & 0x0f);
    /* */
    (VEC_ACCESS(v0) - 1)[0] = meta;

     /* copy data to memory including its meta-data */
    memcpy(v0, vd, nalloc);

    /* if the vec_vector type is specified, create a new vector */
    if ((type & VEC_VECTOR) && (v00 = VEC_create())) {
	/* initialize its first member with the data */
	*v00 = v0;
	/* retain a generic reference for both non-vector or vector using a void ptr */
	v0 = v00;
    }
    return v00 && VEC_expand(vec, v0, index, VEC_APPEND) ? v0 : NULL;
}
/*
#define DELETE(p) (void)0
static __inline__ __NONNULL__ void vec_DELETE(void ***vec) {
    VEC_szType sz, otherVecInfo, i;
    void **v0;

    if (! (v0 = *vec))
	return;
    VEC_COPY_SZ(&sz, VEC_ACCESS(v0) - VEC_DATA_START);
    sz &= ~VEC_PREALLOC;
    for (i = 0; i < sz; i++) {
	if (v0[i] == NULL)
	    continue;
	VEC_COPY_DT(&otherVecInfo, v0[i]);
	if (otherVecInfo & VEC_ARRAY) {
	    free(VEC_ACCESS(v0[i]) - VEC_DATA_START);
	}
	v0 += i;
	vec_DELETE(&v0);
	v0 -= i;
    }
    free(VEC_ACCESS(*vec) - VEC_DATA_START);
    *vec = NULL;
}
*/
int main(void) {
    int num[1024] = {0};
    int p = 0;
    VEC_szType x;
    void **vec = VEC_create();
    void **new;
    int data[][6] = {
	{0, 2, 4, 6, 8, 10}, {1, 3, 5, 7, 9, 11},
	{2, 3, 5, 7, 11, 13}, {4, 16, 32, 64, 128},
	{3, 6, 9, 12, 15, 18}, {7, 14, 21, 28, 35, 42},
	{10, 20, 30, 40, 50, 60}, {15, 45, 60, 75, 90, 105},
	{20, 40, 60, 80, 100, 120}, {50, 100, 150, 200, 250, 300},
	{0, 0, 0, 0, 0, 0}, {1, 1, 1, 1, 1, 1}
    };
    //  vec_ADD(vec_ADD(void ***vec, void *vd, size_t bytesz, size_t sz, size_t index, uint8_t type) {)
    VEC_add(&vec, data[0], sizeof(int), sizeof data[0], 0, VEC_ARRAY);
    VEC_add(&vec, data[1], sizeof(int), sizeof data[1], 1, VEC_ARRAY);

    new = VEC_add(&vec, data[2], sizeof(int), sizeof data[2], 2, VEC_VECTOR);
    if (new != 0)
	VEC_add(&new, data[3], sizeof(int), sizeof data[3], 1, VEC_ARRAY);

    new = VEC_add(&new, data[4], sizeof(int), sizeof data[4], 3, VEC_VECTOR);
    if (new != 0)
	VEC_add(&new, data[5], sizeof(int), sizeof data[5], 1, VEC_ARRAY);
    p =  (VEC_ACCESS(vec) - 1)[0] & 0x0f;
    
    memcpy(&x, VEC_BLOCK_START(vec, p), p);
    printf("%u\n", x & ~VEC_PREALLOC);

    p = 3;
    VEC_SZ_INCR(&p, sizeof p);
    printf("%d\n", new == 0);
    //free((char *)vec - VEC_DATA_START);
    //vec = (void *)((char *)vec);
    //vec_DELETE(&vec);
    //printf("%ld\n", (long)0);
}
/*
[
 [c, c, c], [c, c, c], [
			 [c, c, c], [c, c, c], [
					  [c, c, c], [c, c, c]
					      ]
		       ]
];
*/
