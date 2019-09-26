#ifndef _MOUSE__H__
#define _MOUSE__H__

#include "LiBOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _USB_MOUSE {
	UINT_8* up_pointer;
	UINT_8* back_pointer;
	void(*usb_mouse_pointer_blitter)(UINT_8* report_packet);
	UINT_32 reserved;
} __attribute__((packed)) USB_MOUSE;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 RegisterMouse(USB_MOUSE* usb_mouse);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  
#endif //! _MOUSE__H__
