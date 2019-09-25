#ifndef _KEYBOARD__H__
#define _KEYBOARD__H__

#include "LiBOS_CORE.h"
#include "IDT.h"
#include "PORT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef enum _DIRECTION_KEYS {
	ARROW_UP    = 0x48,
	ARROW_DOWN  = 0x50,
	ARROW_RIGHT = 0x4D,
	ARROW_LEFT  = 0x4B	
} DIRECTION_KEYS;

typedef struct _KEYBOARD {
	PORT_8 KBD_STATUS_PORT;
	PORT_8 KBD_DATA_PORT;
	UINT_8 last_registered_key;
	UINT_8 arrow_keys;	
} __attribute__((packed)) KEYBOARD;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int RegisterKeyboard(KEYBOARD* keyboard);
void         keyboard_handler(REGS* r);
void         getch           (UINT_8 keycode_to_break);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _KEYBOARD__H__
