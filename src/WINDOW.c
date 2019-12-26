#include "../include/WINDOW.h"
#include "../include/SVGA.h"
#include "../include/MEMORY.h"
#include "../include/PORT.h"

#define SIGNED_8_BIT(UNSIGNED_8_BIT) (INT_8)((UINT_8)(UNSIGNED_8_BIT ^ 0xFF))
#define ALIGN_8(N) (N+7) & (~7) 
void draw_window(WINDOW* wnd, UINT_8* framebuffer);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

PORT_8 p;
static UINT_32 z_order[1024];
static UINT_32 number_of_registered_windows = 0;
static WINDOW* registered_windows[1024];
static WINDOW desktop;
static WINDOW* focus = 0;
static RECT dirty_rect_1;
static RECT dirty_rect_2;

static WINDOW* list_of_overlapped_windows[1024];
static UINT_32 number_of_overlapped_windows = 1;

static UINT_8 off_screen[1024*768*4];


void vsync(void)
{
	Port_8_Register(&p);
	while ( (p.Read(0x03da) & 0x08));
	while (!(p.Read(0x03da) & 0x08));
}


void blit_off_screen_to_screen(WINDOW* wnd)
{
	UINT_64* on  = (UINT_64*)(&((get_svga_instance()->LFB)[(wnd->rect.up_left.y << 12) + (wnd->rect.up_left.x << 2)]));
	UINT_64* off = (UINT_64*)(&(off_screen                [(wnd->rect.up_left.y << 12) + (wnd->rect.up_left.x << 2)]));

	UINT_32 h = wnd->rect.height;
	UINT_32 w = wnd->rect.width; 
	UINT_32 i = 0;
	
	while(h)
	{
		while(i < (w >> 1))
		{
			*on++ = *off++;
			i++;
		}
		i = 0;
		on  = (UINT_64*)(PHYSICAL_ADDRESS(on)  + 4096 - (w << 2));
		off = (UINT_64*)(PHYSICAL_ADDRESS(off) + 4096 - (w << 2));
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

static void add_to_z(WINDOW* new_window)
{
	z_order[number_of_registered_windows] = 0;
	UINT_32 i;
	
	for(i = 0; i < number_of_registered_windows; i++)
	{
		if( overlap(registered_windows[i], registered_windows[number_of_registered_windows]) )
			z_order[number_of_registered_windows]++;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void draw_rect_at(WINDOW* wnd, UINT_8* fb)
{
	RECT r = wnd->rect;
	UINT_32 width = r.width;
	UINT_32 height = r.height;
	
	//UINT_8* src = (UINT_8*)wnd->buffer;
	UINT_8* trg = (UINT_8*)(&fb[(r.up_left.y << 12) + (r.up_left.x << 2)]);
	
	UINT_32 i = 0;
	while(height)
	{
		while(i < (width << 2))
		{
			//*trg++ = *src++;
			*trg++ = 0;
			i++;
		}
		i = 0;
		trg += (4096 - (width << 2));
		height--;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void find_overlapped_rect(WINDOW* src, WINDOW* trg, RECT* r)
{
	if(src->rect.up_left.x <= trg->rect.up_left.x)
	{
		r->up_left.x = trg->rect.up_left.x;
		r->width = ((trg->rect.up_left.x + trg->rect.width) >= (src->rect.up_left.x + src->rect.width)) ? src->rect.width + src->rect.up_left.x - trg->rect.up_left.x : trg->rect.width;
	}
	else
	{
		r->up_left.x = src->rect.up_left.x;
		r->width = ((trg->rect.up_left.x + trg->rect.width) >= (src->rect.up_left.x + src->rect.width)) ? src->rect.width : trg->rect.width + trg->rect.up_left.x - src->rect.up_left.x;
	}
	
	if(src->rect.up_left.y <= trg->rect.up_left.y)
	{
		r->up_left.y = trg->rect.up_left.y;
		r->height = ((trg->rect.up_left.y + trg->rect.height) >= (src->rect.up_left.y + src->rect.height)) ? src->rect.height + src->rect.up_left.y - trg->rect.up_left.y : trg->rect.height;
	}
	else
	{
		r->up_left.y = src->rect.up_left.y;
		r->height = ((trg->rect.up_left.y + trg->rect.height) >= (src->rect.up_left.y + src->rect.height)) ? src->rect.height : trg->rect.height + trg->rect.up_left.y - src->rect.up_left.y;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void reorder_z_right_after_event(WINDOW* focused_window)
{
	if(!focused_window)
        return;
    
    UINT_32 i;
	UINT_32 trg = 0;
	for(i = 0; i < number_of_registered_windows; i++)
	{
		if(registered_windows[i] == focused_window)
			trg = i;
	}
    
    UINT_32 before = z_order[trg];
    UINT_32 final = 0;
    
	for(i = 0; i < number_of_registered_windows; i++)
	{
		if(i == trg)
            continue;
        
        if( overlap(registered_windows[i], registered_windows[trg]) )
		{
			if(before < z_order[i])
            {
                final = (final > z_order[i]) ? final : z_order[i];     
                
                if(z_order[i])
                    z_order[i]--;   
            }
		}
	}
    z_order[trg] = (!final) ? before : final;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static WINDOW* find_responsive_window(INT_32 x, INT_32 y)
{
	UINT_32 i;
	
	INT_32 num = -1;
	INT_32 tmp_num = -1;
	UINT_32 ret_num = 0xFFFFFFFF;
	for(i = 0; i < number_of_registered_windows; i++)
	{
		WINDOW* src = registered_windows[i];
		if((src->rect.up_left.x <= x) && ((src->rect.up_left.x + src->rect.width) >= x))
		{
			if((src->rect.up_left.y <= y) && ((src->rect.up_left.y + src->rect.height) >= y))
			{
				if(tmp_num >= 0)
				{
					num = tmp_num;
					tmp_num = i;
					if(z_order[num] < z_order[tmp_num])
						ret_num = tmp_num;
					else 
						ret_num = num;
				}
				else
				{
					tmp_num = i;
                    ret_num = tmp_num;
				}
			}
		}
	}
    if(ret_num != 0xFFFFFFFF)
    {
        //std::cout << "responsive window is: " << registered_windows[ret_num]->window_name << std::endl;
        return registered_windows[ret_num];
    }
    
    return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void sort_list_of_overlapped_windows(void)
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void draw_partial_desktop(RECT* r)
{
	//UINT_32 h = r->height;
	//UINT_32 w = r->width;
	//UINT_8* fb = get_svga_instance()->LFB;
	//
	//UINT_8* buf = &(desktop.buffer[(r->up_left.y << 12) + (r->up_left.x << 2)]);
	//UINT_8* trg = &(fb[(r->up_left.y << 12) + (r->up_left.x << 2)]);
	//
	//UINT_32* pbuf = (UINT_32*)buf;
	//UINT_32* ptrg = (UINT_32*)trg;
	//
	//UINT_32 i = 0;
	//while(h)
	//{
	//	while(i < w)
	//	{
	//		*ptrg++ = *pbuf++;
	//		i++;
	//	}
	//	i = 0;
	//	ptrg = PHYSICAL_ADDRESS(ptrg) + 4096 - (w << 2);
	//	h--;
	//}

	UINT_8* fb = off_screen; //get_svga_instance()->LFB;
	UINT_32  row_blocks = (r->width >> 1);
	UINT_64* src    = (UINT_64*)(&(desktop.buffer[(r->up_left.y << 12) + (r->up_left.x << 2)]));
	UINT_64* dst    = (UINT_64*)(&(fb[(r->up_left.y << 12) + (r->up_left.x << 2)]));
	UINT_32 h = r->height ;
	INT_32 i = row_blocks ;
	while(h)
	{
		while(i--)
			*dst++ = *src++;
		i   = row_blocks;
		dst = (UINT_64*)((void*)(PHYSICAL_ADDRESS(dst) + 4096 - (r->width << 2)));
		h--;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void draw_partial_window(WINDOW* wnd, RECT* clip, UINT_8* framebuffer)
{
	if(!wnd)
		return;
	
	UINT_32 i = 0;
	UINT_32 h = clip->height;
	UINT_32 w = clip->width;
	UINT_32 x = clip->up_left.x - wnd->rect.up_left.x;
	UINT_32 y = clip->up_left.y - wnd->rect.up_left.y;
	
	UINT_32* pbuf = (UINT_32*)(&(wnd->buffer[((y * wnd->rect.width) + x) << 2]));
	UINT_32* ptrg = (UINT_32*)(&(framebuffer[(clip->up_left.y << 12) + (clip->up_left.x << 2)]));

	while(h)
	{
		while(i < w)
		{
			*ptrg++ = *pbuf++;
			i++;
		}
		i = 0;
		pbuf = PHYSICAL_ADDRESS(pbuf) + ((wnd->rect.width - w) << 2);
		ptrg = PHYSICAL_ADDRESS(ptrg) + 4096 - (w << 2);
		h--;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void draw_list_of_overlapped_windows(WINDOW* focused_window)
{
	if(!focused_window)
		return;
	
	UINT_32 i = 1;
	RECT clip = {{0, 0}, 0, 0};
	while(i < number_of_overlapped_windows)
	{
		//find_overlapped_rect(focused_window, list_of_overlapped_windows[i], &clip);
		//draw_partial_window(list_of_overlapped_windows[i], &clip, get_svga_instance()->LFB);
		//i++;
		draw_window(list_of_overlapped_windows[i], off_screen); //get_svga_instance()->LFB);
		i++;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void event_at(INT_32 x, INT_32 y, INT_32 delta_x, INT_32 delta_y)
{
    WINDOW* trg_wnd = find_responsive_window(x, y);
	if(trg_wnd)
    {
		// keep a copy of the target window
		WINDOW initial = *trg_wnd; 
		
		// update the target window
		trg_wnd->rect.up_left.x += delta_x;
		trg_wnd->rect.up_left.y += delta_y;      
		
		// reorder z after moveing the target window
		reorder_z_right_after_event(trg_wnd); 
		
		// check the windows that can be responsive to the initial window
		UINT_32 i;
		for(i = 0; i < number_of_registered_windows; i++)
		{
			if( (registered_windows[i] != &initial) && overlap(registered_windows[i], &initial) )
			{
				list_of_overlapped_windows[number_of_overlapped_windows] = registered_windows[i];
				number_of_overlapped_windows++;
			}
        }
		
		//sort_list_of_overlapped_windows(); /*TODO*/
		draw_partial_desktop(&(initial.rect));
		draw_list_of_overlapped_windows(&initial);
		
		vsync();
		blit_off_screen_to_screen(&initial);
		
		draw_window(trg_wnd, get_svga_instance()->LFB);
		
		UINT_32 j;
		for(j = 1; j<number_of_overlapped_windows; j++)
			list_of_overlapped_windows[j] = 0;
		number_of_overlapped_windows = 1;
		
        return;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL register_window(WINDOW* wnd, const INT_8* name, UINT_32 _width, UINT_32 _height, POINT* up_left) 
{
	if((!_width) || (!_height))
		return FALSE;
	if(up_left->x >= 1024 || up_left->y >= 768)
		return FALSE;
	UINT_32 width = ALIGN_8(_width);
	UINT_32 height = _height;
	
	wnd->window_name = name;
	wnd->rect.width = width;
	wnd->rect.height = height;
	wnd->rect.up_left.x = up_left->x;
	wnd->rect.up_left.y = up_left->y;
	
	wnd->children = 0;
	wnd->how_many_children = 0;
	
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

void dump_z(void)
{
	//UINT_32 i;
	//std::cout << "-+-+-+ z-order dump -+-+-+-+-+-+-+-+-+" << std::endl;
	//for(i = 0; i < number_of_registered_windows; i++)
	//	std::cout << "\t" << registered_windows[i]->window_name << ": " << z_order[i] << std::endl;
    //std::cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void dump_dirty_rects(void)
{
    //std::cout << "-+-+-+ dirty rects dump -+-+-+-+-+-+-+" << std::endl;
    //std::cout << "\tdr1:up_left={" << dirty_rect_1.up_left.x << "," << dirty_rect_1.up_left.y << "}" << std::endl;
    //std::cout << "\tdr1:width=" << dirty_rect_1.width << std::endl;
    //std::cout << "\tdr1:height=" << dirty_rect_1.height << std::endl;
    //std::cout << "\tdr2:up_left={" << dirty_rect_2.up_left.x << "," << dirty_rect_2.up_left.y << "}" << std::endl;
    //std::cout << "\tdr2:width=" << dirty_rect_2.width << std::endl;
    //std::cout << "\tdr2:height=" << dirty_rect_2.height << std::endl;
    //std::cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void draw_back(WINDOW* wnd)
{
	UINT_32 i = 0, j = 0;
	UINT_32 h = wnd->rect.height;
	UINT_32 w = wnd->rect.width;

	UINT_32* buf = (UINT_32*)wnd->buffer;
	UINT_8* fb = get_svga_instance()->LFB;
	UINT_32* trg = (UINT_32*)(&fb[(wnd->rect.up_left.y << 12) + (wnd->rect.up_left.x << 2)]);
	
	i = 0;
	while(h)
	{
		while(i < w)
		{
			*trg++ = *buf++;
			i++;
		}
		i = 0;
		trg = PHYSICAL_ADDRESS(trg) + (4096 - (w << 2));
		h--;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void window_manager(INT_8* report_packet, UINT_8 event_type, INT_32 x, INT_32 y)
{
	print_mouse_x_y(x,y);
	
	switch (event_type) 
	{
		case lclick_event:
		{
		    if(focus)
				return;

			focus = find_responsive_window(x,y);
			reorder_z_right_after_event(focus);
			draw_window(focus, get_svga_instance()->LFB);
			break;
		}
		case move_event:
		{
			if(!focus)
			    return;

			WINDOW initial = *focus;
			event_at(initial.rect.up_left.x, initial.rect.up_left.y, (x - initial.rect.up_left.x), (y - initial.rect.up_left.y));
			focus->rect.up_left.x = x;
			focus->rect.up_left.y = y;
			
			reorder_z_right_after_event(focus);
			break; 
		}
		case release_event: focus = 0; return; 
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
		dst = (UINT_64*)((void*)(PHYSICAL_ADDRESS(dst) + 4096 - (wnd->rect.width << 2)));
		h--;
	}

	
	
	//UINT_32 i = 0;
	//UINT_32 h = wnd->rect.height;
	//UINT_32 w = wnd->rect.width;
	//UINT_32* pbuf = (UINT_32*)wnd->buffer;
	//UINT_32* ptrg = (UINT_32*)(&(framebuffer[(wnd->rect.up_left.y << 12) + (wnd->rect.up_left.x << 2)]));
//
	//while(h)
	//{
	//	while(i < w)
	//	{
	//		*ptrg++ = *pbuf++;
	//		i++;
	//	}
	//	i = 0;
	//	ptrg = PHYSICAL_ADDRESS(ptrg) + 4096 - (w << 2);
	//	h--;
	//}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void init_desktop(void)
{
	desktop.window_name = "LiBOS_desktop";
	desktop.rect.width = 1024;
	desktop.rect.height = 768;
	desktop.rect.up_left.x = 0;
	desktop.rect.up_left.y = 0;
	
	desktop.children = 0;
	desktop.how_many_children = 0;
	
	desktop.title_bar = 0;
	desktop.border_frame = 0;
	desktop.buffer = (UINT_8*)( Alloc((1024*768*4), 1, 1 ));
	
	UINT_32 i = 0;
	for(i=0; i < 1024*768*4; i += 4)
	{
		desktop.buffer[i]   = 0xD0;
		desktop.buffer[i+1] = 0x8C;
		desktop.buffer[i+2] = 0x44;
	}
	
	list_of_overlapped_windows[0] = &desktop;
	
	draw_window(&desktop, get_svga_instance()->LFB);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
