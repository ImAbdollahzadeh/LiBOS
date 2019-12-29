; ..................................................................................................
; NASM ASSEMBLY CODE FOR THE ACCELERATION OF WINDOW MANAGER CODE                   .................
; ALL RIGHTS RESERVED FOR IMAN ABDOLLAHZADEH                                       .................
; THE TRANSCRIBED MASM CODE FOR X86 MICROSOFT VISUAL C++ IS AVAILABLE              .................
; ..................................................................................................

section .text

global _sse_blit_off_screen_to_screen
global _sse_copy_current_screen_to_other_screen


_sse_blit_off_screen_to_screen:
push   ebp
mov    ebp,  esp
mov    esi,  [ebp +8] ;get src pointer
mov    edi,  [ebp+12] ;get trg pointer
mov    eax,  [ebp+16] ;get 16-byte blocks in width
__@LOOP:
movdqa xmm0, [esi]
movdqa xmm1, [esi + 0x1000]
movdqa xmm2, [esi + 0x2000]
movdqa xmm3, [esi + 0x3000]
movdqa xmm4, [esi + 0x4000]
movdqa xmm5, [esi + 0x5000]
movdqa xmm6, [esi + 0x6000]
movdqa xmm7, [esi + 0x7000]
movdqa [edi         ], xmm0
movdqa [edi + 0x1000], xmm1
movdqa [edi + 0x2000], xmm2
movdqa [edi + 0x3000], xmm3
movdqa [edi + 0x4000], xmm4
movdqa [edi + 0x5000], xmm5
movdqa [edi + 0x6000], xmm6
movdqa [edi + 0x7000], xmm7
add    esi, 16
add    edi, 16
sub    eax, 1
jnz __@LOOP
mov      esp,          ebp
pop      ebp
ret


_sse_copy_current_screen_to_other_screen:
push   ebp
mov    ebp,  esp
mov    esi,  [ebp +8] ;get src pointer
mov    edi,  [ebp+12] ;get trg pointer
mov    ecx,  768
__@H:
mov    eax,  256
__@LOOP2:
movdqa xmm0, [esi]
movdqa xmm1, [esi + 0x1000]
movdqa xmm2, [esi + 0x2000]
movdqa xmm3, [esi + 0x3000]
movdqa xmm4, [esi + 0x4000]
movdqa xmm5, [esi + 0x5000]
movdqa xmm6, [esi + 0x6000]
movdqa xmm7, [esi + 0x7000]
movdqa [edi         ], xmm0
movdqa [edi + 0x1000], xmm1
movdqa [edi + 0x2000], xmm2
movdqa [edi + 0x3000], xmm3
movdqa [edi + 0x4000], xmm4
movdqa [edi + 0x5000], xmm5
movdqa [edi + 0x6000], xmm6
movdqa [edi + 0x7000], xmm7
add    esi, 16
add    edi, 16
sub    eax, 1
jnz    __@LOOP2
add    esi, 0x7000
add    edi, 0x7000
sub    ecx, 8
jnz   __@H
mov    esp,          ebp
pop    ebp
ret