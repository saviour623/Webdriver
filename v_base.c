#define VEC_INTERNAL_IMPLEMENTATION
#include "v_base.h"

/* Allow size-bound check on request */
#define VEC_SAFE_REQUEST 1

/* enable debuging by default */
#if !defined(VEC_DEBUG)
#define VEC_DEBUG 1
#endif

static __NONNULL__ vec_t VEC_append(vec_t *, void *, size_t);
__STATIC_FORCE_INLINE_F bool VEC_free(void *);

/* Meta-data */
typedef struct {
  uint32_t vcapSize;
  /* enable caching */
#ifdef VEC_ENABLE_CACHE
  void     *vcache;
  uint8_t  vcacheSize;
#endif
} vecMetaDataHeader;

/*
  segmentation block-list Header
*/
typedef struct {
  uint8_t blockFill; /* size of block (maximum: 256) */
  void *nextBlock;
} segmentBlock;

enum {
	  VEC_MIN_SIZE         = 0x100,
	  VEC_ALLOC_SIZE       = 0x01,
	  VEC_VECTOR           = 0x80,
	  VEC_SEGTBLK          = 0x01,
	  VEC_MALLOCD_ALL      = 0x04,
	  VEC_MEMFILL          = 0x01,
	  VEC_ERROR_OUTOFBOUND = 0x3b
};

/**
* (gblobal)
*
* @1 flags (fits in a single byte)
* @2 Meta-data size
* @3 Size of allocation block for entries
* @3 Size of segment
*/
const uint8_t  vecDefFlag             = 1;
const uint8_t  vecGblMetaDataSize     = sizeof (vecMetaDataHeader) + vecDefFlag;
const uint8_t  vecGblDataBlockSize    = sizeof (vec_t);
const uint16_t vecGlbSegmentAllocSize = 1 << 8;

/**
* (macros)
*
* @VEC_metaData: Get/set meta-data in header
* @VEC_peekBlockStart: Temporarily view the starting block of the vector
* @VEC_size: Request or update the size of the container
* @VEC_moveToMainBlock: Move pointer to the data section
* @VEC_moveToBlockStart: Move the pointer to the start of the contaner’s block
* @VEC_reinterpret: Reinterpret as pointer to single byte memory
* @VEC_nextblock & @VEC_prevblock: Increment/decrement memory pointer
*/
#define VEC_metaData(vec, ...)    ((VEC_reinterpret(vec) - vecDefFlag)[0])
#define VEC_peekBlockStart(vec)   (void *)(VEC_reinterpret(vec) - vecGblMetaDataSize)
#define VEC_size(vec)             ((vecMetaDataHeader *)VEC_peekBlockStart(vec))->vcapSize
#define VEC_moveToMainBlock(vec)  ((vec) = (void * )(VEC_reinterpret(vec) + vecGblMetaDataSize))
#define VEC_moveToBlockStart(vec) ((vec) = VEC_peekBlockStart(vec))
#define VEC_reinterpret(_addr)    ((uint8_t *)(void *)(_addr))
#define VEC_nextblock(block)      ((block)++)
#define VEC_prevblock(block)      ((block)--)

/**
 * if VEC_TRACE_DEL is defined, caching is enabled. Memory of deleted contents of are cached rather than being freed.
 * This minimizes the frequent call to a memory allocator, at a cost of speed of insertion, since the list of deleted
 * content may have to be scanned for each insertion request.
*/
#ifdef VEC_ENABLE_CACHE

/**
 * TODO: cache only memory, ignore location of content
 */
__STATIC_FORCE_INLINE_F void VEC_cache(const void *vec, size_t at) {
  vec_t slot, cacheLoc;
  vecMetaDataHeader *cacheInfo __MAY_ALIAS__;

  cacheInfo = (vecMetaDataHeader *)VEC_peekBlockStart(vec);
  cacheLoc = &(cacheInfo->vcache);

  /* TODO: size-bound check (use request) */
  slot = (vec_t)vec + at;
  /* cache only slot, discard its content */
  (*slot != NULL) && VEC_free(*slot);
  *slot = *cacheLoc, *cacheLoc = slot;


  cacheInfo->vcacheSize += 1;
  cacheInfo->vcapSize -= 1;
}

__STATIC_FORCE_INLINE_F void VEC_rcache(void *vec, size_t at) {
  vec_t cacheLoc, cachePrevLoc;

  /* retrieve cache list */
  cacheLoc = ((vecMetaDataHeader *)VEC_peekBlockStart(vec))->vcache;
  cachePrevLoc = NULL;

  /*
   * if the difference between the initial memory address of the container and that
   * of the cache pointer equals that of the requested index, we had cached the
   * memory on a previous operation
   */
  while (cacheLoc && (ptrdiff_t)((cacheLoc - (vec_t)vec) != at))
	cachePrevLoc = cacheLoc, cacheLoc = *cacheLoc;

  /*
	cacheloc will never be null assuming no cache miss. remove the found memory from the cache list
  */
  if (cacheLoc) {
	cachePrevLoc && ( *cachePrevLoc = cacheLoc );
	((vecMetaDataHeader *)VEC_peekBlockStart(vec))->vcacheSize -= 1;
  }
}
#endif /**/

/**
 * VEC_SEGMENT
 *
 * segments vector into blocks
*/
static __NONNULL__ vec_t VEC_segment(vec_t vec, size_t size, uint8_t action) {
  segmentBlock **block;
  uint32_t segment;

  /* segment size (each block having atleast 256bytes size) */
  segment = (segment = (size >> 8)) + !!(size - (segment << 8));

  /* allocate memory of size equal to segment to vwctor */
  mvpgAlloc(&vec, (vecGblDataBlockSize * segment) + vecGblMetaDataSize);

  block = (void *)VEC_moveToMainBlock(vec);

  VEC_size(vec) = segment;

  /* enable/disable action (memfill) if segment size is lesser/greater than the number of reasonable malloc call per function. 0 >= n <= 2^8 is choosen as the limit */
  action = action >> (segment > (2 << 8));
  VEC_metaData(vec) = VEC_SEGTBLK & (!!action & VEC_MALLOCD_ALL);

  do {
	mvpgAlloc(block, vecGlbSegmentAllocSize);
	(*block)->blockFill = 0;           /* property fill */
	(*block) += sizeof (segmentBlock); /* move ahead of header */
  } while (VEC_nextblock(block), (action & VEC_MEMFILL) && --segment );

  return vec;
}

/**
 * VEC_CREATE
 *
 * Constructs Vector Container
 */
vec_t VEC_create(size_t size, const VEC_set config) {
  vec_t vec;
  uint8_t _vecMetaData;

  if ( !size )
	size = VEC_MIN_SIZE;

  if (!config.native) {
	_vecMetaData = config.type;
  }

  /* multiblock (table) */
  if (config.type & VEC_SEGTBLK) {
	return VEC_segment(vec, size, config.memfill);
  }
  /* traditional vector with a large, single block */
  mvpgAlloc(&vec, (vecGblDataBlockSize * size) + vecGblMetaDataSize);

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
  ssize_t oldSize, allocSize;

  oldSize = VEC_size(*vec);

  /* TODO:  handle overflow */
  allocSize = (vecGblDataBlockSize * (newSize + VEC_ALLOC_SIZE)) + vecGblMetaDataSize;

  /* resize container */
  if (!(alloc = realloc(VEC_moveToBlockStart(*vec), allocSize)) )
	return (vec_t)NULL;

  allocSize = newSize - oldSize; /* padded size (reuse of var ’allocSize’) */
  (allocSize > 1) &&  memset((alloc + oldSize), 0, vecGblDataBlockSize * allocSize);

  *vec = VEC_moveToMainBlock(alloc);
  VEC_size(alloc) = newSize + 1;

  return alloc;
}

/**
 * VEC_APPEND
 *
 * Appends new data to vector
 */
static __inline__  __NONNULL__ vec_t VEC_append(vec_t *vec, void *new, size_t propertyIndex) {
  vec_t property __MB_UNUSED__;

  if (VEC_size(*vec) > propertyIndex) {
	property = *vec + propertyIndex;

#ifdef VEC_ENABLED_CACHE
	VEC_rcache(*vec, propertyIndex);
#else
	*property && VEC_free(*property);
#endif
	*property = new;
  }
  else {
	VEC_resize(vec, propertyIndex);
	(*vec)[propertyIndex] = new;
  }
  return *vec;
}

/**
 * VEC_add
 */
__NONNULL__ void *VEC_add(vec_t *vec, void *new, size_t bytes, size_t propertyIndex) {
  void *_new;

  if (!*vec || !bytes || !(_new = malloc(bytes + vecDefFlag)))
	return NULL;

  _new = (uint8_t *)_new + vecDefFlag;
  memcpy(_new, new, bytes);

  return VEC_append(vec, _new, propertyIndex);
}

/**
 * VEC_request
 */
__STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_baseRequest(vec_t vec, ssize_t propertyIndex) {
  size_t size;

  size = VEC_size(vec);
  propertyIndex = propertyIndex < 0 ? size + propertyIndex : propertyIndex;

  if (propertyIndex >= size || propertyIndex < 0) {
	throwError("required index is out of bound");
	exit(EXIT_FAILURE);
  }
  return vec + propertyIndex;
}
/* retrieve data from container located at @request */
__NONNULL__ void *VEC_request(vec_t vec, ssize_t propertyIndex) {
  return *(vec_t)(VEC_baseRequest(vec, propertyIndex));
}

/**
 * VEC_requestAt
 */
__STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_requestAt(vec_t *vec, ssize_t propertyIndex, ssize_t at) {
  void *itemAt;

  itemAt = VEC_request(*vec, at);
  return itemAt ? VEC_request(&itemAt, propertyIndex) : NULL;
}
__STATIC_FORCE_INLINE_F __NONNULL__ uint8_t VEC_getLevel(void *vec) {
  /*
   * Non vector if; v & VEC_VECTOR is 0
   */
  return VEC_metaData(vec) & VEC_VECTOR;
}

/**
 * VEC_free
 */
__STATIC_FORCE_INLINE_F bool VEC_free(void *rt) {
  free(VEC_getLevel(rt) ? VEC_moveToBlockStart(rt) : (VEC_reinterpret(rt) - vecDefFlag));

  return true;
}

/**
 * VEC_remove
 */
__NONNULL__ void *VEC_remove(vec_t *vec, ssize_t propertyIndex) {

#ifdef VEC_TRACE_DEL
  /* cache empty vector slot */
  VEC_cache(*vec, propertyIndex);
#else
  vec_t property;

  (property = VEC_baseRequest(*vec, propertyIndex));
  free(*property);
  /* TODO: fix the possibility of overflow */
  memmove(property + 1, property, (VEC_size(*vec) - propertyIndex) * vecGblDataBlockSize);
  /* TODO; shrink container to current size */
#endif
  return NULL;
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
