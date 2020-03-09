#ifndef _MOUSE__H__
#define _MOUSE__H__

#include "LiBOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _USB_MOUSE {
	UINT_8* up_pointer;
	UINT_8* back_pointer;
	UINT_32 reserved;
} __attribute__((packed)) USB_MOUSE;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32    RegisterMouse                        (USB_MOUSE* usb_mouse);
void       fast_pointer_blitter                 (INT_8* report_packet, USB_MOUSE* usb_mouse, UINT_8 level);
void       usb_mouse_construct_up_mouse_pointer (void);
void       usb_mouse_blit_from_to               (UINT_8* from, UINT_8* to, UINT_32 byte, BOOL direction);
USB_MOUSE* get_usb_mouse_instance               (void);
void print_mouse_x_y(INT_32 x, INT_32 y);
UINT_8*    get_mouse_cursor_to_window_manager(void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  
#endif //! _MOUSE__H__
