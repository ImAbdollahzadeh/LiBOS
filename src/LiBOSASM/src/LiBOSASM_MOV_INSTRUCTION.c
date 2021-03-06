#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

void convert_mov_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	if( get_parse_level() == PARSE_LEVEL_2 )
		printf("MOV DECODING: ");
	
	unsigned char* chp          = get_output_buffer();
	unsigned int pl             = get_parse_level();
	unsigned int sizeof_opcodes = get_sizeof_opcodes();
	OPCODE*     opcodes         = get_opcodes();
	
	if( (pl == 0xFF) || (!pl) )
		return;
	
	if(tp->mod1 == 'o' && tp->mod2 == 'o' && tp->mod3 == 'I') // A.K.A. mov reg, imm
	{
		unsigned char prefix = 0;
		unsigned char opc	= 0;
		unsigned char immediate32[8];
		unsigned char immediate16[4];
		unsigned char immediate8 [2];
		
		unsigned char          which_immediate = 0; // BIT(1) : 32, BIT(2) : 16, and BIT(3) : 8
		INSTRUCTION_ATTRIBUTE attribute        = OP_REG_IMM;
	
		int j;
		for(j=0;j<sizeof_opcodes;j++)
		{
			if( _strcmp(tp->str1, opcodes[j].mnemonic) )
			{
				if( attribute == opcodes[j].attribute )
				{
					opc = opcodes[j].base_binary_code;
					if( _strcmp(tp->str2, "eax") )
					{
						opc |= IMM_EAX;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "ecx") )
					{
						opc |= IMM_ECX;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "edx") )
					{
						opc |= IMM_EDX;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "ebx") )
					{
						opc |= IMM_EBX;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "esp") )
					{
						opc |= IMM_ESP;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "ebp") )
					{
						opc |= IMM_EBP;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "esi") )
					{
						opc |= IMM_ESI;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "edi") )
					{
						opc |= IMM_EDI;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "ax") )
					{
						prefix = 0x66;
						opc |= IMM_AX;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "bx") )
					{
						prefix = 0x66;
						opc |= IMM_BX;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "cx") )
					{
						prefix = 0x66;
						opc |= IMM_CX;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "dx") )
					{
						prefix = 0x66;
						opc |= IMM_DX;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "sp") )
					{
						prefix = 0x66;
						opc |= IMM_SP;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "bp") )
					{
						prefix = 0x66;
						opc |= IMM_BP;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "si") )
					{
						prefix = 0x66;
						opc |= IMM_SI;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "di") )
					{
						prefix = 0x66;
						opc |= IMM_DI;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "al") )
					{
						opc -= 8;
						opc |= IMM_AL;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "cl") )
					{
						opc -= 8;
						opc |= IMM_CL;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "dl") )
					{
						opc -= 8;
						opc |= IMM_DL;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "bl") )
					{
						opc -= 8;
						opc |= IMM_BL;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "ah") )
					{
						opc -= 8;
						opc |= IMM_AH;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "ch") )
					{
						opc -= 8;
						opc |= IMM_CH;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "dh") )
					{
						opc -= 8;
						opc |= IMM_DH;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "bh") )
					{
						opc -= 8;
						opc |= IMM_BH;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else {}
					
					if(prefix)
					{
						if( pl == PARSE_LEVEL_2 )
							printf("prefix: 0x%x, ", prefix);
						chp[*PC+0] = prefix;
						*PC = *PC + 1;
					}
					if( pl == PARSE_LEVEL_2 )
						printf("opcode: 0x%x, ", opc);
					chp[*PC+0] = opc;
					*PC = *PC + 1;
					
					if(which_immediate)
					{
						switch(which_immediate)
						{
							case 0x2: 
								if( pl == PARSE_LEVEL_2 )
									printf("imm: %c%c %c%c %c%c %c%c, ", 
									   immediate32[0], immediate32[1], immediate32[2], immediate32[3],
									   immediate32[4], immediate32[5], immediate32[6], immediate32[7]);
								chp[*PC+0] = byte_string_to_byte(immediate32[0], immediate32[1]);
								chp[*PC+1] = byte_string_to_byte(immediate32[2], immediate32[3]);
								chp[*PC+2] = byte_string_to_byte(immediate32[4], immediate32[5]);
								chp[*PC+3] = byte_string_to_byte(immediate32[6], immediate32[7]);
								*PC = *PC + 4;
								//immediate32_string_to_hex(tp->str3);
								break;
							case 0x4:
								if( pl == PARSE_LEVEL_2 )
									printf("imm: %c%c %c%c, ", 
									   immediate16[0], immediate16[1], immediate16[2], immediate16[3]);
								chp[*PC+0] = byte_string_to_byte(immediate16[0], immediate16[1]);
								chp[*PC+1] = byte_string_to_byte(immediate16[2], immediate16[3]);
								*PC = *PC + 2;
								break;
							case 0x8:
								if( pl == PARSE_LEVEL_2 )
									printf("imm: %c%c, ", immediate8[0], immediate8[1]);
								chp[*PC+0] = byte_string_to_byte(immediate8[0], immediate8[1]);
								*PC = *PC + 1;
								break;
						} // end of switch
					} // end of if
				} // end of if 
			} // end of if 
		} // end of for 
	} // end of if
	else if(tp->mod1 == 'o' && tp->mod2 == 'o' && tp->mod3 == 'M') // A.K.A. mov reg, mem
	{
		//printf("MEM\n");
		unsigned char prefix = 0;
		unsigned char opc	= 0;
		unsigned char modrm  = 0;
		unsigned char sib	= 0;
		unsigned char displacement32[8];
		unsigned char displacement16[4];
		unsigned char displacement8[2];
		unsigned char         which_displacement = 0; // BIT(1) : 32, BIT(2) : 16, and BIT(3) : 8
		INSTRUCTION_ATTRIBUTE attribute          = OP_REG_MEM;
	
		int j;
		for(j=0;j<sizeof_opcodes;j++)
		{
			if( _strcmp(tp->str1, opcodes[j].mnemonic) )
			{
				if( attribute == opcodes[j].attribute )
				{
					opc = opcodes[j].base_binary_code;
					if( _strcmp(tp->str2, "eax") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_EAX);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "ecx") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_ECX);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "edx") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_EDX);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "ebx") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_EBX);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "esp") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_ESP);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "ebp") )
					{opc |= _16_32;
						modrm |= MODRM_REG(IMM_EBP);
					 	modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "esi") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_ESI);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "edi") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_EDI);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "ax") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_AX);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "cx") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_CX);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "dx") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_DX);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "bx") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_BX);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "sp") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_SP);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "bp") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_BP);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "si") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_SI);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "di") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_DI);
						modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "al") )
					{
						modrm |= MODRM_REG(IMM_AL);
						//modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "cl") )
					{
						modrm |= MODRM_REG(IMM_CL);
						//modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "dl") )
					{
						modrm |= MODRM_REG(IMM_DL);
						//modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "bl") )
					{
						modrm |= MODRM_REG(IMM_BL);
						//modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "ah") )
					{
						modrm |= MODRM_REG(IMM_AH);
						//modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "ch") )
					{
						modrm |= MODRM_REG(IMM_CH);
						//modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "dh") )
					{
						modrm |= MODRM_REG(IMM_DH);
						//modrm |= MODRM_DISP32;
					}
					else if( _strcmp(tp->str2, "bh") )
					{
						modrm |= MODRM_REG(IMM_BH);
						//modrm |= MODRM_DISP32;
					}
					else {}
					
					if( _contain(tp->str3, "DWORD[") )
					{
						if( _contain(tp->str3, "eax") )
						{
							modrm |= IMM_EAX;
							modrm &= ~(MODRM_DISP32);
						}
						if( _contain(tp->str3, "ecx") )
						{
							modrm |= IMM_ECX;
							modrm &= ~(MODRM_DISP32);
						}
						if( _contain(tp->str3, "edx") )
						{
							modrm |= IMM_EDX;
							modrm &= ~(MODRM_DISP32);
						}
						if( _contain(tp->str3, "ebx") )
						{
							modrm |= IMM_EBX;
							modrm &= ~(MODRM_DISP32);
						}
						if( _contain(tp->str3, "esp") )
						{
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
							modrm &= ~(MODRM_DISP32);
						}
						if( _contain(tp->str3, "ebp") )
						{
							modrm |= (1<<7); // now the mode is [MEMORY+DISP8]
							modrm |= 0x05;   // now ebp selected
							//::::::::::::..modrm &= ~(MODRM_DISP32);
							displacement32[0] = '0';
							displacement32[1] = '0';
							displacement32[2] = '0';
							displacement32[3] = '0';
							displacement32[4] = '0';
							displacement32[5] = '0';
							displacement32[6] = '0';
							displacement32[7] = '0';
							which_displacement = (1<<1);
						}
						if( _contain(tp->str3, "esi") )
						{
							modrm |= IMM_ESI;
							modrm &= ~(MODRM_DISP32);
						}
						if( _contain(tp->str3, "edi") )
						{
							modrm |= IMM_EDI;
							modrm &= ~(MODRM_DISP32);
						}
						
						if( _contain(tp->str3, "+") ) // it has a displacement
						{
							// for now suppose only 32-bit displacement
							extract_from_memory_displacement32(tp->str3, displacement32);
							which_displacement = (1<<1);
							modrm |= (1<<7); // MEMORY+DISP32
						}
						else if( _contain(tp->str3, "0x") ) // a bare address
						{
							extract_from_memory_displacement_as_address(tp->str3, displacement32);
							which_displacement = (1<<1);
						}
						
						else if( _contain(tp->str3, ":") )// therefore there is a LABEL
						/* 
							it is like his mov eax, DWORD[a_label:]
							what we need here is the address of a_label:
							it is a DWORD
							therefore it must be handelled completely different here
						*/
						{
							opc = 0xB8; // OP_REG_IMM !
							if( _strcmp(tp->str2, "eax") )
							{
								opc |= IMM_EAX;
							}
							else if( _strcmp(tp->str2, "ecx") )
							{
								opc |= IMM_ECX;
							}
							else if( _strcmp(tp->str2, "edx") )
							{
								opc |= IMM_EDX;
							}
							else if( _strcmp(tp->str2, "ebx") )
							{
								opc |= IMM_EBX;
							}
							else if( _strcmp(tp->str2, "esp") )
							{
								opc |= IMM_ESP;
							}
							else if( _strcmp(tp->str2, "ebp") )
							{
								opc |= IMM_EBP;
							}
							else if( _strcmp(tp->str2, "esi") )
							{
								opc |= IMM_ESI;
							}
							else if( _strcmp(tp->str2, "edi") )
							{
								opc |= IMM_EDI;
							}
							char IMM32[8];
							extract_from_memory_displacement_as_address(tp->str3, IMM32);
							if( pl == PARSE_LEVEL_2 )
								printf("opcode: 0x%x, ", opc);
							chp[*PC+0] = opc;
							*PC = *PC + 1;
							if( pl == PARSE_LEVEL_2 )
								printf("label_address: %c%c %c%c %c%c %c%c\n", 
								   IMM32[0], IMM32[1], IMM32[2], IMM32[3],
								   IMM32[4], IMM32[5], IMM32[6], IMM32[7]);
							chp[*PC+0] = byte_string_to_byte(IMM32[0], IMM32[1]);
							chp[*PC+1] = byte_string_to_byte(IMM32[2], IMM32[3]);
							chp[*PC+2] = byte_string_to_byte(IMM32[4], IMM32[5]);
							chp[*PC+3] = byte_string_to_byte(IMM32[6], IMM32[7]);
							*PC = *PC + 4;
							return;
						}
					}
					
					else if( _contain(tp->str3, "WORD[") && (!_contain(tp->str3, "DWORD[")) )
					{
						prefix = 0x66;
						if( _contain(tp->str3, "eax") )
						{
							modrm |= IMM_EAX;
						}
						if( _contain(tp->str3, "ecx") )
						{
							modrm |= IMM_ECX;
						}
						if( _contain(tp->str3, "edx") )
						{
							modrm |= IMM_EDX;
						}
						if( _contain(tp->str3, "ebx") )
						{
							modrm |= IMM_EBX;
						}
						if( _contain(tp->str3, "esp") )
						{
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
						}
						if( _contain(tp->str3, "ebp") )
						{
							modrm |= (1<<7); // now the mode is [MEMORY+DISP32]
							modrm |= 0x05;   // now ebp selected
							displacement32[0] = '0';
							displacement32[1] = '0';
							displacement32[2] = '0';
							displacement32[3] = '0';
							displacement32[4] = '0';
							displacement32[5] = '0';
							displacement32[6] = '0';
							displacement32[7] = '0';
							which_displacement = (1<<1);
						}
						if( _contain(tp->str3, "esi") )
						{
							modrm |= IMM_ESI;
						}
						if( _contain(tp->str3, "edi") )
						{
							modrm |= IMM_EDI;
						}
						
						if( _contain(tp->str3, "+") ) // it has a displacement
						{
							// for now suppose only 32-bit displacement
							extract_from_memory_displacement32(tp->str3, displacement32);
							which_displacement = (1<<1);
							modrm |= (1<<7); // MEMORY+DISP32
						}
						else if( _contain(tp->str3, "0x") ) // a bare address
						{
							extract_from_memory_displacement_as_address(tp->str3, displacement32);
							which_displacement = (1<<1);
						}
						
						else if( _contain(tp->str3, ":") )// therefore there is a LABEL
						{
							modrm |= 0x05;
							extract_from_memory_displacement_as_address(tp->str3, displacement32);
							which_displacement = (1<<1);
						}
					}
					
					else if( _contain(tp->str3, "BYTE[") )
					{
						if( _contain(tp->str3, "eax") )
						{
							modrm |= IMM_EAX;
						}
						if( _contain(tp->str3, "ecx") )
						{
							modrm |= IMM_ECX;
						}
						if( _contain(tp->str3, "edx") )
						{
							modrm |= IMM_EDX;
						}
						if( _contain(tp->str3, "ebx") )
						{
							modrm |= IMM_EBX;
						}
						if( _contain(tp->str3, "esp") )
						{
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
						}
						if( _contain(tp->str3, "ebp") )
						{
							modrm |= (1<<7); // now the mode is [MEMORY+DISP32]
							modrm |= 0x05;   // now ebp selected
							displacement32[0] = '0';
							displacement32[1] = '0';
							displacement32[2] = '0';
							displacement32[3] = '0';
							displacement32[4] = '0';
							displacement32[5] = '0';
							displacement32[6] = '0';
							displacement32[7] = '0';
							which_displacement = (1<<1);
						}
						if( _contain(tp->str3, "esi") )
						{
							modrm |= IMM_ESI;
						}
						if( _contain(tp->str3, "edi") )
						{
							modrm |= IMM_EDI;
						}
						
						if( _contain(tp->str3, "+") ) // it has a displacement
						{
							// for now suppose only 32-bit displacement
							extract_from_memory_displacement32(tp->str3, displacement32);
							which_displacement = (1<<1);
							modrm |= (1<<7); // MEMORY+DISP32
						}
						else if( _contain(tp->str3, "0x") ) // a bare address
						{
							extract_from_memory_displacement_as_address(tp->str3, displacement32);
							which_displacement = (1<<1);
						}
						
						else if( _contain(tp->str3, ":") )// therefore there is a LABEL
						{
							modrm |= 0x05;
							extract_from_memory_displacement_as_address(tp->str3, displacement32);
							which_displacement = (1<<1);
						}
					}
					
					else {}
					
					
					if(prefix)
					{
						if( pl == PARSE_LEVEL_2 )
							printf("prefix: 0x%x, ", prefix);
						chp[*PC+0] = prefix;
						*PC = *PC + 1;
					}
					if( pl == PARSE_LEVEL_2 )
						printf("opcode: 0x%x, ", opc);
					chp[*PC+0] = opc;
					*PC = *PC + 1;

					if( pl == PARSE_LEVEL_2 )
						printf("modrm: 0x%x, ", modrm);
					chp[*PC+0] = modrm;
					*PC = *PC + 1;

					if(sib)
					{
						if( pl == PARSE_LEVEL_2 )
							printf("sib: 0x%x, ", sib);
						chp[*PC+0] = sib;
						*PC = *PC + 1;
					}
					switch(which_displacement)
					{
						case 0x1:
							if( pl == PARSE_LEVEL_2 )
								printf("\n");
							break;
						case 0x2: 
							if( pl == PARSE_LEVEL_2 )
								printf("displacement: %c%c %c%c %c%c %c%c, ", 
								   displacement32[0], displacement32[1], displacement32[2], displacement32[3],
								   displacement32[4], displacement32[5], displacement32[6], displacement32[7]);
							chp[*PC+0] = byte_string_to_byte(displacement32[0], displacement32[1]);
							chp[*PC+1] = byte_string_to_byte(displacement32[2], displacement32[3]);
							chp[*PC+2] = byte_string_to_byte(displacement32[4], displacement32[5]);
							chp[*PC+3] = byte_string_to_byte(displacement32[6], displacement32[7]);
							*PC = *PC + 4;
							break;
						case 0x4:
							if( pl == PARSE_LEVEL_2 )
								printf("displacement: %c%c %c%c, ", 
								   displacement16[0], displacement16[1], displacement16[2], displacement16[3]);
							chp[*PC+0] = byte_string_to_byte(displacement16[0], displacement16[1]);
							chp[*PC+1] = byte_string_to_byte(displacement16[2], displacement16[3]);
							*PC = *PC + 2;
							break;
						case 0x8:
							if( pl == PARSE_LEVEL_2 )
								printf("displacement: %c%c, ", displacement8[0], displacement8[1]);
							chp[*PC+0] = byte_string_to_byte(displacement8[0], displacement8[1]);
							*PC = *PC + 1;
							break;
					} // end of switch
				} // end of if 
			} // end of if 
		} // end of for
	} // end of if
	else if(tp->mod1 == 'o' && tp->mod2 == 'M' && tp->mod3 == 'o') // A.K.A. mov mem, reg
	{
		//printf("MEM\n");
		unsigned char prefix = 0;
		unsigned char opc	= 0;
		unsigned char modrm  = 0;
		unsigned char sib	= 0;
		unsigned char displacement32[8];
		unsigned char displacement16[8];
		unsigned char which_displacement  = 0;
		unsigned char displacement8[8];
		INSTRUCTION_ATTRIBUTE attribute   = OP_MEM_REG;
	
		int j;
		for(j=0;j<sizeof_opcodes;j++)
		{
			if( _strcmp(tp->str1, opcodes[j].mnemonic) )
			{
				if( attribute == opcodes[j].attribute )
				{
					opc = opcodes[j].base_binary_code;
					
					if( _strcmp(tp->str3, "eax") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_EAX);
					}
					else if( _strcmp(tp->str3, "ecx") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_ECX);
					}
					else if( _strcmp(tp->str3, "edx") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_EDX);
					}
					else if( _strcmp(tp->str3, "ebx") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_EBX);
					}
					else if( _strcmp(tp->str3, "esp") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_ESP);
					}
					else if( _strcmp(tp->str3, "ebp") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_EBP);
					}
					else if( _strcmp(tp->str3, "esi") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_ESI);
					}
					else if( _strcmp(tp->str3, "edi") )
					{
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_EDI);
					}
					else if( _strcmp(tp->str3, "ax") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_AX);
					}
					else if( _strcmp(tp->str3, "cx") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_CX);
					}
					else if( _strcmp(tp->str3, "dx") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_DX);
					}
					else if( _strcmp(tp->str3, "bx") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_BX);
					}
					else if( _strcmp(tp->str3, "sp") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_SP);
					}
					else if( _strcmp(tp->str3, "bp") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_BP);
					}
					else if( _strcmp(tp->str3, "si") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_SI);
					}
					else if( _strcmp(tp->str3, "di") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= MODRM_REG(IMM_DI);
					}
					else if( _strcmp(tp->str3, "al") )
					{
						modrm |= MODRM_REG(IMM_AL);
					}
					else if( _strcmp(tp->str3, "cl") )
					{
						modrm |= MODRM_REG(IMM_CL);
					}
					else if( _strcmp(tp->str3, "dl") )
					{
						modrm |= MODRM_REG(IMM_DL);
					}
					else if( _strcmp(tp->str3, "bl") )
					{
						modrm |= MODRM_REG(IMM_BL);
					}
					else if( _strcmp(tp->str3, "ah") )
					{
						modrm |= MODRM_REG(IMM_AH);
					}
					else if( _strcmp(tp->str3, "ch") )
					{
						modrm |= MODRM_REG(IMM_CH);
					}
					else if( _strcmp(tp->str3, "dh") )
					{
						modrm |= MODRM_REG(IMM_DH);
					}
					else if( _strcmp(tp->str3, "bh") )
					{
						modrm |= MODRM_REG(IMM_BH);
					}
					else {}

					if( _contain(tp->str2, "DWORD[") )
					{
						if( _contain(tp->str2, "eax") )
						{
							modrm |= IMM_EAX;
						}
						if( _contain(tp->str2, "ecx") )
						{
							modrm |= IMM_ECX;
						}
						if( _contain(tp->str2, "edx") )
						{
							modrm |= IMM_EDX;
						}
						if( _contain(tp->str2, "ebx") )
						{
							modrm |= IMM_EBX;
						}
						if( _contain(tp->str2, "esp") )
						{
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
						}
						if( _contain(tp->str2, "ebp") )
						{
							modrm |= (1<<7); // now the mode is [MEMORY+DISP8]
							modrm |= 0x05;   // now ebp selected
							displacement32[0] = '0';
							displacement32[1] = '0';
							displacement32[2] = '0';
							displacement32[3] = '0';
							displacement32[4] = '0';
							displacement32[5] = '0';
							displacement32[6] = '0';
							displacement32[7] = '0';
							which_displacement = (1<<1);
						}
						if( _contain(tp->str2, "esi") )
						{
							modrm |= IMM_ESI;
						}
						if( _contain(tp->str2, "edi") )
						{
							modrm |= IMM_EDI;
						}
						
						if( _contain(tp->str2, "+") ) // it has a displacement
						{
							// for now suppose only 32-bit displacement
							extract_from_memory_displacement32(tp->str2, displacement32);
							which_displacement = (1<<1);
							modrm |= (1<<7); // MEMORY+DISP32
						}
						else if( _contain(tp->str2, "0x") ) // a bare address
						{
							extract_from_memory_displacement_as_address(tp->str2, displacement32);
							which_displacement = (1<<1);
						}
						
						else if( _contain(tp->str2, ":") )// therefore there is a LABEL
						{
							modrm |= 0x05;
							extract_from_memory_displacement_as_address(tp->str2, displacement32);
							which_displacement = (1<<1);
						}
					}
					
					else if( _contain(tp->str2, "WORD[") && (!_contain(tp->str2, "DWORD[")) )
					{
						prefix = 0x66;
						if( _contain(tp->str2, "eax") )
						{
							modrm |= IMM_EAX;
						}
						if( _contain(tp->str2, "ecx") )
						{
							modrm |= IMM_ECX;
						}
						if( _contain(tp->str2, "edx") )
						{
							modrm |= IMM_EDX;
						}
						if( _contain(tp->str2, "ebx") )
						{
							modrm |= IMM_EBX;
						}
						if( _contain(tp->str2, "esp") )
						{
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
						}
						if( _contain(tp->str2, "ebp") )
						{
							modrm |= (1<<7); // now the mode is [MEMORY+DISP32]
							modrm |= 0x05;   // now ebp selected
							displacement32[0] = '0';
							displacement32[1] = '0';
							displacement32[2] = '0';
							displacement32[3] = '0';
							displacement32[4] = '0';
							displacement32[5] = '0';
							displacement32[6] = '0';
							displacement32[7] = '0';
							which_displacement = (1<<1);
						}
						if( _contain(tp->str2, "esi") )
						{
							modrm |= IMM_ESI;
						}
						if( _contain(tp->str2, "edi") )
						{
							modrm |= IMM_EDI;
						}
						
						if( _contain(tp->str2, "+") ) // it has a displacement
						{
							modrm |= (1<<7); // now the mode is [MEMORY+DISP32]
							
							// for now suppose only 8-bit displacement
							extract_from_memory_displacement32(tp->str2, displacement32);
							which_displacement = (1<<1);
						}
						else if( _contain(tp->str2, "0x") ) // a bare address
						{
							extract_from_memory_displacement_as_address(tp->str2, displacement32);
							which_displacement = (1<<1);
						}
						
						else if( _contain(tp->str2, ":") )// therefore there is a LABEL
						{
							modrm |= 0x05;
							extract_from_memory_displacement_as_address(tp->str2, displacement32);
							which_displacement = (1<<1);
						}
					}
					
					else if( _contain(tp->str2, "BYTE[") )
					{
						if( _contain(tp->str2, "eax") )
						{
							modrm |= IMM_EAX;
						}
						if( _contain(tp->str2, "ecx") )
						{
							modrm |= IMM_ECX;
						}
						if( _contain(tp->str2, "edx") )
						{
							modrm |= IMM_EDX;
						}
						if( _contain(tp->str2, "ebx") )
						{
							modrm |= IMM_EBX;
						}
						if( _contain(tp->str2, "esp") )
						{
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
						}
						if( _contain(tp->str2, "ebp") )
						{
							modrm |= (1<<7); // now the mode is [MEMORY+DISP32]
							modrm |= 0x05;   // now ebp selected
							displacement32[0] = '0';
							displacement32[1] = '0';
							displacement32[2] = '0';
							displacement32[3] = '0';
							displacement32[4] = '0';
							displacement32[5] = '0';
							displacement32[6] = '0';
							displacement32[7] = '0';
							which_displacement = (1<<1);
						}
						if( _contain(tp->str2, "esi") )
						{
							modrm |= IMM_ESI;
						}
						if( _contain(tp->str2, "edi") )
						{
							modrm |= IMM_EDI;
						}
						
						if( _contain(tp->str2, "+") ) // it has a displacement
						{
							modrm |= (1<<7); // now the mode is [MEMORY+DISP32]
							
							// for now suppose only 8-bit displacement
							extract_from_memory_displacement8(tp->str2, displacement32);
							which_displacement = (1<<1);
						}
						else if( _contain(tp->str2, "0x") ) // a bare address
						{
							extract_from_memory_displacement_as_address(tp->str2, displacement32);
							which_displacement = (1<<1);
						}
						
						else if( _contain(tp->str2, ":") )// therefore there is a LABEL
						{
							modrm |= 0x05;
							extract_from_memory_displacement_as_address(tp->str2, displacement32);
							which_displacement = (1<<1);
						}
					}
					
					else {}
					
					if( _contain(tp->str2, "+") ) // now it MUST have a register along
					{
						modrm &= 0x38; //00111000
						modrm |= (1<<7); // now with mode [MEMORY+DISP32]
						if( _contain(tp->str2, "eax") )
						{
							modrm |= IMM_EAX;
						}
						if( _contain(tp->str2, "ecx") )
						{
							modrm |= IMM_ECX;
						}
						if( _contain(tp->str2, "edx") )
						{
							modrm |= IMM_EDX;
						}
						if( _contain(tp->str2, "ebx") )
						{
							modrm |= IMM_EBX;
						}
						if( _contain(tp->str2, "esp") )
						{
							modrm &= ~(0x07);
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
						}
						if( _contain(tp->str2, "ebp") )
						{
							modrm |= IMM_EBP;
						}
						if( _contain(tp->str2, "esi") )
						{
							modrm |= IMM_ESI;
						}
						if( _contain(tp->str2, "edi") )
						{
							modrm |= IMM_EDI;
						}
						
						extract_from_memory_displacement32(tp->str2, displacement32);
						which_displacement = (1<<1);
					}
					
					else if( _contain(tp->str2, "0x") ) // a bare address
					{
						extract_from_memory_displacement_as_address(tp->str2, displacement32);
						which_displacement = (1<<1);
					}
					
					else if( _contain(tp->str2, ":") )// therefore there is a LABEL
					{
						modrm |= 0x05;
						extract_from_memory_displacement_as_address(tp->str2, displacement32);
						which_displacement = (1<<1);
					}
					
					if(prefix)
					{
						if( pl == PARSE_LEVEL_2 )
							printf("prefix: 0x%x, ", prefix);
						chp[*PC+0] = prefix;
						*PC = *PC + 1;
					}
					
					if( pl == PARSE_LEVEL_2 )
						printf("opcode: 0x%x, ", opc);
					chp[*PC+0] = opc;
					*PC = *PC + 1;
					
					if( pl == PARSE_LEVEL_2 )
						printf("modrm: 0x%x, ", modrm);
					chp[*PC+0] = modrm;
					*PC = *PC + 1;
					
					if(sib)
					{
						if( pl == PARSE_LEVEL_2 )
							printf("sib: 0x%x, ", sib);
						chp[*PC+0] = sib;
						*PC = *PC + 1;
					}
					switch(which_displacement)
					{
						case 0x1:
							printf("\n");
							break;
						case 0x2: 
							if( pl == PARSE_LEVEL_2 )
								printf("displacement: %c%c %c%c %c%c %c%c, ", 
								   displacement32[0], displacement32[1], displacement32[2], displacement32[3],
								   displacement32[4], displacement32[5], displacement32[6], displacement32[7]);
							chp[*PC+0] = byte_string_to_byte(displacement32[0], displacement32[1]);
							chp[*PC+1] = byte_string_to_byte(displacement32[2], displacement32[3]);
							chp[*PC+2] = byte_string_to_byte(displacement32[4], displacement32[5]);
							chp[*PC+3] = byte_string_to_byte(displacement32[6], displacement32[7]);
							*PC = *PC + 4;
							break;
						case 0x4:
							if( pl == PARSE_LEVEL_2 )
								printf("displacement: %c%c %c%c, ", 
								   displacement16[0], displacement16[1], displacement16[2], displacement16[3]);
							chp[*PC+0] = byte_string_to_byte(displacement16[0], displacement16[1]);
							chp[*PC+1] = byte_string_to_byte(displacement16[2], displacement16[3]);
							*PC = *PC + 2;
							break;
						case 0x8:
							if( pl == PARSE_LEVEL_2 )
								printf("displacement: %c%c, ", displacement8[0], displacement8[1]);
							chp[*PC+0] = byte_string_to_byte(displacement8[0], displacement8[1]);
							*PC = *PC + 1;
							break;
					} // end of switch
				} // end of if 
			} // end of if 
		} // end of for
	}
	else if(tp->mod1 == 'o' && tp->mod2 == 'M' && tp->mod3 == 'I') // A.K.A. mov mem, imm
	{
		//printf("MEM+IMM\n");
		unsigned char prefix = 0;
		unsigned char opc	= 0;
		unsigned char modrm  = 0;
		unsigned char sib	= 0;
		unsigned char immediate32[8];
		unsigned char immediate16[4];
		unsigned char immediate8 [2];
		unsigned char displacement32[8];
		unsigned char displacement16[4];
		unsigned char displacement8[2];
		
		unsigned char  which_immediate    = 0; // BIT(1) : 32, BIT(2) : 16, and BIT(3) : 8
		unsigned char  which_displacement = 0; // BIT(1) : 32, BIT(2) : 16, and BIT(3) : 8
		
		INSTRUCTION_ATTRIBUTE attribute   = OP_MEM_IMM;
	
		int j;
		for(j=0;j<sizeof_opcodes;j++)
		{
			if( _strcmp(tp->str1, opcodes[j].mnemonic) )
			{
				if( attribute == opcodes[j].attribute )
				{
					opc = opcodes[j].base_binary_code;
					if( _contain(tp->str2, "DWORD[") )
					{
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
						if( _contain(tp->str2, "eax") )
						{
							modrm |= IMM_EAX;
						}
						if( _contain(tp->str2, "ecx") )
						{
							modrm |= IMM_ECX;
						}
						if( _contain(tp->str2, "edx") )
						{
							modrm |= IMM_EDX;
						}
						if( _contain(tp->str2, "ebx") )
						{
							modrm |= IMM_EBX;
						}
						if( _contain(tp->str2, "esp") )
						{
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
						}
						if( _contain(tp->str2, "ebp") )
						{
							modrm |= (1<<7); // now the mode is [MEMORY+DISP32]
							modrm |= 0x05;   // now ebp selected
							displacement32[0] = '0';
							displacement32[1] = '0';
							displacement32[2] = '0';
							displacement32[3] = '0';
							displacement32[4] = '0';
							displacement32[5] = '0';
							displacement32[6] = '0';
							displacement32[7] = '0';
							which_displacement = (1<<1);
						}
						if( _contain(tp->str2, "esi") )
						{
							modrm |= IMM_ESI;
						}
						if( _contain(tp->str2, "edi") )
						{
							modrm |= IMM_EDI;
						}
						
						if( _contain(tp->str2, "+") ) // it has a displacement
						{
							extract_from_memory_displacement32(tp->str2, displacement32);
							which_displacement = (1<<1);
						}
					}
					
					else if( _contain(tp->str2, "WORD[") && (!_contain(tp->str2, "DWORD[")) )
					{
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
						prefix = 0x66;
						if( _contain(tp->str2, "eax") )
						{
							modrm |= IMM_EAX;
						}
						if( _contain(tp->str2, "ecx") )
						{
							modrm |= IMM_ECX;
						}
						if( _contain(tp->str2, "edx") )
						{
							modrm |= IMM_EDX;
						}
						if( _contain(tp->str2, "ebx") )
						{
							modrm |= IMM_EBX;
						}
						if( _contain(tp->str2, "esp") )
						{
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
						}
						if( _contain(tp->str2, "ebp") )
						{
							modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
							modrm |= 0x05;   // now ebp selected
							displacement8[0] = '0';
							displacement8[1] = '0';
							which_displacement = (1<<3);
						}
						if( _contain(tp->str2, "esi") )
						{
							modrm |= IMM_ESI;
						}
						if( _contain(tp->str2, "edi") )
						{
							modrm |= IMM_EDI;
						}
						
						if( _contain(tp->str2, "+") ) // it has a displacement
						{
							modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
							
							// for now suppose only 8-bit displacement
							extract_from_memory_displacement8(tp->str2, displacement8);
							which_displacement = (1<<3);
						}
					}
					
					else if( _contain(tp->str2, "BYTE[") )
					{
						which_immediate = (1<<3);
						opc &= ~(1<<0);
						encode_u8(tp->str3, immediate8);
						if( _contain(tp->str2, "eax") )
						{
							modrm |= IMM_EAX;
						}
						if( _contain(tp->str2, "ecx") )
						{
							modrm |= IMM_ECX;
						}
						if( _contain(tp->str2, "edx") )
						{
							modrm |= IMM_EDX;
						}
						if( _contain(tp->str2, "ebx") )
						{
							modrm |= IMM_EBX;
						}
						if( _contain(tp->str2, "esp") )
						{
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
						}
						if( _contain(tp->str2, "ebp") )
						{
							modrm |= (1<<7); // now the mode is [MEMORY+DISP32]
							modrm |= 0x05;   // now ebp selected
							displacement32[0] = '0';
							displacement32[1] = '0';
							displacement32[2] = '0';
							displacement32[3] = '0';
							displacement32[4] = '0';
							displacement32[5] = '0';
							displacement32[6] = '0';
							displacement32[7] = '0';
							which_displacement = (1<<1);
						}
						if( _contain(tp->str2, "esi") )
						{
							modrm |= IMM_ESI;
						}
						if( _contain(tp->str2, "edi") )
						{
							modrm |= IMM_EDI;
						}
						
						if( _contain(tp->str2, "+") ) // it has a displacement
						{
							modrm |= (1<<7); // now the mode is [MEMORY+DISP32]
							
							// for now suppose only 8-bit displacement
							extract_from_memory_displacement32(tp->str2, displacement32);
							which_displacement = (1<<1);
						}
					}
					
					else {}
					
					if(prefix)
					{
						if( pl == PARSE_LEVEL_2 )
							printf("prefix: 0x%x, ", prefix);
						chp[*PC+0] = prefix;
						*PC = *PC + 1;
					}
					if( pl == PARSE_LEVEL_2 )
						printf("opcode: 0x%x, ", opc);
					chp[*PC+0] = opc;
					*PC = *PC + 1;
					
					if( pl == PARSE_LEVEL_2 )
						printf("modrm: 0x%x, ", modrm);
					chp[*PC+0] = modrm;
					*PC = *PC + 1;
					
					if(sib)
					{
						if( pl == PARSE_LEVEL_2 )
							printf("sib: 0x%x, ", sib);
						chp[*PC+0] = sib;
						*PC = *PC + 1;
					}
					switch(which_displacement)
					{
						case 0x2: 
							if( pl == PARSE_LEVEL_2 )
								printf("displacement: %c%c %c%c %c%c %c%c, ", 
								   displacement32[0], displacement32[1], displacement32[2], displacement32[3],
								   displacement32[4], displacement32[5], displacement32[6], displacement32[7]);
							chp[*PC+0] = byte_string_to_byte(displacement32[0], displacement32[1]);
							chp[*PC+1] = byte_string_to_byte(displacement32[2], displacement32[3]);
							chp[*PC+2] = byte_string_to_byte(displacement32[4], displacement32[5]);
							chp[*PC+3] = byte_string_to_byte(displacement32[6], displacement32[7]);
							*PC = *PC + 4;
							break;
						case 0x4:
							if( pl == PARSE_LEVEL_2 )
								printf("displacement: %c%c %c%c, ", 
								   displacement16[0], displacement16[1], displacement16[2], displacement16[3]);
							chp[*PC+0] = byte_string_to_byte(displacement16[0], displacement16[1]);
							chp[*PC+1] = byte_string_to_byte(displacement16[2], displacement16[3]);
							*PC = *PC + 2;
							break;
						case 0x8:
							if( pl == PARSE_LEVEL_2 )
								printf("displacement: %c%c, ", displacement8[0], displacement8[1]);
							chp[*PC+0] = byte_string_to_byte(displacement8[0], displacement8[1]);
							*PC = *PC + 1;
							break;
					} // end of switch
					
					if(which_immediate)
					{
						switch(which_immediate)
						{
							case 0x2: 
								if( pl == PARSE_LEVEL_2 )
									printf("imm: %c%c %c%c %c%c %c%c, ", 
									   immediate32[0], immediate32[1], immediate32[2], immediate32[3],
									   immediate32[4], immediate32[5], immediate32[6], immediate32[7]);
								chp[*PC+0] = byte_string_to_byte(immediate32[0], immediate32[1]);
								chp[*PC+1] = byte_string_to_byte(immediate32[2], immediate32[3]);
								chp[*PC+2] = byte_string_to_byte(immediate32[4], immediate32[5]);
								chp[*PC+3] = byte_string_to_byte(immediate32[6], immediate32[7]);
								*PC = *PC + 4;
							
								//immediate32_string_to_hex(tp->str3);
								break;
							case 0x4:
								if( pl == PARSE_LEVEL_2 )
									printf("imm: %c%c %c%c, ", 
									   immediate16[0], immediate16[1], immediate16[2], immediate16[3]);
								chp[*PC] = byte_string_to_byte(immediate16[0], immediate16[1]);
								chp[*PC+1] = byte_string_to_byte(immediate16[2], immediate16[3]);
								*PC = *PC + 2;
								break;
							case 0x8:
								if( pl == PARSE_LEVEL_2 )
									printf("imm: %c%c, ", immediate8[0], immediate8[1]);
								chp[*PC] = byte_string_to_byte(immediate8[0], immediate8[1]);
								*PC = *PC + 1;
								break;
						} // end of switch
					} // end of if
				} // end of if 
			} // end of if 
		} // end of for 
	}
	else if(tp->mod1 == 'o' && tp->mod2 == 'o' && tp->mod3 == 'o') // A.K.A. mov reg, reg
	{
		unsigned char         prefix    = 0;
		unsigned char         opc       = 0;
		unsigned char         modrm     = 0;
		INSTRUCTION_ATTRIBUTE attribute = OP_REG_REG;
	
		int j;
		for(j=0;j<sizeof_opcodes;j++)
		{
			if( _strcmp(tp->str1, opcodes[j].mnemonic) )
			{
				if( attribute == opcodes[j].attribute )
				{
					opc = opcodes[j].base_binary_code;
					modrm |= ((1<<7) | (1<<6)); //register
					if( _strcmp(tp->str2, "eax") )
					{
						modrm |= IMM_EAX;
					}
					else if( _strcmp(tp->str2, "ecx") )
					{
						modrm |= IMM_ECX;
					}
					else if( _strcmp(tp->str2, "edx") )
					{
						modrm |= IMM_EDX;
					}
					else if( _strcmp(tp->str2, "ebx") )
					{
						modrm |= IMM_EBX;
					}
					else if( _strcmp(tp->str2, "esp") )
					{
						modrm |= IMM_ESP;
					}
					else if( _strcmp(tp->str2, "ebp") )
					{
						modrm |= IMM_EBP;
					}
					else if( _strcmp(tp->str2, "esi") )
					{
						modrm |= IMM_ESI;
					}
					else if( _strcmp(tp->str2, "edi") )
					{
						modrm |= IMM_EDI;
					}
					else if( _strcmp(tp->str2, "ax") )
					{
						prefix = 0x66;
						modrm |= IMM_AX;
					}
					else if( _strcmp(tp->str2, "bx") )
					{
						prefix = 0x66;
						modrm |= IMM_BX;
					}
					else if( _strcmp(tp->str2, "cx") )
					{
						prefix = 0x66;
						modrm |= IMM_CX;
					}
					else if( _strcmp(tp->str2, "dx") )
					{
						prefix = 0x66;
						modrm |= IMM_DX;
					}
					else if( _strcmp(tp->str2, "sp") )
					{
						prefix = 0x66;
						modrm |= IMM_SP;
					}
					else if( _strcmp(tp->str2, "bp") )
					{
						prefix = 0x66;
						modrm |= IMM_BP;
					}
					else if( _strcmp(tp->str2, "si") )
					{
						prefix = 0x66;
						modrm |= IMM_SI;
					}
					else if( _strcmp(tp->str2, "di") )
					{
						prefix = 0x66;
						modrm |= IMM_DI;
					}
					else if( _strcmp(tp->str2, "al") )
					{
						opc &= ~(1<<0);
						modrm |= IMM_AL;
					}
					else if( _strcmp(tp->str2, "cl") )
					{
						opc &= ~(1<<0);
						modrm |= IMM_CL;
					}
					else if( _strcmp(tp->str2, "dl") )
					{
						opc &= ~(1<<0);
						modrm |= IMM_DL;
					}
					else if( _strcmp(tp->str2, "bl") )
					{
						opc &= ~(1<<0);
						modrm |= IMM_BL;
					}
					else if( _strcmp(tp->str2, "ah") )
					{
						opc &= ~(1<<0);
						modrm |= IMM_AH;
					}
					else if( _strcmp(tp->str2, "ch") )
					{
						opc &= ~(1<<0);
						modrm |= IMM_CH;
					}
					else if( _strcmp(tp->str2, "dh") )
					{
						opc &= ~(1<<0);
						modrm |= IMM_DH;
					}
					else if( _strcmp(tp->str2, "bh") )
					{
						opc &= ~(1<<0);
						modrm |= IMM_BH;
					}
					else {}
					
					if( _strcmp(tp->str3, "eax") )
					{
						modrm |= MODRM_REG(IMM_EAX);
					}
					else if( _strcmp(tp->str3, "ecx") )
					{
						modrm |= MODRM_REG(IMM_ECX);
					}
					else if( _strcmp(tp->str3, "edx") )
					{
						modrm |= MODRM_REG(IMM_EDX);
					}
					else if( _strcmp(tp->str3, "ebx") )
					{
						modrm |= MODRM_REG(IMM_EBX);
					}
					else if( _strcmp(tp->str3, "esp") )
					{
						modrm |= MODRM_REG(IMM_ESP);
					}
					else if( _strcmp(tp->str3, "ebp") )
					{
						modrm |= MODRM_REG(IMM_EBP);
					}
					else if( _strcmp(tp->str3, "esi") )
					{
						modrm |= MODRM_REG(IMM_ESI);
					}
					else if( _strcmp(tp->str3, "edi") )
					{
						modrm |= MODRM_REG(IMM_EDI);
					}
					else if( _strcmp(tp->str3, "ax") )
					{
						prefix = 0x66;
						modrm |= MODRM_REG(IMM_AX);
					}
					else if( _strcmp(tp->str3, "bx") )
					{
						prefix = 0x66;
						modrm |= MODRM_REG(IMM_BX);
					}
					else if( _strcmp(tp->str3, "cx") )
					{
						prefix = 0x66;
						modrm |= MODRM_REG(IMM_CX);
					}
					else if( _strcmp(tp->str3, "dx") )
					{
						prefix = 0x66;
						modrm |= MODRM_REG(IMM_DX);
					}
					else if( _strcmp(tp->str3, "sp") )
					{
						prefix = 0x66;
						modrm |= MODRM_REG(IMM_SP);
					}
					else if( _strcmp(tp->str3, "bp") )
					{
						prefix = 0x66;
						modrm |= MODRM_REG(IMM_BP);
					}
					else if( _strcmp(tp->str3, "si") )
					{
						prefix = 0x66;
						modrm |= MODRM_REG(IMM_SI);
					}
					else if( _strcmp(tp->str3, "di") )
					{
						prefix = 0x66;
						modrm |= MODRM_REG(IMM_DI);
					}
					else if( _strcmp(tp->str3, "al") )
					{
						modrm |= MODRM_REG(IMM_AL);
					}
					else if( _strcmp(tp->str3, "cl") )
					{
						modrm |= MODRM_REG(IMM_CL);
					}
					else if( _strcmp(tp->str3, "dl") )
					{
						modrm |= MODRM_REG(IMM_DL);
					}
					else if( _strcmp(tp->str3, "bl") )
					{
						modrm |= MODRM_REG(IMM_BL);
					}
					else if( _strcmp(tp->str3, "ah") )
					{
						modrm |= MODRM_REG(IMM_AH);
					}
					else if( _strcmp(tp->str3, "ch") )
					{
						modrm |= MODRM_REG(IMM_CH);
					}
					else if( _strcmp(tp->str3, "dh") )
					{
						modrm |= MODRM_REG(IMM_DH);
					}
					else if( _strcmp(tp->str3, "bh") )
					{
						modrm |= MODRM_REG(IMM_BH);
					}
					else {}
					
					if(prefix)
					{
						if( pl == PARSE_LEVEL_2 )
							printf("prefix: 0x%x, ", prefix);
						chp[*PC] = prefix;
						*PC = *PC + 1;
					}
					if( pl == PARSE_LEVEL_2 )
						printf("opcode: 0x%x, ", opc);
					chp[*PC] = opc;
					*PC = *PC + 1;
					
					if( pl == PARSE_LEVEL_2 )
						printf("modrm: 0x%x, ", modrm);
					chp[*PC] = modrm;
					*PC = *PC + 1;
					
				} // end of if 
			} // end of if 
		} // end of for 
	} // end of if
	if( pl == PARSE_LEVEL_2 )
		printf("\n");
}

//....................................................................................................................................