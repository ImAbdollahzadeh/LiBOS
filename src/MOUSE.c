#include "../include/MOUSE.h"
#include "../include/PRINT.h"
#include "../include/SVGA.h"
#include "../include/TIMER.h"
#include "../include/WINDOW.h"

#define SIGNED_8_BIT(UNSIGNED_8_BIT) (INT_8)((UINT_8)(UNSIGNED_8_BIT ^ 0xFF))

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

static USB_MOUSE* global_usb_mouse = 0;
static INT_32 current_x_pos   = 0;
static INT_32 current_y_pos   = 0;

static UINT_8 usb_mouse_up_pointer[1024];
static UINT_8 usb_mouse_back_pointer[1024];

static UINT_32 _blank[35] = {
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static UINT_32 _0[35] = {
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

static UINT_32 _1[35] = {
	0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static UINT_32 _2[35] = {
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

static UINT_32 _3[35] = {
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

static UINT_32 _4[35] = {
	0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
};

static UINT_32 _5[35] = {
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

static UINT_32 _6[35] = {
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

static UINT_32 _7[35] = {
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
};

static UINT_32 _8[35] = {
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

static UINT_32 _9[35] = {
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
	0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 RegisterMouse(USB_MOUSE* usb_mouse)
{
	if(!usb_mouse)
	{
		panic("Invalid usb_mouse object passed\n");
		return 0;
	}
	
	//printk( "        >>> USB Mouse registered successfully <<<\n");
	
	__LiBOS_MemZero((void*)usb_mouse_up_pointer, 1024);
	__LiBOS_MemZero((void*)usb_mouse_back_pointer, 1024);
	
	usb_mouse->up_pointer   = usb_mouse_up_pointer;
	usb_mouse->back_pointer = usb_mouse_back_pointer;
	
	SVGA* svga              = get_svga_instance();
	UINT_8* fb              = svga->LFB;
	UINT_8* desktop_origin  = (UINT_8*)((void*)(&fb[0]));
	usb_mouse_blit_from_to(desktop_origin, usb_mouse->back_pointer, 1024, FALSE);
	usb_mouse_blit_from_to(desktop_origin, usb_mouse->up_pointer, 1024, FALSE);
	
	usb_mouse_construct_up_mouse_pointer();
	
	global_usb_mouse = usb_mouse;
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void fast_pointer_blitter(INT_8* report_packet, USB_MOUSE* usb_mouse, UINT_8 level)
{
	SVGA* svga = get_svga_instance();
	UINT_8* fb = svga->LFB;
	UINT_32 width = svga->width;
	UINT_32 height = svga->height;
	
	INT_8 x = report_packet[1];
	INT_8 y = report_packet[2];
	if(x & 0x80)
		x = -SIGNED_8_BIT(x);
	if(y & 0x80)
		y = -SIGNED_8_BIT(y);
	
	UINT_8* target = (UINT_8*)((void*)(&fb[4 * ((current_y_pos * width) + (current_x_pos))]));
	
	print_mouse_x_y(current_x_pos,current_y_pos);
	if(!level)
	{
		if(report_packet[0] & (1 << 0)) // a.k.a. LEFT CLICK PRESSED
			window_manager(report_packet, lclick_event, current_x_pos, current_y_pos);
		return;
	}
	
	window_manager(report_packet, move_event, current_x_pos, current_y_pos);
	
	usb_mouse_blit_from_to(usb_mouse->back_pointer, target, 1024, TRUE);
	
	current_x_pos += (x);                 
	current_y_pos += (y); 
	
	if(current_x_pos <= 0)         current_x_pos = 0; 
	if(current_x_pos >= (width-8)) current_x_pos = width-8;
	
	if(current_y_pos <= 0)          current_y_pos = 0; 
	if(current_y_pos >= (height-8)) current_y_pos = height-8;
	

	target = (UINT_8*)((void*)(&fb[4 * ((current_y_pos * width) + (current_x_pos))]));
	
	usb_mouse_blit_from_to(target, usb_mouse->back_pointer, 1024, FALSE);
	usb_mouse_blit_from_to(target, usb_mouse->up_pointer, 1024, FALSE);
	usb_mouse_construct_up_mouse_pointer();
	
	usb_mouse_blit_from_to(usb_mouse->up_pointer, target, 1024, TRUE);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void usb_mouse_construct_up_mouse_pointer(void)
{
	UINT_32 width = 64;
	UINT_32 i, j = 0;
	
	for(i=0; i<12; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i    ] = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=0; i<20; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=0; i<32; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=4; i<44; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=4; i<56; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=8; i<64; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=8; i<64; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=8; i<60; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=12; i<56; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=12; i<52; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=12; i<48; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=16; i<44; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=16; i<40; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=16; i<36; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=20; i<32; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	j++;
	for(i=20; i<28; i+=4)
	{
		usb_mouse_up_pointer[(j*width) + i]     = 0x00;
		usb_mouse_up_pointer[(j*width) + i + 1] = 0xFF;
		usb_mouse_up_pointer[(j*width) + i + 2] = 0xFF;
	}
	
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// direction: 0 -> screen to mouse // 1 -> mouse to screen
void usb_mouse_blit_from_to(UINT_8* from, UINT_8* to, UINT_32 byte, BOOL direction)
{
	UINT_32 i, j = 0;
	UINT_32 width = ( get_svga_instance() )->width;
	UINT_64* src = (UINT_64*)((void*)from);
	UINT_64* trg = (UINT_64*)((void*)to);
	
	if(direction)
	{
		while(j < 16)
		{
			*trg++ = *src++;
			*trg++ = *src++;
			*trg++ = *src++;
			*trg++ = *src++;
			*trg++ = *src++;
			*trg++ = *src++;
			*trg++ = *src++;
			*trg++ = *src++;
			trg    = (UINT_64*)((void*)(PHYSICAL_ADDRESS(trg) + (4*width) - 64));
			j++;
		}
		return;
	}
	while(j < 16)
	{
		*trg++ = *src++;
		*trg++ = *src++;
		*trg++ = *src++;
		*trg++ = *src++;
		*trg++ = *src++;
		*trg++ = *src++;
		*trg++ = *src++;
		*trg++ = *src++;
		src    = (UINT_64*)((void*)(PHYSICAL_ADDRESS(src) + (4*width) - 64));
		//for(i=0; i<64; i++)
		//	to[(j*64) + i] = from[(j*4*width) + i];
		j++;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

USB_MOUSE* get_usb_mouse_instance(void)
{
	return global_usb_mouse;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void draw_numeric(UINT_32* pixels, UINT_8* fb)
{
	UINT_32* trg = (UINT_32*)fb;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	 trg   = PHYSICAL_ADDRESS(trg) + 4096 - 28;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	 trg   = PHYSICAL_ADDRESS(trg) + 4096 - 28;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	 trg   = PHYSICAL_ADDRESS(trg) + 4096 - 28;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	 trg   = PHYSICAL_ADDRESS(trg) + 4096 - 28;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	*trg++ = *pixels++;
	 trg   = PHYSICAL_ADDRESS(trg) + 4096 - 28;

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void numeric_print_0(UINT_8 num, UINT_8* fb, UINT_32 offset)
{
	
	
	UINT_8* trg = (fb + 4096 - 28) - (offset * 28);
	switch(num)
	{
		case 0: draw_numeric(_0,trg); break;
		case 1: draw_numeric(_1,trg); break;
		case 2: draw_numeric(_2,trg); break;
		case 3: draw_numeric(_3,trg); break;
		case 4: draw_numeric(_4,trg); break;
		case 5: draw_numeric(_5,trg); break;
		case 6: draw_numeric(_6,trg); break;
		case 7: draw_numeric(_7,trg); break;
		case 8: draw_numeric(_8,trg); break;
		case 9: draw_numeric(_9,trg); break;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void numeric_print_1(UINT_8 num, UINT_8* fb, UINT_32 offset)
{
	
	UINT_8* trg = (fb + (12*4096) - 28) - (offset * 28);
	switch(num)
	{
		case 0: draw_numeric(_0,trg); break;
		case 1: draw_numeric(_1,trg); break;
		case 2: draw_numeric(_2,trg); break;
		case 3: draw_numeric(_3,trg); break;
		case 4: draw_numeric(_4,trg); break;
		case 5: draw_numeric(_5,trg); break;
		case 6: draw_numeric(_6,trg); break;
		case 7: draw_numeric(_7,trg); break;
		case 8: draw_numeric(_8,trg); break;
		case 9: draw_numeric(_9,trg); break;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void print_mouse_x_y(INT_32 xx, INT_32 yy)
{
	UINT_32 i = 0;
	UINT_32 x = (UINT_32)xx;
	UINT_32 y = (UINT_32)yy;
	UINT_8* fb = get_svga_instance()->LFB;
	
	draw_numeric(_blank,(fb + 4096 - 28)); 
	draw_numeric(_blank,(fb + 4096 - 56)); 
	draw_numeric(_blank,(fb + 4096 - 84)); 
	draw_numeric(_blank,(fb + 4096 - 112)); 
	draw_numeric(_blank,(fb + 4096 - 140)); 
	draw_numeric(_blank,(fb + 4096 - 168)); 
	draw_numeric(_blank,(fb + 4096 - 196)); 
	draw_numeric(_blank,(fb + (12*4096) - 28)); 
	draw_numeric(_blank,(fb + (12*4096) - 56)); 
	draw_numeric(_blank,(fb + (12*4096) - 84)); 
	draw_numeric(_blank,(fb + (12*4096) - 112)); 
	draw_numeric(_blank,(fb + (12*4096) - 140)); 
	draw_numeric(_blank,(fb + (12*4096) - 168)); 
	draw_numeric(_blank,(fb + (12*4096) - 196));
	
	UINT_8 rem = x%10;
	while(x)
	{
		switch (rem)
		{
			case 0:
				numeric_print_0(0, fb, i);
				i++;
				break;
			case 1:
				numeric_print_0(1, get_svga_instance()->LFB, i);
				i++;
				break;
			case 2:
				numeric_print_0(2, get_svga_instance()->LFB, i);
				i++;
				break;
			case 3:
				numeric_print_0(3, get_svga_instance()->LFB, i);
				i++;
				break;
			case 4:
				numeric_print_0(4, get_svga_instance()->LFB, i);
				i++;
				break;
			case 5:
				numeric_print_0(5, get_svga_instance()->LFB, i);
				i++;
				break;
			case 6:
				numeric_print_0(6, get_svga_instance()->LFB, i);
				i++;
				break;
			case 7:
				numeric_print_0(7, get_svga_instance()->LFB, i);
				i++;
				break;
			case 8:
				numeric_print_0(8, get_svga_instance()->LFB, i);
				i++;
				break;
			case 9:
				numeric_print_0(9, get_svga_instance()->LFB, i);
				i++;
				break;
		}
		x/=10;
		rem = x%10;
	}
	i = 0;
	rem = y%10;
	while(y)
	{
		switch (rem)
		{
			case 0:
				numeric_print_1(0, get_svga_instance()->LFB, i);
				i++;
				break;
			case 1:
				numeric_print_1(1, get_svga_instance()->LFB, i);
				i++;
				break;
			case 2:
				numeric_print_1(2, get_svga_instance()->LFB, i);
				i++;
				break;
			case 3:
				numeric_print_1(3, get_svga_instance()->LFB, i);
				i++;
				break;
			case 4:
				numeric_print_1(4, get_svga_instance()->LFB, i);
				i++;
				break;
			case 5:
				numeric_print_1(5, get_svga_instance()->LFB, i);
				i++;
				break;
			case 6:
				numeric_print_1(6, get_svga_instance()->LFB, i);
				i++;
				break;
			case 7:
				numeric_print_1(7, get_svga_instance()->LFB, i);
				i++;
				break;
			case 8:
				numeric_print_1(8, get_svga_instance()->LFB, i);
				i++;
				break;
			case 9:
				numeric_print_1(9, get_svga_instance()->LFB, i);
				i++;
				break;
		}
		y/=10;
		rem = y%10;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
