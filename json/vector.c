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
#define CHOOSE_EXPR(cExpr, tVal, fVal)		\
    __builtin_choose_expr(cExpr, tVal, fVal)

typedef uint32_t VEC_szType; 
#define VEC_PREALLOC (uint32_t)0x80000000 /* 2 ^ 31 */
#define VEC_META_DATA_SZ 4 /*sizeof (uint32) */
#define VEC_DATA_START 4
#define VEC_ALLOC_SZ 1024
#define VEC_APPEND 268
#define VEC_VECTOR 2

#define VEC_READ_META_DATA(vec, to) memcpy((to), (uint8_t *)(vec) - VEC_DATA_START, VEC_META_DATA_SZ)
#define VEC_WRITE_META_DATA(vec, to) memcpy((uint8_t *)(vec) - VEC_DATA_START, (to), VEC_META_DATA_SZ)

static __inline__ __FORCE_INLINE__ void *VEC_szcpy(void *dest, void *src, size_t sz) {
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
    void *vec_0;
    VEC_szType sz;

    if (! (vec_0 = malloc((sizeof(void *) * VEC_ALLOC_SZ) + VEC_META_DATA_SZ)))
	return NULL;
    sz = VEC_ALLOC_SZ > 1 ? (1 | VEC_PREALLOC) : 1;
    vec_0 = (uint8_t *)vec_0 + VEC_DATA_START;
    VEC_WRITE_META_DATA(vec_0, &sz);
    *(void **)vec_0 = NULL;
    return vec_0;
}

static __inline__  __NONNULL__ void **vecAppend(void ***vec, void *new, VEC_szType sz) {
    void *v0;

    v0 = *vec ? realloc((uint8_t *)*vec - VEC_DATA_START, (sizeof (void *) * ((sz & ~VEC_PREALLOC) + VEC_ALLOC_SZ) + VEC_META_DATA_SZ)) : NULL;
    if ((v0 == NULL) || (sz < 0))
	return (void **)NULL;
    *vec = (void **)((uint8_t *)v0 + VEC_DATA_START);
    (*vec)[sz - 1] = new;
    sz += VEC_ALLOC_SZ;
    if (VEC_ALLOC_SZ > 1) {
	sz |= VEC_PREALLOC;
    }
    VEC_WRITE_META_DATA(*vec, &sz);
    return *vec;
}

/* The sum of preallocated block: ((sz / pre_alloc_sz) + (1 or 0)) * pre_alloc_sz; where the +1 accounts for the effect of the remainder when sz is not a multiple of pre_alloc_sz */
#define NUMBER_OF_PREALLOC_FROM_SZ(sz) (((sz / VEC_ALLOC_SZ) + !!(sz % VEC_ALLOC_SZ)) * VEC_ALLOC_SZ)

static __inline__  __NONNULL__ void **vecExpand(void ***vec, void *vd, size_t index, size_t vflag) {
    void *v0;
    VEC_szType sz, fl;

    VEC_READ_META_DATA(*vec, &sz);
    fl = sz & VEC_PREALLOC;
    sz = sz & ~VEC_PREALLOC;
    if (**vec == NULL){
        **vec = vd;
	sz++;
    }
    else if (index < sz) {
    	(*vec)[index] = vd;
    }
    else if (fl && (index < (NUMBER_OF_PREALLOC_FROM_SZ(sz)))) {
	(*vec)[index] = vd;
	sz = (sz + 1) | fl;
	VEC_WRITE_META_DATA(*vec, &sz);
    }
    else if (! ((vflag & VEC_APPEND) && (*vec = vecAppend(vec, vd, sz)))) {
	return NULL;
    }
    return *vec;
}

static __inline__ __NONNULL__ void *vec_ADD(void ***vec, void *vd, size_t bytesz, size_t sz, size_t index) {
    void **v0;
    size_t nalloc;

    nalloc = bytesz * sz; /* sizeof(vd) * (n + prealloc) */
    if (*vec == NULL || nalloc == 1 || !(v0 = vecT()) || !(*v0 = malloc(nalloc + VEC_META_DATA_SZ))) {
	return NULL;
    }
    *(VEC_szType *)*v0 = (VEC_szType)sz;
    *v0 = (uint8_t *)*v0 + VEC_META_DATA_SZ;
    memcpy(*v0, vd, nalloc);
    return vecExpand(vec, v0, index, VEC_APPEND);
}
static __inline__ __NONNULL__ void *vec_DELETE(void ***vec, void *vd, size_t bytesz, size_t sz, size_t index) {
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
	if (vec_ADD(&vec, num+p, sizeof(int), 1, p) == NULL) {
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
    //vecAppend(&vec, num, 1);
    //vecAppend(&vec, num, 2);
    //vecAppend(&vec, num, 3);
    //vecAppend(&vec, num, 4);
    memcpy(&x, (char *)vec - VEC_DATA_START, sizeof(VEC_szType));
    printf("%lu\n", (long)x & ~VEC_PREALLOC);
    free((char *)vec - VEC_DATA_START);
    //printf("%ld\n", (long)0);
}


/*
  Add a pointer to object that points to its parent. The base parent is Null
  this reduces the use of stack
 */
