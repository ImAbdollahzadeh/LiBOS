#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

void convert_jmp_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	if( get_parse_level() == PARSE_LEVEL_2 )
		printf("JMP DECODING: ");
	
	unsigned int pl = get_parse_level();
	if( (pl == 0xFF) || (!pl) )
		return;
	
	unsigned char* chp = get_output_buffer();
	
	unsigned int i;
	unsigned char displacement32[8];
	unsigned char which_displacement = 0; // BIT(1) : 32, BIT(2) : 16, and BIT(3) : 8
	unsigned short code_segment16[4] = {'0','0','0','0'};
	unsigned char segment_active = 0; //TRUE = active, FALSE = disactive
	unsigned char opc = 0;
	unsigned char modrm = 0;
	unsigned char sib = 0;
	char* src = tp->str2;
	
	unsigned int    table_of_labels_count = get_table_of_labels_count();
	SYMBOLIC_LABEL* table_of_labels       = get_table_of_labels();
	
	for(i=0; i< table_of_labels_count;i++)
	{
		if( _strcmp(src, table_of_labels[i].string) )
		{
			//address = table_of_labels[i].address;
			//opc = 0x11;
			*PC += 5;
			goto CONVERT_JMP_END;
		}
	}

	if( _strcmp(src, "eax") )
	{
		opc = 0xFF;
		modrm |= ( (3 << 6) | (4 << 3) | IMM_EAX );
		chp[*PC+0] = opc;
		chp[*PC+1] = modrm;
		*PC = *PC + 2;
		goto CONVERT_JMP_END;
	}
	else if( _strcmp(src, "ecx") )
	{
		opc = 0xFF;
		modrm |= ( (3 << 6) | (4 << 3) | IMM_ECX );
		chp[*PC+0] = opc;
		chp[*PC+1] = modrm;
		*PC = *PC + 2;
		goto CONVERT_JMP_END;
	}
	else if( _strcmp(src, "edx") )
	{
		opc = 0xFF;
		modrm |= ( (3 << 6) | (4 << 3) | IMM_EDX );
		chp[*PC+0] = opc;
		chp[*PC+1] = modrm;
		*PC = *PC + 2;
		goto CONVERT_JMP_END;
	}
	else if( _strcmp(src, "ebx") )
	{
		opc = 0xFF;
		modrm |= ( (3 << 6) | (4 << 3) | IMM_EBX );
		chp[*PC+0] = opc;
		chp[*PC+1] = modrm;
		*PC = *PC + 2;
		goto CONVERT_JMP_END;
	}
	else if( _strcmp(src, "esp") )
	{
		opc = 0xFF;
		modrm |= ( (3 << 6) | (4 << 3) | IMM_ESP );
		chp[*PC+0] = opc;
		chp[*PC+1] = modrm;
		*PC = *PC + 2;
		goto CONVERT_JMP_END;
	}
	else if( _strcmp(src, "ebp") )
	{
		opc = 0xFF;
		modrm |= ( (3 << 6) | (4 << 3) | IMM_EBP );
		chp[*PC+0] = opc;
		chp[*PC+1] = modrm;
		*PC = *PC + 2;
		goto CONVERT_JMP_END;
	}
	else if( _strcmp(src, "esi") )
	{
		opc = 0xFF;
		modrm |= ( (3 << 6) | (4 << 3) | IMM_ESI );
		chp[*PC+0] = opc;
		chp[*PC+1] = modrm;
		*PC = *PC + 2;
		goto CONVERT_JMP_END;
	}
	else if( _strcmp(src, "edi") )
	{
		opc = 0xFF;
		modrm |= ( (3 << 6) | (4 << 3) | IMM_EDI );
		chp[*PC+0] = opc;
		chp[*PC+1] = modrm;
		*PC = *PC + 2;
		goto CONVERT_JMP_END;
	}

	if( _contain(src, "BYTE[") )
	{
		printf("cannot jump to less than 32-bit address EROR\n");
		goto CONVERT_JMP_END;
	}
	else if( _contain(src, "WORD[") && (!_contain(src, "DWORD[")) )
	{
		printf("cannot jump to less than 32-bit address EROR\n");
		goto CONVERT_JMP_END;
	}
	else if( _contain(src, "DWORD[") )
	{
		if( _contain(src, "eax") )
		{
			opc = 0xFF;
			modrm |= ( (4 << 3) | IMM_EAX );
			chp[*PC+0] = opc;
			chp[*PC+1] = modrm;
			*PC = *PC + 2;
		}
		else if( _contain(src, "ecx") )
		{
			opc = 0xFF;
			modrm |= ( (4 << 3) | IMM_ECX );
			chp[*PC+0] = opc;
			chp[*PC+1] = modrm;
			*PC = *PC + 2;
		}
		else if( _contain(src, "edx") )
		{
			opc = 0xFF;
			modrm |= ( (4 << 3) | IMM_EDX );
			chp[*PC+0] = opc;
			chp[*PC+1] = modrm;
			*PC = *PC + 2;
		}
		else if( _contain(src, "ebx") )
		{
			opc = 0xFF;
			modrm |= ( (4 << 3) | IMM_EBX );
			chp[*PC+0] = opc;
			chp[*PC+1] = modrm;
		}
		else if( _contain(src, "esp") )
		{
			opc = 0xFF;
			modrm |= ( (4 << 3) | IMM_ESP );
			modrm &= ~(0x07);
			modrm |= 0x04;   // now SIB selected
			
			// SIB fields:
			// scale  index  base
			// --   | ---  | ---
			// [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
			// for us: [ESP] -> [0+ESP*1]
			sib   |= ((IMM_ESP << 3) | IMM_ESP);
			chp[*PC+0] = opc;
			chp[*PC+1] = modrm;
			chp[*PC+2] = sib;
			*PC = *PC + 2;
		}
		else if( _contain(src, "ebp") )
		{
			opc = 0xFF;
			modrm |= ( (1 << 7) | (4 << 3) | IMM_EBP ); // [MEMORY+DISP32]
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
			chp[*PC+0] = opc;
			chp[*PC+1] = modrm;
			*PC = *PC + 2;
		}
		else if( _contain(src, "esi") )
		{
			opc = 0xFF;
			modrm |= ( (4 << 3) | IMM_ESI );
			chp[*PC+0] = opc;
			chp[*PC+1] = modrm;
			*PC = *PC + 2;
		}
		else if( _contain(src, "edi") )
		{
			opc = 0xFF;
			modrm |= ( (4 << 3) | IMM_EDI );
			chp[*PC+0] = opc;
			chp[*PC+1] = modrm;
			*PC = *PC + 2;
		}
		
		if( _contain(src, "+") ) // displacement (only 32 bit hexadecimal numbers are accepted)
		{
			modrm |= (1<<7); // now the mode is [MEMORY+DISP32]
			extract_from_memory_displacement32(src, displacement32);
			which_displacement = (1<<1);
		}
		else if( _contain(src, ":") ) // a label
		/* THE JMP TO A LABEL IS ALWAYS GIVEN IN FORM:
		   jmp DWORD[a_label:] */
		{
			opc = 0xFF;
			modrm |= (0x05 | (4 << 3) );
			extract_from_memory_displacement_as_address(src, displacement32);
			which_displacement = (1<<1);
		}
	}
	
	else if( _contain(src, "0x") ) // a hard-coded hexadecimal value
	/* THIS IS VERY SPECIAL JUMP INSTRUCTION: IT JUMPS IN AN ADDRESS OFFSET FROM CODE SEGMENT VALUE.
	   THEREFORE THE GIVEN HEX VALUE MUST BE - 5 (SINCE SIZE OF THE CURRENT INSTRUCTION IS 5 BYTES)
	   EX.
	   jmp 0xFF0000A0
	   encoded as: EA A0 00 00 FF 00 00 ~> jmp 0x00:0xFF0000A0 
	*/
	{
		opc = 0xEA; // jump far to pre16:ptr32
		modrm = 0;
		chp[*PC+0] = opc;
		*PC = *PC + 1;
		encode_u32(src, displacement32);
		which_displacement = (1<<1);
		segment_active     = 1;
	}
	
	else {}
	
CONVERT_JMP_END:
	if( pl == PARSE_LEVEL_2 )
		printf("opcode: 0x%x, ", opc);
	
	if(modrm)
	{
		if( pl == PARSE_LEVEL_2 )
		printf("modrm: 0x%x, ", modrm);
	}
	if(sib)
	{
		if( pl == PARSE_LEVEL_2 )
			printf("sib: 0x%x, ", sib);
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
		case 0x4: break;
		case 0x8: break;
	}
	if(segment_active)
	{
		if( pl == PARSE_LEVEL_2 )
			printf("segment: %c%c %c%c, ", code_segment16[0], code_segment16[1], code_segment16[2], code_segment16[3]);
		chp[*PC+0] = 0;
		chp[*PC+1] = 0;
		*PC = *PC + 2;
	}
	
	if( pl == PARSE_LEVEL_2 )
			printf("\n");
}

//....................................................................................................................................

void convert_jne_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("JNE DECODING: ");
	
	unsigned int pl = get_parse_level();
	if( (pl == 0xFF) || (!pl) )
		return;
}

//....................................................................................................................................

void convert_je_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("JE DECODING: ");
	
	unsigned int pl = get_parse_level();
	if( (pl == 0xFF) || (!pl) )
		return;
}

//....................................................................................................................................

void convert_jnz_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("JNZ DECODING: ");
	
	unsigned int pl = get_parse_level();
	if( (pl == 0xFF) || (!pl) )
		return;
}

//....................................................................................................................................

void convert_jz_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("JZ DECODING: ");
	
	unsigned int pl = get_parse_level();
	if( (pl == 0xFF) || (!pl) )
		return;
}

//....................................................................................................................................