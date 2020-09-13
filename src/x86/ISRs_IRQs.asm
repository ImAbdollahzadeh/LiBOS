[BITS 32]

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .data
	STRING : db "Iman Abdollahzadeh", 0
	STRING_SIZE equ $ - STRING

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

section .text
	
	global _execute_ect
	global _LiBOSASM_load_ext_image
	global execute_IRA
	global query_cr2
	
	global activate_tss0
	global activate_tss1
	global activate_tss2
	global activate_tss3	

	global IDTLoad
	global GDTLoad
	extern idt_pointer
	extern gdt_pointer
	extern FAULT_HANDLER
	extern IRQ_HANDLER
	global _CLI
	global _STI
	global ISR_DEFAULT
	global ISR0
	global ISR1
	global ISR2
	global ISR3
	global ISR4
	global ISR5
	global ISR6
	global ISR7
	global ISR8
	global ISR9
	global ISR10
	global ISR11
	global ISR12
	global ISR13
	global ISR14
	global ISR15
	global ISR16
	global ISR17
	global ISR18
	global ISR19
	global ISR20
	global ISR21
	global ISR22
	global ISR23
	global ISR24
	global ISR25
	global ISR26
	global ISR27
	global ISR28
	global ISR29
	global ISR30
	global ISR31
	global IRQ0
	global IRQ1
	global IRQ2
	global IRQ3
	global IRQ4
	global IRQ5
	global IRQ6
	global IRQ7
	global IRQ8
	global IRQ9
	global IRQ10
	global IRQ11
	global IRQ12
	global IRQ13
	global IRQ14
	global IRQ15
	global IRQ16
	global IRQ17
	global IRQ18
	global IRQ19
	global IRQ20
	global IRQ21
	global IRQ22
	global IRQ23
	global IRQ24
	global IRQ25
	global IRQ26
	global IRQ27
	global IRQ28
	global IRQ29
	global IRQ30
	global IRQ31
	global IRQ32
	global IRQ33
	global IRQ34
	global IRQ35
	global IRQ36
	global IRQ37
	global IRQ38
	global IRQ39
	global IRQ40
	global IRQ41
	global IRQ42
	global IRQ43
	global IRQ44
	global IRQ45
	global IRQ46
	global IRQ47
	global IRQ48
	global IRQ49
	global IRQ40
	global IRQ41
	global IRQ42
	global IRQ43
	global IRQ44
	global IRQ45
	global IRQ46
	global IRQ47
	global IRQ48
	global IRQ49
	global IRQ50
	global IRQ51
	global IRQ52
	global IRQ53
	global IRQ54
	global IRQ55
	global IRQ56
	global IRQ57
	global IRQ58
	global IRQ59
	global IRQ60
	global IRQ61
	global IRQ62
	global IRQ63
	global IRQ64
	global IRQ65
	global IRQ66
	global IRQ67
	global IRQ68
	global IRQ69
	global IRQ70
	global IRQ71
	global IRQ72
	global IRQ73
	global IRQ74
	global IRQ75
	global IRQ76
	global IRQ77
	global IRQ78
	global IRQ79
	global IRQ80
	global IRQ81
	global IRQ82
	global IRQ83
	global IRQ84
	global IRQ85
	global IRQ86
	global IRQ87
	global IRQ88
	global IRQ89
	global IRQ90
	global IRQ91
	global IRQ92
	global IRQ93
	global IRQ94
	global IRQ95
	global IRQ96
	global IRQ97
	global IRQ98
	global IRQ99
	global IRQ100
	global IRQ101
	global IRQ102
	global IRQ103
	global IRQ104
	global IRQ105
	global IRQ106
	global IRQ107
	global IRQ108
	global IRQ109
	global IRQ110
	global IRQ111
	global IRQ112
	global IRQ113
	global IRQ114
	global IRQ115
	global IRQ116
	global IRQ117
	global IRQ118
	global IRQ119
	global IRQ120
	global IRQ121
	global IRQ122
	global IRQ123
	global IRQ124
	global IRQ125
	global IRQ126
	global IRQ127
	global IRQ128
	global IRQ129
	global IRQ130
	global IRQ131
	global IRQ132
	global IRQ133
	global IRQ134
	global IRQ135
	global IRQ136
	global IRQ137
	global IRQ138
	global IRQ139
	global IRQ140
	global IRQ141
	global IRQ142
	global IRQ143
	global IRQ144
	global IRQ145
	global IRQ146
	global IRQ147
	global IRQ148
	global IRQ149
	global IRQ140
	global IRQ141
	global IRQ142
	global IRQ143
	global IRQ144
	global IRQ145
	global IRQ146
	global IRQ147
	global IRQ148
	global IRQ149
	global IRQ150
	global IRQ151
	global IRQ152
	global IRQ153
	global IRQ154
	global IRQ155
	global IRQ156
	global IRQ157
	global IRQ158
	global IRQ159
	global IRQ160
	global IRQ161
	global IRQ162
	global IRQ163
	global IRQ164
	global IRQ165
	global IRQ166
	global IRQ167
	global IRQ168
	global IRQ169
	global IRQ170
	global IRQ171
	global IRQ172
	global IRQ173
	global IRQ174
	global IRQ175
	global IRQ176
	global IRQ177
	global IRQ178
	global IRQ179
	global IRQ180
	global IRQ181
	global IRQ182
	global IRQ183
	global IRQ184
	global IRQ185
	global IRQ186
	global IRQ187
	global IRQ188
	global IRQ189
	global IRQ190
	global IRQ191
	global IRQ192
	global IRQ193
	global IRQ194
	global IRQ195
	global IRQ196
	global IRQ197
	global IRQ198
	global IRQ199
	global IRQ200
	global IRQ201
	global IRQ202
	global IRQ203
	global IRQ204
	global IRQ205
	global IRQ206
	global IRQ207
	global IRQ208
	global IRQ209
	global IRQ210
	global IRQ211
	global IRQ212
	global IRQ213
	global IRQ214
	global IRQ215
	global IRQ216
	global IRQ217
	global IRQ218
	global IRQ219
	global IRQ220
	global IRQ221
	global IRQ222
	global IRQ223

	global CHECK_DS
	global CHECK_GS
	global CHECK_ES
	global CHECK_FS

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

ISR_DEFAULT:
	iret
	
;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

;0: Divide By Zero Exception
ISR0:
	cli
	push dword 0    ; A normal ISR stub that pops a dummy error code to keep a uniform stack frame
	push dword 0
	jmp isr_common_stub
;1: Debug Exception
ISR1:
	cli
	push dword 0
	push dword 1
	jmp isr_common_stub
ISR2:
	cli
	push dword 0
	push dword 2
	jmp isr_common_stub
ISR3:
	cli
	push dword 0
	push dword 3
	jmp isr_common_stub
ISR4:
	cli
	push dword 0
	push dword 4
	jmp isr_common_stub
ISR5:
	cli
	push dword 0
	push dword 5
	jmp isr_common_stub
ISR6:
	cli
	push dword 0
	push dword 6
	jmp isr_common_stub
ISR7:
	cli
	push dword 0
	push dword 7
	jmp isr_common_stub
; 8: Double Fault Exception (With Error Code!)
ISR8:
	cli
	push dword 8    ;Note that we DON'T push a value on the stack in this one!
                   ;It pushes one already! Use this type of stub for exceptions that pop error codes (http://www.osdever.net/bkerndev/Docs/isrs.htm)
	jmp isr_common_stub
ISR9:
	cli
	push dword 0
	push dword 9
	jmp isr_common_stub
ISR10:
	cli
	push dword 10
	jmp isr_common_stub
ISR11:
	cli
	push dword 11
	jmp isr_common_stub
ISR12:
	cli
	push dword 12
	jmp isr_common_stub
ISR13:
	cli
	push dword 13
	jmp isr_common_stub
ISR14:
	cli
	push dword 14
	jmp isr_common_stub
ISR15:
	cli
	push dword 0
	push dword 15
	jmp isr_common_stub
ISR16:
	cli
	push dword 0
	push dword 16
	jmp isr_common_stub
ISR17:
	cli
	push dword 0
	push dword 17
	jmp isr_common_stub
ISR18:
	cli
	push dword 0
	push dword 18
	jmp isr_common_stub
ISR19:
	cli
	push dword 0
	push dword 19
	jmp isr_common_stub
ISR20:
	cli
	push dword 0
	push dword 20
	jmp isr_common_stub
ISR21:
	cli
	push dword 0
	push dword 21
	jmp isr_common_stub
ISR22:
	cli
	push dword 0
	push dword 22
	jmp isr_common_stub
ISR23:
	cli
	push dword 0
	push dword 23
	jmp isr_common_stub
ISR24:
	cli
	push dword 0
	push dword 24
	jmp isr_common_stub
ISR25:
	cli
	push dword 0
	push dword 25
	jmp isr_common_stub
ISR26:
	cli
	push dword 0
	push dword 26
	jmp isr_common_stub
ISR27:
	cli
	push dword 0
	push dword 27
	jmp isr_common_stub
ISR28:
	cli
	push dword 0
	push dword 28
	jmp isr_common_stub
ISR29:
	cli
	push dword 0
	push dword 29
	jmp isr_common_stub
ISR30:
	cli
	push dword 0
	push dword 30
	jmp isr_common_stub
ISR31:
	cli
	push dword 0
	push dword 31
	jmp isr_common_stub
	
isr_common_stub:
	pusha
	push  ds
	push  es
	push  fs
	push  gs
	mov   ax, 0x10       ;; Load the Kernel Data Segment descriptor!
	mov   ds, ax
	mov   es, ax
	mov   fs, ax
	mov   gs, ax
	push  esp            ;; it will create the REGS* r in C function
	call  FAULT_HANDLER
	pop   esp
	pop   gs
	pop   fs
	pop   es
	pop   ds
	popa
	add   esp, 8         ;; Cleans up the pushed error code and pushed ISR number
	iret                 ;; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

IRQ0:
	cli
	push dword 0    ;Note that these don't push an error code on the stack: We need to push a dummy error code
	push dword 32
	jmp irq_common_stub
IRQ1:
	cli
	push dword 0   
	push dword 33
	jmp irq_common_stub
IRQ2:
	cli
	push dword 0
	push dword 34
	jmp irq_common_stub
IRQ3:
	cli
	push dword 0    
	push dword 35
	jmp irq_common_stub
IRQ4:
	cli
	push dword 0 
	push dword 36
	jmp irq_common_stub
IRQ5:
	cli
	push dword 0
	push dword 37
	jmp irq_common_stub
IRQ6:
	cli
	push dword 0 
	push dword 38
	jmp irq_common_stub
IRQ7:
	cli
	push dword 0   
	push dword 39
	jmp irq_common_stub
IRQ8:
	cli
	push dword 0   
	push dword 40
	jmp irq_common_stub
IRQ9:
	cli
	push dword 0    
	push dword 41
	jmp irq_common_stub
IRQ10:
	cli
	push dword 0    
	push dword 42
	jmp irq_common_stub
IRQ11:
	cli
	push dword 0   
	push dword 43
	jmp irq_common_stub
IRQ12:
	cli
	push dword 0
	push dword 44
	jmp irq_common_stub
IRQ13:
	cli
	push dword 0   
	push dword 45
	jmp irq_common_stub
IRQ14:
	cli
	push dword 0   
	push dword 46
	jmp irq_common_stub
IRQ15:
	cli
	push dword 0   
	push dword 47
	jmp irq_common_stub
IRQ16:
	cli
	push dword 0 
	push dword 48
	jmp irq_common_stub
IRQ17:
	cli
	push dword 0   
	push dword 49
	jmp irq_common_stub
IRQ18:
	cli
	push dword 0   
	push dword 50
	jmp irq_common_stub
IRQ19:
	cli
	push dword 0    
	push dword 51
	jmp irq_common_stub
IRQ20:
	cli
	push dword 0    
	push dword 52
	jmp irq_common_stub
IRQ21:
	cli
	push dword 0   
	push dword 53
	jmp irq_common_stub
IRQ22:
	cli
	push dword 0
	push dword 54
	jmp irq_common_stub
IRQ23:
	cli
	push dword 0   
	push dword 55
	jmp irq_common_stub
IRQ24:
	cli
	push dword 0   
	push dword 56
	jmp irq_common_stub
IRQ25:
	cli
	push dword 0   
	push dword 57
	jmp irq_common_stub
IRQ26:
	cli
	push dword 0 
	push dword 58
	jmp irq_common_stub
IRQ27:
	cli
	push dword 0   
	push dword 59
	jmp irq_common_stub
IRQ28:
	cli
	push dword 0   
	push dword 60
	jmp irq_common_stub
IRQ29:
	cli
	push dword 0    
	push dword 61
	jmp irq_common_stub
IRQ30:
	cli
	push dword 0    
	push dword 62
	jmp irq_common_stub
IRQ31:
	cli
	push dword 0   
	push dword 63
	jmp irq_common_stub
IRQ32:
	cli
	push dword 0
	push dword 64
	jmp irq_common_stub
IRQ33:
	cli
	push dword 0   
	push dword 65
	jmp irq_common_stub
IRQ34:
	cli
	push dword 0   
	push dword 66
	jmp irq_common_stub
IRQ35:
	cli
	push dword 0   
	push dword 67
	jmp irq_common_stub
IRQ36:
	cli
	push dword 0 
	push dword 68
	jmp irq_common_stub
IRQ37:
	cli
	push dword 0   
	push dword 69
	jmp irq_common_stub
IRQ38:
	cli
	push dword 0   
	push dword 70
	jmp irq_common_stub
IRQ39:
	cli
	push dword 0    
	push dword 71
	jmp irq_common_stub
IRQ40:
	cli
	push dword 0    
	push dword 72
	jmp irq_common_stub
IRQ41:
	cli
	push dword 0   
	push dword 73
	jmp irq_common_stub
IRQ42:
	cli
	push dword 0
	push dword 74
	jmp irq_common_stub
IRQ43:
	cli
	push dword 0   
	push dword 75
	jmp irq_common_stub
IRQ44:
	cli
	push dword 0   
	push dword 76
	jmp irq_common_stub
IRQ45:
	cli
	push dword 0   
	push dword 77
	jmp irq_common_stub
IRQ46:
	cli
	push dword 0 
	push dword 78
	jmp irq_common_stub
IRQ47:
	cli
	push dword 0   
	push dword 79
	jmp irq_common_stub
IRQ48:
	cli
	push dword 0   
	push dword 80
	jmp irq_common_stub
IRQ49:
	cli
	push dword 0    
	push dword 81
	jmp irq_common_stub
IRQ50:
	cli
	push dword 0    
	push dword 82
	jmp irq_common_stub
IRQ51:
	cli
	push dword 0   
	push dword 83
	jmp irq_common_stub
IRQ52:
	cli
	push dword 0
	push dword 84
	jmp irq_common_stub
IRQ53:
	cli
	push dword 0   
	push dword 85
	jmp irq_common_stub
IRQ54:
	cli
	push dword 0   
	push dword 86
	jmp irq_common_stub
IRQ55:
	cli
	push dword 0   
	push dword 87
	jmp irq_common_stub
IRQ56:
	cli
	push dword 0 
	push dword 88
	jmp irq_common_stub
IRQ57:
	cli
	push dword 0   
	push dword 89
	jmp irq_common_stub
IRQ58:
	cli
	push dword 0   
	push dword 90
	jmp irq_common_stub
IRQ59:
	cli
	push dword 0    
	push dword 91
	jmp irq_common_stub
IRQ60:
	cli
	push dword 0    
	push dword 92
	jmp irq_common_stub
IRQ61:
	cli
	push dword 0   
	push dword 93
	jmp irq_common_stub
IRQ62:
	cli
	push dword 0
	push dword 94
	jmp irq_common_stub
IRQ63:
	cli
	push dword 0   
	push dword 95
	jmp irq_common_stub
IRQ64:
	cli
	push dword 0   
	push dword 96
	jmp irq_common_stub
IRQ65:
	cli
	push dword 0   
	push dword 97
	jmp irq_common_stub
IRQ66:
	cli
	push dword 0 
	push dword 98
	jmp irq_common_stub
IRQ67:
	cli
	push dword 0   
	push dword 99
	jmp irq_common_stub
IRQ68:
	cli
	push dword 0   
	push dword 100
	jmp irq_common_stub
IRQ69:
	cli
	push dword 0    
	push dword 101
	jmp irq_common_stub
IRQ70:
	cli
	push dword 0    
	push dword 102
	jmp irq_common_stub
IRQ71:
	cli
	push dword 0   
	push dword 103
	jmp irq_common_stub
IRQ72:
	cli
	push dword 0
	push dword 104
	jmp irq_common_stub
IRQ73:
	cli
	push dword 0   
	push dword 105
	jmp irq_common_stub
IRQ74:
	cli
	push dword 0   
	push dword 106
	jmp irq_common_stub
IRQ75:
	cli
	push dword 0   
	push dword 107
	jmp irq_common_stub
IRQ76:
	cli
	push dword 0 
	push dword 108
	jmp irq_common_stub
IRQ77:
	cli
	push dword 0   
	push dword 109
	jmp irq_common_stub
IRQ78:
	cli
	push dword 0   
	push dword 110
	jmp irq_common_stub
IRQ79:
	cli
	push dword 0    
	push dword 111
	jmp irq_common_stub
IRQ80:
	cli
	push dword 0    
	push dword 112
	jmp irq_common_stub
IRQ81:
	cli
	push dword 0   
	push dword 113
	jmp irq_common_stub
IRQ82:
	cli
	push dword 0
	push dword 114
	jmp irq_common_stub
IRQ83:
	cli
	push dword 0   
	push dword 115
	jmp irq_common_stub
IRQ84:
	cli
	push dword 0   
	push dword 116
	jmp irq_common_stub
IRQ85:
	cli
	push dword 0   
	push dword 117
	jmp irq_common_stub
IRQ86:
	cli
	push dword 0 
	push dword 118
	jmp irq_common_stub
IRQ87:
	cli
	push dword 0   
	push dword 119
	jmp irq_common_stub
IRQ88:
	cli
	push dword 0   
	push dword 120
	jmp irq_common_stub
IRQ89:
	cli
	push dword 0    
	push dword 121
	jmp irq_common_stub
IRQ90:
	cli
	push dword 0    
	push dword 122
	jmp irq_common_stub
IRQ91:
	cli
	push dword 0   
	push dword 123
	jmp irq_common_stub
IRQ92:
	cli
	push dword 0
	push dword 124
	jmp irq_common_stub
IRQ93:
	cli
	push dword 0   
	push dword 125
	jmp irq_common_stub
IRQ94:
	cli
	push dword 0   
	push dword 126
	jmp irq_common_stub
IRQ95:
	cli
	push dword 0   
	push dword 127
	jmp irq_common_stub
IRQ96:
	cli
	push dword 0 
	push dword 128
	jmp irq_common_stub
IRQ97:
	cli
	push dword 0   
	push dword 129
	jmp irq_common_stub
IRQ98:
	cli
	push dword 0   
	push dword 130
	jmp irq_common_stub
IRQ99:
	cli
	push dword 0    
	push dword 131
	jmp irq_common_stub
IRQ100:
	cli
	push dword 0
	push dword 132
	jmp irq_common_stub
IRQ101:
	cli
	push dword 0   
	push dword 133
	jmp irq_common_stub
IRQ102:
	cli
	push dword 0
	push dword 134
	jmp irq_common_stub
IRQ103:
	cli
	push dword 0    
	push dword 135
	jmp irq_common_stub
IRQ104:
	cli
	push dword 0 
	push dword 136
	jmp irq_common_stub
IRQ105:
	cli
	push dword 0
	push dword 137
	jmp irq_common_stub
IRQ106:
	cli
	push dword 0 
	push dword 138
	jmp irq_common_stub
IRQ107:
	cli
	push dword 0   
	push dword 139
	jmp irq_common_stub
IRQ108:
	cli
	push dword 0   
	push dword 140
	jmp irq_common_stub
IRQ109:
	cli
	push dword 0    
	push dword 141
	jmp irq_common_stub
IRQ110:
	cli
	push dword 0    
	push dword 142
	jmp irq_common_stub
IRQ111:
	cli
	push dword 0   
	push dword 143
	jmp irq_common_stub
IRQ112:
	cli
	push dword 0
	push dword 144
	jmp irq_common_stub
IRQ113:
	cli
	push dword 0   
	push dword 145
	jmp irq_common_stub
IRQ114:
	cli
	push dword 0   
	push dword 146
	jmp irq_common_stub
IRQ115:
	cli
	push dword 0   
	push dword 147
	jmp irq_common_stub
IRQ116:
	cli
	push dword 0 
	push dword 148
	jmp irq_common_stub
IRQ117:
	cli
	push dword 0   
	push dword 149
	jmp irq_common_stub
IRQ118:
	cli
	push dword 0   
	push dword 150
	jmp irq_common_stub
IRQ119:
	cli
	push dword 0    
	push dword 151
	jmp irq_common_stub
IRQ120:
	cli
	push dword 0    
	push dword 152
	jmp irq_common_stub
IRQ121:
	cli
	push dword 0   
	push dword 153
	jmp irq_common_stub
IRQ122:
	cli
	push dword 0
	push dword 154
	jmp irq_common_stub
IRQ123:
	cli
	push dword 0   
	push dword 155
	jmp irq_common_stub
IRQ124:
	cli
	push dword 0   
	push dword 156
	jmp irq_common_stub
IRQ125:
	cli
	push dword 0   
	push dword 157
	jmp irq_common_stub
IRQ126:
	cli
	push dword 0 
	push dword 158
	jmp irq_common_stub
IRQ127:
	cli
	push dword 0   
	push dword 159
	jmp irq_common_stub
IRQ128:
	cli
	push dword 0   
	push dword 160
	jmp irq_common_stub
IRQ129:
	cli
	push dword 0    
	push dword 161
	jmp irq_common_stub
IRQ130:
	cli
	push dword 0    
	push dword 162
	jmp irq_common_stub
IRQ131:
	cli
	push dword 0   
	push dword 163
	jmp irq_common_stub
IRQ132:
	cli
	push dword 0
	push dword 164
	jmp irq_common_stub
IRQ133:
	cli
	push dword 0   
	push dword 165
	jmp irq_common_stub
IRQ134:
	cli
	push dword 0   
	push dword 166
	jmp irq_common_stub
IRQ135:
	cli
	push dword 0   
	push dword 167
	jmp irq_common_stub
IRQ136:
	cli
	push dword 0 
	push dword 168
	jmp irq_common_stub
IRQ137:
	cli
	push dword 0   
	push dword 169
	jmp irq_common_stub
IRQ138:
	cli
	push dword 0   
	push dword 170
	jmp irq_common_stub
IRQ139:
	cli
	push dword 0    
	push dword 171
	jmp irq_common_stub
IRQ140:
	cli
	push dword 0    
	push dword 172
	jmp irq_common_stub
IRQ141:
	cli
	push dword 0   
	push dword 173
	jmp irq_common_stub
IRQ142:
	cli
	push dword 0
	push dword 174
	jmp irq_common_stub
IRQ143:
	cli
	push dword 0   
	push dword 175
	jmp irq_common_stub
IRQ144:
	cli
	push dword 0   
	push dword 176
	jmp irq_common_stub
IRQ145:
	cli
	push dword 0   
	push dword 177
	jmp irq_common_stub
IRQ146:
	cli
	push dword 0 
	push dword 178
	jmp irq_common_stub
IRQ147:
	cli
	push dword 0   
	push dword 179
	jmp irq_common_stub
IRQ148:
	cli
	push dword 0   
	push dword 180
	jmp irq_common_stub
IRQ149:
	cli
	push dword 0    
	push dword 181
	jmp irq_common_stub
IRQ150:
	cli
	push dword 0    
	push dword 182
	jmp irq_common_stub
IRQ151:
	cli
	push dword 0   
	push dword 183
	jmp irq_common_stub
IRQ152:
	cli
	push dword 0
	push dword 184
	jmp irq_common_stub
IRQ153:
	cli
	push dword 0   
	push dword 185
	jmp irq_common_stub
IRQ154:
	cli
	push dword 0   
	push dword 186
	jmp irq_common_stub
IRQ155:
	cli
	push dword 0   
	push dword 187
	jmp irq_common_stub
IRQ156:
	cli
	push dword 0 
	push dword 188
	jmp irq_common_stub
IRQ157:
	cli
	push dword 0   
	push dword 189
	jmp irq_common_stub
IRQ158:
	cli
	push dword 0   
	push dword 190
	jmp irq_common_stub
IRQ159:
	cli
	push dword 0    
	push dword 191
	jmp irq_common_stub
IRQ160:
	cli
	push dword 0    
	push dword 192
	jmp irq_common_stub
IRQ161:
	cli
	push dword 0   
	push dword 193
	jmp irq_common_stub
IRQ162:
	cli
	push dword 0
	push dword 194
	jmp irq_common_stub
IRQ163:
	cli
	push dword 0   
	push dword 195
	jmp irq_common_stub
IRQ164:
	cli
	push dword 0   
	push dword 196
	jmp irq_common_stub
IRQ165:
	cli
	push dword 0   
	push dword 197
	jmp irq_common_stub
IRQ166:
	cli
	push dword 0 
	push dword 198
	jmp irq_common_stub
IRQ167:
	cli
	push dword 0   
	push dword 199
	jmp irq_common_stub
IRQ168:
	cli
	push dword 0   
	push dword 200
	jmp irq_common_stub
IRQ169:
	cli
	push dword 0    
	push dword 201
	jmp irq_common_stub
IRQ170:
	cli
	push dword 0    
	push dword 202
	jmp irq_common_stub
IRQ171:
	cli
	push dword 0   
	push dword 203
	jmp irq_common_stub
IRQ172:
	cli
	push dword 0
	push dword 204
	jmp irq_common_stub
IRQ173:
	cli
	push dword 0   
	push dword 205
	jmp irq_common_stub
IRQ174:
	cli
	push dword 0   
	push dword 206
	jmp irq_common_stub
IRQ175:
	cli
	push dword 0   
	push dword 207
	jmp irq_common_stub
IRQ176:
	cli
	push dword 0 
	push dword 208
	jmp irq_common_stub
IRQ177:
	cli
	push dword 0   
	push dword 209
	jmp irq_common_stub
IRQ178:
	cli
	push dword 0   
	push dword 210
	jmp irq_common_stub
IRQ179:
	cli
	push dword 0    
	push dword 211
	jmp irq_common_stub
IRQ180:
	cli
	push dword 0    
	push dword 212
	jmp irq_common_stub
IRQ181:
	cli
	push dword 0   
	push dword 213
	jmp irq_common_stub
IRQ182:
	cli
	push dword 0
	push dword 214
	jmp irq_common_stub
IRQ183:
	cli
	push dword 0   
	push dword 215
	jmp irq_common_stub
IRQ184:
	cli
	push dword 0   
	push dword 216
	jmp irq_common_stub
IRQ185:
	cli
	push dword 0   
	push dword 217
	jmp irq_common_stub
IRQ186:
	cli
	push dword 0 
	push dword 218
	jmp irq_common_stub
IRQ187:
	cli
	push dword 0   
	push dword 219
	jmp irq_common_stub
IRQ188:
	cli
	push dword 0   
	push dword 220
	jmp irq_common_stub
IRQ189:
	cli
	push dword 0    
	push dword 221
	jmp irq_common_stub
IRQ190:
	cli
	push dword 0    
	push dword 222
	jmp irq_common_stub
IRQ191:
	cli
	push dword 0   
	push dword 223
	jmp irq_common_stub
IRQ192:
	cli
	push dword 0
	push dword 224
	jmp irq_common_stub
IRQ193:
	cli
	push dword 0   
	push dword 225
	jmp irq_common_stub
IRQ194:
	cli
	push dword 0   
	push dword 226
	jmp irq_common_stub
IRQ195:
	cli
	push dword 0   
	push dword 227
	jmp irq_common_stub
IRQ196:
	cli
	push dword 0 
	push dword 228
	jmp irq_common_stub
IRQ197:
	cli
	push dword 0   
	push dword 229
	jmp irq_common_stub
IRQ198:
	cli
	push dword 0   
	push dword 230
	jmp irq_common_stub
IRQ199:
	cli
	push dword 0    
	push dword 231
	jmp irq_common_stub
IRQ200:
	cli
	push dword 0
	push dword 232
	jmp irq_common_stub
IRQ201:
	cli
	push dword 0   
	push dword 233
	jmp irq_common_stub
IRQ202:
	cli
	push dword 0
	push dword 234
	jmp irq_common_stub
IRQ203:
	cli
	push dword 0    
	push dword 235
	jmp irq_common_stub
IRQ204:
	cli
	push dword 0 
	push dword 236
	jmp irq_common_stub
IRQ205:
	cli
	push dword 0
	push dword 237
	jmp irq_common_stub
IRQ206:
	cli
	push dword 0 
	push dword 238
	jmp irq_common_stub
IRQ207:
	cli
	push dword 0   
	push dword 239
	jmp irq_common_stub
IRQ208:
	cli
	push dword 0   
	push dword 240
	jmp irq_common_stub
IRQ209:
	cli
	push dword 0    
	push dword 241
	jmp irq_common_stub
IRQ210:
	cli
	push dword 0    
	push dword 242
	jmp irq_common_stub
IRQ211:
	cli
	push dword 0   
	push dword 243
	jmp irq_common_stub
IRQ212:
	cli
	push dword 0
	push dword 244
	jmp irq_common_stub
IRQ213:
	cli
	push dword 0   
	push dword 245
	jmp irq_common_stub
IRQ214:
	cli
	push dword 0   
	push dword 246
	jmp irq_common_stub
IRQ215:
	cli
	push dword 0   
	push dword 247
	jmp irq_common_stub
IRQ216:
	cli
	push dword 0 
	push dword 248
	jmp irq_common_stub
IRQ217:
	cli
	push dword 0   
	push dword 249
	jmp irq_common_stub
IRQ218:
	cli
	push dword 0   
	push dword 250
	jmp irq_common_stub
IRQ219:
	cli
	push dword 0    
	push dword 251
	jmp irq_common_stub
IRQ220:
	cli
	push dword 0    
	push dword 252
	jmp irq_common_stub
IRQ221:
	cli
	push dword 0   
	push dword 253
	jmp irq_common_stub
IRQ222:
	cli
	push dword 0
	push dword 254
	jmp irq_common_stub
IRQ223:
	cli
	push dword 0   
	push dword 255
	jmp irq_common_stub

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
irq_common_stub:
	pusha
	push ds
	push es
	push fs
	push gs
	mov  ax, 0x10
	mov  ds, ax
	mov  es, ax
	mov  fs, ax
	mov  gs, ax
	push esp
	call IRQ_HANDLER
	add  esp, 4
	pop  gs
	pop  fs
	pop  es
	pop  ds
	popa
	add  esp, 8
	iret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

IDTLoad:
	push ebp
	mov  ebp, esp
	lidt [idt_pointer]
	mov  eax, dword [ebp + 4]
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

GDTLoad:
	push ebp
	mov  ebp, esp
	lgdt [gdt_pointer]
	mov  ax, 0x10
	mov  ds, ax
	mov  es, ax
	mov  fs, ax
	mov  gs, ax
	mov  ss, ax
	jmp  0x08:@end
@end:
	mov  eax, dword [ebp + 4]
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

_CLI:
	cli
	ret
	
_STI:
	sti
	ret
	
;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

CHECK_DS:
	push ebp
    	mov  ebp, esp
	mov  cx,  0x10
	mov  eax, [ebp + 8]
	mov  bx,  ds
	cmp  bx,  cx
	jne  CHECK_DS_FAILURE
CHECK_DS_RIGHT:
	mov DWORD [eax], 0x00000010
	jmp END_OF_CHECK_DS
CHECK_DS_FAILURE:
	mov DWORD [eax], 0x00
END_OF_CHECK_DS:
	mov  esp, ebp
    	pop  ebp
    	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

CHECK_GS:
	push ebp
    	mov  ebp, esp
	mov  cx,  0x10
	mov  eax, [ebp + 8]
	mov  bx,  gs
	cmp  bx,  cx
	jne  CHECK_GS_FAILURE
CHECK_GS_RIGHT:
	mov DWORD [eax], 0x00000010
	jmp END_OF_CHECK_GS
CHECK_GS_FAILURE:
	mov DWORD [eax], 0x00
END_OF_CHECK_GS:
	mov  esp, ebp
    	pop  ebp
    	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

CHECK_FS:
	push ebp
    	mov  ebp, esp
	mov  cx,  0x10
	mov  eax, [ebp + 8]
	mov  bx,  fs
	cmp  bx,  cx
	jne  CHECK_FS_FAILURE
CHECK_FS_RIGHT:
	mov DWORD [eax], 0x00000010
	jmp END_OF_CHECK_FS
CHECK_FS_FAILURE:
	mov DWORD [eax], 0x00
END_OF_CHECK_FS:
	mov  esp, ebp
    	pop  ebp
    	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

CHECK_ES:
	push ebp
	mov  ebp, esp
	mov  cx,  0x10
	mov  eax, [ebp + 8]
	mov  bx,  es
	cmp  bx,  cx
	jne  CHECK_ES_FAILURE
CHECK_ES_RIGHT:
	mov DWORD [eax], 0x00000010
	jmp END_OF_CHECK_ES
CHECK_ES_FAILURE:
	mov DWORD [eax], 0x00
END_OF_CHECK_ES:
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ _execute_ect(void);

_execute_ect:
	push ebp 
	mov ebp, esp

	mov eax, STRING
	mov ebx, STRING_SIZE
	push ebx
	push eax
	xor ebx, ebx
	mov eax, dword[0xBF500004]
	call eax
	pop eax
	pop ebx
	mov esp, ebp
	pop ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ _LiBOSASM_load_ext_image(char* path, void* buffer);

_LiBOSASM_load_ext_image:
	push ebp 
	mov  ebp, esp
	mov  eax, DWORD[ebp + 8]
	mov  ebx, DWORD[ebp + 12]
	push ebx
	push eax

	call 0x01000040

	pop  eax
	pop  ebx
	mov  esp, ebp
	pop  ebp
	ret


;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void execute_IRA(void);

execute_IRA:
	push ebp 
	mov  ebp, esp
	mov  eax, 0x01000000
	mov  ebx, 0x0DC9B000
	push ebx
	push eax
	call 0x0DC82000 ;call how_many_lines
	pop  eax
	pop  ebx

	mov  eax, 0x0D819000 ;address of ISBA_TPs
	mov  ebx, DWORD[0x0DC9B000] ;number of lines
	push ebx
	push eax
	call 0x0DC82400 ;call initialize_TPs
	pop  eax
	pop  ebx
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ void query_cr2(UINT_32* cr2);

query_cr2:
	push ebp 
	mov  ebp, esp
	mov  ecx, DWORD[ebp + 0x8] ; get pointer to the return UINT_32 as CR2
	mov  eax, cr2
	mov  DWORD[ecx], eax
	mov  esp, ebp
	pop  ebp
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

activate_tss0:
	cli
	mov ax, 0x3B      
	ltr ax
	ret
activate_tss1:
	cli
	mov ax, 0x43     
	ltr ax      
	ret
activate_tss2:
	cli
	mov ax, 0x4B     
	ltr ax      
	ret
activate_tss3:
	cli
	mov ax, 0x53     
	ltr ax      
	ret

;;-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
