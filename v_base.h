
/* MVPG API Vector Type
Copyright (C) 2025 Michael Saviour

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef V_BASE_H
#define V_BASE_H

#include "include.h"
#include "memtool.h"

/* Vector Metadata */
#define vsize_t unsigned long

typedef struct {
  vsize_t __cap; /* Capacity */
  vsize_t __used; /* Total of capacity used */
  vsize_t __dtype; /* sizeof data Type */
} VEC_metaData_;

 /* Metadata size */
const uint16_t VEC_metadtsz  = sizeof(VEC_metaData_);

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
  ( VEC_metaDataType(V) - 1 )

#define VEC_fromMetaDataGet(V)			\
  ( VEC_peekblkst(V)[0] )

#define VEC_mv2blkst(V)				\
  (						\
   (V) = VEC_voidptr( VEC_peekblkst(V) )	\
    )

#define VEC_mv2MainBlk(V)			\
  (						\
   (V) = VEC_voidptr( VEC_metaDataType(V) + 1 )	\
    )

#define VEC_size(V)				\
  VEC_fromMetaDataGet(V).__cap

#define VEC_used(V)				\
  VEC_fromMetaDataGet(V).__used

#define VEC_dtype(V)				\
  VEC_fromMetaDataGet(V).__dtype

#define VEC_tmp(V)				\
  VEC_fromMetaDataGet(V).tmp

#define VEC_begin(V)\
  ( V )
#define VEC_end(V)\
  ( V + VEC_used(V) - 1)

#define VEC_front(V)				\
  ( V )[0]

#define VEC_back(V)				\
  ( V )[VEC_used(V) - 1]

#define VEC_free(V)				\
  ( VEC_size(V) - VEC_used(V) )

#define VEC_new(_size, _type, ...)						\
  VEC_INTERNAL_create(MACR_DO_ELSE(_size, 32, _size), MACR_DO_ELSE(sizeof(_type), 0, _type))

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

#define VEC_pop_i(V, I)					\
  (								\
   (V) = VEC_del(V, VEC_iabs(I)),					\	\
  )

#define VEC_pop(V, ...)\
   MACR_DO_ELSE(VEC_pop_i(V, __VA_ARGS__), VEC_pop_i(V), __VA_ARGS__)

#define VEC_insert(V, N, I)			\
   (void)((V)[VEC_cvtindex(V, I, (I) < 0)] = (N))

#define VEC_del(V, I)				\
   VEC_INTERNAL_del(&V, I)

#define VEC_append(V1, V2)			\
   VEC_INTERNAL_append(V1, V2)

#define VEC_slice(V, S, E)			\
   VEC_INTERNAL_slice(&V, S, E)

#define VEC_shrink(V, ...) (void)			\
  (									\
   (V != NULL)								\
   && (									\
       (V) = VEC_INTERNAL_shrink(V, MACR_DO_ELSE(__VA_ARGS__, VEC_used(V), __VA_ARGS__))\
      ) \
  )

#define VEC_destroy(V)\
  (void)(V != NULL ? mvpgDealloc(VEC_mv2blkst(V)), (V = NULL) : (void)0)


#define VEC_foreach(S, V, T)						\
  for (MACR_DO_ELSE(VEC_type(T), TYPEOF(V), T) K = VEC_begin(V), S = 0; S = K[0], K != VEC_end(V); S++)

#define VEC_map(F, V, ...)				\
   do {						\
     if (V != NULL && F != NULL)		\
       for (vsize_t i = 0; i < VEC_used(V); i++)	\
	 F(V[i]);				\
   }

/*************************************************************

 * Methods: Functions

 ************************************************************/

  __STATIC_FORCE_INLINE_F __NONNULL__ vsize_t VEC_cvtindex(const void *v, vsize_t i, vsize_t lt) {
  register vsize_t _i;

  _i = lt ? (vsize_t)i + VEC_size(v) : i;
  assert( ((_i > 0) && _i < VEC_size(v)) );

  return _i;
}

__STATIC_FORCE_INLINE_F __WARN_UNUSED__ void *VEC_INTERNAL_create(const vsize_t size, const vsize_t dtype) {
  void *v;

  assert ( dtype );

  v = mvpgAlloc(__bsafeUnsignedMulAddl(dtype, size, VEC_metadtsz), VEC_metadtsz);
  VEC_size(v)  = size;
  VEC_dtype(v) = dtype;

  return v;
}
__STATIC_FORCE_INLINE_F __NONNULL__ __WARN_UNUSED__ void *VEC_INTERNAL_append(void *va, void *vb) {
  PASS;
}
__STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_INTERNAL_shrink(void *v, vsize_t shrinkSize) {
  void *p;

  /**
     Shrink/Expand the capacity of Vector
   */

  if (shrinkSize < VEC_used(v))
    VEC_used(v) = shrinkSize;

  VEC_size(v) = shrinkSize;
  __bMulOverflow(VEC_dtype(v), shrinkSize, &shrinkSize);

  /* Copy v shrinked to size (Realloc) */
  p = mvpgAlloc(shrinkSize, 0);
  memcpy(p, VEC_mv2MainBlk(v), shrinkSize);
  VEC_destroy(v);

  return VEC_mv2MainBlk(p);
}

__STATIC_FORCE_INLINE_F __NONNULL__ __WARN_UNUSED__ void *VEC_INTERNAL_slice(void **v, vsize_t b, vsize_t e) {
  /* Slice items from index b to e, returning a new vector of sliced items */
  void *new __MB_UNUSED__;
  vsize_t sz __MB_UNUSED__;

  if(! ((b < VEC_used(*v)) && (e < VEC_used(*v)) && (e > b)) )
    return NULL;

  new = 0;
  //new = VEC_newFrmSize((e - b), VEC_dtype(*v));

  __bMulOverflow(VEC_dtype(*v), (e - b), &sz);
  memcpy(new, (*v + b), e);

  memmove(*v + b, *v + e, sz);
  VEC_used(v) -= (e - b);
  return new;
}

__NONNULL__ void VEC_INTERNAL_del(void *v, vsize_t i) {

  /* vsize_t mvby = (VEC_size(v) - i - 1) * VEC_dtype(v); */
  /* mvby ? memmove(v + i, (v + i) + 1, mvby) /\* Shift memory to left *\/ */
  /*   : ((v)[i] = (void *)MEMCHAR); /\* Last index: reusable *\/ */
  VEC_used(v)--;
}

/* Make ReadOnly */
#undef  VEC_size
#define VEC_size(V)   (VEC_fromMetaDataGet(V).__cap + 0)

#undef  VEC_used
#define VEC_used(V)   (VEC_fromMetaDataGet(V).__used + 0)

#define VEC_sizeof(V) (VEC_fromMetaDataGet(V).__dtype + 0)

/* No need of these */
#undef  VEC_mv2blkst
#undef  VEC_mv2MainBlk
#undef  VEC_dtype
#undef  VEC_newFrmSize
#undef  vsize_t

#endif /* V_BASE_H */
