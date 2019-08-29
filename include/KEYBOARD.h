#ifndef _KEYBOARD__H__
#define _KEYBOARD__H__

#include "../include/IDT.h"
#include "../include/PORT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef enum _DIRECTION_KEYS {
	ARROW_UP    = 0x48,
	ARROW_DOWN  = 0x50,
	ARROW_RIGHT = 0x4D,
	ARROW_LEFT  = 0x4B	
} DIRECTION_KEYS;

typedef struct _KEYBOARD {
	PORT_8        KBD_STATUS_PORT;
	PORT_8        KBD_DATA_PORT;
	unsigned char last_registered_key;
	unsigned char arrow_keys;	
} __attribute__((packed))
KEYBOARD, *PKEYBOARD;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int RegisterKeyboard(PKEYBOARD keyboard);
void         keyboard_handler(PREGS r);
void         getch(unsigned char keycode_to_break);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _KEYBOARD__H__
