; ..................................................................................................
; NASM ASSEMBLY CODE FOR THE ACCELERATION OF WINDOW MANAGER CODE                   .................
; ALL RIGHTS RESERVED FOR IMAN ABDOLLAHZADEH                                       .................
; THE TRANSCRIBED MASM CODE FOR X86 MICROSOFT VISUAL C++ IS AVAILABLE              .................
; ..................................................................................................

section .text

global _sse_blit_off_screen_to_screen

_sse_blit_off_screen_to_screen:
push   ebp
mov    ebp,  esp
mov    esi,  [ebp +8] ;get src pointer
mov    edi,  [ebp+12] ;get trg pointer
mov    eax,  [ebp+16] ;get 16-byte blocks in width
__@LOOP:
movups xmm0, [esi]
movups xmm1, [esi + 0x1000]
movups xmm2, [esi + 0x2000]
movups xmm3, [esi + 0x3000]
movups xmm4, [esi + 0x4000]
movups xmm5, [esi + 0x5000]
movups xmm6, [esi + 0x6000]
movups xmm7, [esi + 0x7000]
movups [edi         ], xmm0
movups [edi + 0x1000], xmm1
movups [edi + 0x2000], xmm2
movups [edi + 0x3000], xmm3
movups [edi + 0x4000], xmm4
movups [edi + 0x5000], xmm5
movups [edi + 0x6000], xmm6
movups [edi + 0x7000], xmm7
add    esi, 16
add    edi, 16
sub    eax, 1
jnz __@LOOP
mov      esp,          ebp
pop      ebp
ret