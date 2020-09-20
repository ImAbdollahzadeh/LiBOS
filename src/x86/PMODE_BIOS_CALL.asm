;--------------------------------------------------------------------------------
;             |   SPACE FOR PUSHA INSTRUCTION   |   RET    |  INT_No  |   REG*   |
;--------------------------------------------------------------------------------
;                32 bytes                         4 bytes    4 bytes    4 bytes


[BITS 32]

%define REBASE(ADDR)             (((ADDR) - @actual_code_at_0x9000) + 0x9000)
%define STACK16                  (0x9000  - 26) ; 26 is sizeof(REGS_16)
%define REBASE_with_paging(ADDR) (((ADDR) - @actual_code_at_0x9000_with_paging) + 0x9000)
%define STACK16_with_paging      (0x9000  - 26)

;; -+-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+

global __LiBOS_BiosCall
global __LiBOS_BiosCall_without_paging
global __LiBOS_BiosCall_with_paging
extern bios_call_tag

;; -+-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+ void __LiBOS_BiosCall(UINT_32 int_no, REGS* r);

section .text
[BITS 32]
__LiBOS_BiosCall:
	mov  ebx, DWORD[bios_call_tag]
	cmp  ebx, 0
	je   invalid_tag
	cmp  ebx, 1
	je   __LiBOS_BiosCall_without_paging
	jmp  __LiBOS_BiosCall_with_paging
invalid_tag:
	ret

__LiBOS_BiosCall_without_paging:
    cli
    pusha ; once in [BITS 32], it pushes 32 bytes
    
; copy the actual code at physical address 0x9000 -+-+-+-+-++-+
    mov      esi, @actual_code_at_0x9000 
    mov      edi, 0x9000
    mov      ecx, (@actual_code_end - @actual_code_at_0x9000)
    cld      
    rep      movsb
    jmp      0x9000
; now bios call code resides at physical address 0x9000 -+-++-+
        
@actual_code_at_0x9000:
    mov     DWORD [REBASE(stack32_ptr)], esp 
    a32     sidt [REBASE(idt32_ptr)]
    a32     sgdt [REBASE(gdt32_ptr)]       
    a32     lgdt [REBASE(gdt16_ptr)]
    a32     lidt [REBASE(idt16_ptr)]
    mov     eax, DWORD[esp + 0x24] 
    mov     BYTE [REBASE(int_no)], al
    mov     esi, DWORD[esp + 0x28]
    mov     edi, STACK16
    mov     ecx, 0x1A
    mov     esp, edi               ; from now on esp is STACK16 
    rep     movsb
    jmp     word 0x18:REBASE(p_mode16)
    
p_mode16:
[BITS 16]
    mov     ax, 0x20
    mov     ds, ax
    mov     es, ax
    mov     fs, ax                        
    mov     gs, ax                        
    mov     ss, ax                        
    mov     eax, cr0                      
    and     al,  ~0x01                    
    mov     cr0, eax                      
    jmp     word 0x0000:REBASE(r_mode16)  
    
r_mode16:  
[BITS 16]     
    xor     ax, ax                        
    mov     ds, ax    
    mov     gs, ax  
    mov     fs, ax  
    mov     es, ax  
    mov     ss, ax;  
    lidt    [REBASE(idt16_ptr)]
    popa                                   ; load general purpose registers from 16bit stack
    pop     gs                             ; load gs from 16bit stack
    pop     fs                             ; load fs from 16bit stack
    pop     es                             ; load es from 16bit stack
    pop     ds
    sti
    db 0xCD
int_no:                         
    db 0x00

; after interrupt we need to, without destroying, save cpu state again on STACK16 for restoring 
    cli
    xor     sp, sp
    mov     ss, sp
    mov     sp, 0x9000
    pushf ; once in [BITS 16], it pushes 2 bytes                              
    push    ds                            
    push    es                            
    push    fs                            
    push    gs                            
    pusha ; once in [BITS 16], it pushes 16 bytes
; now we have all cpu state at hand on STACK16 space 
    
    mov     eax, cr0                      
    or      eax, 0x01                           
    mov     cr0, eax 
    jmp     0x08:REBASE(.LiBOS_GDT_IDT)

.LiBOS_GDT_IDT:
[BITS 32]
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    lgdt    [REBASE(gdt32_ptr)]
    lidt    [REBASE(idt32_ptr)]
    mov     esp, DWORD[REBASE(stack32_ptr)]
; time to restore STACK16 to our actual stack 
    mov     esi, STACK16                
    mov     edi, DWORD[esp + 0x28] 
    mov     ecx, 0x1A            
    cld                                
    rep     movsb
; now stack cleaning 
    popa
    ret
;-------------------------------------------------------
    stack32_ptr:                 
        dd 0x00000000                
    idt32_ptr:                       
        dw 0x0000                    
        dd 0x00000000                
    gdt32_ptr:                       
        dw 0x0000                    
        dd 0x00000000                
    idt16_ptr:                       
        dw 0x03FF                    
        dd 0x00000000                
    gdt16_base:                      
        .null:                       
            dd 0x00000000            
            dd 0x00000000            
        .code32:                     
            dw 0xFFFF                
            dw 0x0000                
            db 0x00                  
            db 0x9A                  
            db 0xCF                  
            db 0x00                  
        .data32:                     
            dw 0xFFFF                
            dw 0x0000                
            db 0x00                  
            db 0x92                  
            db 0xCF                  
            db 0x00                  
        .code16:                     
            dw 0xFFFF                
            dw 0x0000                
            db 0x00                  
            db 0x9A                  
            db 0x0F                  
            db 0x00                  
        .data16:                     
            dw 0xFFFF                
            dw 0x0000                
            db 0x00                  
            db 0x92                  
            db 0x0F                  
            db 0x00                  
    gdt16_ptr:                       
        dw gdt16_ptr - gdt16_base - 1
        dd gdt16_base           

@actual_code_end:

;; -+-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+ void __LiBOS_BiosCall_with_paging(UINT_32 int_no, REGS* r);

__LiBOS_BiosCall_with_paging:
    cli
    pusha
    mov      esi, @actual_code_at_0x9000_with_paging 
    mov      edi, 0x9000
    mov      ecx, (@actual_code_end_with_paging - @actual_code_at_0x9000_with_paging)
    cld      
    rep      movsb
    jmp      0x9000        
@actual_code_at_0x9000_with_paging:
    mov     DWORD [REBASE_with_paging(stack32_ptr_with_paging)], esp 
    a32     sidt  [REBASE_with_paging(idt32_ptr_with_paging)]
    a32     sgdt  [REBASE_with_paging(gdt32_ptr_with_paging)]       
    a32     lgdt  [REBASE_with_paging(gdt16_ptr_with_paging)]
    a32     lidt  [REBASE_with_paging(idt16_ptr_with_paging)]
    mov     eax,  DWORD[esp + 0x24] 
    mov     BYTE  [REBASE_with_paging(int_no_with_paging)], al
    mov     esi,  DWORD[esp + 0x28]
    mov     edi,  STACK16_with_paging
    mov     ecx,  0x1A
    mov     esp,  edi
    rep     movsb
    jmp     word  0x18:REBASE_with_paging(p_mode16_with_paging)
p_mode16_with_paging:
[BITS 16]
    mov     ax, 0x20
    mov     ds, ax
    mov     es, ax
    mov     fs, ax                        
    mov     gs, ax                        
    mov     ss, ax                        
    mov     eax, cr0                      
    and     eax, ~0x80000001
    mov     cr0, eax                      
    jmp     word 0x0000:REBASE_with_paging(r_mode16_with_paging)  
r_mode16_with_paging:  
[BITS 16]     
    xor     ax, ax                        
    mov     ds, ax    
    mov     gs, ax  
    mov     fs, ax  
    mov     es, ax  
    mov     ss, ax
    lidt    [REBASE_with_paging(idt16_ptr_with_paging)]
    popa
    pop     gs
    pop     fs
    pop     es
    pop     ds
    sti
    db 0xCD
int_no_with_paging:                         
    db 0x00
    cli
    xor     sp, sp
    mov     ss, sp
    mov     sp, 0x9000
    pushf                        
    push    ds                            
    push    es                            
    push    fs                            
    push    gs                            
    pusha    
    mov     eax, cr0                      
    or      eax, 0x80000001
    mov     cr0, eax 
    jmp     0x08:REBASE_with_paging(.LiBOS_GDT_IDT_with_paging)

.LiBOS_GDT_IDT_with_paging:
[BITS 32]
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    lgdt    [REBASE_with_paging(gdt32_ptr_with_paging)]
    lidt    [REBASE_with_paging(idt32_ptr_with_paging)]
    mov     esp, DWORD[REBASE_with_paging(stack32_ptr_with_paging)] 
    mov     esi, STACK16_with_paging                
    mov     edi, DWORD[esp + 0x28] 
    mov     ecx, 0x1A            
    cld                                
    rep     movsb
    popa
    ret
;------------------------------------------------------------
    stack32_ptr_with_paging:                 
        dd 0x00000000                
    idt32_ptr_with_paging:                       
        dw 0x0000                    
        dd 0x00000000                
    gdt32_ptr_with_paging:                       
        dw 0x0000                    
        dd 0x00000000                
    idt16_ptr_with_paging:                       
        dw 0x03FF                    
        dd 0x00000000                
    gdt16_base_with_paging:                      
        .null_with_paging:                       
            dd 0x00000000            
            dd 0x00000000            
        .code32_with_paging:                     
            dw 0xFFFF                
            dw 0x0000                
            db 0x00                  
            db 0x9A                  
            db 0xCF                  
            db 0x00                  
        .data32_with_paging:                     
            dw 0xFFFF                
            dw 0x0000                
            db 0x00                  
            db 0x92                  
            db 0xCF                  
            db 0x00                  
        .code16_with_paging:                     
            dw 0xFFFF                
            dw 0x0000                
            db 0x00                  
            db 0x9A                  
            db 0x0F                  
            db 0x00                  
        .data16_with_paging:                     
            dw 0xFFFF                
            dw 0x0000                
            db 0x00                  
            db 0x92                  
            db 0x0F                  
            db 0x00                  
    gdt16_ptr_with_paging:                       
        dw gdt16_ptr_with_paging - gdt16_base_with_paging - 1
        dd gdt16_base_with_paging           

@actual_code_end_with_paging:

