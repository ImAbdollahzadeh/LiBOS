#include "../include/MEMORY.h"
#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

/* 4GiB memory (x86) ********************************************************************************************************
             start         end          size                  region/exception    description
             0x00100000    0x00EFFFFF	0x00E00000 (14 MiB)   RAM, free           Extended memory
             0x00F00000    0x00FFFFFF	0x00100000 (1 MiB)    mmapped hardware    ISA Memory Hole 15-16MB
             0x01000000    ????????     ????????              RAM, free           ...         
             0xC0000000    0xFFFFFFFF	0x40000000 (1 GiB)    various             Mmapped PCI, PnP, APIC, BIOS 
 ****************************************************************************************************************************/


void*        __IMOS_Heap             = (void*)       ( 0x01000000 );
unsigned int __IMOS_Heap_size        = (unsigned int)( 0xC0000000 - 0x01000000 - 1); // ~3 GiB
void*        __IMOS_Urgent_Heap      = (void*)       ( 0x00100000 );
unsigned int __IMOS_Urgent_Heap_size = (unsigned int)( 0x00F00000 - 0x00100000 - 1); // 14 MiB

#define SWAP_HOLE_ENTRIES(ENT1,ENT2) do { FREE_HOLE tmp; tmp = *ENT1; *ENT1 = *ENT2; *ENT2 = tmp; } while(0)
#define MEMBLOCKSIZE (sizeof(MEM_BLOCK_PRECEDENTIAL) + sizeof(MEM_BLOCK_TERMINATION))
#define HEAP 1
#define HOLE 0

static FREE_HOLE    __free_hole[1024];
static unsigned int __free_hole_counter = 0;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern void CHECK_DS(unsigned int* ds);

static void* __IMOS_retrive_alloc_ptr(unsigned int bytes, unsigned int alignment, unsigned int boundary, unsigned int* hole_or_heap, int* entry)
{
	unsigned int i = 0;
	unsigned int size = 0;

	for (i = 0; i< __free_hole_counter; i++)
	{
		void* ptr = __free_hole[i].ptr;
		void* pptr = (void*)((unsigned int)ptr + MEMBLOCKSIZE);
		unsigned int algn = ((unsigned int)(pptr)+(alignment - 1)) & (~(alignment - 1));
		unsigned int bond = ((unsigned int)(pptr)+(boundary - 1)) & (~(boundary - 1));
		size = (algn - (unsigned int)ptr) + bytes;

		// the boundary must be confined in the free hole entry limitation
		if (bond < ((unsigned int)ptr + __free_hole[i].size))
			continue;

		if (size <= __free_hole[i].size)
		{
			*hole_or_heap = HOLE;
			*entry = i;
			return ptr;
		}
	}

	*entry = -1;
	*hole_or_heap = HEAP;
	return __IMOS_Heap;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void __IMOS_Remove_Entry(unsigned int entry)
{
	int i;
	for (i = entry; i < __free_hole_counter; i++)
		__free_hole[i] = __free_hole[i + 1];
	
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void __IMOS_SortFreeList(void)
{
	int i = 0, j = 0;

	while (j<__free_hole_counter)
	{
		for (i = 0; i<__free_hole_counter - 1; i++)
			if (__free_hole[i].ptr > __free_hole[i + 1].ptr)
				SWAP_HOLE_ENTRIES(&__free_hole[i], &__free_hole[i + 1]);
		j++;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void __IMOS_MergeFreeListEntries(void)
{
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int k = 0;
	FREE_HOLE*   tmp = 0;

	if (!__free_hole_counter)
		return;

	while (i < __free_hole_counter - 1)
	{
		j = i + 1;
		tmp = &__free_hole[j];
		while (__free_hole[i].next == tmp->ptr)
		{
			tmp->tag = 0xFB018ADA; //deleting tag
			k = tmp->size;
			__free_hole[i].next = tmp->next;
			__free_hole[i].size += k;
			j++;
			tmp = &__free_hole[j];
		}
		i += j;
		k = 0;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void __IMOS_RemoveTaggedEntries(void)
{
	unsigned int tagged = 0, i, j, ii;

	if (!__free_hole_counter)
		return;

	for (i = 0; i<__free_hole_counter; i++)
		if (__free_hole[i].tag == 0xFB018ADA)
			tagged++;

	for (ii = 0; ii<tagged; ii++)
	{
		i = 0;
		while (i<__free_hole_counter - 1)
		{
			if (__free_hole[i].tag == 0xFB018ADA)
			{
				for (j = i; j<__free_hole_counter - 1; j++)
					__free_hole[j] = __free_hole[j + 1];
				__free_hole_counter--;
				//printk("deleted @ %\n", i);
				i = 0;
				break;
			}
			i++;
		}
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void* Alloc(unsigned int bytes, unsigned int alignment, unsigned int boundary)
{
	unsigned int           i                 = 0;
	unsigned int           tmp               = 0;
	void*                  current_alloc_ptr = 0;
	unsigned int           got_from_hole     = 0;
	int                    hole_entry        = -1024;

	MEM_BLOCK_PRECEDENTIAL blk1;
	MEM_BLOCK_TERMINATION  blk2;

	current_alloc_ptr = __IMOS_retrive_alloc_ptr(bytes, alignment, boundary, &got_from_hole, &hole_entry); // either alloc_ptr or zeroth of an empty hole
	current_alloc_ptr = (void*)((unsigned int)current_alloc_ptr + MEMBLOCKSIZE);

	unsigned int pointer_for_alignment = ((unsigned int)(current_alloc_ptr)+(alignment - 1)) & (~(alignment - 1));
	unsigned int pointer_for_boundary  = ((unsigned int)(current_alloc_ptr)+(boundary  - 1)) & (~(boundary  - 1));

	if ((pointer_for_alignment + bytes) > pointer_for_boundary)
		pointer_for_alignment = pointer_for_boundary;

	tmp = pointer_for_alignment - (unsigned int)current_alloc_ptr;

	blk1.zeroth_ptr = (void*)((unsigned int)current_alloc_ptr - (MEMBLOCKSIZE));
	blk1.alignment_bound_space = tmp;

	blk1.start_ptr = (void*)((unsigned int)current_alloc_ptr + blk1.alignment_bound_space);
	blk1.end_ptr = (void*)((unsigned int)blk1.start_ptr + bytes);
	blk1.actual_allocated_bytes = bytes;
	blk1.from_heap_or_hole = ((got_from_hole) ? HEAP : HOLE);

	current_alloc_ptr = blk1.zeroth_ptr;
	*(MEM_BLOCK_PRECEDENTIAL*)current_alloc_ptr = blk1;

	blk2.alignment_bound_space = blk1.alignment_bound_space;
	*(MEM_BLOCK_TERMINATION*)((void*)(((unsigned int)current_alloc_ptr + sizeof(MEM_BLOCK_PRECEDENTIAL) + blk1.alignment_bound_space))) = blk2;

	if (blk1.from_heap_or_hole == HEAP)
		__IMOS_Heap = (void*)(((unsigned int)current_alloc_ptr + MEMBLOCKSIZE + blk1.alignment_bound_space + blk1.actual_allocated_bytes));
	else
	{
		//.printk("allocated from hole not heap ");
		if(hole_entry != -1)
			__IMOS_Remove_Entry(hole_entry);
	}

	//.printk("ZE = ^. S = ^. E = ^\n", (unsigned int)blk1.zeroth_ptr, (unsigned int)blk1.start_ptr, (unsigned int)blk1.end_ptr);

	//unsigned char* ttmp = (unsigned char*)blk1.start_ptr;
	//for (int j = 0; j < blk1.actual_allocated_bytes; j++)
	//	ttmp[j] = '+';

	return blk1.start_ptr;
}

void Free(void* ptr)
{
	MEM_BLOCK_PRECEDENTIAL* blk1 = 0;
	MEM_BLOCK_TERMINATION*  blk2 = 0;

	blk2 = (MEM_BLOCK_TERMINATION* )((void*)((unsigned int)ptr - sizeof(MEM_BLOCK_TERMINATION)));
	blk1 = (MEM_BLOCK_PRECEDENTIAL*)((void*)((unsigned int)ptr - (MEMBLOCKSIZE)-blk2->alignment_bound_space));

	__free_hole[__free_hole_counter].ptr  = blk1->zeroth_ptr;
	__free_hole[__free_hole_counter].size = MEMBLOCKSIZE + blk1->alignment_bound_space + blk1->actual_allocated_bytes;
	__free_hole[__free_hole_counter].next = (void*)((unsigned int)(blk1->zeroth_ptr) + __free_hole[__free_hole_counter].size);

	if (blk1->from_heap_or_hole == HEAP) //it it have been already from hole, there is no need to increment the hole counter
		__free_hole_counter++;

	__IMOS_SortFreeList();
	__IMOS_MergeFreeListEntries();
	__IMOS_RemoveTaggedEntries();

	//unsigned char* ttmp = (unsigned char*)blk1->zeroth_ptr;
	//unsigned int count = MEMBLOCKSIZE + blk1->alignment_bound_space + blk1->actual_allocated_bytes;
	//for (int j = 0; j < count; j++)
	//	ttmp[j] = '-';

	return;
}

void mwrite(unsigned int base, unsigned int offset, unsigned int value)
{
	unsigned int ds;
	CHECK_DS(&ds);
	if(!ds)
	{
		printk("DS is NOT correct\n");
		return;
	}
	unsigned int target_pointer = base + offset;
	void* target = (void*)target_pointer;
	*(volatile unsigned int*)target = value;
}

unsigned int mread(unsigned int base, unsigned int offset)
{
	unsigned int ds;
	CHECK_DS(&ds);
	if(!ds)
	{
		printk("DS is NOT correct\n");
		return 0;
	}
	unsigned int target_pointer = base + offset;
	void* target = (void*)target_pointer;
	return *(volatile unsigned int*)target;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+