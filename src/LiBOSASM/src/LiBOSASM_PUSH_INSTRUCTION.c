#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

/* we can have only:
push ecx
push cx
push imm32
*/
void convert_push_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	if( get_parse_level() == PARSE_LEVEL_2 )
		printf("PUSH DECODING: ");
	
	unsigned char* chp            = get_output_buffer();
	unsigned int   pl             = get_parse_level();
	unsigned int   sizeof_opcodes = get_sizeof_opcodes();
	OPCODE*        opcodes        = get_opcodes();
	
	if( (pl == 0xFF) || (!pl) )
		return;
	
	if(tp->mod1 == 'o' && tp->mod2 == 'o') // A.K.A. push reg
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
	
	else if(tp->mod1 == 'o' && tp->mod2 == 'I') // A.K.A. push imm32
	{
		unsigned char opc = 0;
		unsigned char immediate32[8];
		
		unsigned char          which_immediate = (1<<1); // BIT(1) : 32, BIT(2) : 16, and BIT(3) : 8
		INSTRUCTION_ATTRIBUTE  attribute       = OP_IMM;
	
		int j;
		for(j=0;j<sizeof_opcodes;j++)
		{
			if( _strcmp(tp->str1, opcodes[j].mnemonic) )
			{
				if( attribute == opcodes[j].attribute )
				{
					opc = opcodes[j].base_binary_code;
					encode_u32(tp->str2, immediate32);

					if( pl == PARSE_LEVEL_2 )
						printf("opcode: 0x%x, ", opc);
					chp[*PC+0] = opc;
					*PC = *PC + 1;
					if( pl == PARSE_LEVEL_2 )
						printf("imm: %c%c %c%c %c%c %c%c, ", 
							immediate32[0], immediate32[1], immediate32[2], immediate32[3],
							immediate32[4], immediate32[5], immediate32[6], immediate32[7]);
					chp[*PC+0] = byte_string_to_byte(immediate32[0], immediate32[1]);
					chp[*PC+1] = byte_string_to_byte(immediate32[2], immediate32[3]);
					chp[*PC+2] = byte_string_to_byte(immediate32[4], immediate32[5]);
					chp[*PC+3] = byte_string_to_byte(immediate32[6], immediate32[7]);
					*PC = *PC + 4;
				}
			}
		}
	}
	if( pl == PARSE_LEVEL_2 )
		printf("\n");
}

//....................................................................................................................................