#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

void ret_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	unsigned char*        chp       = get_output_buffer();
	unsigned int          pl        = get_parse_level();
	INSTRUCTION_ATTRIBUTE attribute = OP;
	unsigned char         opc       = 0xC3;
	if( pl == PARSE_LEVEL_2 )
		printf("ret handler: ");
	if( pl == PARSE_LEVEL_2 )
		printf("opcode: 0x%x\n", opc);
	chp[*PC] = opc;
	*PC = *PC + 1;
}

void cld_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	unsigned char*        chp       = get_output_buffer();
	unsigned int          pl        = get_parse_level();
	INSTRUCTION_ATTRIBUTE attribute = OP;
	unsigned char         opc       = 0xFC;
	if( pl == PARSE_LEVEL_2 )
		printf("cld handler: ");
	if( pl == PARSE_LEVEL_2 )
		printf("opcode: 0x%x\n", opc);
	chp[*PC] = opc;
	*PC = *PC + 1;
}

void std_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	unsigned char*        chp       = get_output_buffer();
	unsigned int          pl        = get_parse_level();
	INSTRUCTION_ATTRIBUTE attribute = OP;
	unsigned char         opc       = 0xFD;
	if( pl == PARSE_LEVEL_2 )
		printf("std handler: ");
	if( pl == PARSE_LEVEL_2 )
		printf("opcode: 0x%x\n", opc);
	chp[*PC] = opc;
	*PC = *PC + 1;
}

void nop_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	unsigned char*        chp       = get_output_buffer();
	unsigned int          pl        = get_parse_level();
	INSTRUCTION_ATTRIBUTE attribute = OP;
	unsigned char         opc       = 0x90;
	if( pl == PARSE_LEVEL_2 )
		printf("nop handler: ");
	if( pl == PARSE_LEVEL_2 )
		printf("opcode: 0x%x\n", opc);
	chp[*PC] = opc;
	*PC = *PC + 1;
}

void hlt_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	unsigned char*        chp       = get_output_buffer();
	unsigned int          pl        = get_parse_level();
	INSTRUCTION_ATTRIBUTE attribute = OP;
	unsigned char         opc       = 0xF4;
	if( pl == PARSE_LEVEL_2 )
		printf("hlt handler: ");
	if( pl == PARSE_LEVEL_2 )
		printf("opcode: 0x%x\n", opc);
	chp[*PC] = opc;
	*PC = *PC + 1;
}

void sti_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	unsigned char*        chp       = get_output_buffer();
	unsigned int          pl        = get_parse_level();
	INSTRUCTION_ATTRIBUTE attribute = OP;
	unsigned char         opc       = 0xFB;
	if( pl == PARSE_LEVEL_2 )
		printf("sti handler: ");
	if( pl == PARSE_LEVEL_2 )
		printf("opcode: 0x%x\n", opc);
	chp[*PC] = opc;
	*PC = *PC + 1;
}

void cli_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	unsigned char*        chp       = get_output_buffer();
	unsigned int          pl        = get_parse_level();
	INSTRUCTION_ATTRIBUTE attribute = OP;
	unsigned char         opc       = 0xFA;
	if( pl == PARSE_LEVEL_2 )
		printf("cli handler: ");
	if( pl == PARSE_LEVEL_2 )
		printf("opcode: 0x%x\n", opc);
	chp[*PC] = opc;
	*PC = *PC + 1;
}

//....................................................................................................................................

singlet_instructions_handler table_of_singlet_instructions[] = {
	&ret_handler,
	&cld_handler,
	&std_handler,
	&nop_handler,
	&hlt_handler,
	&sti_handler,
	&cli_handler,
};

//....................................................................................................................................

unsigned int extract_handler_from_id(unsigned int id)
{
	unsigned int i = 0;
	while( !(id & (1<<i)) )
	{
		i++;
		if(i == 32)
			return 0xFFFFFFFF;
	}
	return i;
}

//....................................................................................................................................

void convert_singlets_instruction(TRIPLE_PACKET* tp, unsigned int id, unsigned int* PC)
{
	unsigned int pl = get_parse_level();
	if( (pl == 0xFF) || (!pl) )
		return;
	
	if( pl == PARSE_LEVEL_2 )
		printf("SINGLETS DECODING %u: ", id);
	
	table_of_singlet_instructions[extract_handler_from_id(id)](tp, PC);
}

//....................................................................................................................................