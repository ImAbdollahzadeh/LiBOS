#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

void convert_cmp_instruction(TRIPLE_PACKET* tp, unsigned int* PC)
{
	printf("CMP DECODING: ");
	
	unsigned int pl = get_parse_level();
	if( (pl == 0xFF) || (!pl) )
		return;
}

//....................................................................................................................................