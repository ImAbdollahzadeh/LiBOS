#include "../include/PAGING.h"
#include "../include/PRINT.h"
#include "../include/MEMORY.h"
#include "../include/IDT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

#define PAGE_DIRECTORY_INDEX(x)     (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x)         (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

/* ------------------------------------------------ temp */
#define MEM_BASE 0x10000000
static UINT_32 current_frame = 0;
/* ----------------------------------------------------- */
       UINT_32 current_pdbr                         = 0;
static BOOL paging                                  = FALSE;
static PAGE_DIRECTORY* libos_initial_page_directory = 0;
UINT_32* table1 = 0, *table2 = 0, *table3 = 0;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 alloc_physical_block(void)
{
	//UINT_32 cur_ptr = MEM_BASE + current_frame * 0x1000;
	//current_frame++;
	return (UINT_32)Alloc(4096, 4096, 4096);
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
	
	UINT_32 i, j;
	for(i = 0; i < 1024; i++)
	{
		table = (PAGE_TABLE*)alloc_physical_block();
		for(j = 0; j < 1024; j++)
			table->entries[j] = (MEGA_BYTE(4*i) + j * 0x1000) | (I86_PTE_WRITABLE | I86_PTE_PRESENT);
		
		libos_initial_page_directory->entries[i] = PHYSICAL_ADDRESS(table) | (I86_PDE_WRITABLE | I86_PDE_PRESENT);
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