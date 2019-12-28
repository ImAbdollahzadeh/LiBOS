//.#include "../include/WINDOW.h"
//.#include "../include/SVGA.h"
//.#include "../include/MEMORY.h"
//.#include "../include/PORT.h"
//.
//.#define SIGNED_8_BIT(UNSIGNED_8_BIT) (INT_8)((UINT_8)(UNSIGNED_8_BIT ^ 0xFF))
//.#define ALIGN_8(N) (N+7) & (~7) 
//.void draw_window(WINDOW* wnd, UINT_8* framebuffer);
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES
//.
//.static PORT_8 p;
//.static UINT_32 z_order[1024];
//.static UINT_32 number_of_registered_windows = 0;
//.static WINDOW* registered_windows[1024];
//.static WINDOW desktop;
//.static WINDOW* focus = 0;
//.static WINDOW* list_of_overlapped_windows[1024];
//.static UINT_32 number_of_overlapped_windows = 1;
//.static UINT_8 off_screen[1024*768*4];
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void vsync(void)
//.{
//.	while ( (p.Read(0x03DA) & 0x08));
//.	while (!(p.Read(0x03DA) & 0x08));
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void blit_off_screen_to_screen(WINDOW* wnd)
//.{
//.	UINT_32 byte_offset = (wnd->rect.up_left.y << 12) + (wnd->rect.up_left.x << 2);
//.	
//.	UINT_64* on  = (UINT_64*)(&((get_svga_instance()->LFB)[byte_offset]));
//.	UINT_64* off = (UINT_64*)(&(off_screen                [byte_offset]));
//.
//.	UINT_32 h = wnd->rect.height;
//.	UINT_32 w = wnd->rect.width; 
//.	UINT_32 i = w >> 1;
//.	
//.	UINT_32 stride_offset = 4096 - (w << 2);
//.	
//.	while(h)
//.	{
//.		while(i--)
//.			*on++ = *off++;
//.		i = w >> 1;
//.		on  = (UINT_64*)(PHYSICAL_ADDRESS(on)  + stride_offset);
//.		off = (UINT_64*)(PHYSICAL_ADDRESS(off) + stride_offset);
//.		h--;
//.	}
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.static BOOL overlap(WINDOW* src, WINDOW* trg)
//.{
//.	if(((src->rect.up_left.x + src->rect.width) < trg->rect.up_left.x) || (trg->rect.up_left.x + trg->rect.width) < src->rect.up_left.x)
//.        if(((src->rect.up_left.y + src->rect.height) < trg->rect.up_left.y) || (trg->rect.up_left.y + trg->rect.height) < src->rect.up_left.y)
//.            return FALSE;
//.	return TRUE;
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.static void add_to_z(WINDOW* new_window)
//.{
//.	z_order[number_of_registered_windows] = 0;
//.	UINT_32 i;
//.	
//.	for(i = 0; i < number_of_registered_windows; i++)
//.	{
//.		if( overlap(registered_windows[i], registered_windows[number_of_registered_windows]) )
//.			z_order[number_of_registered_windows]++;
//.	}
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void find_overlapped_rect(WINDOW* src, WINDOW* trg, RECT* r)
//.{
//.	if(src->rect.up_left.x <= trg->rect.up_left.x)
//.	{
//.		r->up_left.x = trg->rect.up_left.x;
//.		r->width = ((trg->rect.up_left.x + trg->rect.width) >= (src->rect.up_left.x + src->rect.width)) ? src->rect.width + src->rect.up_left.x - trg->rect.up_left.x : trg->rect.width;
//.	}
//.	else
//.	{
//.		r->up_left.x = src->rect.up_left.x;
//.		r->width = ((trg->rect.up_left.x + trg->rect.width) >= (src->rect.up_left.x + src->rect.width)) ? src->rect.width : trg->rect.width + trg->rect.up_left.x - src->rect.up_left.x;
//.	}
//.	
//.	if(src->rect.up_left.y <= trg->rect.up_left.y)
//.	{
//.		r->up_left.y = trg->rect.up_left.y;
//.		r->height = ((trg->rect.up_left.y + trg->rect.height) >= (src->rect.up_left.y + src->rect.height)) ? src->rect.height + src->rect.up_left.y - trg->rect.up_left.y : trg->rect.height;
//.	}
//.	else
//.	{
//.		r->up_left.y = src->rect.up_left.y;
//.		r->height = ((trg->rect.up_left.y + trg->rect.height) >= (src->rect.up_left.y + src->rect.height)) ? src->rect.height : trg->rect.height + trg->rect.up_left.y - src->rect.up_left.y;
//.	}
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.static void reorder_z_right_after_event(WINDOW* focused_window)
//.{
//.	if(!focused_window)
//.        return;
//.    
//.    UINT_32 i;
//.	UINT_32 trg = 0;
//.	for(i = 0; i < number_of_registered_windows; i++)
//.	{
//.		if(registered_windows[i] == focused_window)
//.			trg = i;
//.	}
//.    
//.    UINT_32 before = z_order[trg];
//.    UINT_32 final = 0;
//.    
//.	for(i = 0; i < number_of_registered_windows; i++)
//.	{
//.		if(i == trg)
//.            continue;
//.        
//.        if( overlap(registered_windows[i], registered_windows[trg]) )
//.		{
//.			if(before < z_order[i])
//.            {
//.                final = (final > z_order[i]) ? final : z_order[i];     
//.                
//.                if(z_order[i])
//.                    z_order[i]--;   
//.            }
//.		}
//.	}
//.    z_order[trg] = (!final) ? before : final;
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.static WINDOW* find_responsive_window(INT_32 x, INT_32 y)
//.{
//.	UINT_32 i;
//.	
//.	INT_32 num = -1;
//.	INT_32 tmp_num = -1;
//.	UINT_32 ret_num = 0xFFFFFFFF;
//.	for(i = 0; i < number_of_registered_windows; i++)
//.	{
//.		WINDOW* src = registered_windows[i];
//.		if((src->rect.up_left.x <= x) && ((src->rect.up_left.x + src->rect.width) >= x))
//.		{
//.			if((src->rect.up_left.y <= y) && ((src->rect.up_left.y + src->rect.height) >= y))
//.			{
//.				if(tmp_num >= 0)
//.				{
//.					num = tmp_num;
//.					tmp_num = i;
//.					if(z_order[num] < z_order[tmp_num])
//.						ret_num = tmp_num;
//.					else 
//.						ret_num = num;
//.				}
//.				else
//.				{
//.					tmp_num = i;
//.					ret_num = tmp_num;
//.				}
//.			}
//.		}
//.	}
//.	if(ret_num != 0xFFFFFFFF)
//.	{
//.		//std::cout << "responsive window is: " << registered_windows[ret_num]->window_name << std::endl;
//.		return registered_windows[ret_num];
//.	}
//.    
//.	return 0;
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void sort_list_of_overlapped_windows(void)
//.{
//.
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void draw_partial_desktop(RECT* r)
//.{
//.	UINT_8* fb = off_screen;
//.	UINT_32  row_blocks = (r->width >> 1);
//.	UINT_64* src    = (UINT_64*)(&(desktop.buffer[(r->up_left.y << 12) + (r->up_left.x << 2)]));
//.	UINT_64* dst    = (UINT_64*)(&(fb[(r->up_left.y << 12) + (r->up_left.x << 2)]));
//.	UINT_32 h = r->height ;
//.	INT_32 i = row_blocks ;
//.	while(h)
//.	{
//.		while(i--)
//.			*dst++ = *src++;
//.		i   = row_blocks;
//.		dst = (UINT_64*)(PHYSICAL_ADDRESS(dst) + 4096 - (r->width << 2));
//.		h--;
//.	}
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void draw_partial_window(WINDOW* wnd, RECT* clip, UINT_8* framebuffer)
//.{
//.	if(!wnd)
//.		return;
//.	
//.	UINT_32 i = 0;
//.	UINT_32 h = clip->height;
//.	UINT_32 w = clip->width;
//.	UINT_32 x = clip->up_left.x - wnd->rect.up_left.x;
//.	UINT_32 y = clip->up_left.y - wnd->rect.up_left.y;
//.	
//.	UINT_32* pbuf = (UINT_32*)(&(wnd->buffer[((y * wnd->rect.width) + x) << 2]));
//.	UINT_32* ptrg = (UINT_32*)(&(framebuffer[(clip->up_left.y << 12) + (clip->up_left.x << 2)]));
//.
//.	while(h)
//.	{
//.		while(i < w)
//.		{
//.			*ptrg++ = *pbuf++;
//.			i++;
//.		}
//.		i = 0;
//.		pbuf = PHYSICAL_ADDRESS(pbuf) + ((wnd->rect.width - w) << 2);
//.		ptrg = PHYSICAL_ADDRESS(ptrg) + 4096 - (w << 2);
//.		h--;
//.	}
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void draw_list_of_overlapped_windows(WINDOW* focused_window)
//.{
//.	if(!focused_window)
//.		return;
//.	
//.	UINT_32 i = 1;
//.	RECT clip = {{0, 0}, 0, 0};
//.	while(i < number_of_overlapped_windows)
//.	{
//.		//find_overlapped_rect(focused_window, list_of_overlapped_windows[i], &clip);
//.		//draw_partial_window(list_of_overlapped_windows[i], &clip, get_svga_instance()->LFB);
//.		//i++;
//.		draw_window(list_of_overlapped_windows[i], off_screen);
//.		i++;
//.	}
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void event_at(INT_32 x, INT_32 y, INT_32 delta_x, INT_32 delta_y)
//.{
//.    WINDOW* trg_wnd = find_responsive_window(x, y);
//.	if(trg_wnd)
//.    {
//.		// keep a copy of the target window
//.		WINDOW initial = *trg_wnd; 
//.		
//.		// update the target window
//.		trg_wnd->rect.up_left.x += delta_x;
//.		trg_wnd->rect.up_left.y += delta_y;      
//.		
//.		// reorder z after moveing the target window
//.		reorder_z_right_after_event(trg_wnd); 
//.		
//.		// check the windows that can be responsive to the initial window
//.		UINT_32 i;
//.		for(i = 0; i < number_of_registered_windows; i++)
//.		{
//.			if( (registered_windows[i] != &initial) && overlap(registered_windows[i], &initial) )
//.			{
//.				list_of_overlapped_windows[number_of_overlapped_windows] = registered_windows[i];
//.				number_of_overlapped_windows++;
//.			}
//.        }
//.		
//.		//sort_list_of_overlapped_windows(); /*TODO*/
//.		draw_partial_desktop(&(initial.rect));
//.		draw_list_of_overlapped_windows(&initial);
//.		
//.		vsync();
//.		blit_off_screen_to_screen(&initial);
//.		
//.		draw_window(trg_wnd, get_svga_instance()->LFB);
//.		
//.		UINT_32 j;
//.		for(j = 1; j<number_of_overlapped_windows; j++)
//.			list_of_overlapped_windows[j] = 0;
//.		number_of_overlapped_windows = 1;
//.		
//.        return;
//.    }
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.BOOL register_window(WINDOW* wnd, const INT_8* name, UINT_32 _width, UINT_32 _height, POINT* up_left) 
//.{
//.	if((!_width) || (!_height))
//.		return FALSE;
//.	if(up_left->x >= 1024 || up_left->y >= 768)
//.		return FALSE;
//.	UINT_32 width = ALIGN_16(_width);
//.	UINT_32 height = _height;
//.	
//.	wnd->window_name = name;
//.	wnd->rect.width = width;
//.	wnd->rect.height = height;
//.	wnd->rect.up_left.x = up_left->x;
//.	wnd->rect.up_left.y = up_left->y;
//.	
//.	wnd->children = 0;
//.	wnd->how_many_children = 0;
//.	
//.	TITLE_BAR title_bar;
//.
//.	title_bar.window_name_position.x = 0;
//.	title_bar.window_name_position.y = 0;
//.	title_bar.rect.width = width;
//.	title_bar.rect.height = 16;
//.	title_bar.rect.up_left = *up_left;
//.	wnd->title_bar = &title_bar;
//.	
//.	BORDER_FRAME border_frame;
//.
//.	border_frame.rect.width = width;
//.	border_frame.rect.height = height;
//.	border_frame.rect.up_left = *up_left;
//.	border_frame.color = 0x00000000;
//.	
//.	wnd->border_frame = &border_frame;
//.
//.	wnd->buffer = (UINT_8*)( Alloc(width * height * 4, 1, 1) );
//.	
//.	UINT_32 i = 0;
//.	for(i=0; i < width * height * 4; i += 4)
//.	{
//.		wnd->buffer[i]   = 0xA4;
//.		wnd->buffer[i+1] = 0xD1;
//.		wnd->buffer[i+2] = 0xCA;
//.	}
//.	
//.	UINT_32* trg = (UINT_32*)wnd->buffer;
//.	UINT_32 jj = 16;
//.	UINT_32 ii = 0;
//.	while(jj)
//.	{
//.		while(ii < width)
//.		{
//.			*trg++ = 0x0041707E;
//.			ii++;
//.		}
//.		ii = 0;
//.		jj--;
//.	}
//.	
//.	trg = (UINT_32*)wnd->buffer;
//.	jj = height;
//.	while(jj)
//.	{
//.		*trg = 0;
//.		*(UINT_32*)(PHYSICAL_ADDRESS(trg) + ((width-1) << 2)) = 0;
//.		trg = PHYSICAL_ADDRESS(trg) + (width << 2);
//.		jj--;
//.	}
//.	trg = (UINT_32*)wnd->buffer;
//.	ii = width;
//.	while(ii)
//.	{
//.		*trg = 0;
//.		*(UINT_32*)(PHYSICAL_ADDRESS(trg) + ((height-1) * width * 4)) = 0;
//.		trg++;
//.		ii--;
//.	}
//.	
//.	registered_windows[number_of_registered_windows] = wnd;
//.	add_to_z(wnd);
//.	number_of_registered_windows++;
//.    
//.	return TRUE;
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.//.void draw_back(WINDOW* wnd)
//.//.{
//.//.	UINT_32 i = 0, j = 0;
//.//.	UINT_32 h = wnd->rect.height;
//.//.	UINT_32 w = wnd->rect.width;
//.//.
//.//.	UINT_32* buf = (UINT_32*)wnd->buffer;
//.//.	UINT_8* fb = get_svga_instance()->LFB;
//.//.	UINT_32* trg = (UINT_32*)(&fb[(wnd->rect.up_left.y << 12) + (wnd->rect.up_left.x << 2)]);
//.//.	
//.//.	i = 0;
//.//.	while(h)
//.//.	{
//.//.		while(i < w)
//.//.		{
//.//.			*trg++ = *buf++;
//.//.			i++;
//.//.		}
//.//.		i = 0;
//.//.		trg = PHYSICAL_ADDRESS(trg) + (4096 - (w << 2));
//.//.		h--;
//.//.	}
//.//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void window_manager(INT_8* report_packet, UINT_8 event_type, INT_32 x, INT_32 y)
//.{
//.	print_mouse_x_y(x,y);
//.	
//.	switch (event_type) 
//.	{
//.		case lclick_event:
//.		{
//.		    if(focus)
//.				return;
//.
//.			focus = find_responsive_window(x,y);
//.			reorder_z_right_after_event(focus);
//.			draw_window(focus, get_svga_instance()->LFB);
//.			break;
//.		}
//.		case move_event:
//.		{
//.			if(!focus)
//.			    return;
//.
//.			WINDOW initial = *focus;
//.			event_at(initial.rect.up_left.x, initial.rect.up_left.y, (x - initial.rect.up_left.x), (y - initial.rect.up_left.y));
//.			focus->rect.up_left.x = x;
//.			focus->rect.up_left.y = y;
//.			
//.			reorder_z_right_after_event(focus);
//.			break; 
//.		}
//.		case release_event: 
//.		{
//.			focus = 0;
//.			return; 
//.		}
//.	}
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void draw_window(WINDOW* wnd, UINT_8* framebuffer)
//.{
//.	if(!wnd)
//.		return;
//.	
//.	UINT_32  row_blocks = (wnd->rect.width >> 1);
//.	UINT_64* src    = (UINT_64*)wnd->buffer;
//.	UINT_64* dst    = (UINT_64*)(&(framebuffer[(wnd->rect.up_left.y << 12) + (wnd->rect.up_left.x << 2)]));
//.	UINT_32 h = wnd->rect.height;
//.	INT_32 i = row_blocks;
//.	while(h)
//.	{
//.		while(i--)
//.			*dst++ = *src++;
//.		i   = row_blocks;
//.		dst = (UINT_64*)(PHYSICAL_ADDRESS(dst) + 4096 - (wnd->rect.width << 2));
//.		h--;
//.	}
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void init_desktop(void)
//.{
//.	Port_8_Register(&p);
//.	
//.	desktop.window_name = "LiBOS_desktop";
//.	desktop.rect.width = 1024;
//.	desktop.rect.height = 768;
//.	desktop.rect.up_left.x = 0;
//.	desktop.rect.up_left.y = 0;
//.	
//.	desktop.children = 0;
//.	desktop.how_many_children = 0;
//.	
//.	desktop.title_bar = 0;
//.	desktop.border_frame = 0;
//.	desktop.buffer = (UINT_8*)( Alloc((1024*768*4), 1, 1 ));
//.	
//.	UINT_32 i = 0;
//.	for(i=0; i < 1024*768*4; i += 4)
//.	{
//.		desktop.buffer[i]   = 0xD0;
//.		desktop.buffer[i+1] = 0x8C;
//.		desktop.buffer[i+2] = 0x44;
//.	}
//.	
//.	list_of_overlapped_windows[0] = &desktop;
//.	
//.	draw_window(&desktop, get_svga_instance()->LFB);
//.}
//.
//.//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+




#include "../include/WINDOW.h"
#include "../include/SVGA.h"
#include "../include/MEMORY.h"
#include "../include/PORT.h"

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
void draw_window(WINDOW* wnd, UINT_8* framebuffer);

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
	big_rect.rect.up_left.x = (delta_x > 0) ? wnd->rect.up_left.x : wnd->rect.up_left.x + delta_x;
	big_rect.rect.up_left.y = (delta_y > 0) ? wnd->rect.up_left.y : wnd->rect.up_left.y + delta_y;
	big_rect.rect.width     = ALIGN_16(wnd->rect.width  + ABSOLUTE(delta_x));
	big_rect.rect.height    = ALIGN_8(wnd->rect.height + ABSOLUTE(delta_y));
	
	draw_partial_desktop(&(big_rect.rect));
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
	
	wnd->children = 0;
	wnd->how_many_children = 0;
	wnd->window_objects    = 0;
	
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
			*trg++ = 0x0041707E;
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
			focus = find_responsive_window(x,y);
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
	desktop.window_objects    = 0;
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
















//#include "../include/WINDOW.h"
//#include "../include/SVGA.h"
//#include "../include/MEMORY.h"
//#include "../include/PORT.h"
//
////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES
//
//static PORT_8 p;
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
//void sse_copy_current_screen_to_other_screen(void)
//{
//	UINT_64* src       = (UINT_64*)current_screen;
//	UINT_64* dst       = (UINT_64*)other_screen;
//	UINT_32 i          = 393216; // 384 K BLOCKS
//	while(i--)
//		*dst++ = *src++;
//}
//
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
//	sse_copy_current_screen_to_other_screen();
//	draw_partial_desktop(&(big_rect.rect));
//	draw_list_of_overlapped_windows(&big_rect);
//	
//	/* UPDATE THE FOCUS WINDOW AND DRAW IT IN THE NEW POSITION */
//	wnd->rect.up_left.x += delta_x;
//	wnd->rect.up_left.y += delta_y;
//	draw_window(wnd, other_screen);
//	
//	page_flip((PHYSICAL_ADDRESS(LFB) - PHYSICAL_ADDRESS(other_screen))  );//>> 5 );
//	UINT_32 swap   = PHYSICAL_ADDRESS(current_screen);
//	current_screen = (UINT_8*)((void*)(PHYSICAL_ADDRESS(other_screen)));
//	other_screen   = (UINT_8*)((void*)(swap));
//	
//	//blit_off_screen_to_screen(&big_rect);
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
//	wnd->window_objects    = 0;
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
//			*trg++ = 0x0041707E;
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
//	desktop.window_objects    = 0;
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+