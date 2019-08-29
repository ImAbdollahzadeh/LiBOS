[BITS 32]

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .data
	LENGTH_OF_ARGS       DD  0
	ENDC                 equ '\0'
	DECIMALSYMB          equ '%'
	HEXADECIMALSYMB      equ '^'
	CONSOLE_OUTPUT_COLOR equ 0x02
	XHCI_PENDING_STATUS  DB  0
	
;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .text
	extern PARGS_LIST
	extern printk_impl
	extern idt_pointer
	extern gdt_pointer
	extern FAULT_HANDLER
	extern IRQ_HANDLER
	extern CRITICAL_XHCI_HANDLER

	global xhci_set_pending_status
	global xhci_clear_pending_status
	
	global printk
	global IDTLoad
	global Gdt_load
	global CHECK_DS
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

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

ISR_DEFAULT:
	iret
	
;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

;0: Divide By Zero Exception
ISR0:
    cli
    push byte 0    ; A normal ISR stub that pops a dummy error code to keep a uniform stack frame
    push byte 0
    jmp isr_common_stub
;1: Debug Exception
ISR1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub
ISR2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub
ISR3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub
ISR4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub
ISR5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub
ISR6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub
ISR7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub
; 8: Double Fault Exception (With Error Code!)
ISR8:
    cli
    push byte 8    ;Note that we DON'T push a value on the stack in this one, It pushes one already!
    jmp isr_common_stub
ISR9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub
ISR10:
    cli
    push byte 10
    jmp isr_common_stub
ISR11:
    cli
    push byte 11
    jmp isr_common_stub
ISR12:
    cli
    push byte 12
    jmp isr_common_stub
ISR13:
    cli
    push byte 13
    jmp isr_common_stub
ISR14:
    cli
    push byte 14
    jmp isr_common_stub
ISR15:
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub
ISR16:
    cli
    push byte 0
    push byte 16
    jmp isr_common_stub
ISR17:
    cli
    push byte 0
    push byte 17
    jmp isr_common_stub
ISR18:
    cli
    push byte 0
    push byte 18
    jmp isr_common_stub
ISR19:
    cli
    push byte 0
    push byte 19
    jmp isr_common_stub
ISR20:
    cli
    push byte 0
    push byte 20
    jmp isr_common_stub
ISR21:
    cli
    push byte 0
    push byte 21
    jmp isr_common_stub
ISR22:
    cli
    push byte 0
    push byte 22
    jmp isr_common_stub
ISR23:
    cli
    push byte 0
    push byte 23
    jmp isr_common_stub
ISR24:
    cli
    push byte 0
    push byte 24
    jmp isr_common_stub
ISR25:
    cli
    push byte 0
    push byte 25
    jmp isr_common_stub
ISR26:
    cli
    push byte 0
    push byte 26
    jmp isr_common_stub
ISR27:
    cli
    push byte 0
    push byte 27
    jmp isr_common_stub
ISR28:
    cli
    push byte 0
    push byte 28
    jmp isr_common_stub
ISR29:
    cli
    push byte 0
    push byte 29
    jmp isr_common_stub
ISR30:
    cli
    push byte 0
    push byte 30
    jmp isr_common_stub
ISR31:
    cli
    push byte 0
    push byte 31
    jmp isr_common_stub
	
isr_common_stub:
    pusha
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
    call  FAULT_HANDLER 
    pop   esp
    pop   gs
    pop   fs
    pop   es
    pop   ds
    popa
    add   esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret             ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

IRQ0:
    cli
    push byte 0  
    push byte 32
    jmp irq_common_stub
IRQ1:
    cli
    push byte 0   
    push byte 33
    jmp irq_common_stub
IRQ2:
    cli
    push byte 0
    push byte 34
    jmp irq_common_stub
IRQ3:
    cli
    push byte 0    
    push byte 35
    jmp irq_common_stub
IRQ4:
    cli
    push byte 0 
    push byte 36
    jmp irq_common_stub
IRQ5:
    cli
    push byte 0
    push byte 37
    jmp irq_common_stub
IRQ6:
    cli
    push byte 0 
    push byte 38
    jmp irq_common_stub
IRQ7:
    cli
    push byte 0   
    push byte 39
    jmp irq_common_stub
IRQ8:
    cli
    push byte 0   
    push byte 40
    jmp irq_common_stub
IRQ9:
    cli
    push byte 0    
    push byte 41
    jmp irq_common_stub
IRQ10:
    cli
    push byte 0    
    push byte 42
    jmp irq_common_stub
IRQ11:
    cli
    push byte 0   
    push byte 43
    jmp irq_common_stub
IRQ12:
    cli
    push byte 0
    push byte 44
    jmp irq_common_stub
IRQ13:
    cli
    push byte 0   
    push byte 45
    jmp irq_common_stub
IRQ14:
    cli
    push byte 0   
    push byte 46
    jmp irq_common_stub
IRQ15:
    cli
    push byte 0   
    push byte 47
    jmp irq_common_stub

irq_common_stub:
    pusha
    push  ds
    push  es
    push  fs
    push  gs
    mov   ax, 0x10
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   eax, esp
    push  esp
    call  IRQ_HANDLER
	;mov cl, BYTE [XHCI_PENDING_STATUS]
	;cmp cl, 0
	;jne @@XHCI_PENDING_CRITICAL_HANDLER
	;@irq_common_stub_ending:
    pop   esp
    pop   gs
    pop   fs
    pop   es
    pop   ds
    popa
    add   esp, 8
    iret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

;@@XHCI_PENDING_CRITICAL_HANDLER:
;	push ebp
;    mov  ebp, esp
;    call  CRITICAL_XHCI_HANDLER
;	mov  esp, ebp
;    pop  ebp
;	jmp @irq_common_stub_ending
	
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

Gdt_load:
	push ebp
    mov  ebp, esp
    lgdt [gdt_pointer]
	mov  ax, 0x10
	mov  ds, ax
	mov  es, ax
	mov  fs, ax
	mov  gs, ax
	mov  ss, ax
	jmp  0x08:@GDTLoad_end
@GDTLoad_end:
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
	jne  @CHECK_DS_FAILURE
@CHECK_DS_RIGHT:
	mov DWORD [eax], 0x00000010
	jmp @END_OF_CHECK_DS
@CHECK_DS_FAILURE:
	mov DWORD [eax], 0x00
@END_OF_CHECK_DS:
	mov  esp, ebp
    pop  ebp
    ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

xhci_set_pending_status:
	push ebp
    mov  ebp, esp
	mov  cl,  1
	mov  BYTE [XHCI_PENDING_STATUS], cl
	mov  esp, ebp
    pop  ebp
    ret
	
;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

xhci_clear_pending_status:
	push ebp
    mov  ebp, esp
	mov  cl,  0
	mov  BYTE [XHCI_PENDING_STATUS], cl
	mov  esp, ebp
    pop  ebp
    ret
	
;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
