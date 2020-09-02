#ifndef PROCESS__H__
#define PROCESS__H__

#include "LiBOS_CORE.h"
#include "PAGING.h"
#include "MP.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ CONSTANTS

#define PROCESS_STATE_SLEEP  0
#define PROCESS_STATE_ACTIVE 1

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ ENUMS



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ DATA STRUCTURES

typedef struct _TRAP_FRAME {
	UINT_32 esp;
	UINT_32 ebp;
	UINT_32 eip;
	UINT_32 edi;
	UINT_32 esi;
	UINT_32 eax;
	UINT_32 ebx;
	UINT_32 ecx;
	UINT_32 edx;
	UINT_32 flags;
} __attribute__ ((packed)) TRAP_FRAME;

typedef struct _THREAD {
	struct _PROCESS* parent;
	void*            initial_stack;
	void*            stack_limit;
	void*            kernel_stack;
	UINT_32          priority;
	UINT_32          state;
	TRAP_FRAME       frame;
} __attribute__ ((packed)) THREAD;

typedef struct _PROCESS {
	UINT_32            pid;
	UINT_32            priority;
	PAGE_DIRECTORY*    page_directory;
	UINT_32            state;
	struct _PROCESS*   next;
	THREAD*            thread_list;
   	LiBOS_CPUs*        cpu;
	LiBOS_IOAPICs*     ioapic;
} __attribute__ ((packed)) PROCESS;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ FUNCTIONS PRORTOTYPE

BOOL            initialize_process     (void);
BOOL            create_process         (UINT_32 function_address);
void            execute_process        (PROCESS* process);
void            terminate_process      (PROCESS* process);
THREAD*         create_thread          (PROCESS* process);

#endif //! PROCESS__H__
