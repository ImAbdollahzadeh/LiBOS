#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

int main(void)
{
	printf("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	unsigned int lines = 0;
	unsigned int j;
	TRIPLE_PACKET* tp = 0;
	char p[128];
	
	const char* file = 
		"[LiBOSASM 32-bit]\n"
		"[ORIGIN 0x00009400]\n"
		"[SECTION .CODE]\n"
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
		"\tjmp _THIRD_AND_LAST@@_LABEL:\n"
		"\tmov esp, ebp\n"
		"__SECOND_LABEL:\n"
		"\tjmp __SECOND_LABEL:\n"
		"\tadd esp, ebp\n"
		"\tpop ebp\n"
		"_THIRD_AND_LAST@@_LABEL:\n"
		"\tret\n"
		"[SECTION .DATA]\n"
		"string_to_be_printed: db ""ImanAbdollahzadehLiBOS"", 0x0A, 0x00\n"
		"_rgb@@mask_black: dw 0x00000000\n";
	
	parse_0(file, &tp, &lines, p);
	
	printf(file);
	
	parse_1_or__convert_instructions_line_by_line(tp, lines);
	dump_table_of_labels();
	zero_programCounter();
	printf("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	
	IMAGE_FILE_MEMORY ifm;
	parse_2(tp, lines);
	ifm.total_sizeof_image = get_programCounter();
	image_file_make(tp, lines, &ifm);
	printf("DUMP TABLE-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	dump_table_of_labels();
	printf("IMAGE_FILE_MEMORY-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	dump_image_file_memory(&ifm);
	printf("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	dump_output_beffer();
}

