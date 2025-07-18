
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

/***********************************************************

 * Methods: MACRO

************************************************************/

#define VEC_Macro_Exec_True(...) __VA_ARGS__
#define VEC_Macro_Exec_False(...)

#define VEC_Macro_If_Args_Exec(...)					\
  VEC_Macro_Exec_ ## MACR_DO_ELSE(True, False, __VA_ARGS__) (__VA_ARGS__)

#define VEC_type(T) T*

#define VEC_refType(T) T**

#define VEC_metaDataType(V)			\
  ( (VEC_metaData_ *)(void *)(V) )

#define VEC_voidptr(V)				\
  ( (void *)(uintptr_t)(V) )

#ifndef VEC_UNSAFE
    #define VEC_assert(expr, ...) debugAssert(expr, __VA_ARGS__)
#else
    #define VEC_assert(...)
#endif

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
/**/

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

#define VEC_new(_size, _type, ...)						\
  VEC_INTERNAL_create(MACR_DO_ELSE(_size, 32, _size), MACR_DO_ELSE(sizeof(_type), 0, _type))

#define VEC_newFrmSize(_size, _dsize, ...)					\
  VEC_create(MACR_DO_ELSE(_size, 32, _size), MACR_DO_ELSE(_dsize, 0, _dsize))

#define VEC_push(V, N)							\
  (									\
  VEC_assert((V != NULL) && (VEC_vdtype(V) == sizeof(N)), \
   ((VEC_vsize(V) < 1) || (VEC_vsize(V) == VEC_vused(V)) ) && VEC_INTERNAL_resize(V, 1), \
   ((V)[VEC_vused(V)++] = (N))					\
  )

#define VEC_popni(V)				\
  (\
   VEC_assert((V) != NULL && VEC_vused(V) > 0),	\
   (V)[--VEC_vused((V))]		  \
  )

#define VEC_popi(V, I)					\
  (								\
   (V) = VEC_del(V, VEC_iabs(I)),					\
  )

#define VEC_pop(V, ...)\
  MACR_DO_ELSE(VEC_popi(V, __VA_ARGS__), VEC_popni(V), __VA_ARGS__)

#define VEC_insert(V, N, I)			\
   (void)((V)[VEC_cvtindex(V, I, (I) < 0)] = (N))

#define VEC_append(V1, V2)			\
   VEC_INTERNAL_append(V1, V2)

#define VEC_foreach(S, V, T)						\
  for (MACR_DO_ELSE(VEC_type(T), TYPEOF(V), T) K = VEC_begin(V), S = K[0]; K != VEC_end(V); S = *++k)

#define VEC_map(F, V, T, ...)			\
   do {						\
   VEC_type(T) Vv = V;
     if (Vv != NULL && F != NULL)		\
       for (vsize_t i = 0; i < VEC_vused(V); i++)	\
	 F(Vv[i] VEC_Macro_If_Args_Exec(__VA_ARGS__));	\
   }

#define VEC_slice(V, S, E)			\
   VEC_INTERNAL_slice(&V, S, E)

#define VEC_shrink(V, ...) (void)			\
  (									\
   (V != NULL)								\
   && (									\
       (V) = VEC_INTERNAL_shrink(V, MACR_DO_ELSE(__VA_ARGS__, VEC_vused(V), __VA_ARGS__))\
      ) \
  )

#if __GNUC_LLVM__
#define VEC_tgeneric(V)\
  __builtin_classify_type(TYPEOF(*V)))

#elif __STDC_GTEQ_11__
#else
     VEC_generic(V) VEC_UNKNOWN_TYPE
#endif
#define VEC_prettyPrint(V)						\
       do {								\
	 char bf[VEC_BUFFER_SIZE] = "<Object %s -> Vector(%s, %s)> [";	\
	 vsize_t type = tgeneric(V), bfcnt = 0;				\
	 bfcnt  = VEC_memAddr(v, bf + 31) + 31;				\
	 bfcnt += VEC_itoa(VEC_vsize(v), bf + bfcnt,  10);		\
	 bfcnt += VEC_itoa(VEC_vused(v), bf + bfcnt,  10);		\
	 for (;;) {							\
									\
	 }								\
       } while (0)

#define VEC_del(V, I)				\
       VEC_INTERNAL_del(&V, I)

#define VEC_clear(V)\
  ( VEC_vused(V) = 0)

#define VEC_sort(V)				\
  PASS

#define VEC_destroy(V)							\
  (void)(V != NULL ? mvpgDealloc(VEC_mv2blkst(V)), (V = NULL) : (void)0)



/*************************************************************

 * Methods: Functions

 ************************************************************/

  __STATIC_FORCE_INLINE_F __NONNULL__ vsize_t VEC_cvtindex(const void *v, vsize_t i, vsize_t lt) {
  register vsize_t _i;

  _i = lt ? (vsize_t)i + VEC_vsize(v) : i;
  VEC_assert( ((_i > 0) && _i < VEC_vsize(v)) );

  return _i;
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
  /* Shrink/Expand the capacity of Vector */

  void *p;

  /* Create a new vector with shrinkSize (Realloc) */
  p = VEC_INTERNAL_create(shrinkSize, VEC_vdtype(v));
  /* Copy old to new */
  memcpy(p, v, __bsafeUnsignedMull(shrinkSize, VEC_vdtype(v)));
  VEC_vused(p) = shrinkSize < VEC_vused(v) ? shrinkSize : VEC_vused(v);
  /* Destroy Old */
  VEC_destroy(v);

  return p;
}

__STATIC_FORCE_INLINE_F __NONNULL__ __WARN_UNUSED__ void *VEC_INTERNAL_slice(void **v, vsize_t b, vsize_t e) {
  /* Slice items from index b to e, returning a new vector of sliced items */

  void *new __MB_UNUSED__;

  if(! ((b < VEC_vused(*v)) && (e < VEC_vused(*v)) && (e > b)) )
    return NULL;

  new = 0;
  //new = VEC_newFrmSize((e - b), VEC_vdtype(*v));

  memcpy(new, (*v + b), e);

  memmove(*v + b, *v + e, __bsafeUnsignedMull(VEC_vdtype(*v), (e - b)));
  VEC_vused(v) -= e - b;
  return new;
}

__NONNULL__ void VEC_INTERNAL_del(void *v, vsize_t i) {

  /* vsize_t mvby = (VEC_vsize(v) - i - 1) * VEC_vdtype(v); */
  /* mvby ? memmove(v + i, (v + i) + 1, mvby) /\* Shift memory to left *\/ */
  /*   : ((v)[i] = (void *)MEMCHAR); /\* Last index: reusable *\/ */
  VEC_vused(v)--;
}

#define VEC_BUFFER_SIZE USHRT_MAX
#define VEC_MAX_INT_LEN 32
#define EOFMT(c, f) (((c) = (f)) & ((c) ^ 58))
#define VEC_appendComma(bf, i)\
  (((bf)[i] = ','), ((bf)[i + 1] = ' ', i + 2))

#define VEC_itoa(n, bf, base, negtv)			\
  cvtInt2Str(n, bf, base, negtv)

typedef struct {
  char   *Pp_buf;
  vsize_t Pp_size;
  vsize_t Pp_used;
  uint8_t Pp_base;
  uint8_t PP_signed;
  uint8_t Pp_overflw;
  uint8_t Pp_genrc;
} Pp_Setup;

__STATIC_FORCE_INLINE_F int VEC_TostrInt(void *v, const Pp_Setup *cf) {
  switch (( c = cf->width )) {
  case LONG:
    VEC_map(v, VEC_itoa, long, &setup);
  case LLNG:
    VEC_map(v, VEC_itoa, long long, &setup);
  case SIZE:
    VEC_map(v, VEC_itoa, size_t, &setup);
  default:
    VEC_map(v, VEC_itoa, int, &setup);
  }
}
__STATIC_FORCE_INLINE_F int VEC_TostrFlt(void *v, const Pp_Setup *cf) {
  switch (( c = mask & cf->width )) {
  case LONG:
    VEC_map(v, VEC_strtof, double, &setup);
  case LLNG:
    VEC_map(v, VEC_strtof, long double, &setup);
  default:
    VEC_map(v, VEC_strtof, float, &setup);
    break;
  }
}
__STATIC_FORCE_INLINE_F int VEC_str(char *__restrict__ str, const Pp_Setup *cf) {
  vsize_t j, e;
  char c, *bf;

  for (bf = cf->bf, e = bf->size, j = 0; (c = str[j]) && (j < e); j++) {
    bf[j] = c;
  }
  return j;
}

__NONNULL__ vsize_t VEC_INTERNAL_repr(char *v, char *fmt, char *bf, vsize_t bfsize) {
  /* Return the representation of vector */
  struct Pp_Setup setup = {bf, VEC_vused(v), 10, 0};
  uint16_t mask;
  uint8_t c, fc[15] = {0};

  enum {
	TYPE = 0x7f,  SPEC = 0xf00, BASE = 0x400
	LONG = 0x100, LLNG = 0x200, SIZE = 0x300,
  };

  fc[0] = *fmt++;
  EOFMT(c, *fmt++)   ? (fc[1]  = c), EOFMT(c, *fmt++) ? (fc[3]  = c),
    EOFMT(c, *fmt++) ? (fc[7]  = c), EOFMT(c, *fmt)   ? (fc[15] = c):
    PASS : PASS : PASS : PASS;


  mask =  ((fc[0] & 0x5fu)    == 76);
  mask |= ((fc[mask] & 76)    == 76) << 1;
  mask |= ((fc[mask] & 0x5fu) == 88) << 2;
  mask |= ((fc[mask] & 0x5fu) == 90) << 3;
  mask =  (mask << 8)| fc[mask];

  VEC_assert(c = (((mask & TYPE) ^ 0x73) ^ (mask & SPEC))
	     |   (((mask & TYPE) ^ 0x66) ^ (mask & BASE))
	     |   ( (mask & SPEC)         ^ (mask & TYPE)), "Invalid Format");

  switch (( c = mask & TYPE )) {
  case 'p':
    setup.Pp_genrc = true;
  case 'u':
    setup.Pp_signd = mask & UNSIGNED;
  case 'd':
  case 'i':
    setup.Pp_base  = mask & BASE;
    VEC_TostrInt(v, &setup);
    break;
  case 'f':
    VEC_TostrFlt(v, &setup);
  case 'c':
    setup.Pp_char = True;
  case 's':
    VEC_map(v)
  default :
    PASS;
  }

  return setup.used;
}
/* No need of these */
#undef  VEC_newFrmSize
#undef  vsize_t

#endif /* V_BASE_H */
