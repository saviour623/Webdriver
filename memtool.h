/* MEMTOOL HEADER FILE
Copyright (C) 2025 Michael Saviour

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

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
__NONNULL__ __WARN_UNUSED__ void *mvpgAlloc(const size_t size, const size_t offset);

/* Reallocate Memory Block returned by mvpgAlloc, keep alignment */
__NONNULL__ __WARN_UNUSED__ void *mvpgRealloc(void *memptr, const size_t size);

/* Free Allocated Block */
void mvpgDealloc(void *memptr);

#endif
