[BITS 32]

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .text
	global set_MSR
	global get_MSR

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

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void get_MSR(UINT_32 base_addr, UINT_32* addr_lo, UINT_32* addr_hi);

get_MSR:
	push ebp 
	mov ebp, esp
	mov ecx, DWORD [ebp+0x08]
	rdmsr
	mov ebx, DWORD [ebp+0x0C]
	mov DWORD [ebx], eax
	mov ebx, DWORD [ebp+0x10]
	mov DWORD [ebx], edx
	mov esp, ebp
	pop ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

