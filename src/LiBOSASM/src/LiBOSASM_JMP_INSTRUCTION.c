#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

void convert_jmp_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("JMP DECODING\n\t");
	
	unsigned int pl = get_parse_level();
	if( (pl == 0xFF) || (!pl) )
		return;
	
	unsigned int i;
	unsigned int address = 0x00000000;
	unsigned int found = 0;
	unsigned char opc = 0;
	char* src = tp->str2;
	
	unsigned int    table_of_labels_count = get_table_of_labels_count();
	SYMBOLIC_LABEL* table_of_labels       = get_table_of_labels();
	
	for(i=0; i< table_of_labels_count;i++)
	{
		if( _strcmp(src, table_of_labels[i].string) )
		{
			address = table_of_labels[i].address;
			//opc = 0x11;
			*PC += 5;
			goto CONVERT_JMP_END;
		}
	}

	if( _contain(src, "0x") )
	{
		address = immediate32_string_to_hex(src);
		//opc = 0x22;
		*PC += 5;
		goto CONVERT_JMP_END;
	}

	if( _contain(src, "eax") )
	{
		// opc = 0x33;
		*PC += 2;
		goto CONVERT_JMP_END;
	}
	else if( _contain(src, "ecx") )
	{
		// opc = 0x33;
		*PC += 2;
		goto CONVERT_JMP_END;
	}
	else if( _contain(src, "edx") )
	{
		// opc = 0x33;
		*PC += 2;
		goto CONVERT_JMP_END;
	}
	else if( _contain(src, "ebx") )
	{
		// opc = 0x33;
		*PC += 2;
		goto CONVERT_JMP_END;
	}
	else if( _contain(src, "esp") )
	{
		// opc = 0x33;
		*PC += 2;
		goto CONVERT_JMP_END;
	}
	else if( _contain(src, "ebp") )
	{
		// opc = 0x33;
		*PC += 2;
		goto CONVERT_JMP_END;
	}
	else if( _contain(src, "esi") )
	{
		// opc = 0x33;
		*PC += 2;
		goto CONVERT_JMP_END;
	}
	else if( _contain(src, "edi") )
	{
		// opc = 0x33;
		*PC += 2;
		goto CONVERT_JMP_END;
	}

CONVERT_JMP_END:
	printf("opcode: 0x%x, ", opc);
	printf("address: 0x%x\n", address);
}

//....................................................................................................................................