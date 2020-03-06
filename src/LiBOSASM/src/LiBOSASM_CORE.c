#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

unsigned int ProgramCounter = 0;
unsigned int parse_level    = 0xFF;
unsigned char output_buffer[16*1024 /* for nor 16 KiB */];
unsigned int  origin = 0;
//....................................................................................................................................

NUMERIC_TOKEN table_of_numeric_tokens[0xFFFF];
unsigned int  table_of_numeric_tokens_count = 0;

SYMBOLIC_LABEL table_of_labels[0xFFFF];
unsigned int table_of_labels_count = 0;

DATA_SECTION_ENTRIES data_entries_table[1024];
unsigned int data_entries_table_count = 0;

unsigned int data_section_identifier = 0;

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
	"eflags", "flags", "pushf", "popf", "int", "jz", 
	"jnz", "xor",
};
unsigned int lookup_table_entries = sizeof(global_lookup_table) / sizeof(const char*);

//....................................................................................................................................

const char* global_mem_table[] = {
	"[", "]", 
	"byte", "BYTE", 
	"word", "WORD", 
	"dword", "DWORD", 
	"qword", "QWORD",
	"xmmword", "XMMWORD", 
	"VALUE", "value",
};
unsigned int mem_table_entries = sizeof(global_mem_table) / sizeof(const char*);

//....................................................................................................................................

OPCODE opcodes[] = {
	{"add",  OP_REG_IMM, 0x80},
	{"add",  OP_REG_MEM, 0x00},
	{"add",  OP_REG_REG, 0x00},
	{"add",  OP_MEM_REG, 0x00}, 
	{"add",  OP_MEM_IMM, 0x80},
	{"mov",  OP_REG_IMM, 0xB8},
	{"mov",  OP_REG_MEM, 0x8A},
	{"mov",  OP_MEM_REG, 0x88},
	{"mov",  OP_MEM_IMM, 0xC7},
	{"mov",  OP_REG_REG, 0x89},
	{"sub",  OP_REG_IMM, 0x80}, 
	{"sub",  OP_REG_MEM, 0x2A},
	{"sub",  OP_REG_REG, 0x28},
	{"sub",  OP_MEM_REG, 0x28}, 
	{"sub",  OP_MEM_IMM, 0x80},
	{"xor",  OP_REG_IMM, 0x80},
	{"xor",  OP_REG_MEM, 0x32}, 
	{"xor",  OP_REG_REG, 0x30}, 
	{"xor",  OP_MEM_REG, 0x30}, 
	{"xor",  OP_MEM_IMM, 0x80}, 
	{"cmp",  OP_REG_IMM, 0x80}, 
	{"cmp",  OP_REG_MEM, 0x3A}, 
	{"cmp",  OP_REG_REG, 0x3A}, 
	{"cmp",  OP_MEM_REG, 0x38}, 
	{"cmp",  OP_MEM_IMM, 0x80}, 
	{"push", OP_REG,     0x50}, 
	{"push", OP_IMM,     0x68}, 
	{"pop",  OP_REG,     0x58}, 
	{"pop",  OP_MEM,     0x8F}, 
	{"jmp",  OP_REG,     0xFF}, // it gets /4 in its MODRM
	{"jmp",  OP_MEM,     0xFF}, // it gets /4 in its MODRM
	{"call", OP_MEM,     0xFF}, // hard-coded
	{"call", OP_IMM,     0xFF}, // hard-coded
	{"int",  OP_IMM,     0xCD}, 
	{"ret",  OP,         0xFF}, // they are hard-coded
	{"nop",  OP,         0xFF}, // they are hard-coded
	{"hlt",  OP,         0xFF}, // they are hard-coded
	{"cli",  OP,         0xFF}, // they are hard-coded
	{"sti",  OP,         0xFF}, // they are hard-coded
};
unsigned int sizeof_opcodes = sizeof(opcodes) / sizeof(OPCODE);

//....................................................................................................................................

unsigned int is_numeric_token(const char* str)
{
	unsigned int length = string_length(str);
	if(*str == '$' && str[length-1] != ':')
		return 1;
	return 0;
}

//....................................................................................................................................

unsigned int is_comment(const char* str)
{
	unsigned int length = string_length(str);
	if(*str == ';')
	{
		printf("comment:%s\n", str);
		return 1;
	}
	return 0;
}

//....................................................................................................................................

unsigned int in_opcodes(const char* str)
{
	unsigned int i = 0;
	unsigned int length = 0, ret = 1;
	unsigned int str_length = string_length(str);
	unsigned int str_length_copy = str_length;
	char* trg = 0;
	char* src = str;
	while(i < lookup_table_entries)
	{
		trg	= global_lookup_table[i];
		length = string_length(global_lookup_table[i]);
		
		while(length--)
		{
			str_length_copy--;
			if(*src++ != *trg++)
				ret = 0;
		}
		if(ret && !str_length_copy)
			return 1;
		i++;
		src = str;
		ret = 1;
		str_length_copy = str_length;
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
	if(str[length - 1] == ':' && !data_section_identifier)
		return 1;
	if(str[length - 1] == ':' && data_section_identifier)
		return 2;
	return 0;
}

//....................................................................................................................................

void map_machine_codes(TRIPLE_PACKET* tp)
{
	char* s = tp->str1;
	if(*s != '!')
	{
		if( is_comment(s) )
		{
			//printf("c");
			tp->mod1 = 'c';
		}
		else if( in_opcodes(s) )
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
			
			// [SECTION .DATA] case
			if( _contain(tp->str1, ".DATA") )
				data_section_identifier = 1;
		}
		else if( is_immediate(s) )
		{
			//printf("%c", 'I');
			tp->mod1 = 'I';
		}
		else if( is_label(s) == 1 )
		{
			//printf("%c", 'L');
			tp->mod1 = 'L';
			table_of_labels[table_of_labels_count].string = s;
			table_of_labels[table_of_labels_count].address = 0;
			table_of_labels_count++;
		}
		
		else if( is_label(s) == 2 )
		{
			//printf("%c", 'L');
			tp->mod1 = 'L';
		}
		
		else if( is_numeric_token(s) )
		{
			//printf("%c", 'N');
			tp->mod1 = 'N';
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
			if(*f == ';') // the whole line is comment
			{
				while(*f != '\0')
					f++;
			}
			else if(*f == '[')
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
	printf("........ DUMP TABLE OF LABELS ..................................\n");
	for(i=0;i<table_of_labels_count;i++)
		printf("%u: string: %s, address: 0x%x\n", i, table_of_labels[i].string, table_of_labels[i].address);
	printf("................................................................\n");
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
	else if( _strcmp(tp->str1, "xor") )
		convert_xor_instruction(tp, &ProgramCounter); 
	else if( _strcmp(tp->str1, "push") )
		convert_push_instruction(tp, &ProgramCounter);   
	else if( _strcmp(tp->str1, "pop") )
		convert_pop_instruction(tp, &ProgramCounter);  
	else if( _strcmp(tp->str1, "int") )
		convert_int_instruction(tp, &ProgramCounter);
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
	else if( _strcmp(tp->str1, "call") )
		convert_call_instruction(tp, &ProgramCounter);
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
	else if( tp->mod1 == 'L' && _strcmp(tp->str2, "!") )
		handle_labels(tp, &ProgramCounter);
	else if( tp->mod1 == 'L' && (!_strcmp(tp->str2, "!")) )
		handle_data_section(tp);
	else if( tp->mod1 == 'N' )
		handle_numeric_table(tp);
	else if( tp->mod1 == 'c' )
		handle_comment(tp);
	
	else {}
}

//....................................................................................................................................

void parse_1_or__convert_instructions_line_by_line(TRIPLE_PACKET* tp, unsigned int counts)
{
	parse_level = PARSE_LEVEL_1;
	
	unsigned int i = 0;
	printf("........ FIRST PARSE ...........................................\n");
	while(i < counts)
	{
		_selection_stub(&tp[i]);
		i++;
	}
	printf("................................................................\n");
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
	printf("........ ZEROTH PARSE ..........................................\n");
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
	printf("................................................................\n");
}

//....................................................................................................................................

void parse_2(TRIPLE_PACKET* tp, unsigned int counts)
{
	parse_level = PARSE_LEVEL_2;
	
	unsigned int i = 0;
	printf("........ SECOND PARSE ..........................................\n");
	while(i < counts)
	{
		_selection_stub(&tp[i]);
		i++;
	}
	printf("................................................................\n");
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
 			table_of_labels[i].address = *PC + origin; // CONCENTRATE HERE
			return;
 		}
 	}
}

//....................................................................................................................................

void handle_data_section(TRIPLE_PACKET* tp)
{
	unsigned int parse_level = get_parse_level();
	if( parse_level == PARSE_LEVEL_2 )
		return;
	
	char* ch = tp->str1;
	unsigned int sz = 0, i = 0;
	unsigned char immediate128[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	while(*ch != ':')
	{
		data_entries_table[data_entries_table_count].data_name[i] = *ch++;
		i++;
	}
	data_entries_table[data_entries_table_count].data_name[i] = ':';
	data_entries_table[data_entries_table_count].data_name[i+1] = '\0';
	ch = tp->str2;
	while(*ch++ != 'd');
	 
	if(*ch == 'b')
	{
		data_entries_table[data_entries_table_count].data_type = DATA_TYPE_BYTE;
		ch++;
	}
	else if(*ch == 'w')
	{
		data_entries_table[data_entries_table_count].data_type = DATA_TYPE_WORD;
		ch++;
	}
	else if(*ch == 'd')
	{
		data_entries_table[data_entries_table_count].data_type = DATA_TYPE_DWORD;
		ch++;
	}
	else if(*ch == 'q')
	{
		data_entries_table[data_entries_table_count].data_type = DATA_TYPE_QWORD;
		ch++;
	}
	else if(*ch == 'X')
	{
		data_entries_table[data_entries_table_count].data_type = DATA_TYPE_XMMWORD;
		ch++;
	}
	
	ch = tp->str3;
	if(data_entries_table[data_entries_table_count].data_type == DATA_TYPE_BYTE)    
	{
		while(*ch++ != 39) // ascii code for '
			;
		char* tmp_ch = ch;
		while(*ch++ != 39)
			sz++;
		
		data_entries_table[data_entries_table_count].data_size = sz;
		data_entries_table[data_entries_table_count].data_buffer = (unsigned char*)malloc(sz);
		for(i=0; i<sz; i++)
			data_entries_table[data_entries_table_count].data_buffer[i] = *tmp_ch++;
	} 
	
	else if(data_entries_table[data_entries_table_count].data_type == DATA_TYPE_WORD)    
	{
		data_entries_table[data_entries_table_count].data_size = 2;
		data_entries_table[data_entries_table_count].data_buffer = (unsigned char*)malloc(2);
		encode_u16(ch, immediate128);
		data_entries_table[data_entries_table_count].data_buffer[0] = byte_string_to_byte(immediate128[0], immediate128[1]);
		data_entries_table[data_entries_table_count].data_buffer[1] = byte_string_to_byte(immediate128[2], immediate128[3]);
	}
	
	else if(data_entries_table[data_entries_table_count].data_type == DATA_TYPE_DWORD)    
	{
		data_entries_table[data_entries_table_count].data_size = 4;
		data_entries_table[data_entries_table_count].data_buffer = (unsigned char*)malloc(4);
		encode_u32(ch, immediate128);
		data_entries_table[data_entries_table_count].data_buffer[0] = byte_string_to_byte(immediate128[0], immediate128[1]);
		data_entries_table[data_entries_table_count].data_buffer[1] = byte_string_to_byte(immediate128[2], immediate128[3]);
		data_entries_table[data_entries_table_count].data_buffer[2] = byte_string_to_byte(immediate128[4], immediate128[5]);
		data_entries_table[data_entries_table_count].data_buffer[3] = byte_string_to_byte(immediate128[6], immediate128[7]);
	}
	
	else if(data_entries_table[data_entries_table_count].data_type == DATA_TYPE_QWORD)    
	{
		data_entries_table[data_entries_table_count].data_size = 8;
		data_entries_table[data_entries_table_count].data_buffer = (unsigned char*)malloc(8);
		encode_u64(ch, immediate128);
		data_entries_table[data_entries_table_count].data_buffer[0] = byte_string_to_byte(immediate128[0], immediate128[1]);
		data_entries_table[data_entries_table_count].data_buffer[1] = byte_string_to_byte(immediate128[2], immediate128[3]);
		data_entries_table[data_entries_table_count].data_buffer[2] = byte_string_to_byte(immediate128[4], immediate128[5]);
		data_entries_table[data_entries_table_count].data_buffer[3] = byte_string_to_byte(immediate128[6], immediate128[7]);
		data_entries_table[data_entries_table_count].data_buffer[4] = byte_string_to_byte(immediate128[8], immediate128[9]);
		data_entries_table[data_entries_table_count].data_buffer[5] = byte_string_to_byte(immediate128[10], immediate128[11]);
		data_entries_table[data_entries_table_count].data_buffer[6] = byte_string_to_byte(immediate128[12], immediate128[13]);
		data_entries_table[data_entries_table_count].data_buffer[7] = byte_string_to_byte(immediate128[14], immediate128[15]);
	}
	
	else if(data_entries_table[data_entries_table_count].data_type == DATA_TYPE_XMMWORD)    
	{
		data_entries_table[data_entries_table_count].data_size = 16;
		data_entries_table[data_entries_table_count].data_buffer = (unsigned char*)malloc(16);
		encode_u128(ch, immediate128);
		data_entries_table[data_entries_table_count].data_buffer[0]  = byte_string_to_byte(immediate128[0], immediate128[1]);
		data_entries_table[data_entries_table_count].data_buffer[1]  = byte_string_to_byte(immediate128[2], immediate128[3]);
		data_entries_table[data_entries_table_count].data_buffer[2]  = byte_string_to_byte(immediate128[4], immediate128[5]);
		data_entries_table[data_entries_table_count].data_buffer[3]  = byte_string_to_byte(immediate128[6], immediate128[7]);
		data_entries_table[data_entries_table_count].data_buffer[4]  = byte_string_to_byte(immediate128[8], immediate128[9]);
		data_entries_table[data_entries_table_count].data_buffer[5]  = byte_string_to_byte(immediate128[10], immediate128[11]);
		data_entries_table[data_entries_table_count].data_buffer[6]  = byte_string_to_byte(immediate128[12], immediate128[13]);
		data_entries_table[data_entries_table_count].data_buffer[7]  = byte_string_to_byte(immediate128[14], immediate128[15]);
		data_entries_table[data_entries_table_count].data_buffer[8]  = byte_string_to_byte(immediate128[16], immediate128[17]);
		data_entries_table[data_entries_table_count].data_buffer[9]  = byte_string_to_byte(immediate128[18], immediate128[19]);
		data_entries_table[data_entries_table_count].data_buffer[10] = byte_string_to_byte(immediate128[20], immediate128[21]);
		data_entries_table[data_entries_table_count].data_buffer[11] = byte_string_to_byte(immediate128[22], immediate128[23]);
		data_entries_table[data_entries_table_count].data_buffer[12] = byte_string_to_byte(immediate128[24], immediate128[25]);
		data_entries_table[data_entries_table_count].data_buffer[13] = byte_string_to_byte(immediate128[26], immediate128[27]);
		data_entries_table[data_entries_table_count].data_buffer[14] = byte_string_to_byte(immediate128[28], immediate128[29]);
		data_entries_table[data_entries_table_count].data_buffer[15] = byte_string_to_byte(immediate128[30], immediate128[31]);
	}

	data_entries_table_count++;
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

void extract_origin(TRIPLE_PACKET* tp, unsigned int counts)
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
	origin = (void*)( address_string_to_hex(tmp) );
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
	printf("........ DUMP IMAGE MEMORY OUTPUT ..............................\n");
	printf("ORIGIN: 0x%x, SIZE: %u bytes\n", image_file_memory->physical_origin, image_file_memory->total_sizeof_image);
	printf("................................................................\n");
}

//....................................................................................................................................

unsigned char* get_output_buffer(void)
{
	return output_buffer;
}

//....................................................................................................................................

void dump_output_beffer(void)
{
	unsigned int count = get_programCounter(), i, jumper = 0x10;
	unsigned char* out = get_output_buffer();
	unsigned int j = 0;
	
	unsigned int SIZEOF_CODE_SECTION = count;
	unsigned int SIZEOF_DATA_SECTION = 0;
	unsigned int SIZEOF_MODULES_SECTION = 0;
	unsigned int SIZEOF_HEADER_SECTION = 0;
	
	
	for(i = 0; i < data_entries_table_count; i++)
	{
		char*        tmp = (char*)(&data_entries_table[i]);
		unsigned int sz  = string_length(data_entries_table[i].data_name);
		
		while(sz--)
		{
			out[count + j] = *tmp++;
			j++;
		}
		count += j;
		SIZEOF_DATA_SECTION += j;
		j = 0;
		
		tmp = (char*)(&(data_entries_table[i].data_size));
		out[count+0] = *(unsigned char*)tmp++;
		out[count+1] = *(unsigned char*)tmp++;
		out[count+2] = *(unsigned char*)tmp++;
		out[count+3] = *(unsigned char*)tmp++;
		count += 4;
		SIZEOF_DATA_SECTION += 4;
	
		sz = data_entries_table[i].data_size;
		tmp = (char*)(data_entries_table[i].data_buffer);
		while(sz--)
		{
			out[count + j] = *tmp++;
			j++;
		}
		count += j;
		SIZEOF_DATA_SECTION += j;
		j = 0;
		
		tmp = (char*)(&(data_entries_table[i].data_type));
		unsigned int k;
		for(k=0; k<sizeof(DATA_TYPE); k++)
			out[count + k] = *(DATA_TYPE*)tmp++;
		count += sizeof(DATA_TYPE);
		SIZEOF_DATA_SECTION += sizeof(DATA_TYPE);
	}
	
	EXT_HEADER ext_header;
	const char* iid = "LiBOS_EXT_IMAGE";
	const char* wwriter = "Iman_Abdollahzadeh";
	const char* ddate = "01_Mar_2020";
	unsigned int iid_length = string_length(iid);
	unsigned int wwriter_length = string_length(wwriter);
	unsigned int ddate_length = string_length(ddate);
	unsigned int k;
	for(k=0; k<iid_length; k++)
		ext_header.id[k] = iid[k];
	ext_header.id[iid_length] = '\0';
	for(k=0; k<wwriter_length; k++)
		ext_header.writer[k] = wwriter[k];
	ext_header.writer[wwriter_length] = '\0';
	for(k=0; k<ddate_length; k++)
		ext_header.date[k] = ddate[k];
	ext_header.date[ddate_length] = '\0';
	
	SIZEOF_HEADER_SECTION += iid_length + wwriter_length + ddate_length + (5 * sizeof(unsigned int));
	
	ext_header.start_of_code = iid_length + wwriter_length + ddate_length + (5 * sizeof(unsigned int)); // ??
	ext_header.start_of_data = get_programCounter() + ext_header.start_of_code;
	ext_header.start_of_modules = ext_header.start_of_data + SIZEOF_DATA_SECTION;
	ext_header.sizeof_writer = wwriter_length;
	ext_header.sizeof_date   = ddate_length;
	
	char* tmp = (char*)(&ext_header.id);
	j = 0;
	while(iid_length--)
	{
		out[count + j] = *tmp++;
		j++;
	}
	count += j;
	j = 0;
	
	tmp = (char*)(&ext_header.sizeof_writer);
	out[count+0] = *(unsigned char*)tmp++;
	out[count+1] = *(unsigned char*)tmp++;
	out[count+2] = *(unsigned char*)tmp++;
	out[count+3] = *(unsigned char*)tmp++;
	count += 4;
	
	tmp = (char*)(&ext_header.writer);
	while(wwriter_length--)
	{
		out[count + j] = *tmp++;
		j++;
	}
	count += j;
	j = 0;
	
	tmp = (char*)(&ext_header.sizeof_date);
	out[count+0] = *(unsigned char*)tmp++;
	out[count+1] = *(unsigned char*)tmp++;
	out[count+2] = *(unsigned char*)tmp++;
	out[count+3] = *(unsigned char*)tmp++;
	count += 4;
	
	tmp = (char*)(&ext_header.date);
	while(ddate_length--)
	{
		out[count + j] = *tmp++;
		j++;
	}
	count += j;
	j = 0;
	
	tmp = (char*)(&ext_header.start_of_code);
	out[count+0] = *(unsigned char*)tmp++;
	out[count+1] = *(unsigned char*)tmp++;
	out[count+2] = *(unsigned char*)tmp++;
	out[count+3] = *(unsigned char*)tmp++;
	count += 4;
	tmp = (char*)(&ext_header.start_of_data);
	out[count+0] = *(unsigned char*)tmp++;
	out[count+1] = *(unsigned char*)tmp++;
	out[count+2] = *(unsigned char*)tmp++;
	out[count+3] = *(unsigned char*)tmp++;
	count += 4;
	tmp = (char*)(&ext_header.start_of_modules);
	out[count+0] = *(unsigned char*)tmp++;
	out[count+1] = *(unsigned char*)tmp++;
	out[count+2] = *(unsigned char*)tmp++;
	out[count+3] = *(unsigned char*)tmp++;
	count += 4;
	
	printf("........ DUMP OUTPUT BUFFER ....................................\n");
	printf("        00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	printf("................................................................\n00:     ");
	
	unsigned char swap_buffer[16*1024];
	unsigned int MAJOR_COUNTER;
	for(MAJOR_COUNTER=0; MAJOR_COUNTER<SIZEOF_HEADER_SECTION; MAJOR_COUNTER++)
		swap_buffer[MAJOR_COUNTER] = out[SIZEOF_CODE_SECTION+SIZEOF_DATA_SECTION+SIZEOF_MODULES_SECTION + MAJOR_COUNTER];
	
	for(MAJOR_COUNTER=0; MAJOR_COUNTER<SIZEOF_CODE_SECTION; MAJOR_COUNTER++)
		swap_buffer[SIZEOF_HEADER_SECTION + MAJOR_COUNTER] = out[MAJOR_COUNTER];
	
	for(MAJOR_COUNTER=0; MAJOR_COUNTER<SIZEOF_DATA_SECTION; MAJOR_COUNTER++)
		swap_buffer[SIZEOF_HEADER_SECTION + SIZEOF_CODE_SECTION + MAJOR_COUNTER] = out[SIZEOF_CODE_SECTION + MAJOR_COUNTER];
	
	for(MAJOR_COUNTER=0; MAJOR_COUNTER<SIZEOF_MODULES_SECTION; MAJOR_COUNTER++)
		swap_buffer[SIZEOF_HEADER_SECTION + SIZEOF_CODE_SECTION + SIZEOF_DATA_SECTION + MAJOR_COUNTER] = out[SIZEOF_CODE_SECTION+SIZEOF_DATA_SECTION + MAJOR_COUNTER];
	
	
	unsigned int column_counter = 0x10;
	for(i = 0; i < count; i++)
	{
		jumper --;
		printf((unsigned int)(swap_buffer[i]) < 0x10 ? "0%x " : "%x ", swap_buffer[i]);
		if( !jumper )
		{
			printf("\n%x:     ", column_counter);
			//printf("\n", column_counter);
			column_counter += 0x10;
			jumper = 0x10;
		}
	}
	
	printf("\n................................................................\n");
	
	printf("........ DUMP OUTPUT BUFFER (ASCII) ............................\n");
	printf("        00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	printf("................................................................\n00:     ");
	jumper = 0x10;
	column_counter = 0x10;
	for(i = 0; i < count; i++)
	{
		jumper --;
		if(swap_buffer[i] >= 33 && swap_buffer[i] < 126)
			printf("%c  ", swap_buffer[i]);
		else if(swap_buffer[i] == '!' )
			printf("!  ");
		else if(swap_buffer[i] == '?' )
			printf("?  ");
		else 
			printf(".  ");
		
		if( !jumper )
		{
			printf("\n%x:     ", column_counter);
			column_counter += 0x10;
			jumper = 0x10;
		}
	}
	
	printf("\n................................................................\n");
}

//....................................................................................................................................

void dump_data_section_table_entries(void)
{
	unsigned int i;
	printf("........ DUMP TABLE OF DATA ENTRIES ............................\n");
	for(i = 0; i < data_entries_table_count; i++)
	{
		printf("entry %u:\n", i);
		printf("    data_name: %s\n", data_entries_table[i].data_name);
		printf("    data_type: %i\n", data_entries_table[i].data_type);
		printf("    data_size: %u\n", data_entries_table[i].data_size);
		switch( data_entries_table[i].data_type )
		{
			case DATA_TYPE_BYTE:
				printf("    data_buffer: %s\n", data_entries_table[i].data_buffer);
				break;
			case DATA_TYPE_WORD:
				printf("    data_buffer: 0x%x\n", *(unsigned short*)(data_entries_table[i].data_buffer));
				break;
			case DATA_TYPE_DWORD:
				printf("    data_buffer: 0x%x\n", *(unsigned int*)(data_entries_table[i].data_buffer));
				break;
			case DATA_TYPE_QWORD:
				printf("    data_buffer: 0x%llx\n", *(unsigned long long*)(data_entries_table[i].data_buffer));
				break;
			case DATA_TYPE_XMMWORD:
				printf("    data_buffer: 0x%llx", *(unsigned long long*)((data_entries_table[i].data_buffer) + 8));
				printf("%llx\n",                  *(unsigned long long*)(data_entries_table[i].data_buffer));
				break;
		}
		printf("    data_buffer_address 0x%x\n", data_entries_table[i].data_buffer_2);
	}
	printf("................................................................\n");
}

//....................................................................................................................................

void print_file(char* file)
{
	printf("........ PRINT FILE ............................................\n");
	printf(file);
	printf("................................................................\n");
}

//....................................................................................................................................

void zero_data_section_identifier(void)
{
	data_section_identifier = 0;
}

//....................................................................................................................................

void handle_numeric_table(TRIPLE_PACKET* tp)
{
	unsigned int pl = get_parse_level();
	unsigned int pc = get_programCounter();
	unsigned int i;
	
	if( pl == PARSE_LEVEL_1 )
	{
		for(i=0; i<table_of_numeric_tokens_count; i++)
		{
			if( _strcmp(table_of_numeric_tokens[i].string, tp->str1) )
			{
				printf("Numeric token has been already defined ERROR\n");
				table_of_numeric_tokens[table_of_numeric_tokens_count].string = "FATAL ERROR";
				table_of_numeric_tokens[table_of_numeric_tokens_count].PC     = 0xFFFFFFFF;
				table_of_numeric_tokens_count++;
				return;
			}
		}
		table_of_numeric_tokens[table_of_numeric_tokens_count].string = tp->str1;
		table_of_numeric_tokens[table_of_numeric_tokens_count].PC     = pc + origin; // CONCENTRATE HERE
		table_of_numeric_tokens_count++;
	}
}

//....................................................................................................................................

void dump_numeric_table(void)
{
	unsigned int i;
	printf("........ DUMP TABLE OF NUMERIC TOKEN ...........................\n");
	for(i=0; i<table_of_numeric_tokens_count; i++)
		printf("entry %u: string = %s, PC = 0x%x\n", i, table_of_numeric_tokens[i].string, table_of_numeric_tokens[i].PC);
	printf("................................................................\n");
}

//....................................................................................................................................

unsigned int get_table_of_data_count()
{
	return data_entries_table_count;
}

//....................................................................................................................................

DATA_SECTION_ENTRIES* get_table_of_data()
{
	return data_entries_table;
}

//....................................................................................................................................

void handle_comment(TRIPLE_PACKET* tp)
{
	/* nothing */
}

//....................................................................................................................................

void append_data_section_after_code_section(void)
{
	unsigned int pc = get_programCounter();
	unsigned int i;
	DATA_SECTION_ENTRIES* ds = get_table_of_data();
	printf("data: pc = %x\n", pc);
	for(i = 0; i < data_entries_table_count; i++)
	{
		ds[i].data_buffer_2 = pc + origin;
		pc += ds[i].data_size;
	}
}

//....................................................................................................................................
