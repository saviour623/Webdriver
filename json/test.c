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

typedef uint32_t VEC_szType;
#define VEC_PREALLOC (uint32_t)0x80000000 /* 2 ^ 31 */
#define VEC_BYTE_SZ 4
#define VEC_DATA_BLOCK_SZ (sizeof (void *))
#define VEC_META_DATA_SZ 5/*VEC_BYTE_SZ + 1 */
#define VEC_DATA_START 5
#define VEC_ALLOC_SZ 1
#define VEC_APPEND 268
#define VEC_VECTOR 32
#define VEC_ARRAY 64

/* remove the type alignment of bytes so that each block can be addressed like a byte array */
#define VEC_ACCESS(_addr) ((uint8_t *)(void *)(_addr))

/* write vec_meta_data_sz bytes to a vector/vector-child block */
#define VEC_WRITE_META_DATA(vec, to) vec_CopyMetaData(VEC_ACCESS(vec) - VEC_DATA_START, (to))
/* copy vec_meta_data_sz bytes from vector/vector-child block to a different block */
#define VEC_COPY_META_DATA(vec, to) vec_CopyMetaData((to), VEC_ACCESS(vec) - VEC_DATA_START)
 /* copy size from meta-data block */
#define VEC_COPY_SZ(to, from) (*(VEC_szType *)(void *)(to) = *(VEC_szType *)(void *)(from))
/* copy other informations stored at the last byte of meta-block */
#define VEC_COPY_DT(to, from) (*(uint8_t *)(void *)(to) = (*(uint8_t *)(void*)(from)) + VEC_BYTE_SZ)
 /* move pointer ahead of the meta-data block */
#define VEC_MOVTO_DATA(vec) ((vec) = VEC_ACCESS(vec) + VEC_META_DATA_SZ)

/* copy both size and other info of meta-block to another dest */
static __inline__ __FORCE_INLINE__ void *vec_CopyMetaData(void *__restrict to, void *__restrict from) {
    uint8_t *_to, *_from;

    _to = to; _from = from;
    /* @to and @from must both be VEC_META_DATA_SZ bytes */
    *(VEC_szType *)_to = *(VEC_szType *)_from;
    *(_to + VEC_BYTE_SZ) = *(_from + VEC_BYTE_SZ);
    return _to;
}

static __inline__ void **vecT(void) {
    void *vec;
    VEC_szType sz;

    if (! (vec = malloc((VEC_DATA_BLOCK_SZ * VEC_ALLOC_SZ) + VEC_META_DATA_SZ)))
	return NULL;
    sz = (VEC_ALLOC_SZ > 1) ? ((VEC_szType)VEC_PREALLOC | 1) : 1;
    VEC_COPY_SZ(vec, &sz);
    /* move pointer to end of the meta-data block, which is the actual address of vec allowed to store data */
    VEC_MOVTO_DATA(vec);
/*
 * initializing the first block to 0. This indicates that a vector is empty.
 * A better way to check if a vector is empty is to assert if its of size 1 and its first member is null
 * This is needful because the initial size of a vector is 1. The block may have data written to it at any point
 */
    *(void **)vec = NULL;
    return vec;
}

static __inline__  __NONNULL__ void **vecAppend(void ***vec, void *new, VEC_szType sz) {
    void *v0;
    sz &= ~VEC_PREALLOC;
    /* dynamically resize vector to accomodate a new data */
    v0 = *vec ? realloc(VEC_ACCESS(*vec) - VEC_DATA_START, (VEC_DATA_BLOCK_SZ * (sz + VEC_ALLOC_SZ)) + VEC_META_DATA_SZ) : NULL;

    if (v0 == NULL)
	return (void **)NULL;
    sz += 1;
    if (VEC_ALLOC_SZ > 1) {
	sz |= VEC_PREALLOC;
    }
    VEC_COPY_SZ(v0, &sz);
    *vec = VEC_MOVTO_DATA(v0);
    (*vec)[sz - 1] = new;
    return *vec;
}

/*
 * The sum of preallocated block: ((sz / pre_alloc_sz) + (1 or 0))
 * pre_alloc_sz; where the +1 accounts for the effect of the remainder when sz is not a multiple of pre_alloc_sz
 * If sz is lesser than pre_alloc_sz, then there is only a single pre-allocation. if sz falls in the range of pre_alloc_sz and (n * pre_alloc_sz) then n allocations has been done
 */
#define NUMBER_OF_PREALLOC_FROM_SZ(sz) (((sz / VEC_ALLOC_SZ) + !!(sz % VEC_ALLOC_SZ)) * VEC_ALLOC_SZ)

static __inline__  __NONNULL__ void **vecExpand(void ***vec, void *vd, size_t index, size_t vflag) {
    void *v0;
    VEC_szType sz, fl;

    VEC_COPY_SZ(&sz, VEC_ACCESS(*vec) - VEC_DATA_START); /* read size into sz */
    fl = sz & VEC_PREALLOC; /* This is set if a preallocation for vector was made */
    sz = sz & ~VEC_PREALLOC;
    /* vector is empty. Index is ignored if non-zero */
    if ((*vec)[0] == NULL){
	(*vec)[0] = vd;
    }
    else if (index < sz) {
	    (*vec)[index] = vd;
    }
    else if (fl && (index < (NUMBER_OF_PREALLOC_FROM_SZ(sz)))) {
	/* vec has a preallocated sz */
	(*vec)[index] = vd;
	sz = (sz + 1) | fl;
	VEC_COPY_SZ(VEC_ACCESS(*vec) - VEC_DATA_START, &sz);
    }
    else if (! ((vflag & VEC_APPEND) && (*vec = vecAppend(vec, vd, sz)))) {
	return NULL;
    }
    return *vec;
}

static __inline__ __NONNULL__ void *vec_ADD(void ***vec, void *vd, size_t bytesz, size_t sz, size_t index, uint8_t type) {
    void *v0 /* ptr to memory block */, **v00 /* ptr to vector */;
    uint8_t metaData[VEC_META_DATA_SZ];
    size_t nalloc;

    nalloc = bytesz * sz; /* sizeof(vd) * sz */
    if (*vec == NULL || nalloc < 1 
	|| /* block alloc  */ !(v0 = malloc(nalloc + VEC_META_DATA_SZ))) {
	return NULL;
    }
    VEC_COPY_SZ(metaData, &sz);
    metaData[VEC_META_DATA_SZ - 1] = type;

    VEC_MOVTO_DATA(v0);
     /* copy data to memory including its meta-data */
    memcpy(v0, vd, nalloc);
    VEC_WRITE_META_DATA(v0, metaData);

    /* if vec_vector type is specified, create a new vector */
    if ((type & VEC_VECTOR) && (v00 = vecT())) {
	/* include the data to the new vector */
	v00[0] = v0;
	/* To retain the same interface for both non-vector or vector, reference vector through a void pointer, v0 */
	v0 = v00;
    }
    return v00 && vecExpand(vec, v0, index, VEC_APPEND) ? v0 : NULL;
}
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

int main(void) {
    int num[1024] = {0};
    int p = 0;
    VEC_szType x;
    void **vec = vecT();
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
    vec_ADD(&vec, data[0], sizeof(int), sizeof data[0], 0, VEC_ARRAY);
    vec_ADD(&vec, data[1], sizeof(int), sizeof data[1], 1, VEC_ARRAY);
    
    new = vec_ADD(&vec, data[2], sizeof(int), sizeof data[2], 2, VEC_VECTOR);
    if (new != 0)
	vec_ADD(&new, data[3], sizeof(int), sizeof data[3], 1, VEC_ARRAY);
    
    new = vec_ADD(&new, data[4], sizeof(int), sizeof data[4], 3, VEC_VECTOR);
    if (new != 0)
	vec_ADD(&new, data[5], sizeof(int), sizeof data[5], 1, VEC_ARRAY);
    
    memcpy(&x, (uint8_t *)vec - VEC_DATA_START, sizeof(VEC_szType));
    printf("%u\n", x & ~VEC_PREALLOC);
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
