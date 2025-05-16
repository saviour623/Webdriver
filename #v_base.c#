#include "v_base.h"

/* Allow size-bound check on request */
#define VEC_SAFE_REQUEST 1
/* Allow neccessary warnings */
#define VEC_ALW_WARNING 1

/* NULL */
#define VEC_EOV (char *)(long) 0

/**
 * Types
 */
typedef uint64_t VEC_sizeType;
typedef void ** vec_t;

/**
 * Prototypes
 */
static __NONNULL__ vec_t VEC_append(vec_t *, void *, size_t);
__STATIC_FORCE_INLINE_F bool VEC_free(void *);

/* Meta-data */
typedef struct {
  uint32_t vcapSize;
  /* enable caching */
#ifdef VEC_ENABLE_CACHE
  void *vcache;
  uint8_t vcacheSize;
#endif
} vecMetaDataHeader;

/*
  segmentation block-list Header
*/
typedef struct {
  uint8_t blockFill; /* size of block (maximum: 256) */
  void *nextBlock;
} segmentBlock;

/* feature flags vector for */
typedef struct {
  uint16_t alignSize;
  uint8_t  native;
  uint8_t  type;
  uint8_t  memfill;
} VEC_set;

enum {
	  VEC_ARRAY          = 0,
	  VEC_MINIMUM_SIZE   = 0x01,
	  VEC_ALLOC_SIZE     = 0x01,
	  VEC_NATIVE         = 0x18,
	  VEC_NO_TRACK_SIZE  = 0x20,
	  VEC_READONLY       = 0x08,
	  VEC_VECTOR         = 0x80,
	  VEC_SEGTBLK       = 0x01,
	  VEC_M
	  VEC_MEMFILL        = 0x01,
	  VEC_EROUT_OF_BOUND = 0x3b
};

/* we reserve some simple vector data in just a single byte */
const uint8_t vecDefFlagLoc         = 1;

/* Meta-data size */
const uint8_t vecGblMetaDataSize  = sizeof (vecMetaDataHeader) + vecDefFlagLoc;

/* Sizeof allocation block for entries */
const uint8_t vecGblDataBlockSize = sizeof (vec_t);

/* segmentation size */
const uint16_t vecGlbSegmentBlockSize = 1 << 7;
const uint16_t vecGlbSegmentAllocSize = sizeof(segmentBlock) + (vecGblDataBlockSize * vecGlbSegmentBlockSize);

/**
 * VEC_(get/set)MetaData (lvalue)
 * Return the meta-data of a container. If a second argument is passed
 * to the macro as a varadic parameter, it is assigned the value returned
 * by meta-data
 */
#define VEC_setMetaData VEC_getMetaData
#define VEC_getMetaData(vec, ...) ((VEC_ACCESS(vec) - vecDefFlagLoc)[0])

/* Temporarily view the starting block of the vector */
#define VEC_peekBlockStart(vec)   (void *)(VEC_ACCESS(vec) - vecGblMetaDataSize)

/* Request or update the size of the container */
#define VEC_getMagnitude(vec)     ((vecMetaDataHeader *)VEC_peekBlockStart(vec))->vcapSize

/* Update the pointer to the start of the wriitable section of the container */
#define VEC_moveToMainBlock(vec)  ((vec) = (void * )(VEC_ACCESS(vec) + vecGblMetaDataSize))

/* Move the pointer to the start of the contaner’s block */
#define VEC_moveToBlockStart(vec) ((vec) = VEC_peekBlockStart(vec))
  #
#define ignoreExprReturn(...) (void) (__VA_ARGS__)
  #
  /* reinterpret memory block as pointer of single bytes */
#define VEC_ACCESS(_addr) ((uint8_t *)(void *)(_addr))

/* increment memory pointer */
#define VEC_nextblock(block) ((block)++)
#define VEC_prevblock(block) ((block)--)

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
 * vector_new (macro: alias -> VEC_create)
 */
#define VEC_new(size_t_size, ...)											\
  MACR_DO_ELSE(VEC_create(size_t_size, MACR_DO_ELSE((__VA_ARGS__), 0, __VA_ARGS__)), (throwError(NULL)), size_t_size)

/**
 * vector_create
 */

/* (1) */

/**
 segment vector into blocks
*/

static vec_t VEC_segment(vec_t vec, size_t size, uint8_t action) {
  segmentBlock **block;
  uint32_t segment;

  /*  segment size (each block having atleast 256bytes size) */
  segment = (segment = (size >> 8)) + !!(size - (segment << 8));

  puti(segment);
  /* allocate memory of size equal to segment to vwctor */
  mvpgAlloc(&vec, (vecGblDataBlockSize * segment) + vecGblMetaDataSize);

  block = (void *)VEC_moveToMainBlock(vec);

  /* virtual size of vector */
  VEC_getMagnitude(vec) = segment;
  /* construct block table
   * THIS LOOP SHOULD ONLY BE DONE FOR 0 < SEGMENT SIZE < 2^8
 */
  action = action >> (segment > (2 << 8)); /* enable/disable action if segment size is lesser/greater than the number of reasonable malloc call per function */
  VEC_setMetaData(vec) = VEC_SEGTBLK & (!!action & VEC_MALLOCD_ALL);
  do {
	mvpgAlloc(block, vecGlbSegmentAllocSize);
	(*block)->blockFill = 0;

	/* move ahead of header */
	(*block) += sizeof (segmentBlock);
  } while (VEC_nextblock(block), (action & VEC_MEMFILL) && --segment );

  return vec;
}
static vec_t VEC_create(size_t size, const VEC_set config) {
  vec_t vec;
  uint8_t _vecMetaData;

  if ( !size )
	size = VEC_MINIMUM_SIZE;

  /* default config */
  _vecMetaData = VEC_NATIVE;

  if (!config.native) {
	_vecMetaData = config.type;
  }

  /* multiblock (table) */
  if (config.type & VEC_SEGTBLK) {
	return VEC_segment(vec, size, config.memfill);
  }
  /* single block (type array) */
  mvpgAlloc(&vec, (vecGblDataBlockSize * size) + vecGblMetaDataSize);

  VEC_moveToMainBlock(vec);
  VEC_getMagnitude(vec) = size;

  return vec;
}

/**
 * vector_expand: expands to vector/resize vector
 */
static __inline__  __NONNULL__ vec_t VEC_expand(vec_t *vec, void *new, ssize_t  size, ssize_t propertyIndex) {
  void *alloc;
  register uintmax_t newSize;

  if (size > propertyIndex)
	return VEC_append(vec, new, propertyIndex);

  /* TODO: newSize may overflow */
  newSize = (vecGblDataBlockSize * (propertyIndex + VEC_ALLOC_SIZE)) + vecGblMetaDataSize;
  /* resize container */
  if (!(alloc = realloc(VEC_moveToBlockStart(*vec), newSize)) )
	return (vec_t)NULL;

  newSize = propertyIndex - size; /* padded size */
  (newSize > 1) &&  memset((alloc + size), 0, vecGblDataBlockSize * newSize);

  *vec = VEC_moveToMainBlock(alloc);
  VEC_getMagnitude(alloc) = propertyIndex + 1; /* update vector size */
  (*vec)[propertyIndex] = new; /* add to vector */

  return alloc;
}

/**
 * vec_append: add/append new data to vector
 */
static __inline__  __NONNULL__ vec_t VEC_append(vec_t *vec, void *new, size_t propertyIndex) {
  register size_t magnitude;
  vec_t loc __MB_UNUSED__;

  magnitude = VEC_getMagnitude(*vec);

  if (magnitude > propertyIndex) {
	loc = *vec + propertyIndex;
#ifdef VEC_TRACE_DEL
	VEC_rcache(*vec, propertyIndex);
#else
	(*loc != VEC_EOV) && VEC_free(*loc);
#endif
	*loc = new;
  }
  else if (! VEC_expand(vec, new, magnitude, propertyIndex)) {
	return (vec_t)NULL;
  }
  return *vec;
}

/**
 * VEC_add
 */
static  __NONNULL__ void *VEC_add(vec_t *vec, void *new, size_t bytes,  size_t propertyIndex) {
  void *_new;

  if (!*vec || !bytes || !(_new = malloc(bytes + vecDefFlagLoc)))
	return NULL;

  _new = (uint8_t *)_new + vecDefFlagLoc;
  (VEC_ACCESS(_new) - vecDefFlagLoc)[0] = VEC_ARRAY;
  memcpy(_new, new, bytes);

  return VEC_append(vec, _new, propertyIndex);
}

/**
 * VEC_request
 */
__STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_baseRequest(vec_t vec, ssize_t propertyIndex) {
  size_t magnitude;

  magnitude = VEC_getMagnitude(vec);
  propertyIndex = propertyIndex < 0 ? magnitude + propertyIndex : propertyIndex;

  if (propertyIndex >= magnitude || propertyIndex < 0) {
	throwError("required index is out of bound");
	exit(EXIT_FAILURE);
  }
  return vec + propertyIndex;
}
/* retrieve data from container located at @request */
__STATIC_FORCE_INLINE_F __NONNULL__ void *VEC_request(vec_t vec, ssize_t propertyIndex) {
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
  return VEC_getMetaData(vec) & VEC_VECTOR;
}

/**
 * VEC_free
 */
__STATIC_FORCE_INLINE_F bool VEC_free(void *rt) {
  free(VEC_getLevel(rt) ? VEC_moveToBlockStart(rt) : (VEC_ACCESS(rt) - vecDefFlagLoc));

  return true;
}

/**
 * VEC_remove
 */
static __NONNULL__ void *VEC_remove(vec_t *vec, ssize_t propertyIndex) {

#ifdef VEC_TRACE_DEL
  /* cache empty vector slot */
  VEC_cache(*vec, propertyIndex);
#else
  vec_t property;

  (property = VEC_baseRequest(*vec, propertyIndex));
  free(*property);
  /* TODO: fix the possibility of overflow */
  memmove(property + 1, property, (VEC_getMagnitude(*vec) - propertyIndex) * vecGblDataBlockSize);
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
static __NONNULL__ void *VEC_delete(vec_t *vec) {

  /* NOT IMPLEMENTED */

  return NULL;
}

int main(void) {
  size_t x;
  int *intArray;
  VEC_set config = {.type = 1 , .memfill = 0};
  int **vec;

  vec = (void *)VEC_new(131072, config);

  int data[][6] = {
				   {0, 2, 4, 6, 8, 10}, {1, 3, 5, 7, 9, 11},
				   {2, 3, 5, 7, 11, 13}, {4, 16, 32, 64, 128},
  };

  VEC_delete((void *)&vec);
  return 0;
}
