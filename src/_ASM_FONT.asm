; ..................................................................................................
; NASM ASSEMBLY CODE FOR THE ACCELERATION OF DRAWING FONTS CODE                    .................
; ALL RIGHTS RESERVED FOR IMAN ABDOLLAHZADEH                                       .................
; THE TRANSCRIBED MASM CODE FOR X86 MICROSOFT VISUAL C++ IS AVAILABLE              .................
; ..................................................................................................
section .data
_0xFFs: DD 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF

section .text

global _asm_draw_character
global _sse_draw_character

; THIS IS THIS C CODE: -----------------------------------------------------------
;   UINT_8 h = 12;
;   UINT_8 w = 9;
;   while(h--)
;   {
;   	while(w--)
;   	{
;   		if((*src) & (0xFF000000)) 
;   		{
;   			*trg++ = background_color;
;   			src++;
;   		}
;   		else *trg++ = *src++;
;   	}
;   	trg = PHYSICAL_ADDRESS(trg) + (wnd->rect.width << 2) - 36;
;   	w = 9;
;   }
; -------------------------------------------------------------------------------
_asm_draw_character:
push   ebp
mov    ebp,  esp
mov    esi,  [ebp +8] ;get src pointer
mov    edi,  [ebp+12] ;get trg pointer
mov    edx,  12
mov    ebx,  [ebp+16] ;get window steps 
push   esp
mov    esp,  [ebp+20] ;get background_color

LiBOS_NEXT_ROW_IN_WINDOW:
mov    ecx,  9

LiBOS_NEXT_PIXEL_IN_CURRENT_ROW:
mov    eax,  [esi]
and    eax,  0xFF000000
jnz    LiBOS_COPY_BACKGROUND_COLOR
mov    [edi], eax
jmp    LiBOS_ONE_PIXEL_FURTHER

LiBOS_COPY_BACKGROUND_COLOR:
mov    [edi], esp

LiBOS_ONE_PIXEL_FURTHER:
add    esi,  4
add    edi,  4
sub    ecx,  1
jnz    LiBOS_NEXT_PIXEL_IN_CURRENT_ROW
add    edi,  ebx
sub    edx,  1
jnz    LiBOS_NEXT_ROW_IN_WINDOW

pop    esp
mov    esp,  ebp
pop    ebp
ret




;void _sse_draw_character(UINT_32* src, UINT_32* trg, UINT_32* background_color);
_sse_draw_character:
push   ebp
mov    ebp,  esp
mov    esi,  [ebp +8] ;get src pointer
mov    edi,  [ebp+12] ;get trg pointer
mov    ebx,  [ebp+16] ;get background_color

movdqu   xmm0, [esi]
movdqu   xmm2, [esi+16]
movdqu   xmm1, xmm0
movdqu   xmm3, xmm2
movdqu   xmm7, [_0xFFs]
movdqu   xmm6, [ebx]

pcmpeqd  xmm0, xmm7
pcmpeqd  xmm2, xmm7

pand     xmm6, xmm0
pxor     xmm1, xmm0
paddsw   xmm1, xmm6

movdqu   xmm6, [ebx]
pand     xmm6, xmm2
pxor     xmm3, xmm2
paddsw   xmm3, xmm6

movdqu   [edi], xmm1
movdqu   [edi+16], xmm3

mov    esp,  ebp
pop    ebp
ret