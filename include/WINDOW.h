#ifndef _WINDOW__H__
#define _WINDOW__H__

#include "LiBOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

enum {
    lclick_event,
    move_event,
};

typedef struct _POINT {
	UINT_32 x;
	UINT_32 y; 
} POINT;

typedef struct _RECT {
	POINT   up_left;
	POINT   down_right;
	UINT_32 width;
	UINT_32 height;   
} RECT;

typedef struct _TITLE_BAR {
	RECT    rect;
	POINT   window_name_position; // default = {0, 0}
	UINT_32 color;                // default = light blue
} TITLE_BAR;

typedef struct _BORDER_FRAME {
	RECT    rect;
	UINT_32 color; // default = black
} BORDER_FRAME;

typedef struct _WINDOW {
	RECT            rect;
	TITLE_BAR*      title_bar;
	BORDER_FRAME*   border_frame;
	const INT_8*    window_name;
	struct _WINDOW* children;
	UINT_32         how_many_children;  
	UINT_8*         buffer;
} WINDOW;

void init_desktop(void);
BOOL register_window(WINDOW* wnd, const INT_8* name, UINT_32 width, UINT_32 height, POINT* up_left);
void draw_window(WINDOW* wnd, UINT_8* framebuffer);
void window_manager(INT_8* report_packet, UINT_8 event_type, INT_32 x, INT_32 y);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _WINDOW__H__
