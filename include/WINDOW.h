#ifndef _WINDOW__H__
#define _WINDOW__H__

#include "LiBOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _ALPHANUMERIC {
	UINT_32 pixels[4*7];
} ALPHANUMERIC;

enum {
	lclick_event,
	move_event,
	release_event,
};

enum {
	OBJECT_BUTTON,
	OBJECT_SLIDEBAR,
	OBJECT_TEXTFIELD,
};

typedef struct _POINT {
	INT_32 x;
	INT_32 y; 
} POINT;

typedef struct _RECT {
	POINT   up_left;
	UINT_32 width;
	UINT_32 height;   
} RECT;


typedef struct _WINDOW_OBJECT {
	UINT_8           object_identifier;
	UINT_8*          upper_left_corner_address;
	const INT_8*     text;
	union {
		BOOL         pressed;
		UINT_32      slide_bar_distance;
		const INT_8* text_field_capture_text;
	} attributes;
	void(*object_handler)(void*);
} WINDOW_OBJECT;

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
	UINT_32         z_order; 
	UINT_8*         buffer;
	WINDOW_OBJECT*  window_objects[64];
	UINT_32         number_of_window_objects;
} WINDOW;

extern void _sse_blit_off_screen_to_screen(UINT_8* src, UINT_8* trg, UINT_32 blocks_of_16_in_width);
void        init_desktop(void);
BOOL        register_window(WINDOW* wnd, const INT_8* name, UINT_32 width, UINT_32 height, POINT* up_left);
void        draw_window(WINDOW* wnd, UINT_8* framebuffer);
void        window_manager(INT_8* report_packet, UINT_8 event_type, INT_32 x, INT_32 y, UINT_8* ioctl_payload);
BOOL        register_object(WINDOW* wnd, WINDOW_OBJECT* obj);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _WINDOW__H__
