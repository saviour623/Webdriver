#ifndef MVPG_MEM_TOOL
#define MVPG_MEM_TOOL

#include "include.h"
#include <malloc.h>

#define MVPG_ALLOC_MEMALIGN 32
#define MEMCHAR UINT_MAX


/***********************************************************************

* ALIGNED MEMORY ALLOCATOR

***********************************************************************/

/* Allocate Memory Block Aligned to MVPG_ALLOC_MEMALIGN */
__WARN_UNUSED__ __NONNULL__ void *mvpgAlloc(void *memptr, size_t size, size_t offset);

/* Reallocate Memory Block returned by mvpgAlloc, keep alignment */
__WARN_UNUSED__ __NONNULL__ void *mvpgAlloc(void *memptr, size_t size, size_t offset);

/* Free Allocated Block */
void mvpgDealloc(void *memptr);

#endif
