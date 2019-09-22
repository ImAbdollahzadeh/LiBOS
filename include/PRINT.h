#ifndef _PRINT__H__
#define _PRINT__H__

#include "IMOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/* There are only 'TWO' formatted printk.
   The first one is to show a decimal value with single character %
   and the second is to show hexadecimal numeric value by single character ^
*/

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern void    printk       ( INT_8* fmt, ... );
       void    clear_screen ( void );
       void    panic        ( INT_8* id );
       void    mouse_pointer_update ( INT_8* report_packet );
       UINT_8* Disps        (void);
       void __IMOS_HexDump  (void* pointer_to_buffer, UINT_32 bytes, INT_8* begin_message);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _PRINT__H__
