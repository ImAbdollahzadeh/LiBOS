
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
global set_libos_main_page_directory

extern ap_cpu_1_main
extern ap_cpu_2_main
extern ap_cpu_3_main

extern enable_ap_lapic

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
	jmp 0x08:REBASE32(.LiBOS_GDT_IDT_PAGING_TSS_set)
[BITS 32]
.LiBOS_GDT_IDT_PAGING_TSS_set:
	cli
; most important thing before loading any IDT or GDT pointer ~> stack
; assign separate stacks to each AP CPU
	mov     esp, @startup_stack
	mov     ax, 0x10
	mov     ds, ax
	mov     es, ax
	mov     fs, ax
	mov     gs, ax
	mov     ss, ax
	lgdt    [gdt_pointer]
	lidt    [idt_pointer]
; activate paging on this AP CPU
	mov     eax, DWORD[libos_mpd]
	mov     cr3, eax
	mov     eax, cr0
	or      eax, 0x80000000 ; paging is active
	mov     cr0, eax
; activate the lapic for this AP cpu
	mov     edi, enable_ap_lapic
	call    edi
; create each AP CPU, its own stack
	xor     eax, eax
	mov     al, BYTE[_idd_]
	mov     ebx, @mp_stack
	mov     edx, 3
	sub     edx, eax
	imul    edx, 4096
	sub     ebx, edx
	mov     esp, ebx
; print out the welcome message (it takes a long time to finish. Therefore set an appropriate timer sleep in SIPI)
	push    message        
	mov     eax, printk
	call    eax           ; 'call printk' is relative to this assembly code (don't want it since this code will have been reloacated)
	                      ; instead I want to call the exact physical address of printk label (therefore use it in a Reg/Mem).
	add     esp, 4
	xor     eax, eax
	mov     al, BYTE[_idd_]
	cmp     eax, 1
	jne     __2
__1:
	cli
	mov     ax, 0x43
	ltr     ax
	mov     edx, ap_cpu_1_main
	jmp     edx
__2:
	cmp     eax, 2
	jne     __3
	cli
	mov     ax, 0x4B
	ltr     ax
	mov     edx, ap_cpu_2_main
	jmp     edx
__3:
	cli
	mov     ax, 0x53
	ltr     ax
	mov     edx, ap_cpu_3_main
	jmp     edx
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

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void set_libos_main_page_directory (UINT_32);

set_libos_main_page_directory:
	push ebp
	mov  ebp, esp
	mov  eax, DWORD[ebp + 8]
	mov  DWORD[libos_mpd], eax
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .bss

; startup stack
	resb 4096
@startup_stack:

; our main AP CPUs stacks (each 4KB)
	resb 3 * 4096
@mp_stack:

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .data
	message:   db "AP CPU woke up and ready to be used", 0x0A, 0x00
	_idd_:     db 0x00
	libos_mpd: dd 0x00000000
	
;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

