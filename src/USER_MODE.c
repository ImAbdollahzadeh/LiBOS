#include "../include/PRINT.h"
#include "../include/USER_MODE.h"
#include "../include/PROCESS.h"
#include "../include/IDT.h"
#include "../include/PAGING.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

void __process_test(void)
{
	UINT_32 tmp = 652;
	printk("during executing process with number %\n", tmp);
	
	/* create a page fault to see the handling */
	void* addr = (void*)0x40009F00;
	*(UINT_32*)addr = 42;
	*(UINT_32*)addr = 475;
	addr = (void*)0x40009000;
	*(UINT_32*)addr = 12;
}




void printu(UINT_32 string_address_to_print)
{ 
	printk(string_address_to_print);
}

void enter_critical_section(void)
{ ; }

void leave_critical_section(void)
{ ; }

void user_set_pdbr(void)
{ ; }

void user_get_pdbr(void)
{ ; }

void user_mode_execute_process(UINT_32 pointer_to_process)
{
	PROCESS* process = (PROCESS*)pointer_to_process;
	execute_process(process);
}



void* LiBOS_system_calls[] = 
{
	&printu,
	&enter_critical_section,
	&leave_critical_section,
	&user_set_pdbr,
	&user_get_pdbr,
	&user_mode_execute_process,
};

void syscall_dispatcher(REGS* r) 
{
	void(*hnd)(UINT_32) = (void(*)(UINT_32))(LiBOS_system_calls[r->eax]);
	hnd(r->ebx);
}

void initialize_system_calls(void)
{
	irq_install_handler(96, &syscall_dispatcher);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void __LiBOS_user_mode_start(void)
{
	printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n");
	printk("         Welcome to LiBOS usermode           \n");
	printk("All system calls can be done through INT 0x80\n");
	printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n");
	
	/* register interrupt vector 0x80 (decimal 128) in first place */
	initialize_system_calls();
	
	/*a minimal sysem call test */
	const INT_8* str = "from user mode system call int 0x80 test\n";
	asm_user_int_0x80(0, (UINT_32)str, 0);
	
	/* start the user space processes */
	PROCESS* process = create_process( PHYSICAL_ADDRESS(&__process_test) );
	if(process)
	{
		/* execute process */
		asm_user_int_0x80(5, (UINT_32)process, 0);
		
		/* terminate process */
		terminate_process(process); 
	}
	
	while(1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void initiate_usermode(void)
{
	/* give the actual address of the usermode starting function */
	UINT_32 entry = PHYSICAL_ADDRESS(&__LiBOS_user_mode_start);
	enter_user_mode(entry);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
