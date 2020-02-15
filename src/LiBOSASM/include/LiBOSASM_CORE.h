#ifndef __LIBOSASM__H__
#define __LIBOSASM__H__

#include  <stdio.h>
#include  <stdlib.h>
#include  "LiBOSASM_STRING.h"

#define MODRM_REG(R) ((R)<<3)

typedef struct _SYMBOLIC_LABEL {
    char* string;
    void* address;
} SYMBOLIC_LABEL;

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

enum ATTRIBUTE {
    ATT_DIGIT,
    ATT_r_plus,
    ATT_r,
    ATT_MEMORY_IMM,
	ATT_REG_REG,
};

typedef struct _OPCODE {
    char* mnemonic;
    enum ATTRIBUTE attribute;
    unsigned char base_binary_code;
} OPCODE;

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
    MEM_TO_REG   = (1<<1),
    REG_TO_MEM   = ~(1<<1),
    _16_32       = (1<<0),
    _8           = ~(1<<0),
    MODRM_DISP32 = 0x05
};

enum PARSE_LEVEL {
    PARSE_LEVEL_0,
	PARSE_LEVEL_1,
	PARSE_LEVEL_2,
};

//....................................................................................................................................

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
void            parse_0                                       (const char* file, TRIPLE_PACKET** tp, unsigned int* lines, char* p);
void            parse_2                                       (TRIPLE_PACKET* tp, unsigned int counts);
void            handle_labels                                 (TRIPLE_PACKET* tp, unsigned int* PC);
void            zero_programCounter                           (void);
unsigned int    get_programCounter                            (void);
unsigned int    get_parse_level                               (void);
void            image_file_make                               (TRIPLE_PACKET* tp, unsigned int counts, IMAGE_FILE_MEMORY* image_file_memory);

#endif // !__LIBOSASM__H__
