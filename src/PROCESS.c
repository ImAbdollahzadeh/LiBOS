#include "../include/PROCESS.h"
#include "../include/PRINT.h"
#include "../include/MEMORY.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

PAGE_DIRECTORY* create_address_space(void)
{
	PAGE_DIRECTORY* dir = (PAGE_DIRECTORY*)alloc_physical_block();
	if (!dir)
		return 0;
	__LiBOS_MemZero(dir, sizeof(PAGE_DIRECTORY));
	return dir;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+