/* THIS IS MAIN */

#include "../include/WINDOW.h"
#include "../include/SVGA.h"
#include "../include/MEMORY.h"
#include "../include/PORT.h"
#include "../include/TIMER.h"
#include "../include/MOUSE.h"
#include "../include/IDT.h"
#include "../include/FONT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

static PORT_8 p;
static UINT_32 number_of_registered_windows = 0;
static WINDOW* registered_windows[1024];
static WINDOW  desktop;
static WINDOW* focus = 0;
static WINDOW* list_of_overlapped_windows[1024];
static UINT_32 number_of_overlapped_windows = 1;
static UINT_8 off_screen[3145728] __attribute__ ((aligned(16))); //a.k.a. 3MB
static UINT_8* LFB = 0;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ WINDOW.c CORE DATA AND CODE

#define SIGNED_8_BIT(UNSIGNED_8_BIT) (INT_8)((UINT_8)(UNSIGNED_8_BIT ^ 0xFF))
#define ALIGN_16(N) (N+15) & (~15) 
#define ALIGN_8(N) (N+7) & (~7) 
#define ABSOLUTE(INT) ((INT>0)?INT:-INT)
void draw_window         (WINDOW* wnd, UINT_8* framebuffer);
void global_button_effect(INT_32 x, INT_32 y, UINT_32 which_object);

static void vsync(void)
{
	while ( (p.Read(0x03DA) & 0x08));
	while (!(p.Read(0x03DA) & 0x08));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void sse_blit_off_screen_to_screen(WINDOW* wnd)
{
	UINT_32 byte_offset = (wnd->rect.up_left.y << 12) + (wnd->rect.up_left.x << 2);
	
	UINT_8* on  = (UINT_8*)(&((LFB)     [byte_offset]));
	UINT_8* off = (UINT_8*)(&(off_screen[byte_offset]));

	UINT_32 h           = wnd->rect.height;
	UINT_32 w_16_blocks = (wnd->rect.width >> 2);
	
	while(h)
	{
		_sse_blit_off_screen_to_screen(off, on, w_16_blocks);
		on  += 0x8000;
		off += 0x8000;
		h -= 8;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void blit_off_screen_to_screen(WINDOW* wnd)
{
	UINT_32 byte_offset = (wnd->rect.up_left.y << 12) + (wnd->rect.up_left.x << 2);
	
	UINT_64* on  = (UINT_64*)(&((LFB)     [byte_offset]));
	UINT_64* off = (UINT_64*)(&(off_screen[byte_offset]));

	UINT_32 h = wnd->rect.height;
	UINT_32 w = wnd->rect.width; 
	UINT_32 i = w >> 1;
	
	UINT_32 stride_offset = 4096 - (w << 2);
	
	while(h)
	{
		while(i--)
			*on++ = *off++;
		i = w >> 1;
		on  = (UINT_64*)(PHYSICAL_ADDRESS(on)  + stride_offset);
		off = (UINT_64*)(PHYSICAL_ADDRESS(off) + stride_offset);
		h--;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static BOOL overlap(WINDOW* src, WINDOW* trg)
{
	if(((src->rect.up_left.x + src->rect.width) < trg->rect.up_left.x) || (trg->rect.up_left.x + trg->rect.width) < src->rect.up_left.x)
        if(((src->rect.up_left.y + src->rect.height) < trg->rect.up_left.y) || (trg->rect.up_left.y + trg->rect.height) < src->rect.up_left.y)
            return FALSE;
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void add_to_z(WINDOW* wnd)
{
	UINT_32 i;
	for(i = 0; i < number_of_registered_windows; i++)
	{
		if( overlap(registered_windows[i], wnd) )
			wnd->z_order++;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static WINDOW* find_responsive_window(INT_32 x, INT_32 y)
{
	UINT_32 i;
	UINT_32 target_window_number = 0xFFFFFFFF;
	UINT_32 max_z                = 0;
	WINDOW* src                  = 0;
	
	for(i = 0; i < number_of_registered_windows; i++)
	{
		src = registered_windows[i];
		if((src->rect.up_left.x <= x) && ((src->rect.up_left.x + src->rect.width) >= x))
		{
			if((src->rect.up_left.y <= y) && ((src->rect.up_left.y + src->rect.height) >= y))
			{
				if(!i)
				{
					max_z = src->z_order;
					target_window_number = 0;
				}
				
				else if(max_z <= src->z_order)
				{
					max_z = src->z_order;
					target_window_number = i;
				}
			}
		}
	}
	
	if(target_window_number != 0xFFFFFFFF)
		return registered_windows[target_window_number];
	return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void sort_list_of_overlapped_windows(void)
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void draw_partial_desktop(RECT* r)
{
	UINT_32 row_blocks = (r->width >> 1);
	UINT_32 byte_offset = (r->up_left.y << 12) + (r->up_left.x << 2);
	UINT_64* src       = (UINT_64*)(&(desktop.buffer[byte_offset]));
	UINT_64* dst       = (UINT_64*)(&(off_screen    [byte_offset]));
	UINT_32 h          = r->height;
	INT_32 i           = row_blocks;
	while(h)
	{
		while(i--)
			*dst++ = *src++;
		i   = row_blocks;
		dst = (UINT_64*)(PHYSICAL_ADDRESS(dst) + 4096 - (r->width << 2));
		h--;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void draw_list_of_overlapped_windows(WINDOW* focused_window)
{
	if(!focused_window)
		return;
	
	UINT_32 i = 1;
	while(i < number_of_overlapped_windows)
	{
		draw_window(list_of_overlapped_windows[i], off_screen);
		i++;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void move_handler(WINDOW* wnd, INT_32 delta_x, INT_32 delta_y)
{
	/* FIND WINDOWS THAT MUST PAINT BACK OF THE FOCUS WINDOW. IT HAS BEEN ALREADY DONE IN LCLICK EVENT HANDLER */

	//sort_list_of_overlapped_windows(); /*TODO*/
	WINDOW big_rect;
	WINDOW small_rect;
	big_rect.rect.up_left.x = (delta_x > 0) ? wnd->rect.up_left.x : wnd->rect.up_left.x + delta_x;
	big_rect.rect.up_left.y = (delta_y > 0) ? wnd->rect.up_left.y : wnd->rect.up_left.y + delta_y;
	big_rect.rect.width     = ALIGN_16(wnd->rect.width  + ABSOLUTE(delta_x));
	big_rect.rect.height    = ALIGN_8(wnd->rect.height + ABSOLUTE(delta_y));

	UINT_32 X  = ALIGN_16(big_rect.rect.up_left.x);
	
	if(X > big_rect.rect.up_left.x) 
	{
		big_rect.rect.up_left.x = X - 16;
		big_rect.rect.width += 16;
	}
	else 
		big_rect.rect.up_left.x = X;
	
	draw_partial_desktop(&(big_rect.rect));
	
	number_of_overlapped_windows = 1;
	UINT_32 i;
	for(i = 0; i < number_of_registered_windows; i++)
	{
		if( overlap(&big_rect, registered_windows[i]) && (wnd != registered_windows[i]) )
		{
			list_of_overlapped_windows[number_of_overlapped_windows] = registered_windows[i];
			number_of_overlapped_windows++;
		}
	}
	draw_list_of_overlapped_windows(&big_rect);
	
	/* UPDATE THE FOCUS WINDOW AND DRAW IT IN THE NEW POSITION */
	wnd->rect.up_left.x += delta_x;
	wnd->rect.up_left.y += delta_y;
	draw_window(wnd, off_screen);
	
	vsync();
	sse_blit_off_screen_to_screen(&big_rect);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL register_window(WINDOW* wnd, const INT_8* name, UINT_32 _width, UINT_32 _height, POINT* up_left) 
{
	if((!_width) || (!_height))
		return FALSE;
	if(up_left->x >= 1024 || up_left->y >= 768)
		return FALSE;
	
	UINT_32 width = ALIGN_16(_width);
	UINT_32 height = _height;
	
	wnd->z_order = 0;
	wnd->window_name = name;
	wnd->rect.width = width;
	wnd->rect.height = height;
	wnd->rect.up_left.x = up_left->x;
	wnd->rect.up_left.y = up_left->y;
	
	wnd->children                 = 0;
	wnd->how_many_children        = 0;
	wnd->number_of_window_objects = 0;
	TITLE_BAR title_bar;

	title_bar.window_name_position.x = 0;
	title_bar.window_name_position.y = 0;
	title_bar.rect.width = width;
	title_bar.rect.height = 16;
	title_bar.rect.up_left = *up_left;
	wnd->title_bar = &title_bar;
	
	BORDER_FRAME border_frame;

	border_frame.rect.width = width;
	border_frame.rect.height = height;
	border_frame.rect.up_left = *up_left;
	border_frame.color = 0x00000000;
	
	wnd->border_frame = &border_frame;

	wnd->buffer = (UINT_8*)( Alloc(width * height * 4, 1, 1) );
	
	UINT_32 i = 0;
	for(i=0; i < width * height * 4; i += 4)
	{
		wnd->buffer[i]   = 0xA4;
		wnd->buffer[i+1] = 0xD1;
		wnd->buffer[i+2] = 0xCA;
	}
	
	UINT_32* trg = (UINT_32*)wnd->buffer;
	UINT_32 jj = 16;
	UINT_32 ii = 0;
	while(jj)
	{
		while(ii < width)
		{
			*trg++ = LiBOS_WINDOW_TITLEBAR_COLOR;
			ii++;
		}
		ii = 0;
		jj--;
	}
	
	trg = (UINT_32*)wnd->buffer;
	jj = height;
	while(jj)
	{
		*trg = 0;
		*(UINT_32*)(PHYSICAL_ADDRESS(trg) + ((width-1) << 2)) = 0;
		trg = PHYSICAL_ADDRESS(trg) + (width << 2);
		jj--;
	}
	trg = (UINT_32*)wnd->buffer;
	ii = width;
	while(ii)
	{
		*trg = 0;
		*(UINT_32*)(PHYSICAL_ADDRESS(trg) + ((height-1) * width * 4)) = 0;
		trg++;
		ii--;
	}
	
	registered_windows[number_of_registered_windows] = wnd;
	add_to_z(wnd);
	number_of_registered_windows++;
    
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL click_on_a_button(INT_32 x, INT_32 y, UINT_32* which_object)
{
	UINT_32 i;
	INT_32 button_x = focus->rect.up_left.x + focus->rect.width  - 84;
	INT_32 button_y = focus->rect.up_left.y + focus->rect.height - 32;
	
	for(i = 0; i < focus->number_of_window_objects; i++)
	{
		if((button_x <= x) && ((button_x + 32) >= x))
		{
			if((button_y <= y) && ((button_y + 16) >= y))
			{
				*which_object = i;
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL lclick_handler(WINDOW* wnd)
{
	UINT_32 i;
	UINT_32 max_z = wnd->z_order;
	for(i = 0; i < number_of_registered_windows; i++)
	{
		if( overlap(wnd, registered_windows[i]) && (wnd != registered_windows[i]) )
		{
			if(registered_windows[i]->z_order > max_z)
				max_z = registered_windows[i]->z_order;
			list_of_overlapped_windows[number_of_overlapped_windows] = registered_windows[i];
			number_of_overlapped_windows++;
		}
	}
	
	if(number_of_overlapped_windows == 1) // a.k.a. NO OVERLAP DETECTED
		return FALSE;
	
	if(max_z == wnd->z_order) // a.k.a. FOCUSED WINDOW IS ALREADY ON TOP OF THE CLUSTER
		return FALSE;
	
	wnd->z_order = max_z;
	draw_window(wnd, LFB);
	
	for(i = 1; i < number_of_overlapped_windows; i++)
		if( list_of_overlapped_windows[i]->z_order )
			list_of_overlapped_windows[i]->z_order--;
	
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void window_manager(INT_8* report_packet, UINT_8 event_type, INT_32 x, INT_32 y, UINT_8* ioctl_payload)
{
	print_mouse_x_y(x,y);
	
	switch (event_type) 
	{
		case lclick_event:
		{
		    if(focus)
				return;
			
			if( !(focus = find_responsive_window(x,y)) )
				return;
			
			WINDOW_OBJECT* obj   = 0;
			UINT_32 which_object = 0xFFFFFFFF;
			if( click_on_a_button(x, y, &which_object) )
			{
				global_button_effect(x, y, which_object);
				(focus->window_objects[which_object])->object_handler(0);
			}
			
			if( lclick_handler(focus) )
				*ioctl_payload = 1; // GIVES A HINT TO MOUSE HANDLER TO REDRAW THE POINTER
			return;
		}
		case move_event:
		{
			if(!focus)
			    return;
			move_handler(focus, (x - focus->rect.up_left.x), (y - focus->rect.up_left.y));

			/* RE-ORDER THE Z VALUE OF THE UPDATE FOSUS WINDOW. THE REST OF WINDOWS HAD BEEN ALREADY UPDATED IN LCLICK HANDLER */
			UINT_32 i;
			focus->z_order = 0;
			UINT_32 max_z  = 0;

			number_of_overlapped_windows = 1;
			for(i = 0; i < number_of_registered_windows; i++)
			{
				if( overlap(focus, registered_windows[i]) && (focus != registered_windows[i]) )
				{
					list_of_overlapped_windows[number_of_overlapped_windows] = registered_windows[i];
					number_of_overlapped_windows++;
					
					if(registered_windows[i]->z_order > max_z)
						max_z = registered_windows[i]->z_order;
				}
			}
			focus->z_order = (max_z) ? max_z + 1 : 0;
			
			return; 
		}
		case release_event: 
		{
			focus = 0;
			number_of_overlapped_windows = 1;
			return; 
		}
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void draw_window(WINDOW* wnd, UINT_8* framebuffer)
{
	if(!wnd)
		return;
	
	UINT_32  row_blocks = (wnd->rect.width >> 1);
	UINT_64* src    = (UINT_64*)wnd->buffer;
	UINT_64* dst    = (UINT_64*)(&(framebuffer[(wnd->rect.up_left.y << 12) + (wnd->rect.up_left.x << 2)]));
	UINT_32 h = wnd->rect.height;
	INT_32 i = row_blocks;
	while(h)
	{
		while(i--)
			*dst++ = *src++;
		i   = row_blocks;
		dst = (UINT_64*)(PHYSICAL_ADDRESS(dst) + 4096 - (wnd->rect.width << 2));
		h--;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void init_desktop(void)
{
	Port_8_Register(&p);
	LFB = get_svga_instance()->LFB;

	desktop.window_name       = "LiBOS_desktop";
	desktop.rect.width        = 1024;
	desktop.rect.height       = 768;
	desktop.rect.up_left.x    = 0;
	desktop.rect.up_left.y    = 0;
	desktop.z_order           = 0;
	desktop.children          = 0;
	desktop.how_many_children = 0;
	desktop.title_bar         = 0;
	desktop.border_frame      = 0;
	desktop.buffer            = (UINT_8*)( Alloc((3145728), 1, 1 ));
	
	UINT_32 i = 0;
	for(i=0; i < 3145728; i += 4)
	{
		desktop.buffer[i]   = 0xD0;
		desktop.buffer[i+1] = 0x8C;
		desktop.buffer[i+2] = 0x44;
	}
	
	list_of_overlapped_windows[0] = &desktop;
	draw_window(&desktop, LFB);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void draw_button(WINDOW* wnd, WINDOW_OBJECT* obj)
{
	UINT_32 w4 = wnd->rect.width << 2;
	UINT_32 h  = wnd->rect.height;
	
	UINT_32* trg = (UINT_32*)(wnd->buffer + ((h - 32) * w4) + (w4 - 336));
	UINT_32 j = 16;
	UINT_32 i = 32;
	while(j)
	{
		while(i--)
			*trg++ = LiBOS_WINDOW_BODY_COLOR;
		i = 32;
		trg = PHYSICAL_ADDRESS(trg) + ((wnd->rect.width - 32) << 2);
		j--;
	}
	
	trg = (UINT_32*)(wnd->buffer + ((h - 32) * w4) + (w4 - 336));
	i = 32;
	while(i--)
	{
		*trg = 0x00FFFFFF;
		*(UINT_32*)(PHYSICAL_ADDRESS(trg) + (w4 << 4)) = 0;
		trg++;
	}
	
	trg = (UINT_32*)(wnd->buffer + ((h - 17) * w4) + (w4 - 332));
	i = 31;
	while(i--)
		*trg++ = LiBOS_BUTTON_SHADOW_COLOR;
	
	j = 16;
	trg = (UINT_32*)(wnd->buffer + ((h - 32) * w4) + (w4 - 336));
	while(j)
	{
		*trg = 0x00FFFFFF;
		*(UINT_32*)(PHYSICAL_ADDRESS(trg) + 128) = 0;
		trg = PHYSICAL_ADDRESS(trg) + w4;
		j--;
	}
	
	j = 15;
	trg  = (UINT_32*)(wnd->buffer + ((h - 31) * w4) + (w4 - 212));
	while(j)
	{
		*trg = LiBOS_BUTTON_SHADOW_COLOR;
		trg  = PHYSICAL_ADDRESS(trg) + w4;
		j--;
	}
	
	obj->upper_left_corner_address = (UINT_8*)(wnd->buffer + ((h - 32) * w4) + (w4 - 336));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void window_force_LiBOS_to_reboot(void* arg)
{
	go_to_reset();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL register_object(WINDOW* wnd, WINDOW_OBJECT* obj)
{
	if(!wnd)
		return FALSE;
	if(!obj)
		return FALSE;
	
	if(obj->object_identifier == OBJECT_BUTTON)
	{
		obj->object_handler = &window_force_LiBOS_to_reboot; // TODO
		draw_button(wnd, obj);
	}
	
	wnd->window_objects[wnd->number_of_window_objects] = obj;
	wnd->number_of_window_objects++;
	
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void global_button_effect(INT_32 x, INT_32 y, UINT_32 which_object)
{
	if(which_object == 0xFFFFFFFF)
		return;
	
	/* FIRST SAVE A POINTER TO MOUSE POINTER BUFFER (to keep the mouse pointer alive) */
	/* THEN PERFORM THE BUTTON EFFECT */
	/* BLIT THE MOUSE POINTER */
	/* WAIT 200 MSEC TO HAVE A NICE EFFECT */
	/* PERFORM THE SECOND BUTTON EFFECT */
	/* BLIT THE MOUSE POINTER AGAIN */
	
	UINT_32* mouse = (UINT_32*)get_mouse_cursor_to_window_manager();
	
	UINT_32* buffer = (UINT_32*)((&LFB[(focus->rect.up_left.y << 12) + (focus->rect.up_left.x << 2) + ((focus->rect.height - 32) << 12) + ((focus->rect.width << 2) - 336)]));

	UINT_32 i = 32;
	while(i--)
	{
		*buffer = 0;
		*(UINT_32*)(PHYSICAL_ADDRESS(buffer) + (4096 << 4)) = 0x00FFFFFF;
		buffer++;
	}
	
	buffer = (UINT_32*)((&LFB[(focus->rect.up_left.y << 12) + (focus->rect.up_left.x << 2) + ((focus->rect.height - 17) << 12) + ((focus->rect.width << 2) - 332)]));
	i = 31;
	while(i--)
	{
		*buffer = LiBOS_WINDOW_BODY_COLOR;
		*(UINT_32*)(PHYSICAL_ADDRESS(buffer) - (4096 * 14)) = LiBOS_BUTTON_SHADOW_COLOR;
		buffer++;
	}
	
	UINT_32 h = 16;
	UINT_32* target = (UINT_32*)(&LFB[(y << 12) + (x << 2)]);
	UINT_32 w = 16;
	while(h)
	{
		while(w--)
		{
			if((*mouse) & (0xFF000000)) 
			{
				target++;
				mouse++;
			}
			else
				*target++ = *mouse++;
		}
		w = 16;
		target = PHYSICAL_ADDRESS(target) + 4032;
		h--;
	}
	
	WaitMiliSecond(200);
	
	buffer = (UINT_32*)((&LFB[(focus->rect.up_left.y << 12) + (focus->rect.up_left.x << 2) + ((focus->rect.height - 32) << 12) + ((focus->rect.width << 2) - 336)]));

	i = 32;
	while(i--)
	{
		*buffer = 0x00FFFFFF;
		*(UINT_32*)(PHYSICAL_ADDRESS(buffer) + (4096 << 4)) = 0;
		buffer++;
	}
	
	buffer = (UINT_32*)((&LFB[(focus->rect.up_left.y << 12) + (focus->rect.up_left.x << 2) + ((focus->rect.height - 17) << 12) + ((focus->rect.width << 2) - 332)]));
	i = 31;
	while(i--)
	{
		*buffer = LiBOS_BUTTON_SHADOW_COLOR;
		*(UINT_32*)(PHYSICAL_ADDRESS(buffer) - (4096 * 14)) = LiBOS_WINDOW_BODY_COLOR;
		buffer++;
	}
	
	h = 16;
	mouse  = (UINT_32*)get_mouse_cursor_to_window_manager();
	target = (UINT_32*)(&LFB[(y << 12) + (x << 2)]);
	w = 16;
	while(h)
	{
		while(w--)
		{
			if((*mouse) & (0xFF000000)) 
			{
				target++;
				mouse++;
			}
			else
				*target++ = *mouse++;
		}
		w = 16;
		target = PHYSICAL_ADDRESS(target) + 4032;
		h--;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+











//#include "../include/WINDOW.h"
//#include "../include/SVGA.h"
//#include "../include/MEMORY.h"
//#include "../include/PORT.h"
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES
//
//static PORT_8 p;
//static PORT_16 pp;
//static UINT_32 number_of_registered_windows = 0;
//static WINDOW* registered_windows[1024];
//static WINDOW  desktop;
//static WINDOW* focus = 0;
//static WINDOW* list_of_overlapped_windows[1024];
//static UINT_32 number_of_overlapped_windows = 1;
//static UINT_8 off_screen[3145728]; //a.k.a. 3MB
//static UINT_8* LFB = 0;
//
//static UINT_8* current_screen = 0;
//static UINT_8* other_screen   = 0;
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ WINDOW.c CORE DATA AND CODE
//
//#define SIGNED_8_BIT(UNSIGNED_8_BIT) (INT_8)((UINT_8)(UNSIGNED_8_BIT ^ 0xFF))
//#define ALIGN_16(N) (N+15) & (~15) 
//#define ABSOLUTE(INT) ((INT>0)?INT:-INT)
//void draw_window(WINDOW* wnd, UINT_8* framebuffer);
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//void blit_off_screen_to_screen(WINDOW* wnd)
//{
//	UINT_32 byte_offset = (wnd->rect.up_left.y << 12) + (wnd->rect.up_left.x << 2);
//	
//	UINT_64* on  = (UINT_64*)(&((LFB)     [byte_offset]));
//	UINT_64* off = (UINT_64*)(&(off_screen[byte_offset]));
//
//	UINT_32 h = wnd->rect.height;
//	UINT_32 w = wnd->rect.width; 
//	UINT_32 i = w >> 1;
//	
//	UINT_32 stride_offset = 4096 - (w << 2);
//	
//	while(h)
//	{
//		while(i--)
//			*on++ = *off++;
//		i = w >> 1;
//		on  = (UINT_64*)(PHYSICAL_ADDRESS(on)  + stride_offset);
//		off = (UINT_64*)(PHYSICAL_ADDRESS(off) + stride_offset);
//		h--;
//	}
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//static BOOL overlap(WINDOW* src, WINDOW* trg)
//{
//	if(((src->rect.up_left.x + src->rect.width) < trg->rect.up_left.x) || (trg->rect.up_left.x + trg->rect.width) < src->rect.up_left.x)
//        if(((src->rect.up_left.y + src->rect.height) < trg->rect.up_left.y) || (trg->rect.up_left.y + trg->rect.height) < src->rect.up_left.y)
//            return FALSE;
//	return TRUE;
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//static void add_to_z(WINDOW* wnd)
//{
//	UINT_32 i;
//	for(i = 0; i < number_of_registered_windows; i++)
//	{
//		if( overlap(registered_windows[i], wnd) )
//			wnd->z_order++;
//	}
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//static WINDOW* find_responsive_window(INT_32 x, INT_32 y)
//{
//	UINT_32 i;
//	UINT_32 target_window_number = 0xFFFFFFFF;
//	UINT_32 max_z                = 0;
//	WINDOW* src                  = 0;
//	
//	for(i = 0; i < number_of_registered_windows; i++)
//	{
//		src = registered_windows[i];
//		if((src->rect.up_left.x <= x) && ((src->rect.up_left.x + src->rect.width) >= x))
//		{
//			if((src->rect.up_left.y <= y) && ((src->rect.up_left.y + src->rect.height) >= y))
//			{
//				if(!i)
//				{
//					max_z = src->z_order;
//					target_window_number = 0;
//				}
//				
//				else if(max_z <= src->z_order)
//				{
//					max_z = src->z_order;
//					target_window_number = i;
//				}
//			}
//		}
//	}
//	
//	if(target_window_number != 0xFFFFFFFF)
//		return registered_windows[target_window_number];
//	return 0;
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//void sort_list_of_overlapped_windows(void)
//{
//
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//void draw_partial_desktop(RECT* r)
//{
//	UINT_32 row_blocks = (r->width >> 1);
//	UINT_32 byte_offset = (r->up_left.y << 12) + (r->up_left.x << 2);
//	UINT_64* src       = (UINT_64*)(&(desktop.buffer[byte_offset]));
//	UINT_64* dst       = (UINT_64*)(&(other_screen  [byte_offset]));
//	UINT_32 h          = r->height;
//	INT_32 i           = row_blocks;
//	while(h)
//	{
//		while(i--)
//			*dst++ = *src++;
//		i   = row_blocks;
//		dst = (UINT_64*)(PHYSICAL_ADDRESS(dst) + 4096 - (r->width << 2));
//		h--;
//	}
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//void draw_list_of_overlapped_windows(WINDOW* focused_window)
//{
//	if(!focused_window)
//		return;
//	
//	UINT_32 i = 1;
//	while(i < number_of_overlapped_windows)
//	{
//		draw_window(list_of_overlapped_windows[i], other_screen);
//		i++;
//	}
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//extern void _sse_copy_current_screen_to_other_screen(UINT_8* src, UINT_8* trg);
//void flp(UINT_32 addr)
//{
//	while ( (p.Read(0x03DA) & 0x08));
//	
//	p.Write (0x03D4, );
//	pp.Write(0x03D4, (UINT_16)addr | 0x0D);
//	pp.Write(0x03D4, (UINT_16)(addr >> 16) | 0x0C);
//	
//	while (!(p.Read(0x03DA) & 0x08));
//}
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//void move_handler(WINDOW* wnd, INT_32 delta_x, INT_32 delta_y)
//{
//	WINDOW big_rect;
//	big_rect.rect.up_left.x = (delta_x > 0) ? wnd->rect.up_left.x : wnd->rect.up_left.x + delta_x;
//	big_rect.rect.up_left.y = (delta_y > 0) ? wnd->rect.up_left.y : wnd->rect.up_left.y + delta_y;
//	big_rect.rect.width     = ALIGN_16(wnd->rect.width  + ABSOLUTE(delta_x));
//	big_rect.rect.height    = wnd->rect.height + ABSOLUTE(delta_y);
//	
//	_sse_copy_current_screen_to_other_screen(current_screen, other_screen);
//	draw_partial_desktop(&(big_rect.rect));
//	draw_list_of_overlapped_windows(&big_rect);
//	
//	/* UPDATE THE FOCUS WINDOW AND DRAW IT IN THE NEW POSITION */
//	wnd->rect.up_left.x += delta_x;
//	wnd->rect.up_left.y += delta_y;
//	draw_window(wnd, other_screen);
//	
//	// IT DOES NOT WORK!! flp( (current_screen == LFB) ? (PHYSICAL_ADDRESS(LFB)+4096 * 768 ) : PHYSICAL_ADDRESS(LFB) );
//	page_flip( (current_screen == LFB) ? (4096 * 768 * 16) : 0 );
//	UINT_32 swap   = PHYSICAL_ADDRESS(current_screen);
//	current_screen = (UINT_8*)((void*)(PHYSICAL_ADDRESS(other_screen)));
//	other_screen   = (UINT_8*)((void*)(swap));
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//BOOL register_window(WINDOW* wnd, const INT_8* name, UINT_32 _width, UINT_32 _height, POINT* up_left) 
//{
//	if((!_width) || (!_height))
//		return FALSE;
//	if(up_left->x >= 1024 || up_left->y >= 768)
//		return FALSE;
//	
//	UINT_32 width = ALIGN_16(_width);
//	UINT_32 height = _height;
//	
//	wnd->z_order = 0;
//	wnd->window_name = name;
//	wnd->rect.width = width;
//	wnd->rect.height = height;
//	wnd->rect.up_left.x = up_left->x;
//	wnd->rect.up_left.y = up_left->y;
//	
//	wnd->children = 0;
//	wnd->how_many_children = 0;
//	
//	TITLE_BAR title_bar;
//
//	title_bar.window_name_position.x = 0;
//	title_bar.window_name_position.y = 0;
//	title_bar.rect.width = width;
//	title_bar.rect.height = 16;
//	title_bar.rect.up_left = *up_left;
//	wnd->title_bar = &title_bar;
//	
//	BORDER_FRAME border_frame;
//
//	border_frame.rect.width = width;
//	border_frame.rect.height = height;
//	border_frame.rect.up_left = *up_left;
//	border_frame.color = 0x00000000;
//	
//	wnd->border_frame = &border_frame;
//
//	wnd->buffer = (UINT_8*)( Alloc(width * height * 4, 1, 1) );
//	
//	UINT_32 i = 0;
//	for(i=0; i < width * height * 4; i += 4)
//	{
//		wnd->buffer[i]   = 0xA4;
//		wnd->buffer[i+1] = 0xD1;
//		wnd->buffer[i+2] = 0xCA;
//	}
//	
//	UINT_32* trg = (UINT_32*)wnd->buffer;
//	UINT_32 jj = 16;
//	UINT_32 ii = 0;
//	while(jj)
//	{
//		while(ii < width)
//		{
//			*trg++ = LiBOS_WINDOW_TITLEBAR_COLOR;
//			ii++;
//		}
//		ii = 0;
//		jj--;
//	}
//	
//	trg = (UINT_32*)wnd->buffer;
//	jj = height;
//	while(jj)
//	{
//		*trg = 0;
//		*(UINT_32*)(PHYSICAL_ADDRESS(trg) + ((width-1) << 2)) = 0;
//		trg = PHYSICAL_ADDRESS(trg) + (width << 2);
//		jj--;
//	}
//	trg = (UINT_32*)wnd->buffer;
//	ii = width;
//	while(ii)
//	{
//		*trg = 0;
//		*(UINT_32*)(PHYSICAL_ADDRESS(trg) + ((height-1) * width * 4)) = 0;
//		trg++;
//		ii--;
//	}
//	
//	registered_windows[number_of_registered_windows] = wnd;
//	add_to_z(wnd);
//	number_of_registered_windows++;
//    
//	draw_window(wnd, current_screen);
//	
//	return TRUE;
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//BOOL lclick_handler(WINDOW* wnd)
//{
//	UINT_32 i;
//	UINT_32 max_z = wnd->z_order;
//	for(i = 0; i < number_of_registered_windows; i++)
//	{
//		if( overlap(wnd, registered_windows[i]) && (wnd != registered_windows[i]) )
//		{
//			if(registered_windows[i]->z_order > max_z)
//				max_z = registered_windows[i]->z_order;
//			list_of_overlapped_windows[number_of_overlapped_windows] = registered_windows[i];
//			number_of_overlapped_windows++;
//		}
//	}
//	
//	if(number_of_overlapped_windows == 1) // a.k.a. NO OVERLAP DETECTED
//		return FALSE;
//	
//	if(max_z == wnd->z_order) // a.k.a. FOCUSED WINDOW IS ALREADY ON TOP OF THE CLUSTER
//		return FALSE;
//	
//	wnd->z_order = max_z;
//	draw_window(wnd, current_screen);
//	
//	for(i = 1; i < number_of_overlapped_windows; i++)
//		if( list_of_overlapped_windows[i]->z_order )
//			list_of_overlapped_windows[i]->z_order--;
//	
//	return TRUE;
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//void window_manager(INT_8* report_packet, UINT_8 event_type, INT_32 x, INT_32 y, UINT_8* ioctl_payload)
//{
//	print_mouse_x_y(x,y);
//	
//	switch (event_type) 
//	{
//		case lclick_event:
//		{
//		    if(focus)
//				return;
//			focus = find_responsive_window(x,y);
//			if( lclick_handler(focus) )
//				*ioctl_payload = 1; // GIVES A HINT TO MOUSE HANDLER TO REDRAW THE POINTER
//			return;
//		}
//		case move_event:
//		{
//			if(!focus)
//			    return;
//			move_handler(focus, (x - focus->rect.up_left.x), (y - focus->rect.up_left.y));
//
//			/* RE-ORDER THE Z VALUE OF THE UPDATE FOSUS WINDOW. THE REST OF WINDOWS HAD BEEN ALREADY UPDATED IN LCLICK HANDLER */
//			UINT_32 i;
//			focus->z_order = 0;
//			UINT_32 max_z  = 0;
//
//			number_of_overlapped_windows = 1;
//			for(i = 0; i < number_of_registered_windows; i++)
//			{
//				if( overlap(focus, registered_windows[i]) && (focus != registered_windows[i]) )
//				{
//					list_of_overlapped_windows[number_of_overlapped_windows] = registered_windows[i];
//					number_of_overlapped_windows++;
//					
//					if(registered_windows[i]->z_order > max_z)
//						max_z = registered_windows[i]->z_order;
//				}
//			}
//			focus->z_order = (max_z) ? max_z + 1 : 0;
//			
//			return; 
//		}
//		case release_event: 
//		{
//			focus = 0;
//			number_of_overlapped_windows = 1;
//			return; 
//		}
//	}
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//void draw_window(WINDOW* wnd, UINT_8* framebuffer)
//{
//	if(!wnd)
//		return;
//	
//	UINT_32  row_blocks = (wnd->rect.width >> 1);
//	UINT_64* src    = (UINT_64*)wnd->buffer;
//	UINT_64* dst    = (UINT_64*)(&(framebuffer[(wnd->rect.up_left.y << 12) + (wnd->rect.up_left.x << 2)]));
//	UINT_32 h = wnd->rect.height;
//	INT_32 i = row_blocks;
//	while(h)
//	{
//		while(i--)
//			*dst++ = *src++;
//		i   = row_blocks;
//		dst = (UINT_64*)(PHYSICAL_ADDRESS(dst) + 4096 - (wnd->rect.width << 2));
//		h--;
//	}
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//void init_desktop(void)
//{
//	Port_8_Register(&p);
//	Port_16_Register(&pp);
//	LFB = get_svga_instance()->LFB;
//	
//	current_screen = LFB;
//	other_screen   = (UINT_8*)((void*)(  (PHYSICAL_ADDRESS(current_screen) + (3145728))  ));
//	
//	desktop.window_name       = "LiBOS_desktop";
//	desktop.rect.width        = 1024;
//	desktop.rect.height       = 768;
//	desktop.rect.up_left.x    = 0;
//	desktop.rect.up_left.y    = 0;
//	desktop.z_order           = 0;
//	desktop.children          = 0;
//	desktop.how_many_children = 0;
//	desktop.title_bar         = 0;
//	desktop.border_frame      = 0;
//	desktop.buffer            = (UINT_8*)( Alloc((3145728), 1, 1 ));
//	
//	UINT_32 i = 0;
//	for(i=0; i < 3145728; i += 4)
//	{
//		desktop.buffer[i]   = 0xD0;
//		desktop.buffer[i+1] = 0x8C;
//		desktop.buffer[i+2] = 0x44;
//	}
//	
//	list_of_overlapped_windows[0] = &desktop;
//	draw_window(&desktop, current_screen);
//	draw_window(&desktop, other_screen);
//}
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+