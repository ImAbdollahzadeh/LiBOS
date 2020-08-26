[BITS 32]

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

segment .data
	PADDING_0:               dd  0
	PADDING_1:               dd  0
	PADDING_2:               dd  0
	mxcsr_loading_mem:       dd  0
	mxcsr_mask:              dd  0
	PADDING_3:               dd  0
	fcw:                     dw 0x037F
	PADDING_4:               dw  0
	fxsave_region: times 512 db 0

align 16
	coeffi : dd 2.0, 3.0, 4.0, 5.0

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .text
	global _activate_sse
	global _check_avx

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

_activate_sse:
	fninit
    mov eax, cr0
    and al, ~0x04
    or al, 0x22
    mov cr0, eax
    mov eax, cr4
    or ax, 0x600
    mov cr4, eax
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

_check_avx:
	push   ebp 
	mov    ebp,     esp  
	mov    eax,     1
	xor    ecx,     ecx
	cpuid
	mov    eax,     [ebp+8]
	mov    [eax],   ecx
	mov    esp,     ebp
	pop    ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
