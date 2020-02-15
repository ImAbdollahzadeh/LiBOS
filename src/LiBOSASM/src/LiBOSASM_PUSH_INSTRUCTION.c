#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

void convert_push_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	if( get_parse_level() == PARSE_LEVEL_2 )
		printf("PUSH DECODING\n\t");
	
	unsigned int pl = get_parse_level();
	if( (pl == 0xFF) || (!pl) )
		return;
}

//....................................................................................................................................