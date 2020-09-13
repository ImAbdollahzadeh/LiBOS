[BITS 32]

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

global execute_kernel_mode_process
global execute_user_mode_process
global restore_kernel_after_process_termination
extern printk

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void execute_kernel_mode_process(UINT_32 process_stack, UINT_32 entry_point);

execute_kernel_mode_process:
	cli
	mov     esi, DWORD[esp + 0x4]
	mov     edi, DWORD[esp + 0x8]
	mov     DWORD[kernel_esp], esp
	mov     ebx, DWORD[esp]
	mov     DWORD[kernel_eip], ebx
	xor     eax, eax
	mov     ax, 0x10
	mov     ds, ax
	mov     es, ax
	mov     fs, ax
	mov     gs, ax
;create stack frame
	;push    0x10			         ; SS, notice it uses same selector as above
	;push    edi                      ; stack
	pushf           		         ; EFLAGS
	push    0x08			         ; CS, kernel mode code selector is 0x08.
	push    edi                      ; EIP
	iretd                            ; by IRETD, cpu pops the return instruction pointer, return code segment selector, and EFLAGS from the stack to the EIP, CS, and EFLAGS registers, 
	                                 ; and then resumes execution of the interrupted program or procedure. If the return is to another privilege level, the IRETD also pops ESP and SS 
									 ; from the stack, before resuming program execution.

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void execute_user_mode_process(UINT_32 process_stack, UINT_32 entry_point);

execute_user_mode_process:
	cli
	mov     esi, DWORD[esp + 0x8]
	mov     edi, DWORD[esp + 0xC]
	mov     DWORD[kernel_esp], esp
	mov     ax, 0x23                 ; user mode data selector is 0x20 (GDT entry 3). Also sets RPL to 3
	mov     ds, ax
	mov     es, ax
	mov     fs, ax
	mov     gs, ax
	push    0x23			            
	push    DWORD[esi]		
	push    0x200		
	push    0x1B			         ; CS, kernel mode code selector is 0x18. Also sets RPL to 3
	push    DWORD[edi]
	iretd 

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void restore_kernel_after_process_termination(void);

restore_kernel_after_process_termination:
	cli
	xor     eax, eax
	mov     ax,  0x10
	mov     ds,  ax
	mov     es,  ax
	mov     fs,  ax
	mov     gs,  ax
	mov     esp, DWORD[kernel_esp]
	jmp     0x08:actual_kernel_eip
actual_kernel_eip:
	jmp     DWORD[kernel_eip]
	sti

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .data
	kernel_esp: dd 0x00000000
	kernel_eip: dd 0x00000000
	string: db "ESP:^", 0xa, 0

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
