#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

unsigned int ProgramCounter = 0;
unsigned int parse_level    = 0xFF;
unsigned char output_buffer[16*1024 /* for nor 16 KiB */];

//....................................................................................................................................

SYMBOLIC_LABEL table_of_labels[0xFFFF];
unsigned int table_of_labels_count = 0;

//....................................................................................................................................

const char* global_lookup_table[] = {
	"mov", "cmp", "add", "sub", "mul", "div", 
	"imul", "idiv", "inc", "dec", "shr", "shl", 
	"lea", "rep", "repe", "repne", "jmp", "call", 
	"push", "pop", "je", "jne", "ret", "iret", 
	"eax", "ax", "ah", "al", "ebx", "bx", "bh", "bl",
	"ecx", "cx", "ch", "cl", "edx", "dx", "dh", "dl",
	"esi", "si", "edi", "di", "eip", "ip", "ebp", "bp",
	"esp", "sp", "ss", "cs", "ds", "es", "fs", "gs", 
	"eflags", "flags", "pushf", "popf", 
};
unsigned int lookup_table_entries = sizeof(global_lookup_table) / sizeof(const char*);

//....................................................................................................................................

const char* global_mem_table[] = {
	"[", "]", "byte", "BYTE", "word", "WORD", 
	"dword", "DWORD", "xmmword", "XMMWORD", "qword", "QWORD",
};
unsigned int mem_table_entries = sizeof(global_mem_table) / sizeof(const char*);

//....................................................................................................................................

OPCODE opcodes[] = {
	{"add",  OP_REG_IMM, 0x80},
	{"add",  OP_REG_MEM, 0x00},
	{"add",  OP_REG_REG, 0x01}, //<-??
	{"add",  OP_MEM_REG, 0xFF}, //<-??
	{"add",  OP_MEM_IMM, 0xFF}, //<-??
	{"mov",  OP_REG_IMM, 0xB8},
	{"mov",  OP_REG_MEM, 0x8B},
	{"mov",  OP_MEM_REG, 0x8B},
	{"mov",  OP_MEM_IMM, 0xC7},
	{"mov",  OP_REG_REG, 0x89},
	{"sub",  OP_REG_IMM, 0xFF}, //<-??
	{"sub",  OP_REG_MEM, 0xFF}, //<-??
	{"sub",  OP_REG_REG, 0xFF}, //<-??
	{"sub",  OP_MEM_REG, 0xFF}, //<-??
	{"sub",  OP_MEM_IMM, 0xFF}, //<-??
	{"cmp",  OP_REG_IMM, 0xFF}, //<-??
	{"cmp",  OP_REG_MEM, 0xFF}, //<-??
	{"cmp",  OP_REG_REG, 0xFF}, //<-??
	{"cmp",  OP_MEM_REG, 0xFF}, //<-??
	{"cmp",  OP_MEM_IMM, 0xFF}, //<-??
	{"push", OP_X,       0xFF}, //<-??
	{"push", OP_X,       0xFF}, //<-??
	{"pop",  OP_X,       0xFF}, //<-??
	{"pop",  OP_X,       0xFF}, //<-??
	{"jmp",  OP_X,       0xFF}, //<-??
	{"jmp",  OP_X,       0xFF}, //<-??
	{"int",  OP_X,       0xFF}, //<-??
	{"ret",  OP,         0xFF}, //<-??
	{"nop",  OP,         0xFF}, //<-??
	{"hlt",  OP,         0xFF}, //<-??
	{"cli",  OP,         0xFF}, //<-??
	{"sti",  OP,         0xFF}, //<-??
};
unsigned int sizeof_opcodes = sizeof(opcodes) / sizeof(OPCODE);

//....................................................................................................................................

unsigned int in_opcodes(const char* str)
{
	unsigned int i = 0;
	unsigned int length = 0, ret = 1;
	char* trg = 0;
	char* src = str;
	while(i < lookup_table_entries)
	{
		trg	= global_lookup_table[i];
		length = string_length(global_lookup_table[i]);
		
		while(length--)
		{
			if(*src++ != *trg++)
				ret = 0;
		}
		if(ret)
			return 1;
		i++;
		src = str;
		ret = 1;
	}
	return 0;
}

//....................................................................................................................................

unsigned int memory_operands(const char* str)
{
	unsigned int i = 2;
	unsigned int length = 0;
	unsigned int mem_ret = 1;
	unsigned int directive_ret = 0;
	char* trg = 0;
	char* src = str;

	while(i < mem_table_entries)
	{
		trg	= global_mem_table[i];
		length = string_length(global_mem_table[i]);
		
		while(length--)
		{
			if(*src++ != *trg++)
				mem_ret = 0;
		}
		if(mem_ret)
			goto DIRECTIVE_CHECKS;
		i++;
		src = str;
		mem_ret = 1;
	}
	mem_ret = 0;
DIRECTIVE_CHECKS:	
	src = str;
	trg	= global_mem_table[0];
	length = string_length(str);
	while(length--)
	{
		if(*trg == *src++)
			directive_ret = 1;
	}
	if(!directive_ret)
		return 0;
	
	trg = global_mem_table[1];
	length = string_length(str);
	while(length--)
	{
		if(*trg == *src++)
			directive_ret = 1;
	}
	if(!directive_ret)
		return 0;
	
	if(directive_ret && mem_ret)
		return 1;
	if(directive_ret && !mem_ret)
		return 2;
	return 0;
}

//....................................................................................................................................

unsigned int is_immediate(const char* str)
{
	unsigned int length = string_length(str);
	char* src = str;
	while(length--)
	{
		if(*src++ == '0')
		{
			if(*src++ == 'x')
				return 1;
		}	
	}

	src = str;
	while(*src != '\0')
		src++;
	
	if(*(src - 1) == 'h' || *(src - 1) == 'H')
		return 1;
	
	return 0;
}

//....................................................................................................................................

unsigned int is_label(const char* str)
{
	unsigned int length = string_length(str);
	if(str[length - 1] == ':')
		return 1;	
	return 0;
}

//....................................................................................................................................

void map_machine_codes(TRIPLE_PACKET* tp)
{
	char* s = tp->str1;
	if(*s != '!')
	{
		if( in_opcodes(s) )
		{
			//printf("o");
			tp->mod1 = 'o';
		}
		else if( memory_operands(s) == 1 )
		{
			//printf("%c", 'M');
			tp->mod1 = 'M';
		}
		else if( memory_operands(s) == 2 )
		{
			//printf("%c", 'D');
			tp->mod1 = 'D';
		}
		else if( is_immediate(s) )
		{
			//printf("%c", 'I');
			tp->mod1 = 'I';
		}
		else if( is_label(s) )
		{
			//printf("%c", 'L');
			tp->mod1 = 'L';
			table_of_labels[table_of_labels_count].string = s;
			table_of_labels[table_of_labels_count].address = 0;
			table_of_labels_count++;
		}
		else
		{
			//printf("%c", '.');
			tp->mod1 = '.';
		}
			
	}
	s += 128;
	if(*s != '!')
	{
		if( in_opcodes(s) )
		{
			//printf("o");
			tp->mod2 = 'o';
		}
		else if( memory_operands(s) == 1 )
		{
			//printf("%c", 'M');
			tp->mod2 = 'M';
		}
		else if( memory_operands(s) == 2 )
		{
			//printf("%c", 'D');
			tp->mod2 = 'D';
		}
		else if( is_immediate(s) )
		{
			//printf("%c", 'I');
			tp->mod2 = 'I';
		}
		else
		{
			//printf("%c", '.');
			tp->mod2 = '.';
		}
	}
	s += 128;
	if(*s != '!')
	{
		if( in_opcodes(s) )
		{
			//printf("o");
			tp->mod3 = 'o';
		}
		else if( memory_operands(s) == 1 )
		{
			//printf("%c", 'M');
			tp->mod3 = 'M';
		}
		else if( memory_operands(s) == 2 )
		{
			//printf("%c", 'D');
			tp->mod3 = 'D';
		}
		else if( is_immediate(s) )
		{
			//printf("%c", 'I');
			tp->mod3 = 'I';
		}
		else
		{
			//printf("%c", '.');
			tp->mod3 = '.';
		}
	}
	//printf("\n");
}

//....................................................................................................................................

TRIPLE_PACKET* alloc_units(unsigned int lines)
{
	return (TRIPLE_PACKET*)malloc(lines * sizeof(TRIPLE_PACKET));	
}

//....................................................................................................................................

void _stub(TRIPLE_PACKET* tp, unsigned int counter, char* tmp)
{
	char* ptr = tp->str1;
	if(*ptr == '!')
	{
		while(counter--)
			*ptr++ = *tmp++;
	}
	else if(*(ptr + 128) == '!')
	{
		ptr += 128;
		while(counter--)
			*ptr++ = *tmp++;
	}
	else
	{
		ptr += 256;
		while(counter--)
			*ptr++ = *tmp++;
	}
}

//....................................................................................................................................

void lex(TRIPLE_PACKET* tp, const char* line)
{
	//printf("*** %s\n\n", line);
	unsigned int size = string_length(line), counter = 0;
	tp->str1[0] = tp->str2[0] = tp->str3[0] = '!';	
	char tmp[128], *f = line;
	while(*f)
	{
		if(*f == '\t')
			f++;
		else
		{
			if(*f == '[')
			{
				while(*f != ']')
				{
					if(*f == '\t' || *f == ',')
						f++;
					else
					{
						tmp[counter] = *f++;
						counter++;
					}
				}
				tmp[counter] = ']';
				counter++;
				tmp[counter] = '\0';			
				_stub(tp, counter, tmp);
				counter = 0;
				f++;
			}
			else
			{
				while(*f != ' ')
				{
					if(*f == '\t' || *f == ',')
						f++;
					else
					{
						if(*f == '\0')
						{
							tmp[counter] = '\0';
							_stub(tp, counter, tmp);
							goto FINALIZE;
						} 
						tmp[counter] = *f++;
						counter++;
					}
				}	 
				tmp[counter] = '\0';
				_stub(tp, counter, tmp);
				counter = 0;
				f++;
			}
		}
	}
FINALIZE:
	//printf("%s %s %s => ", tp->str1, tp->str2, tp->str3);
	map_machine_codes(tp);	
}

//....................................................................................................................................

void dump_table_of_labels(void)
{
	unsigned int i;
	for(i=0;i<table_of_labels_count;i++)
	{
		printf("%u: string: %s, address: 0x%x\n", i, table_of_labels[i].string, table_of_labels[i].address);
	}
}

//....................................................................................................................................

void _selection_stub(TRIPLE_PACKET* tp)
{
	if( _strcmp(tp->str1, "mov") )
		convert_mov_instruction(tp, &ProgramCounter);
	else if( _strcmp(tp->str1, "add") )
		convert_add_instruction(tp, &ProgramCounter);
	else if( _strcmp(tp->str1, "sub") )
		convert_sub_instruction(tp, &ProgramCounter);   
	else if( _strcmp(tp->str1, "jmp") )
		convert_jmp_instruction(tp, &ProgramCounter);
	else if( _strcmp(tp->str1, "je") )
		convert_je_instruction(tp, &ProgramCounter);
	else if( _strcmp(tp->str1, "jne") )
		convert_jne_instruction(tp, &ProgramCounter);
	else if( _strcmp(tp->str1, "jz") )
		convert_jz_instruction(tp, &ProgramCounter);
	else if( _strcmp(tp->str1, "jnz") )
		convert_jnz_instruction(tp, &ProgramCounter);
	else if( _strcmp(tp->str1, "cmp") )
		convert_cmp_instruction(tp, &ProgramCounter);
	else if( _strcmp(tp->str1, "cld") )
		convert_singlets_instruction(tp, SINGLET_INSTRUCTION_ID_CLD, &ProgramCounter);
	else if( _strcmp(tp->str1, "std") )
		convert_singlets_instruction(tp, SINGLET_INSTRUCTION_ID_STD, &ProgramCounter);
	else if( _strcmp(tp->str1, "ret") )
		convert_singlets_instruction(tp, SINGLET_INSTRUCTION_ID_RET, &ProgramCounter);
	else if( _strcmp(tp->str1, "nop") )
		convert_singlets_instruction(tp, SINGLET_INSTRUCTION_ID_NOP, &ProgramCounter);
	else if( _strcmp(tp->str1, "hlt") )
		convert_singlets_instruction(tp, SINGLET_INSTRUCTION_ID_HLT, &ProgramCounter);
	else if( _strcmp(tp->str1, "sti") )
		convert_singlets_instruction(tp, SINGLET_INSTRUCTION_ID_STI, &ProgramCounter);
	else if( _strcmp(tp->str1, "cli") )
		convert_singlets_instruction(tp, SINGLET_INSTRUCTION_ID_CLI, &ProgramCounter);
	else if( tp->mod1 == 'L' )
		handle_labels(tp, &ProgramCounter);
	
	else {}
}

//....................................................................................................................................

void parse_1_or__convert_instructions_line_by_line(TRIPLE_PACKET* tp, unsigned int counts)
{
	parse_level = PARSE_LEVEL_1;
	
	unsigned int i = 0;
	while(i < counts)
	{
		_selection_stub(&tp[i]);
		i++;
	}
}

//....................................................................................................................................

unsigned int get_parse_level(void)
{
	return parse_level;
}

//....................................................................................................................................

unsigned int get_sizeof_opcodes(void)
{
	return sizeof_opcodes;
}

//....................................................................................................................................

OPCODE* get_opcodes(void)
{
	return opcodes;
}

//....................................................................................................................................

unsigned int get_table_of_labels_count(void)
{
	return table_of_labels_count;
}

//....................................................................................................................................

SYMBOLIC_LABEL* get_table_of_labels(void)
{
	return table_of_labels;
}

//....................................................................................................................................

void parse_0(const char* file, TRIPLE_PACKET** tp, unsigned int* lines, char* p)
{
	unsigned int j;
	
	parse_level = PARSE_LEVEL_0;
	
	printf("%u lines\n", *lines = how_many_lines(file));
	*tp = alloc_units(*lines);

	TRIPLE_PACKET* tmp = *tp;
	char* f = file;
	unsigned int counter = 0;
	for(j=0; j<*lines; j++)
	{
		while(*f != '\n')
		{
			p[counter] = *f++;
			counter++;
		}
		p[counter] = '\0';
		lex(&tmp[j], p);
		counter = 0;
		f++;
	}
}

//....................................................................................................................................

void parse_2(TRIPLE_PACKET* tp, unsigned int counts)
{
	parse_level = PARSE_LEVEL_2;
	
	unsigned int i = 0;
	while(i < counts)
	{
		_selection_stub(&tp[i]);
		i++;
	}
}

//....................................................................................................................................

void handle_labels(TRIPLE_PACKET* tp, unsigned int* PC)
{
	char* label = tp->str1;
 	unsigned int i;
	unsigned int    table_of_labels_count = get_table_of_labels_count();
	SYMBOLIC_LABEL* table_of_labels       = get_table_of_labels();
	
 	for(i=0; i < table_of_labels_count; i++)
 	{
 	    if( _strcmp(label, table_of_labels[i].string) )
 	    {
 	        table_of_labels[i].address = (void*)(*PC);
 	    }
 	}
}

//....................................................................................................................................

void zero_programCounter(void)
{
	ProgramCounter = 0;
}

//....................................................................................................................................


unsigned int get_programCounter(void)
{
	return ProgramCounter;
}

//....................................................................................................................................

void image_file_make(TRIPLE_PACKET* tp, unsigned int counts, IMAGE_FILE_MEMORY* image_file_memory)
{
	unsigned int i = 0;
	char tmp[128];
	while(i < counts)
	{
		if( tp[i].mod1 == 'D' )
		{
			if( _contain(tp[i].str1, "ORIGIN") )
			{
				/*there should be 0x inside it otherwise an error */
				char* c = tp[i].str1;
				while(*c != '0')
					c++;

				if( *c == '0' && *(c+1) == 'x' )
				{
					unsigned int cnt = 0;
					while(*c != ']')
					{
						tmp[cnt] = *c++;
						cnt++;
					}
					tmp[cnt] = '\0';
					break;
				}
			}
		}
		i++;
	}
	image_file_memory->physical_origin = (void*)( address_string_to_hex(tmp) );
}

//....................................................................................................................................

void dump_image_file_memory(IMAGE_FILE_MEMORY* image_file_memory)
{
	printf("ORIGIN: 0x%x, SIZE: %u bytes\n", image_file_memory->physical_origin, image_file_memory->total_sizeof_image);
}

//....................................................................................................................................

unsigned char* get_output_buffer(void)
{
	return output_buffer;
}

//....................................................................................................................................

void dump_output_beffer(void)
{
	unsigned int count = get_programCounter(), i;
	unsigned char* out = get_output_buffer();
	
	printf("DUMP OUTPUT BUFFER -+-+-+--+-+-+-+-+\n");
	for(i = 0; i < count; i++)
		printf("%x ", out[i]);
	printf("\n-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+\n");
}

//....................................................................................................................................
