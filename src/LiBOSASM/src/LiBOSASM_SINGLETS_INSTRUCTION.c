#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

void ret_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("ret handler\n");
	*PC = *PC + 1;
}

void cld_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("cld handler\n");
	*PC = *PC + 1;
}

void std_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("std handler\n");
	*PC = *PC + 1;
}

void nop_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("nop handler\n");
	*PC = *PC + 1;
}

void hlt_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("hlt handler\n");
	*PC = *PC + 1;
}

void sti_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("sti handler\n");
	*PC = *PC + 1;
}

void cli_handler(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("cli handler\n");
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
	printf("SINGLETS DECODING %u\n\t", id);
	
	unsigned int pl = get_parse_level();
	if( (pl == 0xFF) || (!pl) )
		return;
	
	table_of_singlet_instructions[extract_handler_from_id(id)](tp, PC);
}

//....................................................................................................................................