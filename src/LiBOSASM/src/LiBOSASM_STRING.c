#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

unsigned char hex_table_codes[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

//....................................................................................................................................

unsigned int how_many_lines(const char* file)
{
    char* p = file;
    unsigned int sz = 0, lines = 0;
    while(*p++)
        sz++;
    p = file;
    while(sz--)
    {
        if(*p == '\n')
            lines++;
        p++;        
    }
    return lines;
}

//....................................................................................................................................

unsigned int string_length(const char* s)
{
    unsigned int ret = 0;
    char* p = s;
    while(*p++)
        ret++;
    return ret;
}

//....................................................................................................................................

unsigned int _strcmp(const char* instruction_name, char* trg)
{
    unsigned int instruction_size = 0, target_size = 0;
    char* src = instruction_name;
    char* target = trg;
    
    while(*src++) 
        instruction_size++;
    
    while(*target++) 
        target_size++;
    
    src    = instruction_name;
    target = trg;
    
    if(instruction_size != target_size)
        return 0;
    
    while(instruction_size--)
    {
        if(*src++ != *target++)
            return 0;
    }
    return 1;
}

//....................................................................................................................................

unsigned int _contain(char* string, const char* substring)
{
    unsigned int substring_size = 0;
    unsigned int    string_size = 0;
    char* src = substring;
    char* trg = string;
    
    while(*src++) 
        substring_size++;
    
    while(*trg++) 
        string_size++;
    
    src = substring;
    trg = string;
    
    if(!substring_size || !string_size)
        return 0;
    
    unsigned int states = string_size - substring_size;
    unsigned int sliding_counter = 0;
    unsigned int alias_substring_size = substring_size;
    unsigned int found = 1;
    while(states)
    {
        trg = &string[sliding_counter];
        while(alias_substring_size--)
        {
            if(*src++ != *trg++)
                found = 0;
        }        
        
        if(found)
            return 1;
        states--;
        sliding_counter++;
        src = substring;
        alias_substring_size = substring_size;
        found = 1;
    }
    
    return 0;
}

//....................................................................................................................................

unsigned int which_entry_from_hex_table(char ch)
{
    unsigned int _entry = 0;
    while(_entry < 0x10)
    {
        if(ch == hex_table_codes[_entry])
            return _entry;
        _entry++;
    }
    return 0xFFFFFFFF;
}

//....................................................................................................................................

unsigned int __pow(unsigned int base, unsigned int power)
{
    unsigned int ret = 1;
    while(power)
    {
        ret *= base;
        power--;
    }
    return ret;
}

//....................................................................................................................................

unsigned int immediate32_string_to_hex(const char* s)
{
    unsigned int ret = 0;
    ret |= ( which_entry_from_hex_table(s[8]) * __pow(16, 7) );
    ret |= ( which_entry_from_hex_table(s[9]) * __pow(16, 6) );
    ret |= ( which_entry_from_hex_table(s[6]) * __pow(16, 5) );
    ret |= ( which_entry_from_hex_table(s[7]) * __pow(16, 4) );
    ret |= ( which_entry_from_hex_table(s[4]) * __pow(16, 3) );
    ret |= ( which_entry_from_hex_table(s[5]) * __pow(16, 2) );
    ret |= ( which_entry_from_hex_table(s[2]) * __pow(16, 1) );
    ret |= ( which_entry_from_hex_table(s[3]) * __pow(16, 0) );
    //printf("0x%x ---", ret);
    return ret;
}

//....................................................................................................................................

unsigned short immediate16_string_to_hex(const char* s)
{
    unsigned int ret = 0;
    ret |= ( which_entry_from_hex_table(s[4]) * __pow(16, 3) );
    ret |= ( which_entry_from_hex_table(s[5]) * __pow(16, 2) );
    ret |= ( which_entry_from_hex_table(s[2]) * __pow(16, 1) );
    ret |= ( which_entry_from_hex_table(s[3]) * __pow(16, 0) );
    //printf("0x%x ---", ret);
    return (unsigned short)ret;
}

//....................................................................................................................................

unsigned char immediate8_string_to_hex(const char* s)
{
    unsigned int ret = 0;
    ret |= ( which_entry_from_hex_table(s[2]) * __pow(16, 1) );
    ret |= ( which_entry_from_hex_table(s[3]) * __pow(16, 0) );
    //printf("0x%x ---", ret);
    return (unsigned char)ret;
}

//....................................................................................................................................

void encode_u32(const char* s, char* src)
{
    src[0] = s[8];
    src[1] = s[9];
    src[2] = s[6];
    src[3] = s[7];
    src[4] = s[4];
    src[5] = s[5];
    src[6] = s[2];
    src[7] = s[3];
}

//....................................................................................................................................

void encode_u16(const char* s, char* src)
{
    src[0] = s[4];
    src[1] = s[5];
    src[2] = s[2];
    src[3] = s[3];
}

//....................................................................................................................................

void encode_u8(const char* s, char* src)
{
    src[0] = s[2];
    src[1] = s[3];
}

//....................................................................................................................................

void extract_from_memory_displacement_as_address(const char* s, char* dspl)
{
    unsigned int i;
    for(i=0;i<8;i++)
        dspl[i] = '0';
    
    unsigned char tmp[10];
    unsigned int sz = string_length(s);
    i = 0;
    while(i < sz)
    {
        if(s[i] == '[')
        {
            i++;
            unsigned int j = 0;
            while(s[i] != ']')
            {
                tmp[j] = s[i];
                j++;
                i++;
            }
            break;
        }
        i++;
    }
    
    if(tmp[0] == '0' && tmp[1] == 'x')
    {
        dspl[0] = tmp[8];
        dspl[1] = tmp[9];
        dspl[2] = tmp[6];
        dspl[3] = tmp[7];
        dspl[4] = tmp[4];
        dspl[5] = tmp[5];
        dspl[6] = tmp[2];
        dspl[7] = tmp[3];
    }
}

//....................................................................................................................................

void extract_from_memory_displacement8(const char* s, char* dspl)
{
    unsigned int i;
    for(i=0;i<2;i++)
        dspl[i] = '0';
    
    unsigned char tmp[10];
    unsigned int sz = string_length(s);
    i = 0;
    while(i < sz)
    {
        if(s[i] == '+')
        {
            i++;
            unsigned int j = 0;
            while(s[i] != ']')
            {
                tmp[j] = s[i];
                j++;
                i++;
            }
            break;
        }
        i++;
    }
    
    if(tmp[0] == '0' && tmp[1] == 'x')
    {
        dspl[0] = tmp[2];
        dspl[1] = tmp[3];
    }
}

//....................................................................................................................................

void extract_from_memory_displacement32(const char* s, char* dspl)
{
    unsigned int i;
    for(i=0;i<8;i++)
        dspl[i] = '0';
    
    unsigned char tmp[10];
    unsigned int sz = string_length(s);
    i = 0;
    while(i < sz)
    {
        if(s[i] == '+')
        {
            i++;
            unsigned int j = 0;
            while(s[i] != ']')
            {
                tmp[j] = s[i];
                j++;
                i++;
            }
            break;
        }
        i++;
    }
    
    if(tmp[0] == '0' && tmp[1] == 'x')
    {
        dspl[0] = tmp[8];
        dspl[1] = tmp[9];
        dspl[2] = tmp[6];
        dspl[3] = tmp[7];
        dspl[4] = tmp[4];
        dspl[5] = tmp[5];
        dspl[6] = tmp[2];
        dspl[7] = tmp[3];
    }
}

//....................................................................................................................................