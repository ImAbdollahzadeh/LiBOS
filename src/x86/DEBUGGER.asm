[BITS 32]

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .data
	LENGTH_OF_ARGS          DD  0
	ENDC                    equ '\0'
	DECIMALSYMB             equ '%'
	HEXADECIMALSYMB         equ '^'
	CONSOLE_OUTPUT_COLOR    equ 0x02

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .text
	extern PARGS_LIST
	extern printk_impl
	global debug
	global printk

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
		
;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void debug(void);

debug:
	push ebp 
	mov  ebp, esp
	int  0x03
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
