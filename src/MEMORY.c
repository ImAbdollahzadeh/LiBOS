#include "../include/MEMORY.h"
#include "../include/PRINT.h"
#include "../include/IDT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

/* 4GiB memory (x86) ********************************************************************************************************
             start         end          size                  region/exception    description
             0x00100000    0x00EFFFFF	0x00E00000 (14 MiB)   RAM, free           Extended memory
             0x00F00000    0x00FFFFFF	0x00100000 (1 MiB)    mmapped hardware    ISA Memory Hole 15-16MB
             0x01000000    ????????     ????????              RAM, free           ...         
             0xC0000000    0xFFFFFFFF	0x40000000 (1 GiB)    various             Mmapped PCI, PnP, APIC, BIOS 
 ****************************************************************************************************************************/


void* __LiBOS_Heap = (void*)( /*0x01000000*/0x40000000 );

#define SWAP_HOLE_ENTRIES(ENT1,ENT2) do { FREE_HOLE tmp; tmp = *ENT1; *ENT1 = *ENT2; *ENT2 = tmp; } while(0)
#define MEMBLOCKSIZE (sizeof(MEM_BLOCK_PRECEDENTIAL) + sizeof(MEM_BLOCK_TERMINATION))
#define HEAP 1
#define HOLE 0

static FREE_HOLE __free_hole[1024];
static UINT_32   __free_hole_counter = 0;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern void CHECK_DS(UINT_32* ds);
extern void CHECK_GS(UINT_32* gs);
extern void CHECK_ES(UINT_32* es);
extern void CHECK_FS(UINT_32* fs);

static void* __LiBOS_retrive_alloc_ptr(UINT_32 bytes, UINT_32 alignment, UINT_32 boundary, UINT_32* hole_or_heap, INT_32* entry)
{
	UINT_32 i = 0;
	UINT_32 size = 0;

	for (i = 0; i< __free_hole_counter; i++)
	{
		void* ptr    = __free_hole[i].ptr;
		void* pptr   = (void*)((UINT_32)ptr + MEMBLOCKSIZE);
		UINT_32 algn = ((UINT_32)(pptr)+(alignment - 1)) & (~(alignment - 1));
		UINT_32 bond = ((UINT_32)(pptr)+(boundary - 1)) & (~(boundary - 1));
		size = (algn - (UINT_32)ptr) + bytes;

		// the boundary must be confined in the free hole entry limitation
		if (bond < ((UINT_32)ptr + __free_hole[i].size))
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
	return __LiBOS_Heap;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void __LiBOS_Remove_Entry(UINT_32 entry)
{
	int i;
	for (i = entry; i < __free_hole_counter; i++)
		__free_hole[i] = __free_hole[i + 1];
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void __LiBOS_SortFreeList(void)
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

static void __LiBOS_MergeFreeListEntries(void)
{
	UINT_32 i = 0;
	UINT_32 j = 0;
	UINT_32 k = 0;
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

static void __LiBOS_RemoveTaggedEntries(void)
{
	UINT_32 tagged = 0, i, j, ii;

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

void* Alloc(UINT_32 bytes, UINT_32 alignment, UINT_32 boundary)
{
	UINT_32 i                 = 0;
	UINT_32 tmp               = 0;
	void*   current_alloc_ptr = 0;
	UINT_32 got_from_hole     = 0;
	int     hole_entry        = -1024;

	MEM_BLOCK_PRECEDENTIAL blk1;
	MEM_BLOCK_TERMINATION  blk2;

	current_alloc_ptr = __LiBOS_retrive_alloc_ptr(bytes, alignment, boundary, &got_from_hole, &hole_entry); // either alloc_ptr or zeroth of an empty hole
	current_alloc_ptr = (void*)((UINT_32)current_alloc_ptr + MEMBLOCKSIZE);

	UINT_32 pointer_for_alignment = ((UINT_32)(current_alloc_ptr)+(alignment - 1)) & (~(alignment - 1));
	UINT_32 pointer_for_boundary  = ((UINT_32)(current_alloc_ptr)+(boundary  - 1)) & (~(boundary  - 1));

	if ((pointer_for_alignment + bytes) > pointer_for_boundary)
		pointer_for_alignment = pointer_for_boundary;

	tmp = pointer_for_alignment - (UINT_32)current_alloc_ptr;

	blk1.zeroth_ptr = (void*)((UINT_32)current_alloc_ptr - (MEMBLOCKSIZE));
	blk1.alignment_bound_space = tmp;

	blk1.start_ptr = (void*)((UINT_32)current_alloc_ptr + blk1.alignment_bound_space);
	blk1.end_ptr = (void*)((UINT_32)blk1.start_ptr + bytes);
	blk1.actual_allocated_bytes = bytes;
	blk1.from_heap_or_hole = ((got_from_hole) ? HEAP : HOLE);

	current_alloc_ptr = blk1.zeroth_ptr;
	*(MEM_BLOCK_PRECEDENTIAL*)current_alloc_ptr = blk1;

	blk2.alignment_bound_space = blk1.alignment_bound_space;
	*(MEM_BLOCK_TERMINATION*)((void*)(((UINT_32)current_alloc_ptr + sizeof(MEM_BLOCK_PRECEDENTIAL) + blk1.alignment_bound_space))) = blk2;

	if (blk1.from_heap_or_hole == HEAP)
		__LiBOS_Heap = (void*)(((UINT_32)current_alloc_ptr + MEMBLOCKSIZE + blk1.alignment_bound_space + blk1.actual_allocated_bytes));
	else
	{
		//.printk("allocated from hole not heap ");
		if(hole_entry != -1)
			__LiBOS_Remove_Entry(hole_entry);
	}

	//.printk("ZE = ^. S = ^. E = ^\n", (UINT_32)blk1.zeroth_ptr, (UINT_32)blk1.start_ptr, (UINT_32)blk1.end_ptr);

	//UINT_8* ttmp = (UINT_8*)blk1.start_ptr;
	//for (int j = 0; j < blk1.actual_allocated_bytes; j++)
	//	ttmp[j] = '+';

	return blk1.start_ptr;
}

void Free(void* ptr)
{
	MEM_BLOCK_PRECEDENTIAL* blk1 = 0;
	MEM_BLOCK_TERMINATION*  blk2 = 0;

	blk2 = (MEM_BLOCK_TERMINATION* )((void*)((UINT_32)ptr - sizeof(MEM_BLOCK_TERMINATION)));
	blk1 = (MEM_BLOCK_PRECEDENTIAL*)((void*)((UINT_32)ptr - (MEMBLOCKSIZE)-blk2->alignment_bound_space));

	__free_hole[__free_hole_counter].ptr  = blk1->zeroth_ptr;
	__free_hole[__free_hole_counter].size = MEMBLOCKSIZE + blk1->alignment_bound_space + blk1->actual_allocated_bytes;
	__free_hole[__free_hole_counter].next = (void*)((UINT_32)(blk1->zeroth_ptr) + __free_hole[__free_hole_counter].size);

	if (blk1->from_heap_or_hole == HEAP) //it it have been already from hole, there is no need to increment the hole counter
		__free_hole_counter++;

	__LiBOS_SortFreeList();
	__LiBOS_MergeFreeListEntries();
	__LiBOS_RemoveTaggedEntries();

	//UINT_8* ttmp = (UINT_8*)blk1->zeroth_ptr;
	//UINT_32 count = MEMBLOCKSIZE + blk1->alignment_bound_space + blk1->actual_allocated_bytes;
	//for (int j = 0; j < count; j++)
	//	ttmp[j] = '-';

	return;
}

void mwrite(const UINT_32 base, const UINT_32 offset, const UINT_32 value)
{
	UINT_32 ds, es, gs, fs;
	CHECK_DS(&ds);
	if(!ds)
	{
		printk("DS is NOT correct\n");
		return;
	}
	CHECK_GS(&gs);
	if(!gs)
	{
		printk("GS is NOT correct\n");
		return;
	}
	CHECK_ES(&es);
	if(!es)
	{
		printk("ES is NOT correct\n");
		return;
	}
	CHECK_FS(&fs);
	if(!fs)
	{
		printk("FS is NOT correct\n");
		return;
	}
	UINT_32 target_pointer = base + offset;
	void* target = (void*)target_pointer;
	*(volatile UINT_32*)target = value;
}

UINT_32 mread(const UINT_32 base, const UINT_32 offset)
{
	UINT_32 ds, es, gs, fs;
	CHECK_DS(&ds);
	if(!ds)
	{
		printk("DS is NOT correct\n");
		return 0;
	}
	CHECK_GS(&gs);
	if(!gs)
	{
		printk("GS is NOT correct\n");
		return 0;
	}
	CHECK_ES(&es);
	if(!es)
	{
		printk("ES is NOT correct\n");
		return 0;
	}
	CHECK_FS(&fs);
	if(!fs)
	{
		printk("FS is NOT correct\n");
		return 0;
	}
	UINT_32 target_pointer = base + offset;
	void* target = (void*)target_pointer;
	return *(volatile UINT_32*)target;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+