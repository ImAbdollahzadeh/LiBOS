#ifndef __LIBOSASM_STRING__H__
#define __LIBOSASM_STRING__H__

unsigned int   how_many_lines                              (const char* file);
unsigned int   string_length                               (const char* s);
unsigned int   _strcmp                                     (const char* instruction_name, char* trg);
unsigned int   _contain                                    (char* string, const char* substring);
unsigned int   which_entry_from_hex_table                  (char ch);
unsigned int   __pow                                       (unsigned int base, unsigned int power);
unsigned int   immediate32_string_to_hex                   (const char* s);
unsigned short immediate16_string_to_hex                   (const char* s);
unsigned char  immediate8_string_to_hex                    (const char* s);
unsigned int   address_string_to_hex                       (const char* s);
void           encode_u128                                 (const char* s, char* src);
void           encode_u64                                  (const char* s, char* src);
void           encode_u32                                  (const char* s, char* src);
void           encode_u16                                  (const char* s, char* src);
void           encode_u8                                   (const char* s, char* src);
void           extract_from_memory_displacement_as_address (const char* s, char* dspl);
void           extract_from_memory_displacement8           (const char* s, char* dspl);
void           extract_from_memory_displacement32          (const char* s, char* dspl);
unsigned char  byte_string_to_byte                         (char*, char*);

#endif // !__LIBOSASM_STRING__H__
