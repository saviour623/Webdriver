/* MVPG API Vector Type
Copyright (C) 2025 Michael Saviour

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef V_BASE_H
#define V_BASE_H
#define VEC_INTERNAL_CCS  // Visible

#include "include.h"
#include "memtool.h"

#define VSIZE_MAX ULONG_MAX

typedef unsigned long vsize_t;

/* Vector Metadata */
typedef struct {
  vsize_t __cap; /* Capacity */
  vsize_t __used; /* Total of capacity used */
  vsize_t __dtype; /* sizeof data Type */
} VEC_metaData_;

/* Metadata size */
static const uint16_t VEC_metadtsz       = sizeof(VEC_metaData_);
static const vsize_t  VEC_sizeOverflwLim = ULONG_MAX & ~LONG_MAX;
 /* Access: (sizeof(N) >> 2) */

/***********************************************************

 * Methods: MACRO

************************************************************/

/* Utils */
#ifndef VEC_UNSAFE
    #define VEC_assert(expr, ...) debugAssert(expr, __VA_ARGS__)
#else
    #define VEC_assert(...) PASS
#endif
#define VEC_NsizeOverflow(N) !(N & VEC_sizeOverflwLim)

/* Types Cvt */
#define VEC_type(T) T*

#define VEC_refType(T) T**

#define VEC_typeCast(V, T) \
  ((VEC_type(T))(V))

#define VEC_metaDataType(V)			\
  ( (VEC_metaData_ *)(void *)(V) )

#define VEC_voidptr(V)				\
  ( (void *)(uintptr_t)(V) )

/* Header Op */
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

/* Header contents Op */
#define VEC_base(V)				\
  ( &(V) )

#define VEC_vsize(V)				\
  VEC_fromMetaDataGet(V).__cap

#define VEC_vused(V)				\
  VEC_fromMetaDataGet(V).__used

#define VEC_vdtype(V)				\
  VEC_fromMetaDataGet(V).__dtype

/* Read Only */
#define VEC_size(V)\
  (VEC_vsize(V) | 0)

#define VEC_used(V)\
  (VEC_vused(V) | 0)

#define VEC_sizeof(V)\
  (VEC_vdtype(V) | 0)

#define VEC_isempty(V)\
  !!( VEC_used(v) )

#define VEC_isfilled(V)\
  (VEC_used(V) == VEC_size(V))

/* Vector Init */
#define VEC_new(SZ, T, ...)						\
  VEC_INTERNAL_create(SZ, MvpgMacro_Select(sizeof(T), 0, T))

#define VEC_newFrmSize(SZ, SZOF)\
  VEC_INTERNAL_create(SZ, MvpgMacro_Select(SZOF, 0, SZOF))


/* Vector Op */
#define VEC_begin(V)				\
  ( V )

#define VEC_end(V)				\
  ( V + VEC_vused(V))

#define VEC_front(V)				\
  (( V )[0] | 0)

/* Get last item of vector, equivalent to vec_front if vector is empty */
#define VEC_back(V)				\
  ( ( V )[VEC_vused(V) - !!VEC_used(V)] | 0)

#define VEC_free(V)				\
  ( VEC_vsize(V) - VEC_vused(V) )

#define VEC_push(V, N)							\
  (									\
   VEC_assert((V != NULL) && (VEC_vdtype(V) == sizeof(N))),		\
									\
   ( (VEC_vsize(V) < 1) || (VEC_vsize(V) == VEC_vused(V)) ) && VEC_INTERNAL_resize(V, 1), \
									\
   ((V)[VEC_vused(V)++] = (N))						\
  )

#define VEC_popni(V, ...)				\
  (\
   VEC_assert((V) != NULL && VEC_vused(V) > 0),	\
   (V)[--VEC_vused((V))]		  \
  )

#define VEC_popi(V, I, ...)						\
  (								\
   VEC_del(&V, I, I < 0)					\
  )

#define VEC_pop(V, ...)\
  MvpgMacro_Select(VEC_popi, VEC_popni, __VA_ARGS__)(V, __VA_ARGS__)

#define VEC_insert(V, N, I)			\
   (void)((V)[VEC_cvtindex(V, I, (I) < 0)] = (N))

#define VEC_append(V1, V2)			\
   VEC_INTERNAL_append(V1, V2)

#define VEC_foreach(S, V, T)						\
  for (VEC_type(T) K = VEC_begin(V); T S; (S = *K++) != VEC_end(V); )

/*
 * Vec_map iterates over a vector object, calling a function on each member.
 * The V, F, T is the vector, function, and type. Other arguments to the function are paassed through as varargs.
 */
#define VEC_map(V, F, T, ...)					\
  do {								\
    VEC_type(T) Vv = V;						\
    if (Vv != NULL && F != NULL) {				\
      VEC_assert(VEC_vdtype(Vv) == sizeof(T));			\
								\
      for (VEC_type(T) Last = Vv + VEC_vused(Vv) - 1; Vv != Last; Vv++)	\
	F(*Vv MvpgMacro_Vaopt(,__VA_ARGS__));			\
    }								\
  } while (0)

#define VEC_slice(V, S, E)			\
   VEC_INTERNAL_slice(&V, S, E)

#define VEC_shrink(V, ...)\
  MvpgMacro_Ignore(							\
		   (V != NULL) && VEC_INTERNAL_shrink(&V, MvpgMacro_Select((__VA_ARGS__), VEC_vused(V), __VA_ARGS__)) \								)

#define VEC_del(V, I)				\
       VEC_INTERNAL_del(&V, I, I < 0)

#define VEC_clear(V)\
  ( VEC_vused(V) = 0)

#define VEC_sort(V)				\
  PASS

#define VEC_destroy(V)							\
     MvpgMacro_Ignore(V != NULL ? mvpgDealloc(VEC_mv2blkst(V)), (V = NULL) : PASS)


/*************************************************************

 * Methods: Functions

 ************************************************************/

__STATIC_FORCE_INLINE_F __NONNULL__ vsize_t VEC_cvtindex(const void *v, vsize_t i, bool lt) {

       i = lt ? (long)i + VEC_vsize(v) : i;
       VEC_assert( VEC_NsizeOverflow(i) && (i < VEC_vsize(v)) );

       return i;
     }

__STATIC_FORCE_INLINE_F __WARN_UNUSED__ void *VEC_INTERNAL_create(const vsize_t size, const vsize_t dtype) {
  void *v;

  VEC_assert ( dtype );
  v = mvpgAlloc(__bsafeUnsignedMulAddl(dtype, size, VEC_metadtsz), VEC_metadtsz);
  VEC_vsize(v)  = size;
  VEC_vdtype(v) = dtype;

  return v;
}

__STATIC_FORCE_INLINE_F void *VEC_INTERNAL_resize(void *v, const vsize_t size) {
  PASS;
}

__STATIC_FORCE_INLINE_F __NONNULL__ __WARN_UNUSED__ void *VEC_INTERNAL_append(void *va, void *vb) {
  PASS;
}
__STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_INTERNAL_shrink(void *v, vsize_t shrinkSize) {
  void *p;

  p = VEC_INTERNAL_create(shrinkSize, VEC_vdtype(v)); /* Create a new vector with shrinkSize (Realloc) */
  memcpy(p, v, __bsafeUnsignedMull(shrinkSize, VEC_vdtype(v)));  /* Copy old to new */
  VEC_vused(p) = shrinkSize < VEC_vused(v) ? shrinkSize : VEC_vused(v);
  VEC_destroy(v); /* Remove old object */

  return p;
}

__STATIC_FORCE_INLINE_F __NONNULL__ __WARN_UNUSED__ void *VEC_INTERNAL_slice(void **v, vsize_t b, vsize_t e) {
  /* Slice items from index b to e, returning a new vector of sliced items */

  void *new __MB_UNUSED__;

  if (! ((b < VEC_vused(*v)) && (e < VEC_vused(*v)) && (e > b)) )
    return NULL;

  // new = VEC_newFrmSize((e - b), VEC_vdtype(*v));

  memcpy(new, (*v + b), e);
  memmove(*v + b, *v + e, __bsafeUnsignedMull(VEC_vdtype(*v), (e - b)));
  VEC_vused(v) -= e - b;
  return new;
}

__NONNULL__ __STATIC_FORCE_INLINE_F void VEC_INTERNAL_del(void *v, vsize_t i) {

  /* vsize_t mvby = (VEC_vsize(v) - i - 1) * VEC_vdtype(v); */
  /* mvby ? memmove(v + i, (v + i) + 1, mvby) /\* Shift memory to left *\/ */
  /*   : ((v)[i] = (void *)MEMCHAR); /\* Last index: reusable *\/ */
  VEC_vused(v)--;
}

#endif /* V_BASE_H */
