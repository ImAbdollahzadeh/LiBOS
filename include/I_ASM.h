#ifndef _I_ASM__H__
#define _I_ASM__H__

#include "LiBOS_CORE.h"

// Input Reader Address
#define __IRA                 0x01000000
#define __IRA_SIZE            0x073FFFFC

// Output Address
#define __OA                  0x07400000
#define __OA _SIZE            0x0D7FFFFC

// Handlers Address
#define __HA                  0x0D800000
#define __HA_SIZE             0x0D818FFC

// Internal Structures and Buffers Address
#define __ISBA                0x0D819000
#define __ISBA_SIZE           0x13C18FFC
#define __ISBA_TPs            0x0D819000
#define __ISBA_LABs           0x0DC19000
#define __ISBA_DATs           0x0DC3C000
#define __ISBA_NUMs           0x0DC5F000
#define __ISBA_STR_HEX        0x0DC82000
#define __ISBA_LINE_PTR       0x0DC9B000
#define __ISBA_GLOB_MN_CNT    0x0DC9B004
#define __ISBA_BOOLEAN        0x0DC9B008
#define __ISBA_ORIGIN         0x0DC9B00C
#define __ISBA_xx             0x0DC9B010
#define __ISBA_DBG_TP_MODs    0x0DC9B014
#define __ISBA_LEX_128_BUFF1  0x0DCDB96C
#define __ISBA_LEX_128_BUFF2  0x0DCDB9EC
#define __ISBA_CONTAIN_BUFF   0x0DCDB9F0
#define __ISBA_HEX_LOOKUP_TAB 0x0DCDB9F4
#define __ISBA_GLOB_MN_ADDR   0x0DCDBA14
#define __ISBA_ENDIAN_SWAP    0x0DCDBE14

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern void execute_IRA (void);
       void enter_I_ASM (UINT_8* file_string);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _I_ASM__H__
