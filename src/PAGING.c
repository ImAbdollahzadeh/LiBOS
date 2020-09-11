#include "../include/PAGING.h"
#include "../include/PRINT.h"
#include "../include/MEMORY.h"
#include "../include/IDT.h"

/*
                     directory index                                             page table index                                                    offset into page
 <-------------------10 bit---------------------------------> <-------------------10 bit--------------------------------> <-------------------12 bit--------------------------------------------->
 
   31    30    29    28    27    26    25    24    23    22    21   20    19    18    17    16    15    14    13    12    11    10     9     8     7     6     5     4     3     2     1     0
  ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
 |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
  ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
  
ex. ((virtual_address >> 22) & 0x3FF) ~> directory index 
ex. ((virtual_address >> 12) & 0x3FF) ~> page table index 
ex. (virtual_address & 0xFFF)         ~> offset into page

bits 12...31 of a page table entry is physical frame address of a page(4KB aligned)
ex. (page table entry & ~0xFFF)       ~> physical frame address of a page

bits 12...31 of a page directory entry is physical frame address of a page table (4KB aligned)
ex. (page directory entry & ~0xFFF)   ~> physical frame address of a page table

 */


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

       UINT_32 current_pdbr                         = 0;
static BOOL paging                                  = FALSE;
static PAGE_DIRECTORY* libos_initial_page_directory = 0;

#define LiBOS_PAGE_SIZE             0x1000                
#define PAGE_DIRECTORY_INDEX(x)     (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x)         (((x) >> 12) & 0x3FF)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xFFF)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 alloc_physical_block(void)
{
	return (UINT_32)Alloc(LiBOS_PAGE_SIZE, LiBOS_PAGE_SIZE, LiBOS_PAGE_SIZE);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void free_physical_block(void* ptr)
{
	Free(ptr);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL paging_is_activate(void)
{
	return paging;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL start_paging(void)
{
	if( paging_is_activate() )
	{
		panic("paging has been already activated\n");
		return FALSE;
	}

	libos_initial_page_directory = (PAGE_DIRECTORY*)alloc_physical_block();
	PAGE_TABLE* table            = 0;
	
	/* clear out all pages */
	__LiBOS_MemZero(libos_initial_page_directory, sizeof(PAGE_DIRECTORY));
	
	/* complete 4GB address space identity map */
	UINT_32 i, j;
	for(i = 0; i < 1024; i++)
	{
		table = (PAGE_TABLE*)alloc_physical_block();
		for(j = 0; j < 1024; j++)
			table->entries[j] = (MEGA_BYTE(4*i) + j * LiBOS_PAGE_SIZE) | (I86_PTE_WRITABLE | I86_PTE_PRESENT | I86_PTE_USER);
		
		libos_initial_page_directory->entries[i] = PHYSICAL_ADDRESS(table) | (I86_PDE_WRITABLE | I86_PDE_PRESENT | I86_PDE_USER);
	}
	
	/* disable interrupts */
	_CLI();
	
	/* set pdbr */
	set_pdbr(libos_initial_page_directory->entries);
	
	/* enable paging */
	paging_enable();
	
	paging = TRUE;
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL create_page_table(PAGE_DIRECTORY* dir, UINT_32 virt, UINT_32 flags)
{
	UINT_32* pagedir = dir->entries;
	if ( pagedir[virt >> 22] == 0 )
	{
		void* block = alloc_physical_block();
		if (!block)
			return FALSE; 
		
		pagedir[virt >> 22] = ((UINT_32) block) | flags;
		__LiBOS_MemZero((void*)(pagedir[virt >> 22]), LiBOS_PAGE_SIZE);

		/* map page table into directory */
		map_physical_address(dir, (UINT_32) block, (UINT_32) block, flags);
	}
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void map_physical_address(PAGE_DIRECTORY* dir, UINT_32 virt, UINT_32 phys, UINT_32 flags)
{
	UINT_32* pagedir = dir->entries;
	if ( pagedir[virt >> 22] == 0 )
		create_page_table (dir, virt, flags);
	((UINT_32*) (pagedir[virt >> 22] & ~0xFFF))[virt << 10 >> 10 >> 12] = phys | flags;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void* get_physical_address(PAGE_DIRECTORY* dir, UINT_32 virt)
{
	UINT_32* pagedir = dir->entries;
	if ( pagedir[virt >> 22] == 0 )
		return 0;
	return (void*)((UINT_32*)(pagedir[virt >> 22] & ~0xFFF))[virt << 10 >> 10 >> 12];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void unmap_page_table (PAGE_DIRECTORY* dir, UINT_32 virt)
{
	UINT_32* pagedir = dir->entries;
	if (pagedir[virt >> 22] != 0)
	{
		/* get mapped frame */
		void* frame = (void*)(pagedir[virt >> 22] & 0x7FFFF000);
		
		/* unmap frame */
		free_physical_block(frame);
		pagedir[virt >> 22] = 0;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void unmap_physical_address(PAGE_DIRECTORY* dir, UINT_32 virt)
{
	UINT_32* pagedir = dir->entries;
	if (pagedir[virt >> 22] != 0)
		unmap_page_table (dir, virt);
	((UINT_32*)(pagedir[virt >> 22] & ~0xFFF))[virt << 10 >> 10 >> 12] = 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+