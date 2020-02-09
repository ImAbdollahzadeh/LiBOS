#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

int main(void)
{
	printf("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n\n");
	unsigned int lines = 0;
	unsigned int j;
	const char* file = 
		"[BITS 32]\n"
		"[ORG 0x9400]\n"
		"section .text\n"
		"global _start\n"
		"global vid_pl\n"
		"_start:\n"
		"\tpush ebp\n"
		"\tmov ebp, esp\n"
		"\tmov  WORD[memory_libos], cx\n"
		"\tmov dh,  BYTE[memory_libos]\n"
		"\tmov ebp, DWORD[0xA0AB9856]\n"
		"\tmov dh,  BYTE[ebp]\n"
		"\tmov dx,  WORD[memory_libos]\n"
		"\tmov edi, 0xAABBCCDD\n"
		"\tmov bx, 0x45FF\n"
		"\tmov ch, 0x7E\n"
		"\tadd edx, 0xDDCCBB56\n"
		"\tmov BYTE[esp+0x28], 0x16\n"
		"\tmov WORD[ebp+0x00124585], esp\n"
		"\tmov esp, ebp\n"
		"\tadd esp, ebp\n"
		"\tpop ebp\n"
		"\tret\n";
	printf("%u lines\n", lines = how_many_lines(file));
	TRIPLE_PACKET* tp = alloc_units(lines);
	
	char p[128];
	char*  f = file;
	unsigned int counter = 0;
	for(j=0; j<lines; j++)
	{
		while(*f != '\n')
		{
			p[counter] = *f++;
			counter++;
		}
		p[counter] = '\0';
		lex(&tp[j], p);
		counter = 0;
		f++;
	}
	
	//printf(file);
	
	convert_instructions_line_by_line(tp, lines);

	//dump_table_of_labels();
	
	printf("\n\n\n-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
}

