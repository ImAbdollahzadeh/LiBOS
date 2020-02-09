#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

//....................................................................................................................................

void convert_mov_instruction(TRIPLE_PACKET* tp)
{
	printf("MOV DECODING\n\t");
	unsigned int sizeof_opcodes = get_sizeof_opcodes();
	OPCODE* opcodes = get_opcodes();
	
    if(tp->mod1 == 'o' && tp->mod2 == 'o' && tp->mod3 == 'I')
    {
        unsigned char prefix = 0;
        unsigned char opc    = 0;
        unsigned char immediate32[8];
        unsigned char immediate16[4];
        unsigned char immediate8 [2];
        
        unsigned char  which_immediate = 0; // BIT(1) : 32, BIT(2) : 16, and BIT(3) : 8
        enum ATTRIBUTE attribute   = ATT_r_plus;
    
        int j;
        for(j=0;j<sizeof_opcodes;j++)
        {
            if( _strcmp(tp->str1, opcodes[j].mnemonic) )
            {
                if( attribute == opcodes[j].attribute )
                {
                    opc = opcodes[j].base_binary_code;
                    if( _strcmp(tp->str2, "eax") )
                    {
                        opc |= IMM_EAX;
                        which_immediate |= (1<<1);
                        encode_u32(tp->str3, immediate32);
                    }
                    else if( _strcmp(tp->str2, "ecx") )
                    {
                        opc |= IMM_ECX;
                        which_immediate |= (1<<1);
                        encode_u32(tp->str3, immediate32);
                    }
                    else if( _strcmp(tp->str2, "edx") )
                    {
                        opc |= IMM_EDX;
                        which_immediate |= (1<<1);
                        encode_u32(tp->str3, immediate32);
                    }
                    else if( _strcmp(tp->str2, "ebx") )
                    {
                        opc |= IMM_EBX;
                        which_immediate |= (1<<1);
                        encode_u32(tp->str3, immediate32);
                    }
                    else if( _strcmp(tp->str2, "esp") )
                    {
                        opc |= IMM_ESP;
                        which_immediate |= (1<<1);
                        encode_u32(tp->str3, immediate32);
                    }
                    else if( _strcmp(tp->str2, "ebp") )
                    {
                        opc |= IMM_EBP;
                        which_immediate |= (1<<1);
                        encode_u32(tp->str3, immediate32);
                    }
                    else if( _strcmp(tp->str2, "esi") )
                    {
                        opc |= IMM_ESI;
                        which_immediate |= (1<<1);
                        encode_u32(tp->str3, immediate32);
                    }
                    else if( _strcmp(tp->str2, "edi") )
                    {
                        opc |= IMM_EDI;
                        which_immediate |= (1<<1);
                        encode_u32(tp->str3, immediate32);
                    }
                    else if( _strcmp(tp->str2, "ax") )
                    {
                        prefix = 0x66;
                        opc |= IMM_AX;
                        which_immediate |= (1<<2);
                        encode_u16(tp->str3, immediate16);
                    }
                    else if( _strcmp(tp->str2, "bx") )
                    {
                        prefix = 0x66;
                        opc |= IMM_BX;
                        which_immediate |= (1<<2);
                        encode_u16(tp->str3, immediate16);
                    }
                    else if( _strcmp(tp->str2, "cx") )
                    {
                        prefix = 0x66;
                        opc |= IMM_CX;
                        which_immediate |= (1<<2);
                        encode_u16(tp->str3, immediate16);
                    }
                    else if( _strcmp(tp->str2, "dx") )
                    {
                        prefix = 0x66;
                        opc |= IMM_DX;
                        which_immediate |= (1<<2);
                        encode_u16(tp->str3, immediate16);
                    }
                    else if( _strcmp(tp->str2, "sp") )
                    {
                        prefix = 0x66;
                        opc |= IMM_SP;
                        which_immediate |= (1<<2);
                        encode_u16(tp->str3, immediate16);
                    }
                    else if( _strcmp(tp->str2, "bp") )
                    {
                        prefix = 0x66;
                        opc |= IMM_BP;
                        which_immediate |= (1<<2);
                        encode_u16(tp->str3, immediate16);
                    }
                    else if( _strcmp(tp->str2, "si") )
                    {
                        prefix = 0x66;
                        opc |= IMM_SI;
                        which_immediate |= (1<<2);
                        encode_u16(tp->str3, immediate16);
                    }
                    else if( _strcmp(tp->str2, "di") )
                    {
                        prefix = 0x66;
                        opc |= IMM_DI;
                        which_immediate |= (1<<2);
                        encode_u16(tp->str3, immediate16);
                    }
                    else if( _strcmp(tp->str2, "al") )
                    {
                        opc -= 8;
                        opc |= IMM_AL;
                        which_immediate |= (1<<3);
                        encode_u8(tp->str3, immediate8);
                    }
                    else if( _strcmp(tp->str2, "cl") )
                    {
                        opc -= 8;
                        opc |= IMM_CL;
                        which_immediate |= (1<<3);
                        encode_u8(tp->str3, immediate8);
                    }
                    else if( _strcmp(tp->str2, "dl") )
                    {
                        opc -= 8;
                        opc |= IMM_DL;
                        which_immediate |= (1<<3);
                        encode_u8(tp->str3, immediate8);
                    }
                    else if( _strcmp(tp->str2, "bl") )
                    {
                        opc -= 8;
                        opc |= IMM_BL;
                        which_immediate |= (1<<3);
                        encode_u8(tp->str3, immediate8);
                    }
                    else if( _strcmp(tp->str2, "ah") )
                    {
                        opc -= 8;
                        opc |= IMM_AH;
                        which_immediate |= (1<<3);
                        encode_u8(tp->str3, immediate8);
                    }
                    else if( _strcmp(tp->str2, "ch") )
                    {
                        opc -= 8;
                        opc |= IMM_CH;
                        which_immediate |= (1<<3);
                        encode_u8(tp->str3, immediate8);
                    }
                    else if( _strcmp(tp->str2, "dh") )
                    {
                        opc -= 8;
                        opc |= IMM_DH;
                        which_immediate |= (1<<3);
                        encode_u8(tp->str3, immediate8);
                    }
                    else if( _strcmp(tp->str2, "bh") )
                    {
                        opc -= 8;
                        opc |= IMM_BH;
                        which_immediate |= (1<<3);
                        encode_u8(tp->str3, immediate8);
                    }
                    else {}
                    
                    if(prefix)
                        printf("prefix: 0x%x, ", prefix);
                    printf("opcode: 0x%x, ", opc);
                    if(which_immediate)
                    {
                        switch(which_immediate)
                        {
                            case 0x2: 
                                printf("imm: %c%c %c%c %c%c %c%c\n", 
                                       immediate32[0], immediate32[1], immediate32[2], immediate32[3],
                                       immediate32[4], immediate32[5], immediate32[6], immediate32[7]);
                            
                                //immediate32_string_to_hex(tp->str3);
                                break;
                            case 0x4:
                                printf("imm: %c%c %c%c\n", 
                                       immediate16[0], immediate16[1], immediate16[2], immediate16[3]);
                                break;
                            case 0x8:
                                printf("imm: %c%c\n", immediate8[0], immediate8[1]);
                                break;
                        } // end of switch
                    } // end of if
                } // end of if 
            } // end of if 
        } // end of for 
    } // end of if
    else if(tp->mod1 == 'o' && tp->mod2 == 'o' && tp->mod3 == 'M')
    {
        //printf("MEM\n");
        unsigned char prefix = 0;
        unsigned char opc    = 0;
        unsigned char modrm  = 0;
        unsigned char sib    = 0;
        unsigned char displacement32[8];
        unsigned char displacement16[4];
        unsigned char displacement8[2];
        unsigned char  which_displacement = 0; // BIT(1) : 32, BIT(2) : 16, and BIT(3) : 8
        enum ATTRIBUTE attribute = ATT_r;
    
        int j;
        for(j=0;j<sizeof_opcodes;j++)
        {
            if( _strcmp(tp->str1, opcodes[j].mnemonic) )
            {
                if( attribute == opcodes[j].attribute )
                {
                    opc = opcodes[j].base_binary_code;
                    if( _strcmp(tp->str2, "eax") )
                    {
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_EAX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "ecx") )
                    {
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_ECX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "edx") )
                    {
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_EDX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "ebx") )
                    {
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_EBX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "esp") )
                    {
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_ESP) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "ebp") )
                    {
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_EBP) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "esi") )
                    {
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_ESI) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "edi") )
                    {
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_EDI) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "ax") )
                    {
                        prefix = 0x66;
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_AX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "cx") )
                    {
                        prefix = 0x66;
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_CX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "dx") )
                    {
                        prefix = 0x66;
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_DX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "bx") )
                    {
                        prefix = 0x66;
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_BX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "sp") )
                    {
                        prefix = 0x66;
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_SP) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "bp") )
                    {
                        prefix = 0x66;
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_BP) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "si") )
                    {
                        prefix = 0x66;
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_SI) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "di") )
                    {
                        prefix = 0x66;
                        opc |= (MEM_TO_REG | _16_32);
                        modrm |= (MODRM_REG(IMM_DI) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "al") )
                    {
                        opc &= _8;
                        opc |= MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_AL) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "cl") )
                    {
                        opc &= _8;
                        opc |= MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_CL) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "dl") )
                    {
                        opc &= _8;
                        opc |= MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_CL) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "bl") )
                    {
                        opc &= _8;
                        opc |= MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_BL) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "ah") )
                    {
                        opc &= _8;
                        opc |= MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_AH) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "ch") )
                    {
                        opc &= _8;
                        opc |= MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_CH) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "dh") )
                    {
                        opc &= _8;
                        opc |= MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_DH) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str2, "bh") )
                    {
                        opc &= _8;
                        opc |= MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_BH) | MODRM_DISP32);
                    }
                    else {}
                    
                    if( _contain(tp->str3, "+") ) // now it MUST have a register along
                    {
                        modrm &= 0x38; //00111000
                        modrm |= (1<<7); // now with mode [MEMORY+DISP32]
                        if( _contain(tp->str3, "eax") )
                        {
                            modrm |= IMM_EAX;
                        }
                        if( _contain(tp->str3, "ecx") )
                        {
                            modrm |= IMM_ECX;
                        }
                        if( _contain(tp->str3, "edx") )
                        {
                            modrm |= IMM_EDX;
                        }
                        if( _contain(tp->str3, "ebx") )
                        {
                            modrm |= IMM_EBX;
                        }
                        if( _contain(tp->str3, "esp") )
                        {
                            modrm &= ~(0x07);
                            modrm |= 0x04;   // now SIB selected
                            
                            // SIB fields:
                            // scale  index  base
                            // --   | ---  | ---
                            // [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
                            // for us: [ESP] -> [0+ESP*1]
                            sib   |= ((IMM_ESP << 3) | IMM_ESP);
                        }
                        if( _contain(tp->str3, "ebp") )
                        {
                            modrm |= IMM_EBP;
                        }
                        if( _contain(tp->str3, "esi") )
                        {
                            modrm |= IMM_ESI;
                        }
                        if( _contain(tp->str3, "edi") )
                        {
                            modrm |= IMM_EDI;
                        }
                        
                        extract_from_memory_displacement32(tp->str3, displacement32);
                        which_displacement = (1<<1);
                    }
                    
                    else if( _contain(tp->str3, "0x") ) // a bare address
                    {
                        extract_from_memory_displacement_as_address(tp->str3, displacement32);
                        which_displacement = (1<<1);                            
                    }
                    
                    else // there is only REG r a LABEL
                    {
                        modrm &= 0x38; //00111000. Now with mode [MEMORY]
                        
                        if( _contain(tp->str3, "eax") )
                        {
                            modrm |= IMM_EAX;
                            goto EXIT_POSITION;
                        }
                        if( _contain(tp->str3, "ecx") )
                        {
                            modrm |= IMM_ECX;
                            goto EXIT_POSITION;
                        }
                        if( _contain(tp->str3, "edx") )
                        {
                            modrm |= IMM_EDX;
                            goto EXIT_POSITION;
                        }
                        if( _contain(tp->str3, "ebx") )
                        {
                            modrm |= IMM_EBX;
                            goto EXIT_POSITION;
                        }
                        if( _contain(tp->str3, "esp") )
                        {
                            modrm &= ~(0x07);
                            modrm |= 0x04;   // now SIB selected
                            
                            // SIB fields:
                            // scale  index  base
                            // --   | ---  | ---
                            // [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
                            // for us: [ESP] -> [0+ESP*1]
                            sib   |= ((IMM_ESP << 3) | IMM_ESP);
                            goto EXIT_POSITION;
                        }
                        if( _contain(tp->str3, "ebp") )
                        {
                            modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
                            modrm |= 0x05;   // now ebp selected
                            displacement8[0] = '0';
                            displacement8[1] = '0';
                            which_displacement = (1<<3);
                            goto EXIT_POSITION;
                        }
                        if( _contain(tp->str3, "esi") )
                        {
                            modrm |= IMM_ESI;
                            goto EXIT_POSITION;
                        }
                        if( _contain(tp->str3, "edi") )
                        {
                            modrm |= IMM_EDI;
                            goto EXIT_POSITION;
                        }
                        
                        // therefore there is a LABEL
                        modrm |= 0x05;
                        extract_from_memory_displacement_as_address(tp->str3, displacement32);
                        which_displacement = (1<<1);
                    }
EXIT_POSITION:                                                
                    if(prefix)
                        printf("prefix: 0x%x, ", prefix);
                    printf("opcode: 0x%x, ", opc);
                    printf("modrm: 0x%x, ", modrm);
                    if(sib)
                        printf("sib: 0x%x, ", sib);
                    switch(which_displacement)
                    {
                        case 0x1:
                            printf("\n");
                            break;
                        case 0x2: 
                            printf("displacement: %c%c %c%c %c%c %c%c\n", 
                                   displacement32[0], displacement32[1], displacement32[2], displacement32[3],
                                   displacement32[4], displacement32[5], displacement32[6], displacement32[7]);
                            break;
                        case 0x4:
                            printf("displacement: %c%c %c%c\n", 
                                   displacement16[0], displacement16[1], displacement16[2], displacement16[3]);
                            break;
                        case 0x8:
                            printf("displacement: %c%c\n", displacement8[0], displacement8[1]);
                            break;
                    } // end of switch
                } // end of if 
            } // end of if 
        } // end of for
    } // end of if
    else if(tp->mod1 == 'o' && tp->mod2 == 'M' && tp->mod3 == 'o')
    {
        //printf("MEM\n");
        unsigned char prefix = 0;
        unsigned char opc    = 0;
        unsigned char modrm  = 0;
        unsigned char sib    = 0;
        unsigned char displacement32[8];
        unsigned char displacement16[8];
        unsigned char which_displacement  = 0;
        unsigned char displacement8[8];
        enum ATTRIBUTE attribute = ATT_r;
    
        int j;
        for(j=0;j<sizeof_opcodes;j++)
        {
            if( _strcmp(tp->str1, opcodes[j].mnemonic) )
            {
                if( attribute == opcodes[j].attribute )
                {
                    opc = opcodes[j].base_binary_code;
                    if( _strcmp(tp->str3, "eax") )
                    {
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_EAX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "ecx") )
                    {
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_ECX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "edx") )
                    {
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_EDX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "ebx") )
                    {
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_EBX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "esp") )
                    {
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_ESP) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "ebp") )
                    {
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_EBP) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "esi") )
                    {
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_ESI) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "edi") )
                    {
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_EDI) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "ax") )
                    {
                        prefix = 0x66;
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_AX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "cx") )
                    {
                        prefix = 0x66;
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_CX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "dx") )
                    {
                        prefix = 0x66;
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_DX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "bx") )
                    {
                        prefix = 0x66;
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_BX) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "sp") )
                    {
                        prefix = 0x66;
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_SP) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "bp") )
                    {
                        prefix = 0x66;
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_BP) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "si") )
                    {
                        prefix = 0x66;
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_SI) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "di") )
                    {
                        prefix = 0x66;
                        opc |= _16_32;
                        opc &= ~MEM_TO_REG;
                        modrm |= (MODRM_REG(IMM_DI) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "al") )
                    {
                        opc &= _8;
                        opc |= REG_TO_MEM;
                        modrm |= (MODRM_REG(IMM_AL) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "cl") )
                    {
                        opc &= _8;
                        opc |= REG_TO_MEM;
                        modrm |= (MODRM_REG(IMM_CL) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "dl") )
                    {
                        opc &= _8;
                        opc |= REG_TO_MEM;
                        modrm |= (MODRM_REG(IMM_CL) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "bl") )
                    {
                        opc &= _8;
                        opc |= REG_TO_MEM;
                        modrm |= (MODRM_REG(IMM_BL) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "ah") )
                    {
                        opc &= _8;
                        opc |= REG_TO_MEM;
                        modrm |= (MODRM_REG(IMM_AH) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "ch") )
                    {
                        opc &= _8;
                        opc |= REG_TO_MEM;
                        modrm |= (MODRM_REG(IMM_CH) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "dh") )
                    {
                        opc &= _8;
                        opc |= REG_TO_MEM;
                        modrm |= (MODRM_REG(IMM_DH) | MODRM_DISP32);
                    }
                    else if( _strcmp(tp->str3, "bh") )
                    {
                        opc &= _8;
                        opc |= REG_TO_MEM;
                        modrm |= (MODRM_REG(IMM_BH) | MODRM_DISP32);
                    }
                    else {}
                    
                    if( _contain(tp->str2, "+") ) // now it MUST have a register along
                    {
                        modrm &= 0x38; //00111000
                        modrm |= (1<<7); // now with mode [MEMORY+DISP32]
                        if( _contain(tp->str2, "eax") )
                        {
                            modrm |= IMM_EAX;
                        }
                        if( _contain(tp->str2, "ecx") )
                        {
                            modrm |= IMM_ECX;
                        }
                        if( _contain(tp->str2, "edx") )
                        {
                            modrm |= IMM_EDX;
                        }
                        if( _contain(tp->str2, "ebx") )
                        {
                            modrm |= IMM_EBX;
                        }
                        if( _contain(tp->str2, "esp") )
                        {
                            modrm &= ~(0x07);
                            modrm |= 0x04;   // now SIB selected
                            
                            // SIB fields:
                            // scale  index  base
                            // --   | ---  | ---
                            // [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
                            // for us: [ESP] -> [0+ESP*1]
                            sib   |= ((IMM_ESP << 3) | IMM_ESP);
                        }
                        if( _contain(tp->str2, "ebp") )
                        {
                            modrm |= IMM_EBP;
                        }
                        if( _contain(tp->str2, "esi") )
                        {
                            modrm |= IMM_ESI;
                        }
                        if( _contain(tp->str2, "edi") )
                        {
                            modrm |= IMM_EDI;
                        }
                        
                        extract_from_memory_displacement32(tp->str2, displacement32);
                        which_displacement = (1<<1);
                    }
                    
                    else if( _contain(tp->str2, "0x") ) // a bare address
                    {
                        extract_from_memory_displacement_as_address(tp->str2, displacement32);
                        which_displacement = (1<<1);                            
                    }
                    
                    else // there is only REG r a LABEL
                    {
                        modrm &= 0x38; //00111000. Now with mode [MEMORY]
                        
                        if( _contain(tp->str2, "eax") )
                        {
                            modrm |= IMM_EAX;
                            goto EXIT_POSITION2;
                        }
                        if( _contain(tp->str2, "ecx") )
                        {
                            modrm |= IMM_ECX;
                            goto EXIT_POSITION2;
                        }
                        if( _contain(tp->str2, "edx") )
                        {
                            modrm |= IMM_EDX;
                            goto EXIT_POSITION2;
                        }
                        if( _contain(tp->str2, "ebx") )
                        {
                            modrm |= IMM_EBX;
                            goto EXIT_POSITION2;
                        }
                        if( _contain(tp->str2, "esp") )
                        {
                            modrm &= ~(0x07);
                            modrm |= 0x04;   // now SIB selected
                            
                            // SIB fields:
                            // scale  index  base
                            // --   | ---  | ---
                            // [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
                            // for us: [ESP] -> [0+ESP*1]
                            sib   |= ((IMM_ESP << 3) | IMM_ESP);
                            goto EXIT_POSITION2;
                        }
                        if( _contain(tp->str2, "ebp") )
                        {
                            modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
                            modrm |= 0x05;   // now ebp selected
                            displacement8[0] = '0';
                            displacement8[1] = '0';
                            which_displacement = (1<<3);
                            goto EXIT_POSITION2;
                        }
                        if( _contain(tp->str2, "esi") )
                        {
                            modrm |= IMM_ESI;
                            goto EXIT_POSITION2;
                        }
                        if( _contain(tp->str2, "edi") )
                        {
                            modrm |= IMM_EDI;
                            goto EXIT_POSITION2;
                        }
                        
                        // therefore there is a LABEL
                        modrm |= 0x05;
                        extract_from_memory_displacement_as_address(tp->str2, displacement32);
                        which_displacement = (1<<1);
                    }
EXIT_POSITION2:                                                
                    if(prefix)
                        printf("prefix: 0x%x, ", prefix);
                    printf("opcode: 0x%x, ", opc);
                    printf("modrm: 0x%x, ", modrm);
                    if(sib)
                        printf("sib: 0x%x, ", sib);
                    switch(which_displacement)
                    {
                        case 0x1:
                            printf("\n");
                            break;
                        case 0x2: 
                            printf("displacement: %c%c %c%c %c%c %c%c\n", 
                                   displacement32[0], displacement32[1], displacement32[2], displacement32[3],
                                   displacement32[4], displacement32[5], displacement32[6], displacement32[7]);
                            break;
                        case 0x4:
                            printf("displacement: %c%c %c%c\n", 
                                   displacement16[0], displacement16[1], displacement16[2], displacement16[3]);
                            break;
                        case 0x8:
                            printf("displacement: %c%c\n", displacement8[0], displacement8[1]);
                            break;
                    } // end of switch
                } // end of if 
            } // end of if 
        } // end of for
    }
    else if(tp->mod1 == 'o' && tp->mod2 == 'M' && tp->mod3 == 'I')
    {
        //printf("MEM+IMM\n");
        unsigned char prefix = 0;
        unsigned char opc    = 0;
        unsigned char modrm  = 0;
        unsigned char sib    = 0;
        unsigned char immediate32[8];
        unsigned char immediate16[4];
        unsigned char immediate8 [2];
        unsigned char displacement32[8];
        unsigned char displacement16[4];
        unsigned char displacement8[2];
        
        unsigned char  which_immediate    = 0; // BIT(1) : 32, BIT(2) : 16, and BIT(3) : 8
        unsigned char  which_displacement = 0; // BIT(1) : 32, BIT(2) : 16, and BIT(3) : 8
        
        enum ATTRIBUTE attribute   = ATT_MEMORY_IMM;
    
        int j;
        for(j=0;j<sizeof_opcodes;j++)
        {
            if( _strcmp(tp->str1, opcodes[j].mnemonic) )
            {
                if( attribute == opcodes[j].attribute )
                {
                    opc = opcodes[j].base_binary_code;
                    if( _contain(tp->str2, "DWORD[") )
                    {
                        which_immediate |= (1<<1);
                        encode_u32(tp->str3, immediate32);
                        if( _contain(tp->str2, "eax") )
                        {
                            modrm |= IMM_EAX;
                        }
                        if( _contain(tp->str2, "ecx") )
                        {
                            modrm |= IMM_ECX;
                        }
                        if( _contain(tp->str2, "edx") )
                        {
                            modrm |= IMM_EDX;
                        }
                        if( _contain(tp->str2, "ebx") )
                        {
                            modrm |= IMM_EBX;
                        }
                        if( _contain(tp->str2, "esp") )
                        {
                            modrm |= 0x04;   // now SIB selected
                            
                            // SIB fields:
                            // scale  index  base
                            // --   | ---  | ---
                            // [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
                            // for us: [ESP] -> [0+ESP*1]
                            sib   |= ((IMM_ESP << 3) | IMM_ESP);
                        }
                        if( _contain(tp->str2, "ebp") )
                        {
                            modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
                            modrm |= 0x05;   // now ebp selected
                            displacement8[0] = '0';
                            displacement8[1] = '0';
                            which_displacement = (1<<3);
                        }
                        if( _contain(tp->str2, "esi") )
                        {
                            modrm |= IMM_ESI;
                        }
                        if( _contain(tp->str2, "edi") )
                        {
                            modrm |= IMM_EDI;
                        }
                        
                        if( _contain(tp->str2, "+") ) // it has a displacement
                        {
                            // for now suppose only 8-bit displacement
                            extract_from_memory_displacement8(tp->str2, displacement8);
                            which_displacement = (1<<3);
                        }
                    }
                    
                    else if( _contain(tp->str2, "WORD[") && (!_contain(tp->str2, "DWORD[")) )
                    {
                        which_immediate |= (1<<2);
                        encode_u16(tp->str3, immediate16);
                        prefix = 0x66;
                        if( _contain(tp->str2, "eax") )
                        {
                            modrm |= IMM_EAX;
                        }
                        if( _contain(tp->str2, "ecx") )
                        {
                            modrm |= IMM_ECX;
                        }
                        if( _contain(tp->str2, "edx") )
                        {
                            modrm |= IMM_EDX;
                        }
                        if( _contain(tp->str2, "ebx") )
                        {
                            modrm |= IMM_EBX;
                        }
                        if( _contain(tp->str2, "esp") )
                        {
                            modrm |= 0x04;   // now SIB selected
                            
                            // SIB fields:
                            // scale  index  base
                            // --   | ---  | ---
                            // [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
                            // for us: [ESP] -> [0+ESP*1]
                            sib   |= ((IMM_ESP << 3) | IMM_ESP);
                        }
                        if( _contain(tp->str2, "ebp") )
                        {
                            modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
                            modrm |= 0x05;   // now ebp selected
                            displacement8[0] = '0';
                            displacement8[1] = '0';
                            which_displacement = (1<<3);
                        }
                        if( _contain(tp->str2, "esi") )
                        {
                            modrm |= IMM_ESI;
                        }
                        if( _contain(tp->str2, "edi") )
                        {
                            modrm |= IMM_EDI;
                        }
                        
                        if( _contain(tp->str2, "+") ) // it has a displacement
                        {
                            modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
                            
                            // for now suppose only 8-bit displacement
                            extract_from_memory_displacement8(tp->str2, displacement8);
                            which_displacement = (1<<3);
                        }
                    }
                    
                    else if( _contain(tp->str2, "BYTE[") )
                    {
                        which_immediate |= (1<<3);
                        opc &= ~(1<<0);
                        encode_u8(tp->str3, immediate8);
                        if( _contain(tp->str2, "eax") )
                        {
                            modrm |= IMM_EAX;
                        }
                        if( _contain(tp->str2, "ecx") )
                        {
                            modrm |= IMM_ECX;
                        }
                        if( _contain(tp->str2, "edx") )
                        {
                            modrm |= IMM_EDX;
                        }
                        if( _contain(tp->str2, "ebx") )
                        {
                            modrm |= IMM_EBX;
                        }
                        if( _contain(tp->str2, "esp") )
                        {
                            modrm |= 0x04;   // now SIB selected
                            
                            // SIB fields:
                            // scale  index  base
                            // --   | ---  | ---
                            // [EBX+EDI*scale] an scale is either of 1,2,4,8. Here EBX is base, and EDI is index
                            // for us: [ESP] -> [0+ESP*1]
                            sib   |= ((IMM_ESP << 3) | IMM_ESP);
                        }
                        if( _contain(tp->str2, "ebp") )
                        {
                            modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
                            modrm |= 0x05;   // now ebp selected
                            displacement8[0] = '0';
                            displacement8[1] = '0';
                            which_displacement = (1<<3);
                        }
                        if( _contain(tp->str2, "esi") )
                        {
                            modrm |= IMM_ESI;
                        }
                        if( _contain(tp->str2, "edi") )
                        {
                            modrm |= IMM_EDI;
                        }
                        
                        if( _contain(tp->str2, "+") ) // it has a displacement
                        {
                            modrm |= (1<<6); // now the mode is [MEMORY+DISP8]
                            
                            // for now suppose only 8-bit displacement
                            extract_from_memory_displacement8(tp->str2, displacement8);
                            which_displacement = (1<<3);
                        }
                    }
                    
                    else {}
                    
                    if(prefix)
                        printf("prefix: 0x%x, ", prefix);
                    printf("opcode: 0x%x, ", opc);
                    printf("modrm: 0x%x, ", modrm);
                    if(sib)
                        printf("sib: 0x%x, ", sib);
                    switch(which_displacement)
                    {
                        case 0x2: 
                            printf("displacement: %c%c %c%c %c%c %c%c, ", 
                                   displacement32[0], displacement32[1], displacement32[2], displacement32[3],
                                   displacement32[4], displacement32[5], displacement32[6], displacement32[7]);
                            break;
                        case 0x4:
                            printf("displacement: %c%c %c%c, ", 
                                   displacement16[0], displacement16[1], displacement16[2], displacement16[3]);
                            break;
                        case 0x8:
                            printf("displacement: %c%c, ", displacement8[0], displacement8[1]);
                            break;
                    } // end of switch
                    
                    if(which_immediate)
                    {
                        switch(which_immediate)
                        {
                            case 0x2: 
                                printf("imm: %c%c %c%c %c%c %c%c\n", 
                                       immediate32[0], immediate32[1], immediate32[2], immediate32[3],
                                       immediate32[4], immediate32[5], immediate32[6], immediate32[7]);
                            
                                //immediate32_string_to_hex(tp->str3);
                                break;
                            case 0x4:
                                printf("imm: %c%c %c%c\n", 
                                       immediate16[0], immediate16[1], immediate16[2], immediate16[3]);
                                break;
                            case 0x8:
                                printf("imm: %c%c\n", immediate8[0], immediate8[1]);
                                break;
                        } // end of switch
                    } // end of if
                } // end of if 
            } // end of if 
        } // end of for 
    }
	printf("\n");
}

//....................................................................................................................................