#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define __FORCE_INLINE__ __attribute__((always_inline))
#define __NONNULL__ __attribute__((nonnull))
#define CHOOSE_EXPR(cExpr, tVal, fVal)		\
    __builtin_choose_expr(cExpr, tVal, fVal)

typedef unsigned long VEC_szType;
#define VEC_PREALLOC ((VEC_szType)LONG_MAX + 1)
#define VEC_META_DATA_SZ sizeof(VEC_szType)
#define VEC_DATA_START VEC_META_DATA_SZ
#define VEC_ALLOC_SZ 1024
#define VEC_APPEND 268
#define VEC_VECTOR 2

static __inline__ __FORCE_INLINE__ void *VEC_szcpy(void *dest, void *src, ssize_t sz) {
    if (sz == sizeof(VEC_szType)) {
        /* We are assuming memory alignment of src and dest to be same */
        *(VEC_szType *)dest = *(VEC_szType *)src;
    }
    else {
        unsigned char *_dest, *_src;

        _dest = (unsigned char *)dest, _src = (unsigned char *)src;
        while (sz--)
            *_dest++ = *_src++; 
    }
    return dest;
}

static __inline__ void **vecT(void) {
    void **vec_0;
    VEC_szType sz;

    vec_0 = malloc((sizeof(void *) * VEC_ALLOC_SZ) + VEC_META_DATA_SZ);

    if (vec_0 == NULL)
	return NULL;
    *(vec_0 + VEC_DATA_START) = NULL;
    sz = 1;
    if (VEC_ALLOC_SZ > 1) {
	sz |= VEC_PREALLOC;
    }
     memcpy(vec_0, &sz, sizeof(sz));

    return vec_0 ? vec_0 + VEC_DATA_START : NULL;
}

static __inline__  __NONNULL__ void **vecAppend(void ***vec, void *new, VEC_szType sz) {
    void **v0;

    v0 = *vec ? realloc(*vec - VEC_DATA_START, (sz & ~VEC_PREALLOC) + VEC_ALLOC_SZ + VEC_META_DATA_SZ) : NULL;
    if ((v0 == NULL) || (sz < 0))
	return (void **)NULL;
    *vec = v0;
    (*vec + VEC_DATA_START)[sz] = new;
    sz += VEC_ALLOC_SZ;
    if (VEC_ALLOC_SZ > 1) {
	sz |= VEC_PREALLOC;
    }
    memcpy(*vec, &sz, sizeof(VEC_szType));
    *vec += VEC_DATA_START;
}

/* The sum of preallocated block: ((sz / pre_alloc_sz) + (1 or 0)) * pre_alloc_sz; where the +1 accounts for the effect of the remainder when sz is not a multiple of pre_alloc_sz */
#define NUMBER_OF_PREALLOC_FROM_SZ(sz) (((sz / VEC_ALLOC_SZ) + !!(sz % VEC_ALLOC_SZ)) * VEC_ALLOC_SZ)

static __inline__  __NONNULL__ void **vecExpand(void ***vec, void *vd, ssize_t index, ssize_t vflag) {
    void *v0;
    VEC_szType sz, fl;

    memcpy(&sz, *vec - VEC_DATA_START, sizeof sz);
    fl = sz & VEC_PREALLOC;
    sz = sz & ~VEC_PREALLOC;
    if ((*vec)[0] == NULL)
        (*vec)[0] = vd;
    else if (index < sz)
    	(*vec)[index] = vd;
    else if (fl && (index < (NUMBER_OF_PREALLOC_FROM_SZ(sz)))) {
	(*vec)[index - 1] = vd;
	sz = (sz + 1) | fl;
	memcpy(*vec - VEC_DATA_START, &sz, sizeof(VEC_szType));
    }
    else if (! ((vflag & VEC_APPEND) && vecAppend(&vec, vd, sz))) {
	return NULL;
    }
    return *vec;
}

int main(void) {
    int num[1024] = {0};
    int p = 0;
    VEC_szType x;
    void **vec = vecT();

    
    if (vec == NULL)
	{
	    puts("null");
	    exit(-1);
	}
    while (p < 1024) {
	num[p] = p;
	p++;
    }
    p = 0;
    while (p < 1024) {
	if (vecExpand(&vec, num+p, p, VEC_APPEND) == NULL) {
	    puts("error");
	    exit(-1);
	}
	p++;
    }
    p = 0; 
    if (vec == NULL) {
	puts("null");
	exit(2);
    }
    memcpy(&x, vec - VEC_DATA_START, sizeof(long));
    printf("%lu\n", (long)x & ~VEC_PREALLOC);
    free(vec - VEC_DATA_START);
}

/*
 * MAIN_0
   ADR 0 - 3 -> META_DATA (first 4 bytes assuming sizeof int = 4)
   ADR 4 -> DATA || MAIN_1 -> META_DATA
                           -> DATA

   META DATA is unused DATA is null
 */
/*
  Add a pointer to object that points to its parent. The base parent is Null
  this reduces the use of stack
 */
