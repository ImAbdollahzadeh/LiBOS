[BITS 32]

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .data
	LENGTH_OF_ARGS          DD  0
	ENDC                    equ '\0'
	DECIMALSYMB             equ '%'
	HEXADECIMALSYMB         equ '^'
	CONSOLE_OUTPUT_COLOR    equ 0x02
	XHCI_SPURIOUS_SEMAPHORE DD  0
	XHCI_POINTER            DD  0

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .text
	extern PARGS_LIST
	extern printk_impl
	global printk
	global IDTLoad
	global GDTLoad
	global set_xhci_pointer
	global get_xhci_pointer
	extern idt_pointer
	extern gdt_pointer
	extern FAULT_HANDLER
	extern IRQ_HANDLER
	global _CLI
	global _STI
	global ISR_DEFAULT
	global ISR0
	global ISR1
	global ISR2
	global ISR3
	global ISR4
	global ISR5
	global ISR6
	global ISR7
	global ISR8
	global ISR9
	global ISR10
	global ISR11
	global ISR12
	global ISR13
	global ISR14
	global ISR15
	global ISR16
	global ISR17
	global ISR18
	global ISR19
	global ISR20
	global ISR21
	global ISR22
	global ISR23
	global ISR24
	global ISR25
	global ISR26
	global ISR27
	global ISR28
	global ISR29
	global ISR30
	global ISR31
	global IRQ0
	global IRQ1
	global IRQ2
	global IRQ3
	global IRQ4
	global IRQ5
	global IRQ6
	global IRQ7
	global IRQ8
	global IRQ9
	global IRQ10
	global IRQ11
	global IRQ12
	global IRQ13
	global IRQ14
	global IRQ15
	global IRQ16
	global IRQ100
	
	global CHECK_DS
	global CHECK_GS
	global CHECK_ES
	global CHECK_FS
	global WM
	global idt_xhci_spurious

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

ISR_DEFAULT:
	iret
	
;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

;0: Divide By Zero Exception
ISR0:
    cli
    push dword 0    ; A normal ISR stub that pops a dummy error code to keep a uniform stack frame
    push dword 0
    jmp isr_common_stub
;1: Debug Exception
ISR1:
    cli
    push dword 0
    push dword 1
    jmp isr_common_stub
ISR2:
    cli
    push dword 0
    push dword 2
    jmp isr_common_stub
ISR3:
    cli
    push dword 0
    push dword 3
    jmp isr_common_stub
ISR4:
    cli
    push dword 0
    push dword 4
    jmp isr_common_stub
ISR5:
    cli
    push dword 0
    push dword 5
    jmp isr_common_stub
ISR6:
    cli
    push dword 0
    push dword 6
    jmp isr_common_stub
ISR7:
    cli
    push dword 0
    push dword 7
    jmp isr_common_stub
; 8: Double Fault Exception (With Error Code!)
ISR8:
    cli
    push dword 8    ;Note that we DON'T push a value on the stack in this one!
                   ;It pushes one already! Use this type of stub for exceptions that pop error codes (http://www.osdever.net/bkerndev/Docs/isrs.htm)
    jmp isr_common_stub
ISR9:
    cli
    push dword 0
    push dword 9
    jmp isr_common_stub
ISR10:
    cli
    push dword 10
    jmp isr_common_stub
ISR11:
    cli
    push dword 11
    jmp isr_common_stub
ISR12:
    cli
    push dword 12
    jmp isr_common_stub
ISR13:
    cli
    push dword 13
    jmp isr_common_stub
ISR14:
    cli
    push dword 14
    jmp isr_common_stub
ISR15:
    cli
    push dword 0
    push dword 15
    jmp isr_common_stub
ISR16:
    cli
    push dword 0
    push dword 16
    jmp isr_common_stub
ISR17:
    cli
    push dword 0
    push dword 17
    jmp isr_common_stub
ISR18:
    cli
    push dword 0
    push dword 18
    jmp isr_common_stub
ISR19:
    cli
    push dword 0
    push dword 19
    jmp isr_common_stub
ISR20:
    cli
    push dword 0
    push dword 20
    jmp isr_common_stub
ISR21:
    cli
    push dword 0
    push dword 21
    jmp isr_common_stub
ISR22:
    cli
    push dword 0
    push dword 22
    jmp isr_common_stub
ISR23:
    cli
    push dword 0
    push dword 23
    jmp isr_common_stub
ISR24:
    cli
    push dword 0
    push dword 24
    jmp isr_common_stub
ISR25:
    cli
    push dword 0
    push dword 25
    jmp isr_common_stub
ISR26:
    cli
    push dword 0
    push dword 26
    jmp isr_common_stub
ISR27:
    cli
    push dword 0
    push dword 27
    jmp isr_common_stub
ISR28:
    cli
    push dword 0
    push dword 28
    jmp isr_common_stub
ISR29:
    cli
    push dword 0
    push dword 29
    jmp isr_common_stub
ISR30:
    cli
    push dword 0
    push dword 30
    jmp isr_common_stub
ISR31:
    cli
    push dword 0
    push dword 31
    jmp isr_common_stub
	
isr_common_stub:
    push  eax
    push  ecx
    push  edx
    push  ebx
    push  esp
    push  ebp
    push  esi
    push  edi
    push  ds
    push  es
    push  fs
    push  gs
    mov   ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    push  esp
    call  FAULT_HANDLER        ; A special call, preserves the 'eip' register
    pop   esp
    pop   gs
    pop   fs
    pop   es
    pop   ds
    pop   edi
    pop   esi
    pop   ebp
    pop   esp
    pop   ebx
    pop   edx
    pop   ecx
    pop   eax
    add   esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret             ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

IRQ0:
    cli
    push dword 0    ;Note that these don't push an error code on the stack: We need to push a dummy error code
    push dword 32
    jmp irq_common_stub
IRQ1:
    cli
    push dword 0   
    push dword 33
    jmp irq_common_stub
IRQ2:
    cli
    push dword 0
    push dword 34
    jmp irq_common_stub
IRQ3:
    cli
    push dword 0    
    push dword 35
    jmp irq_common_stub
IRQ4:
    cli
    push dword 0 
    push dword 36
    jmp irq_common_stub
IRQ5:
    cli
    push dword 0
    push dword 37
    jmp irq_common_stub
IRQ6:
    cli
    push dword 0 
    push dword 38
    jmp irq_common_stub
IRQ7:
    cli
    push dword 0   
    push dword 39
    jmp irq_common_stub
IRQ8:
    cli
    push dword 0   
    push dword 40
    jmp irq_common_stub
IRQ9:
    cli
    push dword 0    
    push dword 41
    jmp irq_common_stub
IRQ10:
    cli
    push dword 0    
    push dword 42
    jmp irq_common_stub
IRQ11:
    cli
    push dword 0   
    push dword 43
    jmp irq_common_stub
IRQ12:
    cli
    push dword 0
    push dword 44
    jmp irq_common_stub
IRQ13:
    cli
    push dword 0   
    push dword 45
    jmp irq_common_stub
IRQ14:
    cli
    push dword 0   
    push dword 46
    jmp irq_common_stub
IRQ15:
    cli
    push dword 0   
    push dword 47
    jmp irq_common_stub
IRQ16:
    cli
    push dword 0   
    push dword 48
    jmp irq_common_stub

IRQ100:
    cli
    push dword 0   
    push dword 132
    jmp irq_common_stub

irq_common_stub:
    push  eax
    push  ecx
    push  edx
    push  ebx
    push  esp
    push  ebp
    push  esi
    push  edi
    push  ds
    push  es
    push  fs
    push  gs
    mov   ax, 0x10
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    push  esp
    call  IRQ_HANDLER
    pop   esp
    pop   gs
    pop   fs
    pop   es
    pop   ds
    pop   edi
    pop   esi
    pop   ebp
    pop   esp
    pop   ebx
    pop   edx
    pop   ecx
    pop   eax
    add   esp, 8
    iret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

IDTLoad:
	push ebp
    mov  ebp, esp
    lidt [idt_pointer]
	mov  eax, dword [ebp + 4]
	mov  esp, ebp
    pop  ebp
    ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

GDTLoad:
	push ebp
    mov  ebp, esp
    lgdt [gdt_pointer]
	mov  ax, 0x10
	mov  ds, ax
	mov  es, ax
	mov  fs, ax
	mov  gs, ax
	mov  ss, ax
	jmp  0x08:@end
@end:
    mov  eax, dword [ebp + 4]
	mov  esp, ebp
    pop  ebp
    ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

_CLI:
	cli
	ret
	
_STI:
	sti
	ret
	
;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

printk: 
    push ebp
    mov  ebp, esp                  
	mov  eax, [ebp + 8] 
	mov  edi, 0
	mov  ecx, 0
@BEGINL:
	mov  bl, BYTE [eax + ecx]
	cmp  bl, BYTE [ENDC]
	je   @ENDL
	cmp  bl, DECIMALSYMB
	jne  @CHECKSECONDL
@ADDERL:
	inc  edi
	inc  ecx
	jmp  @BEGINL
@CHECKSECONDL:
	cmp  bl, HEXADECIMALSYMB
	je  @ADDERL
@IGNOREL:
	inc  ecx
	jmp  @BEGINL
@ENDL:
	inc  edi
	mov  DWORD [LENGTH_OF_ARGS], edi
	mov  eax, DWORD [PARGS_LIST] 
	add  eax, 4
	mov  edx, DWORD [LENGTH_OF_ARGS]
	cmp  edx, 1
	je   @WITHOUTNUMERICARGSL
	mov  ecx, 1
@FILLARGSLISTENTRIESL:         
	mov  ebx, [ebp + (4*ecx + 8)]
	mov  [eax], ebx
	add  eax, 4
	inc  ecx
	cmp  ecx, edx
	jne  @FILLARGSLISTENTRIESL
@WITHOUTNUMERICARGSL:
	mov  eax, [ebp + 8]
	mov  edx, CONSOLE_OUTPUT_COLOR
	push edx
	push eax
	call printk_impl
	pop  eax
	pop  edx
	mov  DWORD [LENGTH_OF_ARGS], 0
    mov  esp, ebp
    pop  ebp
    ret
		
;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

CHECK_DS:
	push ebp
    	mov  ebp, esp
	mov  cx,  0x10
	mov  eax, [ebp + 8]
	mov  bx,  ds
	cmp  bx,  cx
	jne  CHECK_DS_FAILURE
CHECK_DS_RIGHT:
	mov DWORD [eax], 0x00000010
	jmp END_OF_CHECK_DS
CHECK_DS_FAILURE:
	mov DWORD [eax], 0x00
END_OF_CHECK_DS:
	mov  esp, ebp
    	pop  ebp
    	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

CHECK_GS:
	push ebp
    	mov  ebp, esp
	mov  cx,  0x10
	mov  eax, [ebp + 8]
	mov  bx,  gs
	cmp  bx,  cx
	jne  CHECK_GS_FAILURE
CHECK_GS_RIGHT:
	mov DWORD [eax], 0x00000010
	jmp END_OF_CHECK_GS
CHECK_GS_FAILURE:
	mov DWORD [eax], 0x00
END_OF_CHECK_GS:
	mov  esp, ebp
    	pop  ebp
    	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

CHECK_FS:
	push ebp
    	mov  ebp, esp
	mov  cx,  0x10
	mov  eax, [ebp + 8]
	mov  bx,  fs
	cmp  bx,  cx
	jne  CHECK_FS_FAILURE
CHECK_FS_RIGHT:
	mov DWORD [eax], 0x00000010
	jmp END_OF_CHECK_FS
CHECK_FS_FAILURE:
	mov DWORD [eax], 0x00
END_OF_CHECK_FS:
	mov  esp, ebp
    	pop  ebp
    	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

CHECK_ES:
	push ebp
    	mov  ebp, esp
	mov  cx,  0x10
	mov  eax, [ebp + 8]
	mov  bx,  es
	cmp  bx,  cx
	jne  CHECK_ES_FAILURE
CHECK_ES_RIGHT:
	mov DWORD [eax], 0x00000010
	jmp END_OF_CHECK_ES
CHECK_ES_FAILURE:
	mov DWORD [eax], 0x00
END_OF_CHECK_ES:
	mov  esp, ebp
    	pop  ebp
    	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

WM:
	WBINVD

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

;; idt_xhci_spurious(unsigned int execution_flag, unsignet int* ret_value);
	;; execution_flag = 0 -> clear the semaphore
	;; execution_flag = 1 -> set the semaphore
	;; execution_flag = 2 -> report the semaphore

idt_xhci_spurious:
	push ebp
    	mov  ebp, esp
	
	mov eax, [ebp + 8]
	cmp eax, 0
	jne NEXT1
	mov DWORD[XHCI_SPURIOUS_SEMAPHORE], 0x00000000 
	mov ebx, [ebp + 12]
	mov DWORD [ebx], 0x00000000 
	jmp FINISH_IDT_XHCI
NEXT1:
	cmp eax, 1
	jne NEXT2
	mov DWORD[XHCI_SPURIOUS_SEMAPHORE], 0x0000001
	mov ebx, [ebp + 12]
	mov DWORD [ebx], 0x00000001 
	jmp FINISH_IDT_XHCI
NEXT2:
	cmp eax, 2
	jne FINISH_IDT_XHCI
	mov eax, DWORD[XHCI_SPURIOUS_SEMAPHORE]
	mov ebx, [ebp + 12]
	mov DWORD [ebx], eax 
	jmp FINISH_IDT_XHCI

FINISH_IDT_XHCI:
	mov  esp, ebp
    	pop  ebp
    	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
set_xhci_pointer:
	push ebp
    	mov  ebp, esp
	mov  eax, [ebp + 8]
	mov DWORD[XHCI_POINTER], eax
	mov  esp, ebp
    	pop  ebp
    	ret

get_xhci_pointer:
	push ebp
    	mov  ebp, esp
	mov  eax, [ebp + 8]
	mov  ebx, DWORD[XHCI_POINTER]
	mov  [eax], ebx
	mov  esp, ebp
    	pop  ebp
    	ret
