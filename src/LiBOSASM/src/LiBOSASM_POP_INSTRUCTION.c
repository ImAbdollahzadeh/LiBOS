#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

/* we can have only:
pop ecx
pop cx
pop imm32
*/
void convert_pop_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	if( get_parse_level() == PARSE_LEVEL_2 )
		printf("POP DECODING: ");
	
	unsigned char* chp            = get_output_buffer();
	unsigned int   pl             = get_parse_level();
	unsigned int   sizeof_opcodes = get_sizeof_opcodes();
	OPCODE*        opcodes        = get_opcodes();
	
	if( (pl == 0xFF) || (!pl) )
		return;
	
	if(tp->mod1 == 'o' && tp->mod2 == 'o') // A.K.A. pop reg
	{
		unsigned char         prefix    = 0;
		unsigned char         opc       = 0;
		INSTRUCTION_ATTRIBUTE attribute = OP_REG;
	
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
					else if( _strcmp(tp->str2, "ax") )
					{
						prefix = 0x66;
						opc |= IMM_AX;
					}
					else if( _strcmp(tp->str2, "bx") )
					{
						prefix = 0x66;
						opc |= IMM_BX;
					}
					else if( _strcmp(tp->str2, "cx") )
					{
						prefix = 0x66;
						opc |= IMM_CX;
					}
					else if( _strcmp(tp->str2, "dx") )
					{
						prefix = 0x66;
						opc |= IMM_DX;
					}
					else if( _strcmp(tp->str2, "sp") )
					{
						prefix = 0x66;
						opc |= IMM_SP;
					}
					else if( _strcmp(tp->str2, "bp") )
					{
						prefix = 0x66;
						opc |= IMM_BP;
					}
					else if( _strcmp(tp->str2, "si") )
					{
						prefix = 0x66;
						opc |= IMM_SI;
					}
					else if( _strcmp(tp->str2, "di") )
					{
						prefix = 0x66;
						opc |= IMM_DI;
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
				}
			}
		}
	}
	
	else if(tp->mod1 == 'o' && tp->mod2 == 'M') // A.K.A. push mem16/32
	{
		unsigned char prefix = 0;
		unsigned char opc	= 0;
		unsigned char modrm  = 0;
		unsigned char sib	= 0;
		unsigned char displacement32[8];
		unsigned char displacement16[4];
		unsigned char displacement8[2];
		unsigned char which_displacement = 0; // BIT(1) : 32, BIT(2) : 16, and BIT(3) : 8
		INSTRUCTION_ATTRIBUTE attribute = OP_MEM;
	
		int j;
		for(j=0;j<sizeof_opcodes;j++)
		{
			if( _strcmp(tp->str1, opcodes[j].mnemonic) )
			{
				if( attribute == opcodes[j].attribute )
				{
					opc = opcodes[j].base_binary_code;
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
							goto EXIT_POSITION;
						}
						if( _contain(tp->str2, "ecx") )
						{
							modrm |= IMM_ECX;
							goto EXIT_POSITION;
						}
						if( _contain(tp->str2, "edx") )
						{
							modrm |= IMM_EDX;
							goto EXIT_POSITION;
						}
						if( _contain(tp->str2, "ebx") )
						{
							modrm |= IMM_EBX;
							goto EXIT_POSITION;
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
							goto EXIT_POSITION;
						}
						if( _contain(tp->str2, "ebp") )
						{
							modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
							modrm |= 0x05;   // now ebp selected
							displacement8[0] = '0';
							displacement8[1] = '0';
							which_displacement = (1<<3);
							goto EXIT_POSITION;
						}
						if( _contain(tp->str2, "esi") )
						{
							modrm |= IMM_ESI;
							goto EXIT_POSITION;
						}
						if( _contain(tp->str2, "edi") )
						{
							modrm |= IMM_EDI;
							goto EXIT_POSITION;
						}
						
						// therefore there is a LABEL
						modrm |= 0x05;
						extract_from_memory_displacement_as_address(tp->str2, displacement32);
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
			}
		}
	}
	if( pl == PARSE_LEVEL_2 )
		printf("\n");
}

//....................................................................................................................................