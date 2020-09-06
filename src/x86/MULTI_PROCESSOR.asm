%define REBASE16(ADDR) (ADDR - ap_cpu_trampoline_code + 0x7000)
%define REBASE32(ADDR) (ADDR - mp_32_start + 0x8000)

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern printk
extern gdt_pointer
extern idt_pointer
global get_trampoline_start
global get_trampoline_end
global get_mp_32_start
global get_mp_32_end
global set_cpu_id

extern cpu_1_process_zone
extern cpu_2_process_zone
extern cpu_3_process_zone

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ it will be copied at PHYSICAL_ADDRESS 0x7000 (a.k.a. [ORG 0x7000])

ap_cpu_trampoline_code:
[BITS 16]
	cli
	mov eax, cr0
	or  eax, 0x1
	mov cr0, eax
	a32 lidt [REBASE16(idt16_ptr)]
	a32 lgdt [REBASE16(gdt16_ptr)]
	jmp 0x18:0x8000
gdt16_base:                                ; GDT descriptor table
	.null:                                 ; 0x00 - null segment descriptor
		dd 0x00000000                      ; must be left zero'd
		dd 0x00000000                      ; must be left zero'd
	.code32:                               ; 0x01 - 32bit code segment descriptor 0xFFFFFFFF
		dw 0xFFFF                          ; limit  0:15
		dw 0x0000                          ; base   0:15
		db 0x00                            ; base  16:23
		db 0x9A                            ; present, iopl/0, code, execute/read
		db 0xCF                            ; 4Kbyte granularity, 32bit selector; limit 16:19
		db 0x00                            ; base  24:31 
	.data32:                               ; 0x02 - 32bit data segment descriptor 0xFFFFFFFF
		dw 0xFFFF                          ; limit  0:15
		dw 0x0000                          ; base   0:15
		db 0x00                            ; base  16:23
		db 0x92                            ; present, iopl/0, data, read/write
		db 0xCF                            ; 4Kbyte granularity, 32bit selector; limit 16:19
		db 0x00                            ; base  24:31          
	.code16:                               ; 0x03 - 16bit code segment descriptor 0x000FFFFF
		dw 0xFFFF                          ; limit  0:15
		dw 0x0000                          ; base   0:15
		db 0x00                            ; base  16:23
		db 0x9A                            ; present, iopl/0, code, execute/read
		db 0x0F                            ; 1Byte granularity, 16bit selector; limit 16:19
		db 0x00                            ; base  24:31    
	.data16:                               ; 0x04 - 16bit data segment descriptor 0x000FFFFF
		dw 0xFFFF                          ; limit  0:15
		dw 0x0000                          ; base   0:15
		db 0x00                            ; base  16:23
		db 0x92                            ; present, iopl/0, data, read/write
		db 0x0F                            ; 1Byte granularity, 16bit selector; limit 16:19
		db 0x00                            ; base  24:31  
gdt16_ptr:                                 ; GDT table pointer for 16bit access
	dw gdt16_ptr - gdt16_base - 1          ; table limit (size)
	dd gdt16_base                          ; table base address
idt16_ptr:                                 ; IDT table pointer for 16bit access
	dw 0x03FF                              ; table limit (size)
	dd 0x00000000                          ; table base address
trampoline_end:
;;---------------------------------------------------------------------------------------- it will be copied at PHYSICAL_ADDRESS 0x8000
mp_32_start:
	jmp 0x08:REBASE32(.LiBOS_GDT_IDT_LEVEL1)
[BITS 32]
.LiBOS_GDT_IDT_LEVEL1:
	cli
	mov     esp, @mp_stack ;most important thing before loading any IDT or GDT pointer
	mov     ax, 0x10
	mov     ds, ax
	mov     es, ax
	mov     fs, ax
	mov     gs, ax
	mov     ss, ax
	lgdt    [gdt_pointer] ; every time using LGDT, will set DS and CS registers set to 0
	                      ; don't forget to set them correctly later on
	mov     ax, 0x10
	mov     ds, ax
	mov     es, ax
	mov     fs, ax
	mov     gs, ax
	mov     ss, ax
	jmp     0x08:REBASE32(.LiBOS_GDT_IDT_LEVEL2)
.LiBOS_GDT_IDT_LEVEL2:
	lidt    [idt_pointer]
	push    message        
	mov     eax, printk
	call    eax           ; 'call printk' is relative to this assembly code (don't want it since this code will have been reloacated)
	                      ; instead I want to call the exact physical address of printk label (therefore use it in a Reg/Mem).
	
	xor     eax, eax
	mov     al, BYTE[_idd_]
	cmp     eax, 1
	jne     cpu_2_zone
cpu_1_zone:
	mov     eax, cpu_1_process_zone
	;push    esp
	call    eax
	;pop     esp
	jmp     @HLT
cpu_2_zone:
	cmp     eax, 2
	jne     cpu_3_zone
	mov     eax, cpu_2_process_zone
	;push    esp
	call    eax
	;pop     esp
	jmp     @HLT
cpu_3_zone:
	cmp     eax, 3
	jne     @HLT
	mov     eax, cpu_3_process_zone
	;push    esp
	call    eax
	;pop     esp
@HLT:
	hlt                   ; AP CPU stays here for ever until gets intterupted or commanded to jump or call by threads
mp_32_end:

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void get_trampoline_start(UINT_32*);

get_trampoline_start:
	push ebp 
	mov  ebp, esp
	mov  eax, DWORD [ebp + 8]
	mov  DWORD[eax], ap_cpu_trampoline_code
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void get_trampoline_end(UINT_32*);

get_trampoline_end:
	push ebp 
	mov  ebp, esp
	mov  eax, DWORD [ebp + 8]
	mov  DWORD[eax], trampoline_end
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void get_mp_32_start(UINT_32*);

get_mp_32_start:
	push ebp 
	mov  ebp, esp
	mov  eax, DWORD [ebp + 8]
	mov  DWORD[eax], mp_32_start
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void get_mp_32_end(UINT_32*);

get_mp_32_end:
	push ebp 
	mov  ebp, esp
	mov  eax, DWORD [ebp + 8]
	mov  DWORD[eax], mp_32_end
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void set_cpu_id(UINT_32 id);

set_cpu_id:
	push ebp 
	mov  ebp, esp
	mov  eax, DWORD [ebp + 8]
	mov  BYTE[_idd_], al
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .bss
	space resb 4096
@mp_stack:

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .data
	message: db "AP CPU woke up and ready to be used", 0x0A, 0x00
	_idd_:   db 0x00

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
