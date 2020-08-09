#include "../include/I_ASM.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

static void dump_line_number(void)
{
	__LiBOS_HexDump((void*)0x0DC9B000, 4, "Lines");
}

static void dump_TP_mods(void)
{
	unsigned int l = *(UINT_32*)((void*)0x0DC9B000), i;
	void* tp_mod = ((void*)(0x0D819000 + 384));
	for(i=0; i<l + 1; i++)
	{
		__LiBOS_HexDump(tp_mod, 4, 0);
		tp_mod = (void*)((unsigned int)tp_mod + 387);
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void enter_I_ASM(UINT_8* file_string)
{
	__LiBOS_MemZero((void*)0x01000000, 400*1024*1024);
	UINT_8*  IRA            = (UINT_8*) 0x01000000;
	void*    ISBA_TPs       = (void*)   0x0D819000;
	UINT_32* ISBA_LINE_PTR  = (UINT_32*)0x0DC9B000;
	UINT_8*  how_many_lines = (UINT_8*) 0x0DC82000;
	UINT_8*  initialize_TPs = (UINT_8*) 0x0DC82400;
	
	UINT_8   how_many_lines_code[44] = {
		0x55, 0x89, 0xe5, 0x8b, 0x75, 0x08,
		0x31, 0xc9, 0x31, 0xdb, 0x8a, 0x1e,
		0x80, 0xfb, 0x00, 0x74, 0x12, 0x80, 
		0xfb, 0x0a, 0x74, 0x05, 0x83, 0xc6, 
		0x01, 0xeb, 0xef, 0x83, 0xc1, 0x01, 
		0x83, 0xc6, 0x01, 0xeb, 0xe7, 0x8b, 
		0x5d, 0x0c, 0x89, 0x0b, 0x89, 0xec, 
		0x5d, 0xc3, 
	};
	UINT_8   initialize_TPs_code[44] = {
		0x55, 0x89, 0xe5, 0x8b, 0x45, 0x08,
		0x8b, 0x4d, 0x0c, 0x05, 0x80, 0x01,
		0x00, 0x00, 0xc6, 0x00, 0x21, 0x83,
		0xc0, 0x01, 0xc6, 0x00, 0x21, 0x83,
		0xc0, 0x01, 0xc6, 0x00, 0x21, 0x83,
		0xc0, 0x01, 0x83, 0xe9, 0x01, 0x83,
		0xf9, 0x00, 0x75, 0xe1, 0x89, 0xec, 
		0x5d, 0xc3, 
	};
	unsigned int i;
	for(i=0; i<44; i++)
	{
		how_many_lines[i] = how_many_lines_code[i];
		initialize_TPs[i] = initialize_TPs_code[i];
	}
	
	INT_8* IRAp = IRA;
	while(*file_string)
	{
		if(*file_string == '\t')
			*file_string = '-';
		else if(*file_string == '\n')
			*file_string = 0x0A;
		*IRAp = *file_string;
		IRAp++;
		file_string++;
	}
	*IRAp = 0x00;

	execute_IRA();
	dump_line_number();
	//dump_TP_mods();
	
	printk("I_ASM returned successfully\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
