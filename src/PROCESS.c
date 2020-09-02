#include "../include/PROCESS.h"
#include "../include/PRINT.h"
#include "../include/MEMORY.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

#define PROCESS_BEGIN_VIRTUAL_ADDRESS 0x1000000
#define PROCESS_DEFAULT_PAGE_SIZE     0x1000

PAGE_DIRECTORY* LiBOS_main_page_directory = 0; /* VERY IMPORTANT */

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void process_memcopy(void* src, void* trg, UINT_32 bytes)
{
	UINT_32 i;
	INT_8* ssrc = (INT_8*)src;
	INT_8* ttrg = (INT_8*)trg;
	for(i = 0; i< bytes; i++)
		ttrg[i] = ssrc[i];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static PAGE_DIRECTORY* create_address_space(void)
{
	PAGE_DIRECTORY* dir = (PAGE_DIRECTORY*)alloc_physical_block();
	if (!dir)
		return 0;
	__LiBOS_MemZero(dir, sizeof(PAGE_DIRECTORY));
	return dir;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL initialize_process(void)
{
	if( !paging_is_activate() )
	{
		panic("cannot initialize proceses without paging activation\n");
		return FALSE;
	}
	
	LiBOS_main_page_directory = (PAGE_DIRECTORY*)(get_pdbr());
	if( !LiBOS_main_page_directory )
	{
		panic("LiBOS has no initialized PDBR\n");
		return FALSE;
	}
	
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL create_process(UINT_32 function_address)
{
	PAGE_DIRECTORY* address_space;
	PROCESS*        process;
	void*           memory;
	THREAD*         main_thread;
	
	if (!function_address)
	{
	    panic("provided process's function address is invalid\n");
	    return FALSE;
	}
	
	/* get process virtual address space */
	address_space = create_address_space();
	if (!address_space)
	{
	    printk("process with function address ^ failed to create a virtual address space\n", function_address);
	    return FALSE;
	}
	
	main_thread = (THREAD*)Alloc(sizeof(THREAD), 1, 1);
	__LiBOS_MemZero(main_thread, sizeof(THREAD));
	
	/* rgister main_thread into process's list of threads */
	process->thread_list[0] = *main_thread;
		
	/* assign one physical frame */
	memory = alloc_physical_block();
	
	/* map kernel space into process address space (i.e. address 0 to 16MB) */
	UINT_32 i;
	for(i = 0; i < 4; i++)
		process_memcopy(&(LiBOS_main_page_directory->entries[i]), &(address_space->entries[i]), sizeof(UINT_32));
	
	/* create PCB */
	process                 = (PROCESS*)Alloc(sizeof(PROCESS), 1, 1);
	process->pid            = 1;
	process->page_directory = address_space;
	process->priority       = 1;
	process->state          = PROCESS_STATE_ACTIVE;
	process->cpu            = 0 /* for now BSP cpu */;
	process->ioapic         = 0 /* for now BSP cpu */;
	
	/* create thread descriptor */
	main_thread->kernel_stack  = 0;
	main_thread->parent        = process;
	main_thread->priority      = 1;
	main_thread->state         = PROCESS_STATE_ACTIVE;
	main_thread->initial_stack = 0;
	main_thread->stack_limit   = (void*)((UINT_32)main_thread->initial_stack + PROCESS_DEFAULT_PAGE_SIZE); // default: 4KB
	__LiBOS_MemZero(&main_thread->frame, sizeof(TRAP_FRAME));
	main_thread->frame.eip     = function_address;
	main_thread->frame.flags   = 0x200;

	/* map page into address space */
	map_physical_address(address_space, PROCESS_BEGIN_VIRTUAL_ADDRESS, (UINT_32)memory, I86_PTE_PRESENT|I86_PTE_WRITABLE);
	
	/* Create stack */
	void* stack      = (void*)(PROCESS_BEGIN_VIRTUAL_ADDRESS + PROCESS_DEFAULT_PAGE_SIZE);
	void* stack_phys = (void*)alloc_physical_block();

	/* map process's stack space */
	map_physical_address(address_space, (UINT_32)stack, (UINT_32)stack_phys, I86_PTE_PRESENT|I86_PTE_WRITABLE);
	
	/* final initialization */
	main_thread->initial_stack = stack;
	main_thread->frame.esp     = (UINT_32)main_thread->initial_stack;
	main_thread->frame.ebp     = main_thread->frame.esp;
	
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void execute_process(PROCESS* process)
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void terminate_process(PROCESS* process)
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

THREAD* create_thread(PROCESS* process)
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+