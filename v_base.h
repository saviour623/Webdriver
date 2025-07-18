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
const uint16_t VEC_metadtsz       = sizeof(VEC_metaData_);
const vsize_t  VEC_sizeOverflwLim = ULONG_MAX & ~LONG_MAX

/***********************************************************

 * Methods: MACRO

************************************************************/

/* Utils */
#define VEC_Macro_Exec_True(...) __VA_ARGS__
#define VEC_Macro_Exec_False(...)
#define VEC_Macro_If_Args_Exec(...)					\
  VEC_Macro_Exec_ ## MACR_DO_ELSE(True, False, __VA_ARGS__) (__VA_ARGS__)

#ifndef VEC_UNSAFE
    #define VEC_assert(expr, ...) debugAssert(expr, __VA_ARGS__)
#else
    #define VEC_assert(...) PASS
#endif
#define VEC_IGNRET(...)       (void)(...)
#define VEC_NsizeOverflow(N) !(N & VEC_sizeOverflwLim)

/* Types Cvt */
#define VEC_type(T) T*

#define VEC_refType(T) T**

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
#define VEC_new(_size, _type, ...)					\
  VEC_INTERNAL_create(MACR_DO_ELSE(_size, 32, _size), MACR_DO_ELSE(sizeof(_type), 0, _type))

#define VEC_newFrmSize(_size, _dsize, ...)					\
  VEC_create(MACR_DO_ELSE(_size, 32, _size), MACR_DO_ELSE(_dsize, 0, _dsize))


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

#define VEC_push(V, N)\
  (\
   VEC_assert((V != NULL) && (VEC_vdtype(V) == sizeof(N))),\
   ( (VEC_vsize(V) < 1) || (VEC_vsize(V) == VEC_vused(V)) ) && VEC_INTERNAL_resize(V, 1), \
   ((V)[VEC_vused(V)++] = (N))\
   )

#define VEC_popni(V)				\
  (\
   VEC_assert((V) != NULL && VEC_vused(V) > 0),	\
   (V)[--VEC_vused((V))]		  \
  )

#define VEC_popi(V, I)					\
  (								\
   VEC_del(&V, I, I < 0)					\
  )

#define VEC_pop(V, ...)\
  MACR_DO_ELSE(VEC_popi(V, __VA_ARGS__), VEC_popni(V), __VA_ARGS__)

#define VEC_insert(V, N, I)			\
   (void)((V)[VEC_cvtindex(V, I, (I) < 0)] = (N))

#define VEC_append(V1, V2)			\
   VEC_INTERNAL_append(V1, V2)

#define VEC_foreach(S, V, T)						\
  for (MACR_DO_ELSE(VEC_type(T), TYPEOF(V), T) K = VEC_begin(V), S = K[0]; K != VEC_end(V); S = *++k)

#define VEC_map(F, V, T, ...)				\
  do {							\
      VEC_type(T) Vv = V;				\
      if (Vv != NULL && F != NULL) {			\
	VEC_assert(VEC_dtype(Vv) == sizeof(T));		\
	for (vsize_t end = 0; end--;)			\
	  F(*Vv++ VEC_Macro_If_Args_Exec(__VA_ARGS__));	\
      }							\
  }

#define VEC_slice(V, S, E)			\
   VEC_INTERNAL_slice(&V, S, E)

#define VEC_shrink(V, ...) (void)					\
    (									\
     (V != NULL)							\
     && (								\
	 (V) = VEC_INTERNAL_shrink(V, MACR_DO_ELSE(__VA_ARGS__, VEC_vused(V), __VA_ARGS__)) \
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
       VEC_INTERNAL_del(&V, I, I < 0)

#define VEC_clear(V)\
  ( VEC_vused(V) = 0)

#define VEC_sort(V)				\
  PASS

#define VEC_destroy(V)							\
     VEC_IGNRET(V != NULL ? mvpgDealloc(VEC_mv2blkst(V)), (V = NULL) : PASS)


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

  if (! ((b < VEC_vused(*v)) && (e < VEC_vused(*v)) && (e > b)) )
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

#define VEC_itoa cvtInt2Str

enum {
      PTR  = 0x01,  USIGNED = 0x02,  CHAR  = 0x04,
      LONG = 0x100, LLNG    = 0x200, SIZE = 0x800,
      TYPE = 0x7f,  SPEC    = 0xf00, BASE = 0x400
};

typedef struct {
  char    *Pp_buf;
  char    *Pp_ctrl;
  vsize_t  Pp_size;
  vsize_t  Pp_used;
  uint16_t Pp_mask;
  uint16_t Pp_dtype;
} Pp_Setup;

__NONNULL__ __STATIC_FORCE_INLINE_F void VEC_Itoa(const intmax_t n, const Pp_Setup *cf) {
  const int U = cf->Pp_used;

  if ((cf->Pp_size - U) < cf->Pp_overflw)
    return;
  cf->Pp_used += cvtTostrInt(n, cf->Pp_buf+U, cf->mask & BASE, !(cf->mask & UNSIGNED) && (n < 0));
}

__NONNULL__ __STATIC_FORCE_INLINE_F int VEC_TostrInt(const void *v, const Pp_Setup *cf) {

  switch (cf->mask & SPEC) {
  case LONG:
    VEC_map(v, VEC_Itoa, long, setup);
  case LLNG:
    VEC_map(v, VEC_Itoa, long long, setup);
  case SIZE:
    VEC_map(v, VEC_Itoa, size_t, setup);
  default:
    VEC_map(v, VEC_Itoa, int, setup);
  }
}

__NONNULL__ __STATIC_FORCE_INLINE_F int VEC_TostrFlt(const void *v, const Pp_Setup *cf) {
  switch (cf->mask & SPEC) {
  case LONG:
  case LLNG:
  default:
    PASS;
  }
  VEC_assert(false, "TOSTRFLT: UINMPLEMENTED");
}

__NONNULL__ vsize_t VEC_INTERNAL_repr(char *v, char *fmt, char *bf, vsize_t bfsize) {
  struct Pp_Setup setup = {.Pp_buf = bf, .Pp_size = bfsize, .Pp_dtype = VEC_dtype(v)};
  uint16_t mask;
  uint8_t c, fc[15] = {0};


  fc[0] = *fmt++;
  VEC_IGNRET(
	     EOFMT(c, *fmt++) ? (fc[1]  = c), EOFMT(c, *fmt++) ? (fc[3]  = c),
	     EOFMT(c, *fmt++) ? (fc[7]  = c), EOFMT(c, *fmt)   ? (fc[15] = c):
	     PASS : PASS : PASS : PASS);


  mask =  ((fc[0] & 0x5fu)    == 76);
  mask |= ((fc[mask] & 76)    == 76) << 1;
  mask |= ((fc[mask] & 0x5fu) == 88) << 2;
  mask |= ((fc[mask] & 0x5fu) == 90) << 3;
  mask =  (mask << 8)| fc[mask];

  VEC_assert(  (((mask & TYPE) ^ 0x73) ^ (mask & SPEC))
	     | (((mask & TYPE) ^ 0x66) ^ (mask & BASE))
	     | ( (mask & SPEC)         ^ (mask & TYPE))
	     |  EOFMT(c, *fmt), "Repr: Invalid Format");

  setup.Pp_mask = mask & SPEC;
  setup.Pp_ctrl = fmt;

  switch ( mask & TYPE ) {
  case 'p':
    setup.Pp_mask |= PTR;
  case 'u':
    setup.Pp_mask |= USIGNED;
  case 'd': case 'i':
    VEC_TostrInt(v, &setup);
    break;
  case 'f':
    VEC_TostrFlt(v, &setup);
    break;
  case 'c':
    mask |= CHAR;
  case 's':
    VEC_assert((setup.Pp_dtype > 1) && (setup.Pp_dtype != sizeof(VEC_type(char))), "Repr: Type Mismatch");

    if (setup.Pp_used > 0) {
      const char *Vv;
      register vsize_t j, i; const vsize_t e = VEC_vused(v);

      VEC_IGNRET( setup.Pp_char ? (Vv = v, i = e - 1) : (Vv = *v) );

      do {
	for (j = 0; (c = Vv[j]) | (j < bfsize); j++) {
	  bf[j] = c;
	}
	Vv = v[++i];
      } while( i < e );
    }
  default :
    setup.size = 0;
  }

  return setup.size;
}
/* No need of these */
#undef  VEC_newFrmSize
#undef  vsize_t

#endif /* V_BASE_H */
