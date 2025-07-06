#include "memtool.h"

/***********************************************************

 * ALLOCATOR

************************************************************/


/**
 *  MVPGALLOC - Mvpg Memory Allocator
 *
 * According to posix_memalign, type memptr is a void **. Although mvpgAlloc follows this prototype,
 * The actual parameter (memptr) is defined as type ’void *’. This is only to avoid the need
 * for an explicit cast to ’void **’ for pointers of different types on calls to the function.
 *
 * A typical to call to @mvpgAlloc should be: mvpgAlloc(&memptr, 8)
 */


#if (_POSIX_C_SOURCE >= 200112L) || (_DEFAULT_SOURCE || _BSD_SOURCE || (XOPEN_SOURCE >= 500))
#define MvpgMalloc(memptr, size) posix_memalign(memptr, MVPG_ALLOC_MEMALIGN, size)
/* C11 introduced a standard aligned_alloc function */
#elif defined(__STDC__) && (__STDC_VERSION >= 201112L)
#if defined(_MSC_VER) || defined(_WIN32)
#define MvpgMalloc(memptr, size) !(*memptr && (memptr = _aligned_malloc(MVPG_ALLOC_MEMALIGN, size))) /* requires malloc.h */
#define mvpgDeallocate(memptr) _aligned_free(memptr) /* memory can’t be freed with malloc’s free() */
#else
/*__clang__ and __GNUC__ */
#define MvpgMalloc(memptr, size) !(*memptr && (memptr = aligned_alloc(MVPG_ALLOC_MEMALIGN, size))) /* TODO: size must be multiple of alignment  */
#define mvpgDeallocate(memptr) free(memptr)
#endif
#else
/* MANUAL MEMALIGN */

#if !defined(UINTPTR_MAX)
     typedef unsigned long int uintptr_t
#endif
typedef uint16_t offset_t;
#define OFFSET_SZ 2

/* offset + fault (catering for worst cases where returned memory + offset may be unaligned)*/
#define MAX_ALIGN_OFFSET_SZ  (OFFSET_SZ + (MVPG_ALLOC_MEMALIGN - 1))
/* Round n up to next multiple of boundary */
#define ALIGN_UP_MEMALIGN(n) NXTMUL(n, MVPG_ALLOC_MEMALIGN)
/* offset is stored just before the aligned memory address */
#define MEM_OFFSET_LOC(ptr) ((offset_t *)ptr - 1)
/* Move to the initial unaligned (returned by malloc) memory */
#define MV2_INIT_ALLOC(ptr) (void *)((uintptr_t)ptr - MEM_OFFSET_LOC(ptr)[0])

/**********************************************************************************************/

__WARN_UNUSED__ __NONNULL__ static void *NativeAlignedAlloc(void **ptr, size_t size){
  /**
     Align memory to MVPG_ALLOC_MEMALIGN boundary
   */
  void *nalignedPtr;

  *ptr = nalignedPtr = NULL;
  if ( (nalignedPtr = malloc(size + MAX_ALIGN_OFFSET_SZ)) ) {
    /* Align address to required boundary, aligning from the address ahead of the offset
     */
    *ptr = (void *)ALIGN_UP_MEMALIGN((uintptr_t)nalignedPtr + MAX_ALIGN_OFFSET_SZ);
    /* store the offset + fault_offset at pre-offset location before aligned memory (header) */
    MEM_OFFSET_LOC(*ptr)[0] = (uintptr_t)*ptr - (uintptr_t)nalignedPtr;
  }
  return *ptr;
}

__NONNULL__ static void NativeAlignedFree(void *ptr) {
  /* Free aligned block */

  free(MV2_INIT_ALLOC(ptr));
}

/************************************************************************************************/

/* Use native as fallback */
#define MvpgMalloc(memptr, size) NativeAlignedAlloc(&memptr, MVPG_ALLOC_MEMALIGN, size)
#define mvpgDeallocate(memptr) NativeAlignedFree(memptr)

#endif


/* MAIN */

__NONNULL__ void *mvpgAlloc(void *memptr, size_t size, size_t offset) {
  /* Allocate Block */

  void **memAllocPtr;

  memAllocPtr = memptr;

  assert(size != 0);

  if ( MvpgMalloc(memAllocPtr, size) ) {
	/* ERROR */
	fprintf(stderr, "mvpgAlloc: allocation of size %lu failed (%s)\n", (long)size, strerror(errno));

#ifdef EXIT_ON_MEM_ERR
	exit(EXIT_FAILURE);
#else
	return NULL;
#endif
  }
  memset(*memAllocPtr, MEMCHAR, size); /* clear memory */
  *memAllocPtr += offset; /* offset */

  return *memAllocPtr;
}

__WARN_UNUSED__ __NONNULL__ void *mvpgRealloc(void *memptr __MB_UNUSED__, size_t newsize __MB_UNUSED__) {

  /* NOT IMPLEMENTED */
  static_assert(0, "MVPGREALLOC: NOT IMPLEMENTED");

  return NULL;
}


void mvpgDealloc(void *memptr) {
  /* Deallocate Block */

  mvpgDeallocate(memptr);
}
