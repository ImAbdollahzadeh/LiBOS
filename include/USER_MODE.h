#ifndef USERMODE__H__
#define USERMODE__H__

extern void    enter_user_mode   (UINT_32 usermode_entry_point);
       void    initiate_usermode (void);

extern void asm_user_int_0x80(UINT_32 eax, UINT_32 ebx, UINT_32 ecx);
extern void asm_user_iretd(void);

#endif
