#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

void convert_sub_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	if( get_parse_level() == PARSE_LEVEL_2 )
		printf("SUB DECODING: ");
	
	unsigned char* chp = get_output_buffer();
	unsigned int pl    = get_parse_level();
	if( (pl == 0xFF) || (!pl) )
		return;
	
	unsigned int sizeof_opcodes = get_sizeof_opcodes();
	OPCODE* opcodes = get_opcodes();
	
	if(tp->mod1 == 'o' && tp->mod2 == 'o' && tp->mod3 == 'I') // A.K.A: sub, reg, imm 
	{
		unsigned char prefix = 0;
		unsigned char opc	= 0;
		unsigned int  modrm  = 0;
		unsigned char immediate32[8];
		unsigned char immediate16[4];
		unsigned char immediate8 [2];
		
		unsigned char         which_immediate = 0; // BIT(1) : 32, BIT(2) : 16, and BIT(3) : 8
		INSTRUCTION_ATTRIBUTE attribute       = OP_REG_IMM;
	
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
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_EAX;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "ecx") )
					{
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_ECX;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "edx") )
					{
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_EDX;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "ebx") )
					{
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_EBX;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "esp") )
					{
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_ESP;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "ebp") )
					{
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_EBP;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "esi") )
					{
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_ESI;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "edi") )
					{
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_EDI;
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
					}
					else if( _strcmp(tp->str2, "ax") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_AX;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "bx") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_BX;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "cx") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_CX;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "dx") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_DX;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "sp") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_SP;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "bp") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_BP;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "si") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_SI;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "di") )
					{
						prefix = 0x66;
						opc |= _16_32;
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_DI;
						which_immediate |= (1<<2);
						encode_u16(tp->str3, immediate16);
					}
					else if( _strcmp(tp->str2, "al") )
					{
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_AL;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "cl") )
					{
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_CL;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "dl") )
					{
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_DL;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "bl") )
					{
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_BL;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "ah") )
					{
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_AH;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "ch") )
					{
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_CH;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "dh") )
					{
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_DH;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else if( _strcmp(tp->str2, "bh") )
					{
						modrm |= ((1<<7) | (1<<6)); // register
						modrm |= IMM_BH;
						which_immediate |= (1<<3);
						encode_u8(tp->str3, immediate8);
					}
					else {}
					
					// This is the only part which makes SUB different than ADD
					modrm |= (5 << 3);
					
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
	else if(tp->mod1 == 'o' && tp->mod2 == 'o' && tp->mod3 == 'M') // A.K.A: sub, reg, mem
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
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_EAX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "ecx") )
					{
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_ECX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "edx") )
					{
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_EDX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "ebx") )
					{
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_EBX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "esp") )
					{
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_ESP) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "ebp") )
					{
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_EBP) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "esi") )
					{
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_ESI) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "edi") )
					{
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_EDI) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "ax") )
					{
						prefix = 0x66;
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_AX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "cx") )
					{
						prefix = 0x66;
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_CX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "dx") )
					{
						prefix = 0x66;
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_DX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "bx") )
					{
						prefix = 0x66;
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_BX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "sp") )
					{
						prefix = 0x66;
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_SP) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "bp") )
					{
						prefix = 0x66;
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_BP) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "si") )
					{
						prefix = 0x66;
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_SI) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "di") )
					{
						prefix = 0x66;
						opc |= (MEM_TO_REG | _16_32);
						modrm |= (MODRM_REG(IMM_DI) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "al") )
					{
						opc &= _8;
						opc |= MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_AL) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "cl") )
					{
						opc &= _8;
						opc |= MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_CL) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "dl") )
					{
						opc &= _8;
						opc |= MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_CL) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "bl") )
					{
						opc &= _8;
						opc |= MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_BL) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "ah") )
					{
						opc &= _8;
						opc |= MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_AH) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "ch") )
					{
						opc &= _8;
						opc |= MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_CH) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "dh") )
					{
						opc &= _8;
						opc |= MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_DH) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str2, "bh") )
					{
						opc &= _8;
						opc |= MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_BH) | MODRM_DISP32);
					}
					else {}
					
					if( _contain(tp->str3, "+") ) // now it MUST have a register along
					{
						modrm &= 0x38; //00111000
						modrm |= (1<<7); // now with mode [MEMORY+DISP32]
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
							modrm &= ~(0x07);
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
							modrm |= IMM_EBP;
						}
						if( _contain(tp->str3, "esi") )
						{
							modrm |= IMM_ESI;
						}
						if( _contain(tp->str3, "edi") )
						{
							modrm |= IMM_EDI;
						}
						
						extract_from_memory_displacement32(tp->str3, displacement32);
						which_displacement = (1<<1);
					}
					
					else if( _contain(tp->str3, "0x") ) // a bare address
					{
						extract_from_memory_displacement_as_address(tp->str3, displacement32);
						which_displacement = (1<<1);
					}
					
					else // there is only REG or a LABEL
					{
						modrm &= 0x38; //00111000. Now with mode [MEMORY]
						
						if( _contain(tp->str3, "eax") )
						{
							modrm |= IMM_EAX;
							goto EXIT_POSITION;
						}
						if( _contain(tp->str3, "ecx") )
						{
							modrm |= IMM_ECX;
							goto EXIT_POSITION;
						}
						if( _contain(tp->str3, "edx") )
						{
							modrm |= IMM_EDX;
							goto EXIT_POSITION;
						}
						if( _contain(tp->str3, "ebx") )
						{
							modrm |= IMM_EBX;
							goto EXIT_POSITION;
						}
						if( _contain(tp->str3, "esp") )
						{
							modrm &= ~(0x07);
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
							goto EXIT_POSITION;
						}
						if( _contain(tp->str3, "ebp") )
						{
							modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
							modrm |= 0x05;   // now ebp selected
							displacement8[0] = '0';
							displacement8[1] = '0';
							which_displacement = (1<<3);
							goto EXIT_POSITION;
						}
						if( _contain(tp->str3, "esi") )
						{
							modrm |= IMM_ESI;
							goto EXIT_POSITION;
						}
						if( _contain(tp->str3, "edi") )
						{
							modrm |= IMM_EDI;
							goto EXIT_POSITION;
						}
						
						// therefore there is a LABEL
						modrm |= 0x05;
						extract_from_memory_displacement_as_address(tp->str3, displacement32);
						which_displacement = (1<<1);
					}
EXIT_POSITION:
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
	else if(tp->mod1 == 'o' && tp->mod2 == 'M' && tp->mod3 == 'o') // A.K.A: sub, mem, reg
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
		INSTRUCTION_ATTRIBUTE attribute = OP_MEM_REG;

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
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_EAX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "ecx") )
					{
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_ECX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "edx") )
					{
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_EDX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "ebx") )
					{
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_EBX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "esp") )
					{
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_ESP) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "ebp") )
					{
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_EBP) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "esi") )
					{
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_ESI) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "edi") )
					{
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_EDI) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "ax") )
					{
						prefix = 0x66;
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_AX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "cx") )
					{
						prefix = 0x66;
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_CX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "dx") )
					{
						prefix = 0x66;
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_DX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "bx") )
					{
						prefix = 0x66;
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_BX) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "sp") )
					{
						prefix = 0x66;
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_SP) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "bp") )
					{
						prefix = 0x66;
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_BP) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "si") )
					{
						prefix = 0x66;
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_SI) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "di") )
					{
						prefix = 0x66;
						opc |= _16_32;
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_DI) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "al") )
					{
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_AL) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "cl") )
					{
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_CL) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "dl") )
					{
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_CL) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "bl") )
					{
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_BL) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "ah") )
					{
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_AH) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "ch") )
					{
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_CH) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "dh") )
					{
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_DH) | MODRM_DISP32);
					}
					else if( _strcmp(tp->str3, "bh") )
					{
						opc &= ~MEM_TO_REG;
						modrm |= (MODRM_REG(IMM_BH) | MODRM_DISP32);
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
					
					else // there is only REG r a LABEL
					{
						modrm &= 0x38; //00111000. Now with mode [MEMORY]
						
						if( _contain(tp->str2, "eax") )
						{
							modrm |= IMM_EAX;
							goto EXIT_POSITION2;
						}
						if( _contain(tp->str2, "ecx") )
						{
							modrm |= IMM_ECX;
							goto EXIT_POSITION2;
						}
						if( _contain(tp->str2, "edx") )
						{
							modrm |= IMM_EDX;
							goto EXIT_POSITION2;
						}
						if( _contain(tp->str2, "ebx") )
						{
							modrm |= IMM_EBX;
							goto EXIT_POSITION2;
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
							goto EXIT_POSITION2;
						}
						if( _contain(tp->str2, "ebp") )
						{
							modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
							modrm |= 0x05;   // now ebp selected
							displacement8[0] = '0';
							displacement8[1] = '0';
							which_displacement = (1<<3);
							goto EXIT_POSITION2;
						}
						if( _contain(tp->str2, "esi") )
						{
							modrm |= IMM_ESI;
							goto EXIT_POSITION2;
						}
						if( _contain(tp->str2, "edi") )
						{
							modrm |= IMM_EDI;
							goto EXIT_POSITION2;
						}
						
						// therefore there is a LABEL
						modrm |= 0x05;
						extract_from_memory_displacement_as_address(tp->str2, displacement32);
						which_displacement = (1<<1);
					}
EXIT_POSITION2:
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
	}
	else if(tp->mod1 == 'o' && tp->mod2 == 'M' && tp->mod3 == 'I') // A.K.A: sub, mem, imm
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
					opc |= _16_32;
					modrm |= ((0<<7) | (0<<6)); // memory
					
					if( _contain(tp->str2, "DWORD[") )
					{
						which_immediate |= (1<<1);
						encode_u32(tp->str3, immediate32);
						if( _contain(tp->str2, "eax") )
						{
							modrm |= IMM_EAX;
						}
						else if( _contain(tp->str2, "ecx") )
						{
							modrm |= IMM_ECX;
						}
						else if( _contain(tp->str2, "edx") )
						{
							modrm |= IMM_EDX;
						}
						else if( _contain(tp->str2, "ebx") )
						{
							modrm |= IMM_EBX;
						}
						else if( _contain(tp->str2, "esp") )
						{
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
						}
						else if( _contain(tp->str2, "ebp") )
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
						else if( _contain(tp->str2, "esi") )
						{
							modrm |= IMM_ESI;
						}
						else if( _contain(tp->str2, "edi") )
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
						
						// therefore there is a LABEL
						else if( _contain(tp->str2, ":") )
						{
							modrm |= 0x05;
							extract_from_memory_displacement_as_address(tp->str2, displacement32);
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
						else if( _contain(tp->str2, "ecx") )
						{
							modrm |= IMM_ECX;
						}
						else if( _contain(tp->str2, "edx") )
						{
							modrm |= IMM_EDX;
						}
						else if( _contain(tp->str2, "ebx") )
						{
							modrm |= IMM_EBX;
						}
						else if( _contain(tp->str2, "esp") )
						{
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
						}
						else if( _contain(tp->str2, "ebp") )
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
						else if( _contain(tp->str2, "esi") )
						{
							modrm |= IMM_ESI;
						}
						else if( _contain(tp->str2, "edi") )
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
						
						// therefore there is a LABEL
						else if( _contain(tp->str2, ":") )
						{
							modrm |= 0x05;
							extract_from_memory_displacement_as_address(tp->str2, displacement32);
							which_displacement = (1<<1);
						}
					}
					
					else if( _contain(tp->str2, "BYTE[") )
					{
						which_immediate |= (1<<3);
						opc &= ~(1<<0);
						encode_u8(tp->str3, immediate8);
						if( _contain(tp->str2, "eax") )
						{
							modrm |= IMM_EAX;
						}
						else if( _contain(tp->str2, "ecx") )
						{
							modrm |= IMM_ECX;
						}
						else if( _contain(tp->str2, "edx") )
						{
							modrm |= IMM_EDX;
						}
						else if( _contain(tp->str2, "ebx") )
						{
							modrm |= IMM_EBX;
						}
						else if( _contain(tp->str2, "esp") )
						{
							modrm |= 0x04;   // now SIB selected
							
							// SIB fields:
							// scale  index  base
							// --   | ---  | ---
							// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
							// for us: [ESP] -> [0+ESP*1]
							sib   |= ((IMM_ESP << 3) | IMM_ESP);
						}
						else if( _contain(tp->str2, "ebp") )
						{
							modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
							modrm |= 0x05;   // now ebp selected
							displacement8[0] = '0';
							displacement8[1] = '0';
							which_displacement = (1<<3);
						}
						else if( _contain(tp->str2, "esi") )
						{
							modrm |= IMM_ESI;
						}
						else if( _contain(tp->str2, "edi") )
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
						
						// therefore there is a LABEL
						else if( _contain(tp->str2, ":") )
						{
							modrm |= 0x05;
							extract_from_memory_displacement_as_address(tp->str2, displacement32);
							which_displacement = (1<<1);
						}
					}
					
					else {}
					
					// This is the only part which makes SUB different than ADD
					modrm |= (5 << 3);
					
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
	}
	else if(tp->mod1 == 'o' && tp->mod2 == 'o' && tp->mod3 == 'o') // A.K.A: sub, reg, reg
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
						opc   |= _16_32;
						modrm |= IMM_EAX;
					}
					else if( _strcmp(tp->str2, "ecx") )
					{
						opc   |= _16_32;
						modrm |= IMM_ECX;
					}
					else if( _strcmp(tp->str2, "edx") )
					{
						opc   |= _16_32;
						modrm |= IMM_EDX;
					}
					else if( _strcmp(tp->str2, "ebx") )
					{
						opc   |= _16_32;
						modrm |= IMM_EBX;
					}
					else if( _strcmp(tp->str2, "esp") )
					{
						opc   |= _16_32;
						modrm |= IMM_ESP;
					}
					else if( _strcmp(tp->str2, "ebp") )
					{
						opc   |= _16_32;
						modrm |= IMM_EBP;
					}
					else if( _strcmp(tp->str2, "esi") )
					{
						opc   |= _16_32;
						modrm |= IMM_ESI;
					}
					else if( _strcmp(tp->str2, "edi") )
					{
						opc   |= _16_32;
						modrm |= IMM_EDI;
					}
					else if( _strcmp(tp->str2, "ax") )
					{
						opc   |= _16_32;
						prefix = 0x66;
						modrm |= IMM_AX;
					}
					else if( _strcmp(tp->str2, "bx") )
					{
						opc   |= _16_32;
						prefix = 0x66;
						modrm |= IMM_BX;
					}
					else if( _strcmp(tp->str2, "cx") )
					{
						opc   |= _16_32;
						prefix = 0x66;
						modrm |= IMM_CX;
					}
					else if( _strcmp(tp->str2, "dx") )
					{
						opc   |= _16_32;
						prefix = 0x66;
						modrm |= IMM_DX;
					}
					else if( _strcmp(tp->str2, "sp") )
					{
						opc   |= _16_32;
						prefix = 0x66;
						modrm |= IMM_SP;
					}
					else if( _strcmp(tp->str2, "bp") )
					{
						opc   |= _16_32;
						prefix = 0x66;
						modrm |= IMM_BP;
					}
					else if( _strcmp(tp->str2, "si") )
					{
						opc   |= _16_32;
						prefix = 0x66;
						modrm |= IMM_SI;
					}
					else if( _strcmp(tp->str2, "di") )
					{
						opc   |= _16_32;
						prefix = 0x66;
						modrm |= IMM_DI;
					}
					else if( _strcmp(tp->str2, "al") )
					{
						modrm |= IMM_AL;
					}
					else if( _strcmp(tp->str2, "cl") )
					{
						modrm |= IMM_CL;
					}
					else if( _strcmp(tp->str2, "dl") )
					{
						modrm |= IMM_DL;
					}
					else if( _strcmp(tp->str2, "bl") )
					{
						modrm |= IMM_BL;
					}
					else if( _strcmp(tp->str2, "ah") )
					{
						modrm |= IMM_AH;
					}
					else if( _strcmp(tp->str2, "ch") )
					{
						modrm |= IMM_CH;
					}
					else if( _strcmp(tp->str2, "dh") )
					{
						modrm |= IMM_DH;
					}
					else if( _strcmp(tp->str2, "bh") )
					{
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
						*PC = *PC + 1;
					}
					
					if( pl == PARSE_LEVEL_2 )
						printf("opcode: 0x%x, ", opc);
					*PC = *PC + 1;
					
					if( pl == PARSE_LEVEL_2 )
						printf("modrm: 0x%x, ", modrm);
					*PC = *PC + 1;
				} // end of if 
			} // end of if 
		} // end of for 
	} // end of if
	if( pl == PARSE_LEVEL_2 )
		printf("\n");
}

//....................................................................................................................................