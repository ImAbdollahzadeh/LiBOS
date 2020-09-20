#include "../include/PROCESS.h"
#include "../include/PRINT.h"
#include "../include/MEMORY.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

#define PROCESS_BEGIN_VIRTUAL_ADDRESS 0x40000000 // at address 1GB
#define PROCESS_DEFAULT_PAGE_SIZE     0x1000

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL initialize_process(void)
{
	if( !paging_is_activate() )
	{
		panic("cannot initialize proceses without paging activation\n");
		return FALSE;
	}
	
	PAGE_DIRECTORY* pd = (PAGE_DIRECTORY*)(get_pdbr());
	if( !pd )
	{
		panic("LiBOS has no initialized PDBR\n");
		return FALSE;
	}
	
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

PROCESS* create_process(UINT_32 function_address)
{
	PAGE_DIRECTORY* address_space;
	PROCESS*        process;
	THREAD*         main_thread;
	
	/* assign the process virtual address space, zero it, and map it to itself (IMPORTANT for later page fault handlings) */
	address_space = (PAGE_DIRECTORY*)physical_page_alloc();
	if (!address_space)
	{
	    printk("process with function address ^ failed to create a virtual address space\n", function_address);
	    return 0;
	}
	__LiBOS_MemZero(address_space, 4096);
	map_physical_address(address_space, address_space, address_space, (I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER));
	
	/* assign the process main thread, zero it, and map it to the process's page directory */
	main_thread = (THREAD*)physical_page_alloc();
	__LiBOS_MemZero(main_thread, 4096);
	map_physical_address(address_space, main_thread, main_thread, (I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER));
	
	/* map KERN_MEM (address 0 to 1GB) and SYS_MEM into process's address space (address 3GB to 4GB) */
	UINT_32 i;
	PAGE_DIRECTORY* mpgd = get_libos_main_page_directory();
	for(i = 0; i < 256; i++)
	{
		__LiBOS_MemCopy(&(mpgd->entries[i      ]), &(address_space->entries[i      ]), sizeof(UINT_32)); /* 0GB - 1GB ~> for KERN_MEM */
		__LiBOS_MemCopy(&(mpgd->entries[768 + i]), &(address_space->entries[768 + i]), sizeof(UINT_32)); /* 3GB - 4GB ~> for SYS_MEM  */
	}
	
	/* create PCB */
	process                 = (PROCESS*)physical_page_alloc();
	__LiBOS_MemZero(process, 4096);
	map_physical_address(address_space, process, process, (I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER));
	process->pid            = 1;
	process->page_directory = address_space;
	process->priority       = 1;
	process->state          = PROCESS_STATE_ACTIVE;
	process->cpu            = 0 /* for now BSP cpu */;
	process->ioapic         = 0 /* for now BSP cpu */;
	
	/* create thread descriptor */
	__LiBOS_MemZero(&main_thread->frame, sizeof(TRAP_FRAME));
	main_thread->parent        = process;
	main_thread->priority      = 1;
	main_thread->state         = PROCESS_STATE_ACTIVE;
	main_thread->frame.eip     = function_address;
	main_thread->frame.flags   = INTERRUPT_ENABLE_FLAG;
	
	/* Create stack, zero it, and map it to the process's page directory */
	void* stack      = (void*)PROCESS_BEGIN_VIRTUAL_ADDRESS;
	void* stack_phys = physical_page_alloc();
	__LiBOS_MemZero(stack_phys, 4096);
	map_physical_address(address_space, stack, stack_phys, (I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER));
	
	/* final initialization */
	main_thread->initial_stack = stack + PROCESS_DEFAULT_PAGE_SIZE;
	main_thread->stack_limit   = (void*)stack; // default: 4KB
	main_thread->frame.esp     = (UINT_32)main_thread->initial_stack;
	main_thread->frame.ebp     = (UINT_32)main_thread->initial_stack;
	
	/* rgister main_thread into process's list of threads */
	process->thread_list[0] = *main_thread;
	return process;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

THREAD* create_thread(PROCESS* process)
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void execute_process(PROCESS* process)
{
	if (process->pid == PROC_INVALID_ID)
		return;
	if (!process->page_directory)
		return;
	
	/* get esp and eip of main thread */
	UINT_32 entry_point   = (process->thread_list[0]).frame.eip;
	UINT_32 process_stack = (process->thread_list[0]).frame.esp;
	
	/* switch to process address space */
	_CLI();
	set_pdbr(process->page_directory);
	
	/* execute process in kernel mode */
	execute_kernel_mode_process(process_stack, entry_point);
	
	/* switch back to kernel address space */
	PAGE_DIRECTORY* mpgd = get_libos_main_page_directory();
	_CLI();
	set_pdbr(mpgd->entries);
	_STI();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void terminate_process(PROCESS* process) 
{
	if (process->pid == PROC_INVALID_ID)
		return;
	
	/* release threads */
	UINT_8 i = 0;
	THREAD* thread = &(process->thread_list[i]);
	
	/* get physical address of stack */
	void* stack_frame = get_physical_address(process->page_directory, (UINT_32)thread->initial_stack); 
	
	/* unmap and release stack memory */
	unmap_physical_address(process->page_directory, (UINT_32)thread->initial_stack);
	//-------------free_physical_block(stack_frame); // ALREADY DONE ?!
	
	/* unmap and release image memory */
	UINT_32 page;
	for(UINT_32 page = 0; page < 1 /* by default, we assumed no process larger than 4KB */; page++)
	{
		UINT_32 phys = 0;
		UINT_32 virt = 0;
	
		/* get virtual address of page */
		//--------------virt = thread->imageBase + (page * PAGE_SIZE);
	
		/* get physical address of page */
		phys = (UINT_32)get_physical_address(process->page_directory, virt);
	
		/* unmap and release page */
		unmap_physical_address(process->page_directory, virt);
		//----free_physical_block((void*)phys);
	}

	/* restore kernel selectors */
	//------------------restore_kernel_after_process_termination();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+