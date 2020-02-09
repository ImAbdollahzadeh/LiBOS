#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

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
	{"add", ATT_DIGIT,	  0x80},
	{"add", ATT_r,		  0x00},
	{"mov", ATT_r_plus,	 0xB8},
	{"mov", ATT_r,		  0x8B},
	{"mov", ATT_MEMORY_IMM, 0xC7},
	{"mov", ATT_REG_REG,	0x89},
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
	trg	= global_mem_table[0];
	length = string_length(str);
	while(length--)
	{
		if(*trg++ == *src++)
			directive_ret = 1;
	}
	if(!directive_ret)
		return 0;
	
	trg = global_mem_table[1];
	length = string_length(str);
	while(length--)
	{
		if(*trg++ == *src++)
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

void convert_instructions_line_by_line(TRIPLE_PACKET* tp, unsigned int counts)
{
	unsigned int i = 0;
	while(i < counts)
	{
		if( _strcmp(tp[i].str1, "mov") )
			convert_mov_instruction(&tp[i]);
		else if( _strcmp(tp[i].str1, "add") )
			convert_add_instruction(&tp[i]);
		else if( _strcmp(tp[i].str1, "sub") )
			convert_sub_instruction(&tp[i]);   
		
		i++;
	}
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