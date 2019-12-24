#include "../include/WINDOW.h"
#include "../include/SVGA.h"
#include "../include/MEMORY.h"

#define SIGNED_8_BIT(UNSIGNED_8_BIT) (INT_8)((UINT_8)(UNSIGNED_8_BIT ^ 0xFF))

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

static UINT_32 z_order[1024];
static UINT_32 number_of_registered_windows = 0;
static WINDOW* registered_windows[1024];
static WINDOW desktop;
static WINDOW* focus = 0;
static RECT dirty_rect_1;
static RECT dirty_rect_2;

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

static void draw_back(WINDOW* focused_window)
{
    UINT_32* t = (UINT_32*)(get_svga_instance()->LFB);
	*t++ = 0x00FF000;
	*t++ = 0x00FF000;
	*t++ = 0x00FF000;
	*t++ = 0x00FF000;
	
	WINDOW tmp1, tmp2;
    tmp1.rect = dirty_rect_1;
    tmp2.rect = dirty_rect_2;
	tmp1.buffer = (UINT_8*)(Alloc((dirty_rect_1.width * dirty_rect_1.height * 4) , 1 , 1));
    tmp2.buffer = (UINT_8*)(Alloc((dirty_rect_2.width * dirty_rect_2.height * 4) , 1 , 1));
	__LiBOS_MemZero((void*)tmp1.buffer, (dirty_rect_1.width * dirty_rect_1.height * 4));
	__LiBOS_MemZero((void*)tmp2.buffer, (dirty_rect_2.width * dirty_rect_2.height * 4));
	draw_window(&tmp1, get_svga_instance()->LFB);
	draw_window(&tmp2, get_svga_instance()->LFB);
	
    //::::UINT_32 i;
	//::::UINT_32 trg = 0;
	//::::for(i = 0; i < number_of_registered_windows; i++)
	//::::{
	//::::	if(registered_windows[i] == focused_window)
	//::::		trg = i;
	//::::}
    //::::
    //::::// AN IMPORTANT SANITY: draw desktop->buffer into fb @ position dirty_rect_1 and dirty_rect_2
    //::::/* TODO*/    
    //::::
	//::::for(i = 0; i < number_of_registered_windows; i++)
	//::::{
	//::::	if(i == trg)
    //::::        continue;
    //::::    
    //::::    if( overlap(registered_windows[i], &tmp1) )
	//::::	{
	//::::		// draw registered_windows[i]->buffer into fb @ position dirty_rect_1
	//::::		draw_window(registered_windows[i], get_svga_instance()->LFB);
	//::::	}
    //::::    
    //::::    if( overlap(registered_windows[i], &tmp2) )
	//::::	{
	//::::		// draw registered_windows[i]->buffer into fb @ position dirty_rect_2
	//::::		draw_window(registered_windows[i], get_svga_instance()->LFB);
	//::::	}
	//::::}
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

BOOL register_window(WINDOW* wnd, const INT_8* name, UINT_32 width, UINT_32 height, POINT* up_left) 
{
	if((!width) || (!height))
		return FALSE;
	if(up_left->x >= 1024 || up_left->y >= 768)
		return FALSE;
	
	wnd->window_name = name;
	wnd->rect.width = width;
	wnd->rect.height = height;
	wnd->rect.up_left.x = up_left->x;
	wnd->rect.up_left.y = up_left->y;
	wnd->rect.down_right.x = up_left->x + width;
	wnd->rect.down_right.y = up_left->y + height;
	
	wnd->children = 0;
	wnd->how_many_children = 0;
	
	TITLE_BAR title_bar;

	title_bar.window_name_position.x = 0;
	title_bar.window_name_position.y = 0;
	title_bar.rect.width = width;
	title_bar.rect.height = 16;
	title_bar.rect.up_left = *up_left;

	title_bar.rect.down_right.x = up_left->x + width;
	title_bar.rect.down_right.y = up_left->y + 16;
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

void window_manager(INT_8* report_packet, UINT_8 event_type, INT_32 x, INT_32 y)
{
	print_mouse_x_y(x,y);
	switch (event_type) 
    {
        case lclick_event:
        {
            focus = find_responsive_window(x,y);
			if(focus)
			{
				reorder_z_right_after_event(focus);
				draw_window(focus, get_svga_instance()->LFB);
			}
			
			focus = 0;
            break;
        }
        case move_event:
        {
            if(!focus)
                return;
            
            WINDOW* initial = focus;
            
            focus->rect.up_left.x += x;
            focus->rect.up_left.y += y;
            focus->rect.down_right.x += x;
            focus->rect.down_right.y += y;
            focus->title_bar->rect.up_left = focus->rect.up_left;
            focus->title_bar->rect.down_right.x = focus->rect.up_left.x + focus->rect.width;
            focus->title_bar->rect.down_right.y = focus->rect.up_left.y + 16;            
            dirty_rect_1.up_left.x = initial->rect.up_left.x;
            dirty_rect_1.up_left.y = initial->rect.up_left.y;
            dirty_rect_1.width  = x;
            dirty_rect_1.height = focus->rect.height;            
            dirty_rect_2.up_left.x = initial->rect.up_left.x + x;
            dirty_rect_2.up_left.y = initial->rect.up_left.y + y;
            dirty_rect_2.width  = focus->rect.width - x;
            dirty_rect_2.height = y; 

            draw_back(focus);
            
            focus = 0;
            break; 
        }
    }
}

////////////-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//////////
//////////    // suppose a lclick
//////////    window_manager(NULL, lclick_event, 55, 55);
//////////
//////////    // suppose a move to right down side
//////////    window_manager(NULL, move_event, 3, 1);
//////////}




//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void draw_window(WINDOW* wnd, UINT_8* framebuffer)
{
	UINT_32 i = 0, j = 0;
	UINT_32 h = wnd->rect.height;
	UINT_32 w = wnd->rect.width;

	UINT_32 J = wnd->rect.up_left.y;
	UINT_32 I = 4 * wnd->rect.up_left.x;
	UINT_8* buf = wnd->buffer;
	UINT_8* trg = &(framebuffer[(J<<12) + I]);
	
	i = 0;
	while(h)
	{
		while(i < (w << 2))
		{
			*trg++ = *buf++;
			i++;
		}
		i = 0;
		trg += (4 * (1024 - w));
		h--;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void init_desktop(void)
{
	desktop.window_name = "LiBOS_desktop";
	desktop.rect.width = 1024;
	desktop.rect.height = 768;
	desktop.rect.up_left.x = 0;
	desktop.rect.up_left.y = 0;
	desktop.rect.down_right.x = 1024;
	desktop.rect.down_right.y = 768;
	
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
	
	draw_window(&desktop, get_svga_instance()->LFB);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
