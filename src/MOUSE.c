#include "../include/MOUSE.h"
#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

static UINT_8 usb_mouse_up_pointer[384];
static UINT_8 usb_mouse_back_pointer[384];

void fast_pointer_blitter(UINT_8*);

UINT_32 RegisterMouse(USB_MOUSE* usb_mouse)
{
	if(!usb_mouse)
	{
		panic("Invalid usb_mouse object passed\n");
		return 0;
	}
	
	//printk( "        >>> USB Mouse registered successfully <<<\n");
	
	__LiBOS_MemZero((void*)usb_mouse_up_pointer, 384);
	__LiBOS_MemZero((void*)usb_mouse_back_pointer, 384);
	
	usb_mouse->up_pointer                = usb_mouse_up_pointer;
	usb_mouse->back_pointer              = usb_mouse_back_pointer;
	usb_mouse->usb_mouse_pointer_blitter = fast_pointer_blitter;
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void fast_pointer_blitter(UINT_8* packet_report)
{
	// suppose screen buffer is W * H * 4 bytes
	// I have current_x and current_y values
	// UINT_8* target = *(UINT_8*)(&screen[(current_y * 4 * W) + (current_x * 4)]);
	// blit target into back_pointer
	// get x, y values from report_packet and update current_x and current_y
	// target = *(UINT_8*)(&screen[(current_y * 4 * W) + (current_x * 4)]);
	// blit target into back_pointer
	// blit up_pointer into target
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

