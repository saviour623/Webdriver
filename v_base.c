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
  uint32_t cap;
#if !defined(VEC_MAX_RM)
#define VEC_MAX_RM 0xff
#endif
  size_t rmbf[VEC_MAX_RM];
  uint8_t rmct;
} vecMetaDataHeader;


enum {
      VEC_MIN_SIZE         = 0x100,
      VEC_ALLOC_SIZE       = 0x01,
      VEC_MEMFILL          = 0x01,
      VEC_REM_OPTMZ        = USHRT_MAX
};

/**
 * (gblobal)
 *
 * @1 flags (fits in a single byte)
 * @2 Meta-data size
 * @3 Size of allocation block for entries
 */
const uint8_t  vecDefFlag          = 1;
const uint16_t vecGblMetaDataSize  = sizeof(vecMetaDataHeader) + vecDefFlag;
const uint16_t vecGblDataBlockSize = sizeof(vec_t);

/**
 * (macros)
 *
 * @VEC_metaData: Get/set meta-data in header
 * @VEC_peekBlockStart: Temporarily view the starting block of the vector
 * @VEC_size: Request or update the size of the container
 * @VEC_moveToMainBlock: Move pointer to the data section
 * @VEC_moveToBlockStart: Move the pointer to the start of the contaner’s block
 * @VEC_reinterpret: Reinterpret as pointer to single byte memory
 */

#define VEC_metaData(vec, ...)    ((VEC_reinterpret(vec) - vecDefFlag)[0])
#define VEC_peekBlockStart(vec)   (void *)(VEC_reinterpret(vec) - vecGblMetaDataSize)
#define VEC_size(vec)             ((vecMetaDataHeader *)VEC_peekBlockStart(vec))->cap
#define VEC_moveToMainBlock(vec)  ((vec) = (void * )(VEC_reinterpret(vec) + vecGblMetaDataSize))
#define VEC_moveToBlockStart(vec) ((vec) = VEC_peekBlockStart(vec))
#define VEC_reinterpret(_addr)    ((uint8_t *)(void *)(_addr))
#define VEC_rmbuffer(new)         ((vecMetaDataHeader *)VEC_peekBlockStart(vec))->rmbf
#define VEC_rmCounter(vec)        ((vecMetaDataHeader *)VEC_peekBlockStart(vec))->rmct


/**
 * VEC_CREATE
 *
 * Constructor
 */
__attribute__ ((warn_unused_result)) vec_t VEC_create(size_t size, const VEC_set config) {
  vec_t vec;
  uint8_t _vecMetaData;

  mvpgAlloc(&vec, safeMulAdd(vecGblDataBlockSize, size, vecGblMetaDataSize), 0);
  VEC_moveToMainBlock(vec);
  VEC_size(vec) = size;

  return vec;
}

/**
 * VEC_RESIZE
 *
 * resizes vector
 */
__NONNULL__ vec_t VEC_resize(vec_t *vec, ssize_t newSize) {
  void *alloc;
  ssize_t allocSize;

  allocSize = safeMulAdd(vecGblDataBlockSize, newSize, vecGblMetaDataSize);

  VEC_moveToBlockStart(*vec);
  mvpgAlloc(&alloc, allocSize, 0);
  memcpy(alloc, *vec, safeMulAdd(vecGblDataBlockSize, VEC_size(*vec), vecGblMetaDataSize));
  free(*vec);

  *vec = VEC_moveToMainBlock(alloc);

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

  if (VEC_size(*vec) > VEC_REM_OPTMZ) {
    ((vec_t)VEC_get(*vec, i))[0] = NULL;
    VEC_rmbuffer(*vec)[VEC_rmCounter(*vec)] = i;
    VEC_rmCounter(*vec) += 1;
    if (VEC_rmCounter(*vec) == VEC_MAX_RM) {
      /* TODO: cleanup */
      VEC_rmCounter(*vec) = 0;
    }
  }
  else {
    size_t mvby = (VEC_size(*vec) - i - 1) * sizeof(vec_t);
    mvby != 1 ? memmove(*vec + i, (*vec + i) + 1, mvby) /* Shift memory to left */
      : ((*vec)[i] = (void *)MEMCHAR); /* Last index: reusable */
  }
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

  free( VEC_moveToBlockStart(vec) );

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
