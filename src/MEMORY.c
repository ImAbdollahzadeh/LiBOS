#include "../include/MEMORY.h"
#include "../include/PRINT.h"
#include "../include/IDT.h"
#include "../include/USER_MODE.h"
#include "../include/PAGING.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

/* 4GiB memory (x86) ********************************************************************************************************
             start         end          size                  region/exception    description
             0x00100000    0x00EFFFFF	0x00E00000 (14 MiB)   RAM, free           Extended memory
             0x00F00000    0x00FFFFFF	0x00100000 (1 MiB)    mmapped hardware    ISA Memory Hole 15-16MB
             0x01000000    ????????     ????????              RAM, free           ...         
             0xC0000000    0xFFFFFFFF	0x40000000 (1 GiB)    various             Mmapped PCI, PnP, APIC, BIOS 
 ****************************************************************************************************************************/

/* 750 MB, i.e. 192000 pages each 4KB, i.e. 6000 UINT_32 */
static UINT_32 mem_block_allocator_bitmap[6000];

#define BLOCK_ALLOCATOR_HEAP     0x80000000
#define NON_BLOCK_ALLOCATOR_HEAP 0xB0000000

void* non_block_allocator_heap = (void*)NON_BLOCK_ALLOCATOR_HEAP;
void* current_pointer          = (void*)NON_BLOCK_ALLOCATOR_HEAP;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern void CHECK_DS(UINT_32* ds);
extern void CHECK_GS(UINT_32* gs);
extern void CHECK_ES(UINT_32* es);
extern void CHECK_FS(UINT_32* fs);

static void* __LiBOS_retrive_alloc_ptr(UINT_32 bytes, UINT_32 alignment, UINT_32 boundary, UINT_32* hole_or_heap, INT_32* entry)
{
	return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void __LiBOS_Remove_Entry(UINT_32 entry)
{
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void __LiBOS_SortFreeList(void)
{
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void __LiBOS_MergeFreeListEntries(void)
{
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void __LiBOS_RemoveTaggedEntries(void)
{
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void* Alloc(UINT_32 bytes, UINT_32 alignment, UINT_32 boundary)
{
	return 0;
}

void Free(void* ptr)
{
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

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

void* physical_page_alloc(void)
{
	/* find the first clear page */
	UINT_32 counter   = 0;
	UINT_32 clear_bit = 0;
	while(counter < 6000)
	{
		if( mem_block_allocator_bitmap[counter] != 0xFFFFFFFF ) // there is at least a bit 0 inside
		{
			/* find the first 0 bit */
			UINT_32 bit = 0;
			while(bit < 32)
			{
				if( (mem_block_allocator_bitmap[counter] & (1 << bit)) == 0 )
				{
					clear_bit = bit;
					goto FOUND_CLEAR_BIT;
				}
				bit++;
			}
		}
		else
			counter++;
	}
	
	/* if it reaches here, there is no page available */
	panic("there is no memmory page available anymore\n");
	return 0;
	
FOUND_CLEAR_BIT:
	/* label the bitmap as dirty */
	mem_block_allocator_bitmap[counter] |= (1 << clear_bit);
	
	/* return its address */
	UINT_32 page = BLOCK_ALLOCATOR_HEAP + ((32 * counter + clear_bit) * 0x1000);
	return (void*)page;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void physical_page_free(void* page)
{
	/* calculate the corresponding bit */
	UINT_32 global_bit = ((UINT_32)page - BLOCK_ALLOCATOR_HEAP) / 0x1000;
	UINT_32 index      = global_bit / 32;
	UINT_32 bit        = global_bit - index;
	
	/* label the bitmap as clear */
	mem_block_allocator_bitmap[index] &= ~(1 << bit);
	
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void initialize_page_allocator(void)
{
	/* make all pages as clear (a.k.a. non-allocated) */
	__LiBOS_MemZero(mem_block_allocator_bitmap, 6000*sizeof(UINT_32));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void initialize_non_page_allocator(void)
{
	/* make all memory space clear (a.k.a. non-allocated) */
	__LiBOS_MemZero(non_block_allocator_heap, 0x10000000 /* 256 MB */);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void* LiBOS_non_page_alloc(UINT_32 bytes, UINT_32 alignment, UINT_32 boundary, UINT_32 user_or_kernel)
{
	UINT_32 process_pdbr = 0;
	
	/* very important: switch to LiBOS main PDBR to be able to dig into memory space */
	if(user_or_kernel == USER_MODE_PROCESS)
	{
		/* invoke int 0x80 */
		PAGE_DIRECTORY* libos_pd = get_libos_main_page_directory();
		asm_user_int_0x80(4, &process_pdbr, 0);
		asm_user_int_0x80(3, libos_pd, 0);
	}
	else { ; }
	
	/* now search for clear space, non-allocated-before, and big enough for the allocation */
	UINT_32 i = 0;
	LIBOS_MEMORY_DESCRIPTOR* md = 0;
	while(i < 0x10000000)
	{
		md = (LIBOS_MEMORY_DESCRIPTOR*)(PHYSICAL_ADDRESS(non_block_allocator_heap) + i);
		if( md->descriptor_id == 0 )
		{
			/* a free space found. calculate the needed space */
			UINT_32 begin         = PHYSICAL_ADDRESS(md) + sizeof(LIBOS_MEMORY_DESCRIPTOR);
			UINT_32 begin_aligned = (begin + (alignment - 1)) &~ (alignment - 1);
			UINT_32 actual_bytes  = begin_aligned - begin + bytes;
			
			/*  */
			UINT_32 space = 0;
			UINT_8* p = (UINT_8*)md;
			while( *p == 0 )
			{
				space++;
				p++;
				
				/* you shouldn't pass HEAP_MEM */
				if(space >= actual_bytes)
					break;
			}
			
			//--if(actual_bytes > boundary)
			//--{
			//--	/* look for further non-allocated space */
			//--	i += ( PHYSICAL_ADDRESS(md->end) - PHYSICAL_ADDRESS(non_block_allocator_heap) ); 
			//--	space = 0; // a trick to jump over the next block
			//--}
			
			/* check if the space is big enough */
			if(actual_bytes <= space)
			{
				md->descriptor_id = MAGIC_MEMORY_DESCROPTOR;
				md->start = (void*)( begin_aligned );
				md->end   = (void*)( begin_aligned + bytes );
				goto ALLOC_FINAL_SETTINGS;
			}
		}
		else
			i += ( PHYSICAL_ADDRESS(md->end) - PHYSICAL_ADDRESS(non_block_allocator_heap) ); 
	}
	
	/* take care of paging switching back to the user or kernel process */
ALLOC_FINAL_SETTINGS:
	if(user_or_kernel == USER_MODE_PROCESS)
		asm_user_int_0x80(3, process_pdbr, 0);
	else { ; }
	
	/* return the allocated pointer */
	return (void*)(md->start);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void LiBOS_non_page_free(void* ptr, UINT_32 user_or_kernel)
{
	UINT_32            space_to_clear = 0;
	LIBOS_MEMORY_DESCRIPTOR* md             = (LIBOS_MEMORY_DESCRIPTOR*)(PHYSICAL_ADDRESS(ptr) - sizeof(LIBOS_MEMORY_DESCRIPTOR));
	
	/* very important: switch off paging to be able to dig into memory space */
	if(user_or_kernel == USER_MODE_PROCESS)
	{
		/* invoke int 0x80 for paging_off {eax = 1, ebx = ecx = 0 } */
		asm_user_int_0x80(1, 0, 0);
	}
	else
	{ ; }
	
	/* sanity check */
	if(md->descriptor_id != MAGIC_MEMORY_DESCROPTOR)
	{
		panic("provided address to free is invalid\n");
		goto FREE_FINAL_SETTING;
	}
	
	/* get the space and zero it */
	__LiBOS_MemZero(md, PHYSICAL_ADDRESS(md->end));
	
FREE_FINAL_SETTING:
	if(user_or_kernel == USER_MODE_PROCESS)
	{
		/* invoke int 0x80 for paging_on {eax = 2, ebx = ecx = 0 } */
		/* invoke int 0x80 for paging_off {eax = 1, ebx = ecx = 0 } */
		//asm_user_int_0x80(1, 0, 0);
		PAGE_DIRECTORY* pd = get_libos_main_page_directory();
		UINT_32 pdbr = 0;
		asm_user_int_0x80(4, &pdbr, 0);
		asm_user_int_0x80(3, pd, 0);
	}
	else
	{ ; }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void* kernel_alloc(UINT_32 bytes, UINT_32 alignment, UINT_32 boundary)
{
	return LiBOS_non_page_alloc(bytes, alignment, boundary, KERNEL_MODE_PROCESS);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void* user_alloc(UINT_32 bytes, UINT_32 alignment)
{
	return LiBOS_non_page_alloc(bytes, alignment, 0xFFFFFFFF, USER_MODE_PROCESS);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void kernel_free(void* pointer)
{
	return LiBOS_non_page_free(pointer, KERNEL_MODE_PROCESS);
}

//--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void user_free(void* pointer)
{
	return LiBOS_non_page_free(pointer, USER_MODE_PROCESS);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+