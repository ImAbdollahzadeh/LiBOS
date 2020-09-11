#ifndef PROCESS__H__
#define PROCESS__H__

#include "LiBOS_CORE.h"
#include "PAGING.h"
#include "MP.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ CONSTANTS

#define BIT(X) (1 << X)

#define PROCESS_STATE_SLEEP  0
#define PROCESS_STATE_ACTIVE 1
#define PROC_INVALID_ID     -1

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ ENUMS

typedef enum _TRAP_FRAME_FLAG_REGISTER {
	CARRY_FLAG                         = BIT(0),
	RESVD1_FLAG                        = BIT(1),
	PARITY_FLAG                        = BIT(2),
	RESVD2_FLAG                        = BIT(3),
	ADJUST_FLAG                        = BIT(4),
	RESVD3_FLAG                        = BIT(5),
	ZERO_FLAG                          = BIT(6),
	SIGN_FLAG                          = BIT(7),
	TRAP_FLAG                          = BIT(8),
	INTERRUPT_ENABLE_FLAG              = BIT(9),
	DIRECTION_FLAG                     = BIT(10),
	OVERFLOW_FLAG                      = BIT(11),
	IO_PRIVILAGE_LEVEL1_FLAG           = BIT(12),
	IO_PRIVILAGE_LEVEL2_FLAG           = BIT(13),
	NESTED_TASK_FLAG                   = BIT(14),
	RESVD4_FLAG                        = BIT(15),
	RESUME_FLAG                        = BIT(16),
	VIRTUAL_8086_MODE_FLAG             = BIT(17),
	ALIGNMENT_CHECK_FLAG               = BIT(18),
	VIRTUAL_INTERRUPT_FLAG             = BIT(19),
	VIRTUAL_INTERRUPT_PENDING_FLAG     = BIT(20),
	ABLE_TO_USE_CPUID_INSTRUCTION_FLAG = BIT(21),
	/* bits 22...31 all are reserved */
} TRAP_FRAME_FLAG_REGISTER;

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

extern void     execute_kernel_mode_process              (UINT_32 process_stack, UINT_32 entry_point);
extern void     execute_user_mode_process                (UINT_32 process_stack, UINT_32 entry_point);
extern void     restore_kernel_after_process_termination (void);
       BOOL     initialize_process                       (void);
       PROCESS* create_process                           (UINT_32 function_address);
       void     execute_process                          (PROCESS* process);
       void     terminate_process                        (PROCESS* process);
       THREAD*  create_thread                            (PROCESS* process);

#endif //! PROCESS__H__
