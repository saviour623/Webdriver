#ifndef V_BASE_H
#define V_BASE_H

#include "include.h"

/* Vector Metadata */
typedef struct {
  size_t cap; /* Capacity */
  size_t used; /* Total of capacity used */
  size_t dtype; /* sizeof data Type */
  size_t tmp;   /* Temporary variable */
} VEC_metaData_;

 /* Metadata size */
const uint16_t VEC_metadtSz  = sizeof(VEC_metaData_);

enum {
      VEC_MIN_SIZE         = 0x100,
      VEC_ALLOC_SIZE       = 0x01,
      VEC_MEMFILL          = 0x01,
      VEC_REM_OPTMZ        = USHRT_MAX
};


/***********************************************************

 * Methods: MACRO

************************************************************/

#define VEC_type(T) T*

#define VEC_metaDataType(V)			\
  ( (VEC_metaData_ *)(void *)(V) )

#define VEC_voidptr(V)				\
  ( (void *)(V) )

#define VEC_peekblkst(V)			\
  ( (VEC_metaDataType(V)) - 1 )

#define VEC_fromMetaDataGet(V)			\
  ( VEC_peekblkst(V)[0] )

#define VEC_mv2blkst(V)				\
  ( (V) = VEC_voidptr( VEC_peekblkst(V) ) )

#define VEC_mv2MainBlk(vec)						\
  ( (vec) = VEC_voidptr( (VEC_metaDataType(V) + 1) ) /* Move pointer to data section */

#define VEC_size(V)				\
  VEC_fromMetaDataGet(V).cap

#define VEC_used(V)				\
  VEC_fromMetaDataGet(V).used

#define VEC_dtype(V)				\
  VEC_fromMetaDataGet(V).dtype

#define VEC_tmp(V)				\
  VEC_fromMetaDataGet(V).tmp

#define VEC_front(V)				\
  ( V )

#define VEC_back(V)				\
  ( V + (VEC_used(V) - 1) )

#define VEC_free(V)				\
  ( VEC_size(V) - VEC_used(V) )

#define VEC_new(_size, _type, ...)						\
  VEC_create(MACR_DO_ELSE(_size, 32, _size), MACR_DO_ELSE(sizeof(_type), 0, _type))

#define VEC_newFrmSize(_size, _dsize, ...)					\
  VEC_create(MACR_DO_ELSE(_size, 32, _size), MACR_DO_ELSE(_dsize, 0, _dsize))

#define VEC_push(V, N)							\
  (									\
   assert((V != NULL) && (VEC_dtype == sizeof(N))),			\
   ((VEC_size(V) < 1) || (VEC_size(V) == VEC_used(V)) ) && VEC_resize(V, 1), \
   ((V)[++VEC_used(V) - 2] = (N))\
  )

#define VEC_pop_ni(V)				\
  (\
   assert((V) != NULL && VEC_size(V) > 0),\
   (V)[--VEC_used((V))]		  \
  )

#define VEC_pop_i(V, N, I)					\
  (								\
   (VEC_tmp(V) = (V)[VEC_cvtindex(V, I, (I) < 0)]),		\
    VEC_del(V, VEC_iabs(I)),					\
   VEC_tmp(V)						\
  )

#define VEC_pop(V, ...)\
   MACR_DO_ELSE(VEC_pop_i(V, __VA_ARGS__), VEC_pop_i(V), __VA_ARGS__)

#define VEC_insert(V, N, I)			\
   (void)((V)[VEC_cvtindex(V, I, (I) < 0)] = (N))

#define VEC_del(V, I)				\
   VEC_delInternal(&V, I)

#define VEC_append(V1, V2)			\
   VEC_appendInternal(V1, V2)

#define VEC_slice(V, S, E)			\
   VEC_sliceInternal(&V, S, E)

#define VEC_shrink(V, ...) (void)			\
  (\
   (V != NULL) && (\
		   (V) = VEC_shrinkInternal(V, MACR_DO_ELSE(__VA_ARGS__, VEC_used(V), __VA_ARGS__))\
		   )							\
   )

#define VEC_destroy(V)\
  (void)(V != NULL ? free(VEC_mv2blkst(V)), (V = NULL) : (void)0)


#define VEC_foreach(S, V, T)						\
  for (MACR_DO_ELSE(VEC_type(T), TYPEOF(V), T) S = V; S != VEC_back(V); S++)

#define VEC_map(F, V, ...)				\
   do {						\
     if (V != NULL && F != NULL)		\
       for (size_t i = 0; i < VEC_used(V); i++)	\
	 F(V[i]);				\
   }

/*************************************************************

 * Methods: Functions

 ************************************************************/

  __STATIC_FORCE_INLINE_F __NONNULL__ size_t VEC_cvtindex(const void *v, size_t i, size_t lt) {
  register size_t _i;

  _i = lt ? (ssize_t)i + VEC_size(v) : i;
  assert( ((_i > 0) && _i < VEC_size(v)) );

  return _i;
}

__STATIC_FORCE_INLINE_F __WARN_UNUSED__ void *VEC_create(const size_t size, const size_t dtype) {
  void *v;

  assert ( dtype );

  mvpgAlloc(&v, safeUnsignedMulAdd(dtype, size, VEC_metadtSz), 0);
  VEC_mv2MainBlk(v);
  VEC_size(v)  = size;
  VEC_dtype(v) = dtype;

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


#endif /* V_BASE_H */
