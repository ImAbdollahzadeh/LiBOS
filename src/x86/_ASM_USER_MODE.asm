[BITS 32]

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

global enter_user_mode
extern set_TSS_esp
global asm_user_int_0x80
global asm_user_iretd

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void asm_user_int_0x80(UINT_32 eax, UINT_32 ebx, UINT_32 ecx);

asm_user_int_0x80:
	mov eax, DWORD[esp + 4]
	mov ebx, DWORD[esp + 8]
	mov ecx, DWORD[esp + 12]
	int 0x80
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void asm_user_iretd(void);

asm_user_iretd:
	iret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void enter_user_mode(UINT_32 usermode_entry_point);

enter_user_mode:
	cli
	mov  esi, DWORD[esp + 4]
	push esp
	call set_TSS_esp         ;; CRITICAL--> here we give esp address as the current kernel stack to our TSS
	add  esp, 4
	mov  ax, 0x33
	mov  ds, ax
	mov  es, ax
	mov  fs, ax
	mov  gs, ax
	push 0x33
	mov  eax, @user_kernel  ;; CRITICAL--> if I would give again the same esp address here, it works out but later with the first interrupt handling, 
	                        ;; CRITICAL--> CPU looks for the kernel esp and gets it from TSS.esp0 which was given above. With many pushes and pops during handling, 
	                        ;; CRITICAL--> the user mode esp will be destroyed and after IRET in IRQ handler, you lost your EIP
	push eax                 
	pushf
;; modify IF flag in EFLAGS to enable interrupts 
	pop  eax
	or   eax, 0x200
	push eax
	push 0x2B               ;; cs ~> GDT descriptor 5 + RPL 3
	push esi                ;; eip
	iret
;; we are in user mode

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 

section .bss
	@user_kernel: resb 512

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
