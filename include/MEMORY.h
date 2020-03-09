#ifndef _MEMORY__H__
#define _MEMORY__H__

#include "LiBOS_CORE.h"

#define FAST_MWRITE(ADDRESS_BASE, ADDRESS_OFFSET, VALUE) do { (*(volatile UINT_32*)((void*)(ADDRESS_BASE + ADDRESS_OFFSET)) = VALUE); } while(0)
#define FAST_MREAD(ADDRESS_BASE, ADDRESS_OFFSET, RET_POINTER)                          \
do {                                                                                   \
	*RET_POINTER = (*(volatile UINT_32*)((void*)(ADDRESS_BASE + ADDRESS_OFFSET))); \
} while(0)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _FREE_HOLE {
	void*   ptr;                      // the very beginning of the free chunk hole
	void*   next;                     // pointer to the next entry in the hole list
	UINT_32 size;                     // the total size (byte) of the free chunk hole
	UINT_32 tag;                      // flag: a tag for merging adjacent hole list entries
} __attribute__ ((packed)) FREE_HOLE; // size = 16 bytes

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _MEM_BLOCK_PRECEDENTIAL {
	void*   zeroth_ptr;              // the very beginning of the allocated chunk
	void*   start_ptr;               // the returned pointer to the allocated chunk
	void*   end_ptr;                 // the pointer to the end of allocated chunk
	UINT_32 alignment_bound_space;   // bytes which will be reserved for the alignment-boundary purpose
	UINT_32 actual_allocated_bytes;  // the requested bytes
	UINT_16 from_heap_or_hole;       // flag: heap = 1, hole = 0
	UINT_8  reserved[32 - 22];       // reserved
} __attribute__ ((packed)) MEM_BLOCK_PRECEDENTIAL; // size = 32 bytes

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _MEM_BLOCK_TERMINATION {
	UINT_32 alignment_bound_space;    // bytes which will be reserved for the alignment-boundary purpose
	UINT_8  reserved[16 - 4];         // reserved
} __attribute__ ((packed)) MEM_BLOCK_TERMINATION; // size = 16 bytes

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void*   Alloc ( UINT_32 bytes, UINT_32 alignment, UINT_32 boundary );
void    Free  ( void* ptr );
UINT_32 mread (const UINT_32, const UINT_32);
void    mwrite(const UINT_32, const UINT_32, const UINT_32);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _MEMORY__H__
