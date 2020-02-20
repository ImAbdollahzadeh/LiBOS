#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

/* we can have only:
int 3
int imm8
*/
void convert_int_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	if( get_parse_level() == PARSE_LEVEL_2 )
		printf("INT DECODING\n\t");
	
	unsigned char* chp            = get_output_buffer();
	unsigned int   pl             = get_parse_level();
	unsigned int   sizeof_opcodes = get_sizeof_opcodes();
	OPCODE*        opcodes        = get_opcodes();
	
	if( (pl == 0xFF) || (!pl) )
		return;

	if(tp->mod1 == 'o' && tp->mod2 == 'I') // A.K.A. int imm8
	{
		unsigned char         opc       = 0;
		unsigned char         immediate8[2];
		INSTRUCTION_ATTRIBUTE attribute = OP_IMM;
	
		int j;
		for(j=0;j<sizeof_opcodes;j++)
		{
			if( _strcmp(tp->str1, opcodes[j].mnemonic) )
			{
				if( attribute == opcodes[j].attribute )
				{
					if( _strcmp(tp->str2, "0x03") )
						opc = 0xCC;
					else 
						opc = opcodes[j].base_binary_code;
					
					encode_u8(tp->str2, immediate8);
					
					if( pl == PARSE_LEVEL_2 )
						printf("opcode: 0x%x, ", opc);
					chp[*PC] = opc;
					*PC = *PC + 1;
					
					if( _strcmp(tp->str2, "0x03") )
						goto EXIT_FROM_INT;

					if( pl == PARSE_LEVEL_2 )
						printf("imm: %c%c\n", immediate8[0], immediate8[1]);
					chp[*PC] = byte_string_to_byte(immediate8[0], immediate8[1]);
					*PC = *PC + 1;
				}
			}
		}
	}
EXIT_FROM_INT:
	if( pl == PARSE_LEVEL_2 )
		printf("\n");
}

//....................................................................................................................................