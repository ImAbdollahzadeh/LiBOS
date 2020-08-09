[BITS 32]

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .text
	global disable_PIC
	global set_MSR

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ disable_PIC(void);

disable_PIC:
	push ebp 
	mov  ebp, esp
	mov  al, 0xff
	out  0xA1, al
	out  0x21, al
; Enable symmetric I/O Mode as per MPS
    mov al, 0x70        ; select the IMCR register
    out 0x22, al
    mov al, 0x01        ; instruct NMIs and 8259 INTs to go through APIC
    out 0x23, al
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void set_MSR(UINT_32 base_addr, UINT_32 phys_addr_lo, UINT_32 phys_addr_hi);

set_MSR:
	push ebp 
	mov ebp, esp
	mov ecx, DWORD [ebp+0x08]
	mov eax, DWORD [ebp+0x0C]
	mov edx, DWORD [ebp+0x10]
	wrmsr
	mov esp, ebp
	pop ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
