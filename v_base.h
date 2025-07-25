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
const vsize_t  VEC_sizeOverflwLim = ULONG_MAX & ~LONG_MAX;
const uint8_t  VEC_widthOfCInt[5] = {5, 10, 20, 0, 39   }; /* Access: (sizeof(N) >> 2) */

/***********************************************************

 * Methods: MACRO

************************************************************/

/* Utils */
#define VEC_select(True, False, ...)\
  MACR_DO_ELSE(True, False, __VA_ARGS__)
#define VEC_Macro_Exec_True(...) __VA_ARGS__
#define VEC_Macro_Exec_False(...)
#define VEC_Macro_If_Args_Exec(...)					\
  VEC_Macro_Exec_ ## VEC_select(True, False, __VA_ARGS__) (__VA_ARGS__)

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
  VEC_INTERNAL_create(VEC_select(_size, 32, _size), VEC_select(sizeof(_type), 0, _type))

#define VEC_newFrmSize(_size, _dsize, ...)					\
  VEC_create(VEC_select(_size, 32, _size), VEC_select(_dsize, 0, _dsize))


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
  VEC_select(VEC_popi(V, __VA_ARGS__), VEC_popni(V), __VA_ARGS__)

#define VEC_insert(V, N, I)			\
   (void)((V)[VEC_cvtindex(V, I, (I) < 0)] = (N))

#define VEC_append(V1, V2)			\
   VEC_INTERNAL_append(V1, V2)

#define VEC_foreach(S, V, T)						\
  for (VEC_type(T) K = VEC_begin(V); T S; (S = *K++) != VEC_end(V); )

/*
 * Vec_map as other map functions, iterates over an iterable object, calling a function on each member.
 * The first three arguments are the callback function, vector and type. The Last (optional),
 * is a vararg  which although being a vararg, expects only a single argument that evaluates to
 * ’True’. When Passed, the mapping is done over the base of each member rather than their values.
 */
#define VEC_map(F, V, T, ...)						\
  do {									\
      VEC_select(VEC_refType, VEC_type, __VA_ARGS__)(T) Vv, Last;	\
      if (V != NULL && F != NULL)					\
	{								\
	  Vv = VEC_select(&V, V, __VA_ARGS__);				\
	  VEC_assert(VEC_dtype(V) == sizeof(T));			\
									\
	  for (Last = Vv + VEC_used(V); Vv != Last; )			\
	    F(*Vv++ VEC_Macro_If_Args_Exec(__VA_ARGS__));		\
	}								\
  } while (0)

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

#define DOCUMENTATION(LABEL) LABEL
#define VEC_BUFFER_SIZE USHRT_MAX
#define VEC_MAX_INT_LEN 32
#define EOFMT(c, f) (((c) = (f)) & ((c) ^ 58))
#define VEC_appendComma(bf, i)\
  (((bf)[i] = ','), ((bf)[i + 1] = ' ', i + 2))


enum {
      PTR  = 0x01,  USIGNED = 0x02,  CHAR  = 0x04,
      LONG = 0x100, LLNG    = 0x200, SIZE = 0x800,
      TYPE = 0x7f,  SPEC    = 0xf00, BASE = 0x400
};

typedef struct {
  char    *Pp_buf, *Pp_fmt;
  vsize_t  Pp_size, Pp_used;
  uint16_t Pp_mask, Pp_dtype;
  uint8_t  Pp_skip, Pp_overflw;
} Pp_Setup;

__NONNULL__ __STATIC_FORCE_INLINE_F void VEC_Itoa(const intmax_t n, Pp_Setup *cf) {
  const int U = cf->Pp_used;

  if ((cf->Pp_size - U) < cf->Pp_overflw)
    return;
  cf->Pp_used += cvtTostrInt(n, cf->Pp_buf+U, cf->Pp_mask & BASE, !(cf->Pp_mask & UNSIGNED) && (n < 0));
}

__NONNULL__ void VEC_TostrInt(void *v, Pp_Setup *cf) {

  if (cf->mask & PTR) {
    VEC_map(v, VEC_Itoa, uintptr_t, cf, true);
    cf->Pp_overflw = VEC_widthOfCInt[sizeof(uintptr_t)>>2];
    goto end;
  }
  switch (cf->Pp_mask & SPEC) {
  case LONG:
    cf>Pp_overflw = VEC_widthOfCInt[sizeof(long)>>2];
    VEC_map(v, VEC_Itoa, long, cf);
    goto end;
  case LLNG:
    cf->Pp_overflw = VEC_widthOfCInt[sizeof(long long)>>2];
    VEC_map(v, VEC_Itoa, long long, cf);
    goto end;
  case SIZE:
    cf->Pp_overflw = VEC_widthOfCInt[sizeof(size_t)>>2];
    VEC_map(v, VEC_Itoa, size_t, cf);
    goto end;
  default:
    cf->Pp_overflw = VEC_widthOfCInt[sizeof(int)>>2];
    VEC_map(v, VEC_Itoa, int, cf);
  }
 end:
}

/*
  DOCUMENTATION
  *
  * @NoEmptyFormatStr: Format is first checked to be non-empty
  * @CopyFormatToFc:
  *
  *             Then it is splitted into indices 0, 1, 3, 7, 15, 31 (these indices
  *             corresponds to the value of @mask when rshfed by 1) of format checker (fc)
  * @GetFormatInExpectedOrder:
  *
  *             The format checker buffer, is checked
  * @CheckIgnoredSPecOrFormat:
  *
  *             Check if No format specifier is ignored (invalid at position) and there
  *             are no left over characters after format chars
  * @ErrorCheck:
  *
               The mask is checked for the following errors:
               (1). ’s’ is specified with any other specifiers (width);
               (2). ’f’ is specified with an ’x’ (hexedecimal float is unsupported);
               (3). There is no Type specifier;
               (4). Ignored mask value due to invalid positioning of specifiers or extra
		    * characters in format. usually if format is correct, a right shift of
		    * @mask by the total length of the specifier string (max: 6) should be
		    * non-zero, else otherwise.
  *
  *Note* There is no significant speedup attributed to this approach, it is just chosen to minimize the use of multiple swtich and if statements for checkks.
 */

__NONNULL__ __STATIC_FORCE_INLINE_F void VEC_TostrFlt(const void *v, Pp_Setup *cf) {
  switch (cf->mask & SPEC) {
  case LONG:
  case LLNG:
  default:
    PASS;
  }
  VEC_assert(false, "Repr: TOSTRFLT: UINMPLEMENTED");
}

__NONNULL__ vsize_t VEC_INTERNAL_repr(void *v, Pp_Setup *setup) {
  register uint32_t mask;
  register uint8_t  c;

  /* It may be desirable to skip the format processing, especially on a repeated call after a buffer overflow case (There should be no modification of the previous returned mask else it’s UB) */
  if (setup->Pp_skip && (mask=setup->Pp_mask))
    goto TypeCheckNAction;

  {
    register uint8_t b, error, fc[32] = {0};

    DOCUMENTATION (NoEmptyFormatStr):
    VEC_assert(EOFMT(c, *fmt++), "Repr: Empty Format is unsupported");

    DOCUMENTATION (CopyFormatToFc):
    fc[0] = c;
    for (b = 0; (b < 6) && EOFMT(c, *fmt); i++)
      fc[(1 << b) - 1] = c;

    DOCUMENTATION (GetFormatInExpectedOrder):

    mask = fc[0] == 0x6c;
    mask = (mask << (c=fc[mask] == 0x6c)) | c;
    mask = (mask << (c=fc[mask] == 0x7a)) | c;
    mask = (mask << (c=fc[mask] == 0x78)) | c;
    mask = (mask << (c=fc[mask] == 0x68)) | c;
    mask = (mask << 8) | fc[mask];

    DOCUMENTAION (CheckIgnoredSPecOrFormat):

    #define IgnoredMaskOrFormat(M, L, F, C)		\
    (!((M) >> ((L)+6)) || EOFMT(C, F))

    DOCUMENTATION (ErrorCheck):

    error = !(
	      (((mask & TYPE) == 0x73) && (mask & SPEC))
	    | (((mask & TYPE) == 0x66) && (mask & BASE))
	    | (!(mask & TYPE)          && (mask & SPEC))
	    | IgnoredMaskOrFormat(mask, b, *fmt, c)
	    );
    VEC_assert(error, "Repr: Invalid Format");

    #undef IgnoredMaskOrFormat

    setup->Pp_fmt = fmt;
  }

 TypeCheckNAction:
  setup->Pp_mask = mask & SPEC; /* Igore Type (Already handled by local mask) */

  switch ( mask & TYPE ) {
  case 'p':
    setup->Pp_mask |= PTR;
  case 'q':
    setup->Pp_mask |= LLONG;
  case 'u':
    setup->Pp_mask |= USIGNED;
  case 'd': case 'i':
    VEC_TostrInt(v, &setup);
    break;
  case 'f':
    VEC_TostrFlt(v, &setup);
    break;
  case 'c':
    setup->Pp_mask |= CHAR;
  case 's':
    VEC_assert(
	       (setup->Pp_dtype > 1) && (setup->Pp_dtype != sizeof(VEC_type(char))),
	       "Repr: Type Mismatch"
	       );

    if ( setup->Pp_used > 0 ) {
      char *Vv, *bf;
      register vsize_t j, i, e;

      e = VEC_vused(v);
      Vv = setup->Pp_mask & CHAR ? (i=e-1), v : (i=0), *v;
      bf = setup->Pp_buf;
      do {
	for (j = 0; (c = Vv[j]) | (j < bfsize); j++) {
	  bf[j] = c;
	}
	Vv = v[++i];
      } while( i < e );
    }
  default :
    /* TODO: Raise Error */
    setup->size = 0;
  }

  setup->Pp_mask |= mask & TYPE; /* restore Type */
  return setup.size;
}
/* No need of these */
#undef  VEC_newFrmSize
#undef  vsize_t
undef VMAP_MEM

#endif /* V_BASE_H */
