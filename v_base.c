#define VEC_INTERNAL_IMPLEMENTATION
#include "v_base.h"

/* Allow size-bound check on request */
#define VEC_SAFE_REQUEST 1

/* enable debuging by default */
#if !defined(VEC_DEBUG)
#define VEC_DEBUG 1
#endif

/* Meta-data */
typedef struct {
  size_t cap; /* Capacity */
  size_t used; /* Total used */
  size_t dtype; /* sizeof data Type */
  size_t tmp;
#endif
} VEC_metaheader;


enum {
      VEC_MIN_SIZE         = 0x100,
      VEC_ALLOC_SIZE       = 0x01,
      VEC_MEMFILL          = 0x01,
      VEC_REM_OPTMZ        = USHRT_MAX
};


const uint16_t GLB_metadtSz  = sizeof(VEC_metaheader); /* Metadata size */
const uint16_t GLB_datablksz = sizeof(vec_t); /* Size of a block */


#define VEC_metaData(vec, ...)    ((VEC_bytePtr(vec) - vecDefFlag)[0]) /*  Get/set meta-data in header */

q#define VEC_size(vec)             ((VEC_metaheader *)VEC_peekblkst(vec))->cap /* Request or update the size of the container */
#define VEC_dtype(vec)            ((VEC_metaheader *)VEC_peekblkst(vec))->dtype /* datatype size */
#define VEC_mv2MainBlk(vec)       ((vec) = (void * )(VEC_bytePtr(vec) + GLB_metadtSz)) /* Move pointer to data section */

#define VEC_bytePtr(vec)          ((byte *)(void *)(vec)) /* Cast to byte pointer */
#define VEC_bin(vec)              ((VEC_metaheader *)VEC_peekblkst(vec))->bin /* Waste bin for removed indices */
#define VEC_bincnt(vec)           ((VEC_metaheader *)VEC_peekblkst(vec))->bincnt /* bin counter */
#define VEC_tmp(vec)           ((VEC_metaheader *)VEC_peekblkst(vec))->tmp
#define VEC_iabs(V, I)             (I < 0 ? I + VEC_size(V) ? I)

/**
 * VEC_create
 *
 * Constructor
 */
__attribute__ ((warn_unused_result)) vec_t VEC_create(const size_t size, const size_t dtype) {
  vec_t vec;

  assert ( dtype );

  mvpgAlloc(&vec, safeMulAdd(dtype, size, GLB_metadtSz), 0);
  VEC_mv2MainBlk(vec);
  VEC_size(vec)  = size;
  VEC_dtype(vec) = dtype;

  return vec;
}

/**
 * VEC_resize
 *
 * resizes vector
 */
__NONNULL__ vec_t VEC_resize(vec_t *vec, ssize_t newSize) {
  void *alloc;
  ssize_t allocSize;

  allocSize = safeMulAdd(GLB_datablksz, newSize, GLB_metadtSz);

  mv2blkst(*vec);
  mvpgAlloc(&alloc, allocSize, 0);
  memcpy(alloc, *vec, safeMulAdd(GLB_datablksz, VEC_size(*vec), GLB_metadtSz));
  free(*vec);

  *vec = VEC_mv2MainBlk(alloc);
  VEC_size(*vec) = newSize;

  return *vec;
}

/**
 * VEC_add
 */
__NONNULL__ void VEC_add(vec_t *vec, void *new, size_t size, size_t i) {

  assert (*vec);

  (VEC_size(*vec) < i) && VEC_resize(vec, i);
  VEC_findNextNonEmpty(*vec, i)[0] = new;
}

/**
 * VEC_get
 */
__STATIC_FORCE_INLINE_F __NONNULL__ vec_t VEC_findNextNonEmpty(vec_t vec, size_t frm) {
  register size_t end;

  for (end = VEC_size(vec) - frm; (frm < end) && !vec[frm]; frm++) {
    PASS;
  }
  return vec + frm;
}
__STATIC_FORCE_INLINE_F __NONNULL__ vec_t VEC_get(vec_t vec, ssize_t i) {

  if (i < 0)
    i = -i;
  assert(i < VEC_size(vec));

  return VEC_findNextNonEmpty(vec, i);
}

/**
 * VEC_remove
 */
__NONNULL__ void VEC_remove(vec_t *vec, ssize_t i) {

#ifdef VEC_CLOBBER_REM
  if (VEC_size(*vec) > VEC_REM_OPTMZ) {
    ((vec_t)VEC_get(*vec, i))[0] = NULL;
    /* store removed index */
    VEC_bin(*vec)[VEC_bincnt(*vec)] = i;
    VEC_bincnt(*vec) += 1;
    /* cleanup all removed items if bin is full */
    if (VEC_bincnt(*vec) == VEC_MAX_RM) {
      /* TODO: cleanup */
      VEC_bincnt(*vec) = 0;
    }
  }
  return;
#endif
  size_t mvby = (VEC_size(*vec) - i - 1) * sizeof(vec_t);
  mvby ? memmove(*vec + i, (*vec + i) + 1, mvby) /* Shift memory to left */
    : ((*vec)[i] = (void *)MEMCHAR); /* Last index: reusable */
}

/**
 * Get size of vector
 */
__NONNULL__ size_t VEC_getsize(const vec_t vec) {

  return VEC_size(vec);
}

/**
 * VEC_free
 */
__FORCE_INLINE__ inline bool VEC_free(void *vec) {

  free( mv2blkst(vec) );

  return true;
}

/**
 * VEC_shrink
 */
__STATIC_FORCE_INLINE_F void *VEC_shrink(void *ptr) {

  /* NOT IMPLEMENTED */

  return NULL;
}

/**
 *  VEC_delete
 */
__NONNULL__ void *VEC_delete(vec_t *vec) {

  /* NOT IMPLEMENTED */

  return NULL;
}

/*
#ifdef VEC_CLOBBER_REM
    #if !defined(VEC_MAX_RM)
       /* Default size of bin is 255 *
       #define VEC_MAX_RM 0xff
    #endif
  size_t bin[VEC_MAX_RM];
  uint8_t bincnt;



#ifdef VEC_CLOBBER_REM
  if (VEC_size(*vec) > VEC_REM_OPTMZ) {
    ((vec_t)VEC_get(*vec, i))[0] = NULL;
    /* store removed index *
    VEC_bin(*vec)[VEC_bincnt(*vec)] = i;
    VEC_bincnt(*vec) += 1;
    /* cleanup all removed items if bin is full *
    if (VEC_bincnt(*vec) == VEC_MAX_RM) {
      /* TODO: cleanup *
      VEC_bincnt(*vec) = 0;
    }
  }
  return;
#endif
*/
