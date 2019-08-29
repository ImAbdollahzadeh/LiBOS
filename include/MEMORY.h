#ifndef _MEMORY__H__
#define _MEMORY__H__

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _FREE_HOLE {
	void*        ptr;                       // the very beginning of the free chunk hole
	void*        next;                      // pointer to the next entry in the hole list
	unsigned int size;                      // the total size (byte) of the free chunk hole
	unsigned int tag;                       // flag: a tag for merging adjacent hole list entries
} __attribute__ ((packed)) FREE_HOLE; // size = 16 bytes

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _MEM_BLOCK_PRECEDENTIAL {
	void*          zeroth_ptr;              // the very beginning of the allocated chunk
	void*          start_ptr;               // the returned pointer to the allocated chunk
	void*          end_ptr;                 // the pointer to the end of allocated chunk
	unsigned int   alignment_bound_space;   // bytes which will be reserved for the alignment-boundary purpose
	unsigned int   actual_allocated_bytes;  // the requested bytes
	unsigned short from_heap_or_hole;       // flag: heap = 1, hole = 0
	unsigned char  reserved[32 - 22];       // reserved
} __attribute__ ((packed)) MEM_BLOCK_PRECEDENTIAL; // size = 32 bytes

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _MEM_BLOCK_TERMINATION {
	unsigned int  alignment_bound_space;    // bytes which will be reserved for the alignment-boundary purpose
	unsigned char reserved[16 - 4];         // reserved
} __attribute__ ((packed)) MEM_BLOCK_TERMINATION; // size = 16 bytes

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void* Alloc ( unsigned int bytes, unsigned int alignment, unsigned int boundary );
void  Free  ( void* ptr );
unsigned int mread(unsigned int, unsigned int);
void mwrite(unsigned int, unsigned int, unsigned int);
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _MEMORY__H__
