;-----------------------------------------------------------------------------------------------------------------------------
;;  WE NEED SOME PLACES TO STORE IDT-32 AND GDT-32 AND IVT, AND SOME PLACE FOR A VALID 16-BIT STACK. THE LAST
;;  ONE IS VERY IMPORTANT SINCE WE ALWAYS NEED A VALID STACK. BETTER WOULD BE TO RESERVE SOMEWHERE IN UNUSED AREAS < 1 MiB.
;;  IN CODE BELOW, WE SIMPLY SAY STACK WOULD BE IN ADDRESS 0x2000:
;;  AND PLACE FOR OUR CODE AND DATA SEGMENTS, WE SAY TO BE AT ADDRESS (0x9000 * 16)
;-----------------------------------------------------------------------------------------------------------------------------

SECTION .text
    global  prot2real
    global  _prot2real
    global  real2prot
    global  _real2prot
    global vesa_get_info
    global _vesa_get_info

; OS Loader base address
OSLDRSEG    equ 0x9000
OSLDRBASE   equ (OSLDRSEG * 16)

; Segment selectors for data and code in real and protected mode
PROT_CSEG   equ 0x08
PROT_DSEG   equ 0x10
REAL_CSEG   equ 0x18
REAL_DSEG   equ 0x20

; Location of real mode stack
REAL_STACK  equ (0x2000 - 0x10)

; Convert linear address to osldr segment-relative address
%define LOCAL(addr) (addr - OSLDRBASE)

;+++++++++++++++++++++++ Switch from protected to real mode ++++++++++++++++++++++++
prot2real:
_prot2real:
        BITS    32
        ; No interrupts while switching mode
		cli
        ; Save protected mode stack and base pointers
        mov     eax, esp
        mov     [prot_esp], eax
        mov     eax, ebp
        mov     [prot_ebp], eax
        ; Save interrupt descriptors
        sidt    [prot_idt]
        sgdt    [prot_gdt]
        ; Store return address in real mode stack (since 5 lines below we will destroy esp)
        mov     eax,  [esp]
        mov     [REAL_STACK], eax
        ; Set up real mode stack
        mov     eax, REAL_STACK
        mov     esp, eax
        mov     ebp, eax
        ; Setup segment registers
        mov     ax, REAL_DSEG
        mov     ds, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax
        mov     ss, ax
        ; Transition to 16 bit segment
        jmp     REAL_CSEG:(LOCAL(start16))
start16:
        BITS    16
        ; Exit protected mode by clearing the PE bit of CR0
        mov     eax, cr0
        and     eax, ~1
        mov     cr0, eax
        ; Reload code segment register and clear prefetch
        jmp     dword OSLDRSEG:(LOCAL(realmode))
realmode:
        ; Setup real mode segment registers
        mov     ax, OSLDRSEG
        mov     ds, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax
        xor     ax, ax
        mov     ss, ax      
        ; Load real mode IDT
        a32 lidt [LOCAL(real_idt)]
        ; Return on the real mode stack
        sti
        ret
;+++++++++++++++++++++++ Switch from real to protected mode ++++++++++++++++++++++++
real2prot:
_real2prot:
        BITS    16
        ; Disable interrupts
        cli
        ; Restore protected mode descriptors
        mov     ax, OSLDRSEG
        mov     ds, ax
        a32 lidt    [LOCAL(prot_idt)]
        a32 lgdt    [LOCAL(prot_gdt)]
        ; Enable protected mode
        mov     eax, cr0
        or      eax, 1
        mov     cr0, eax
        ; Set code segment and clear prefetch
        jmp     dword PROT_CSEG:protmode
protmode:
        BITS    32
        ; Setup rest of segment registers for protected mode
        mov     ax, PROT_DSEG
        mov     ds, ax
        mov     es, ax
        mov     ss, ax
        mov     fs, ax
        mov     gs, ax
        ; Store return address in real mode stack (since 5 lines below we will destroy esp)
        mov     eax, [esp]
        mov     [REAL_STACK], eax
        ; Restore protected mode stack
        mov     eax, [prot_esp]
        mov     esp, eax
        mov     eax, [prot_ebp]
        mov     ebp, eax
        ; Put return address onto protected mode stack
        mov     eax, [REAL_STACK]
        mov     [esp], eax
        ; Return on protected mode stack
        xor     eax, eax
        ret
;++++++++++++++++++++++ int vesa_get_info(VESA_INFO* info) +++++++++++++++++++++++++++
vesa_get_info:
_vesa_get_info:
        push ebp
        mov  ebp, esp
        ; Save register
        push    ebx
        push    esi
        push    edi
        ; Get info buffer address
        mov     edi, [ebp + 8]
        sub     edi, OSLDRBASE
        ; Enter real mode
        call    _prot2real
        BITS     16
        ; Call VESA
        mov     ax, OSLDRSEG
        mov     es, ax
        mov     ax, 0x4f00
        int     0x10
        mov     bx, ax
        ; Return to protected mode
        call    _real2prot
        BITS    32
        ; Return status
        xor     eax, eax
        mov     ax, bx
        ; Restore registers
        pop     edi
        pop     esi
        pop     ebx
        ; Return
        pop    ebp
        ret
;+++++++ Storage for protected model stack pointer, and descriptor tables ++++++++++++
prot_esp:   dd 0
prot_ebp:   dd 0
prot_idt:   dw 0, 0, 0
prot_gdt:   dw 0, 0, 0
; Real mode IDT for BIOS
real_idt:   dw 0x3ff   ; 256 entries, 4b each = 1K
            dd 0       ; Real mode IVT @ 0x0000
