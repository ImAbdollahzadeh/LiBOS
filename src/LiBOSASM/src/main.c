#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

int main(void)
{
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
		"\tpush 0xAA001258\n"
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
		"\tnop\n"
		"\tnop\n"
		"\tadd edx, 0xDDCCBB56\n"
		"\tmov BYTE[esp+0x28], 0x16\n"
		"\tmov WORD[ebp+0x00124585], esp\n"
		"\tjmp _THIRD_AND_LAST@@_LABEL:\n"
		"\tmov esp, ebp\n"
		"__SECOND_LABEL:\n"
		"\tjmp __SECOND_LABEL:\n"
		"\tadd esp, ebp\n"
		"\tpop DWORD[esp+0xFFDDAA56]\n"
		"\tint 0x03\n"
		"_THIRD_AND_LAST@@_LABEL:\n"
		"\tret\n"
		"[SECTION .DATA]\n"
		"string_to_be_printed: db 'ImanAbdollahzadehLiBOS'\n"
		"_rgb@@mask_black:     dd  0x23AB8FC6\n"
		"libos_word_data:      dw  0x8F7A\n"
		"_@@an_example_qword:  dq  0x112233445678AEF0\n";
	
	parse_0(file, &tp, &lines, p);
	
	print_file(file);
	
	parse_1_or__convert_instructions_line_by_line(tp, lines);
	dump_table_of_labels();
	zero_programCounter();
	zero_data_section_identifier();
	
	IMAGE_FILE_MEMORY ifm;
	parse_2(tp, lines);
	ifm.total_sizeof_image = get_programCounter();
	image_file_make(tp, lines, &ifm);
	dump_table_of_labels();
	dump_image_file_memory(&ifm);
	dump_output_beffer();
	dump_data_section_table_entries();
}

