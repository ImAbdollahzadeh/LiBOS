#ifndef __LIBOSASM__H__
#define __LIBOSASM__H__

#include  <stdio.h>
#include  <stdlib.h>
#include  "LiBOSASM_STRING.h"

#define MODRM_REG(R) ((R)<<3)

typedef struct _SYMBOLIC_LABEL {
	char*        string;
	unsigned int address;
} SYMBOLIC_LABEL;

typedef struct _NUMERIC_TOKEN {
	char*        string;
	unsigned int PC;
} NUMERIC_TOKEN;

typedef struct _IMAGE_FILE_MEMORY {
	void*        physical_origin;
	unsigned int total_sizeof_image;
	
} IMAGE_FILE_MEMORY;

typedef struct _TRIPLE_PACKET {
	char str1[128];
	char str2[128];
	char str3[128];
	unsigned char mod1;
	unsigned char mod2;
	unsigned char mod3;
} TRIPLE_PACKET;

typedef enum _INSTRUCTION_ATTRIBUTE {
	OP_REG_REG = 0x00,  /* source: register, destination: register  ex. mov eax, edi                */ 
	OP_REG_IMM = 0x01,  /* source: register, destination: immediate ex. mov eax, 0xAABBCCDD         */ 
	OP_REG_MEM = 0x02,  /* source: register, destination: memory    ex. mov eax, DWORD[address]     */ 
	OP_MEM_REG = 0x03,  /* source: memory,   destination: register  ex. mov DWORD[address], eax     */ 
	OP_MEM_IMM = 0x04,  /* source: memory  , destination: immediate ex. mov DWORD[addr], 0xAABBCCDD */ 
	/* 0x05 was reserved for the OS                                                                 */ 
	OP_MEM_MEM = 0xFF,  /* forbidden                                                                */ 
	OP_REG     = 0x06,  /* source: register                         ex. push ebp                    */
	OP_MEM     = 0x07,  /* source: memory                           ex. pop DWORD[addr]             */
	OP_IMM     = 0x08,  /* source: immediate                        ex. push 0xFDDAA45              */ 
	OP         = 0x09,  /* source: --,       destination: --        ex. ret                         */ 
} INSTRUCTION_ATTRIBUTE;

typedef struct _OPCODE {
	char*                 mnemonic;
	INSTRUCTION_ATTRIBUTE attribute;
	unsigned char         base_binary_code;
} OPCODE;

typedef enum {
    DATA_TYPE_BYTE    = 1,
    DATA_TYPE_WORD    = 2,
    DATA_TYPE_DWORD   = 4,
    DATA_TYPE_QWORD   = 8,
    DATA_TYPE_XMMWORD = 16,
} DATA_TYPE;

typedef struct _DATA_SECTION_ENTRIES {
    char           data_name[128];
    unsigned int   data_size;
    unsigned char* data_buffer;
    DATA_TYPE      data_type;
} DATA_SECTION_ENTRIES;

typedef struct _EXT_HEADER {
	char         id    [128];
	unsigned int sizeof_writer;
    char         writer[128];
	unsigned int sizeof_date;
	char         date  [128];
	unsigned int start_of_code;
	unsigned int start_of_data;
	unsigned int start_of_modules;
} EXT_HEADER;

enum REG_32 {
	IMM_EAX,
	IMM_ECX,
	IMM_EDX,
	IMM_EBX,
	IMM_ESP,
	IMM_EBP,
	IMM_ESI,
	IMM_EDI
};

enum REG_16 {
	IMM_AX,
	IMM_CX,
	IMM_DX,
	IMM_BX,
	IMM_SP,
	IMM_BP,
	IMM_SI,
	IMM_DI
};

enum REG_8 {
	IMM_AL,
	IMM_CL,
	IMM_DL,
	IMM_BL,
	IMM_AH,
	IMM_CH,
	IMM_DH,
	IMM_BH
};

enum MOD_RM {
	MEM_TO_REG   =  (1<<1),
	REG_TO_MEM   = ~(1<<1),
	_16_32       =  (1<<0),
	_8           = ~(1<<0),
	MODRM_DISP32 =   0x05
};

enum PARSE_LEVEL {
	PARSE_LEVEL_0,
	PARSE_LEVEL_1,
	PARSE_LEVEL_2,
};

enum SINGLET_INSTRUCTION_IDS {
	SINGLET_INSTRUCTION_ID_RET = (1<<0),
	SINGLET_INSTRUCTION_ID_CLD = (1<<1),
	SINGLET_INSTRUCTION_ID_STD = (1<<2),
	SINGLET_INSTRUCTION_ID_NOP = (1<<3),
	SINGLET_INSTRUCTION_ID_HLT = (1<<4),
	SINGLET_INSTRUCTION_ID_STI = (1<<5),
	SINGLET_INSTRUCTION_ID_CLI = (1<<6),
};

typedef void(*singlet_instructions_handler)(TRIPLE_PACKET* tp, unsigned int* PC);

//....................................................................................................................................

unsigned int    is_numeric_token                              (const char* str);
unsigned int    in_opcodes                                    (const char* str);
unsigned int    memory_operands                               (const char* str);
unsigned int    is_immediate                                  (const char* str);
unsigned int    is_label                                      (const char* str);
void            map_machine_codes                             (TRIPLE_PACKET* tp);
TRIPLE_PACKET*  alloc_units                                   (unsigned int lines);
void            _stub                                         (TRIPLE_PACKET* tp, unsigned int counter, char* tmp);
void            lex                                           (TRIPLE_PACKET* tp, const char* line);
void            dump_table_of_labels                          (void);
unsigned int    get_sizeof_opcodes                            (void);
OPCODE*         get_opcodes                                   (void);
unsigned int    get_table_of_labels_count                     (void);
SYMBOLIC_LABEL* get_table_of_labels                           (void);
unsigned char*  get_output_beffer                             (void);
void            parse_1_or__convert_instructions_line_by_line (TRIPLE_PACKET* tp, unsigned int counts);
void            convert_mov_instruction                       (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_add_instruction                       (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_sub_instruction                       (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_jmp_instruction                       (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_jnz_instruction                       (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_jz_instruction                        (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_cmp_instruction                       (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_jne_instruction                       (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_je_instruction                        (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_xor_instruction                       (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_pop_instruction                       (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_push_instruction                      (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_int_instruction                       (TRIPLE_PACKET* tp, unsigned int* PC);
void            convert_singlets_instruction                  (TRIPLE_PACKET* tp, unsigned int id, unsigned int* PC);
void            parse_0                                       (const char* file, TRIPLE_PACKET** tp, unsigned int* lines, char* p);
void            parse_2                                       (TRIPLE_PACKET* tp, unsigned int counts);
void            handle_labels                                 (TRIPLE_PACKET* tp, unsigned int* PC);
void            handle_data_section                           (TRIPLE_PACKET* tp);
void            handle_numeric_table                          (TRIPLE_PACKET* tp);
void            zero_programCounter                           (void);
void            zero_data_section_identifier                  (void);
unsigned int    get_programCounter                            (void);
unsigned int    get_parse_level                               (void);
void            image_file_make                               (TRIPLE_PACKET* tp, unsigned int counts, IMAGE_FILE_MEMORY* image_file_memory);
void            dump_output_buffer                            (void);
void            dump_data_section_table_entries               (void);
void            dump_numeric_table                            (void);
void            print_file                                    (char* file);

#endif // !__LIBOSASM__H__
