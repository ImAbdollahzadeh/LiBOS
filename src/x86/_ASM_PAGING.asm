[BITS 32]

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

global set_pdbr
global get_pdbr
global paging_enable 
global paging_disable

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void set_pdbr(UINT_32 pdbr);

set_pdbr:
    push ebp
    mov  ebp, esp
    mov  eax, DWORD[ebp + 8] ;the physical address of PDBR
	mov  cr3, eax
    mov  esp, ebp
    pop  ebp
    ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ UINT_32 get_pdbr(void);

get_pdbr:
    push ebp
    mov  ebp, esp
    mov  eax, cr3
    mov  esp, ebp
    pop  ebp
    ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void paging_enable(void);

paging_enable:
    push ebp
    mov  ebp, esp
    mov  eax, cr0
	or   eax, 0x80000000 ;set bit 31
	mov  cr0, eax
    mov  esp, ebp
    pop  ebp
    ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void paging_disable(void);

paging_disable:
    push ebp
    mov  ebp, esp
    mov  eax, cr0
	or   eax, 0x7FFFFFFF ;clear bit 31
	mov  cr0, eax
    mov  esp, ebp
    pop  ebp
    ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

