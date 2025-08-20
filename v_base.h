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

#if __GNU_LLVM__
    #define FORCEI_PURE __attribute__((pure, always_inline))
    #define LIKELY___(x, p) __builtin_expect(x, p)
#elif __WINDOWS__
    #define FORCEI_PURE __forceinline
    #define LIKELY___()
#else
    #define FORCEI_PURE
    #define LIKELY___()
#endif
#define INLINE(T) __inline__ FORCEI_PURE T

#if __GNU_LLVM__ || ((-16 >> 1) == -8 && ((-1 & (16 - 1)) == 15))
// Check if compiler supports correct bit operations on signed bits (according to the standard, this is implementation defined)
// Test is still untrusted since CPP may implemented by a different implementator from that of the actual compiler, but it is unlikely
    #define COMPILER_SUPPORT_SIGNED_BIT_OP 1
#else
    #define COMPILER_SUPPORT_SIGNED_BIT_OP 0
#endif

// We require an integer of bit-width W, wide enough to accomodate the underlying bit-width of float or double
#if !defined(UINT64_MAX) || defined(USE_FLOAT32__)
    #define DISABLE_DBL_SUPPORT__
    #ifndef UINT32_MAX
        #error "No float32 or float64 support"
    #endif
    #define UINT_ uint32_t
    #define INT_  int32_t
#else
    #define UINT_ uint64_t
    #define INT_  int64_t
#endif

// Fallback to float32: for systems without double (rarely, but some embedded system); Manually disabling use of double by defining the macro USE_FLOAT32__
#if (defined(FLT_DIG) && defined(DBL_DIG) && (DBL_DIG == FLT_DIG)) || defined(DISABLE_DBL_SUPPORT__)
    #define DBLT__      float
    #define DFLT_BIAS__ 127ul
    #define DBLT_MANT_SHFT   23
    #define Precalc_AllOnesMantBits 0x3ffffful
#else
    #define DBLT__      double
    #define DFLT_BIAS__ 1023ull
    #define DBLT_MANT_SHFT   52
    #define Precalc_AllOnesMantBits 0xfffffffffffffull
#endif

/* Vector Metadata */
typedef unsigned long vsize_t;

typedef struct {
  vsize_t __cap; /* Capacity */
  vsize_t __used; /* Total of capacity used */
  vsize_t __dtype; /* sizeof data Type */
} VEC_metaData_;

/* V_BASE_C */
typedef union {
  DBLT__  F;
  UINT_   N;
} bits_t;

typedef struct {
  bits_t  fmt_Num;
  int16_t fmt_Exp;
  int16_t fmt_Err;
} fmt;

typedef struct {
  char    *Pp_buf, *Pp_fmt;
  vsize_t  Pp_size, Pp_used;
  uint16_t Pp_mask, Pp_dtype;
  uint8_t  Pp_skip, Pp_overflw;
} Pp_Setup;

void   reprfloat   (bits_t bits);
DBLT__ reprexp     (bits_t bits);
DBLT__ xpow10__    (const DBLT__ x);
DBLT__ exp___      (const DBLT__ x);
DBLT__ exp__       (const DBLT__ x);
vsize_t VEC_Repr(void *v, Pp_Setup *setup);

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
  VEC_INTERNAL_create(SZ, MvpgMacro_select(sizeof(T), 0, T))

#define VEC_newFrmSize(SZ, SZOF)\
  VEC_INTERNAL_create(SZ, MvpgMacro_select(SZOF, 0, SZOF))


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
  MvpgMacro_select(VEC_popi, VEC_popni, __VA_ARGS__)(V, __VA_ARGS__)

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
	F(*Vv MvpgMacro_vaopt(,__VA_ARGS__));			\
    }								\
  } while (0)

#define VEC_slice(V, S, E)			\
   VEC_INTERNAL_slice(&V, S, E)

#define VEC_shrink(V, ...)\
  MvpgMacro_ignore(							\
		   (V != NULL) && VEC_INTERNAL_shrink(&V, MvpgMacro_select((__VA_ARGS__), VEC_vused(V), __VA_ARGS__)) \								)

#define VEC_del(V, I)				\
       VEC_INTERNAL_del(&V, I, I < 0)

#define VEC_clear(V)\
  ( VEC_vused(V) = 0)

#define VEC_sort(V)				\
  PASS

#define VEC_destroy(V)							\
     MvpgMacro_ignore(V != NULL ? mvpgDealloc(VEC_mv2blkst(V)), (V = NULL) : PASS)


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
