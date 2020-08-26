#include "../include/KEYBOARD.h"
#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

KEYBOARD global_OS_keyboard;
UINT_32  SHIFT_PRESSED           = 0;
UINT_32  ALT_PRESSED             = 0;
UINT_32  CTRL_PRESSED            = 0;
UINT_8   DisplayMovedDown        = 0;
UINT_8   KEYBOARD_SCANCODES[128] =
{
	0,  27, '1', '2', '3', '4', '5', '6', '7', '8',		/* 9 */
	'9', '0', '-', '=', '\b',							/* Backspace */
	'\t',												/* Tab */
	'q', 'w', 'e', 'r',									/* 19 */
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
	0,													/* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
	'\'', '`',   0,										/* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',					/* 49 */
	'm', ',', '.', '/',   0,							/* Right shift */
	'*',
	0,		/* Alt */
	' ',	/* Space bar */
	0,		/* Caps lock */
	0,		/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,		/* < ... F10 */
	0,		/* 69 - Num lock*/
	0,		/* Scroll Lock */
	0,		/* Home key */
	0,		/* Up Arrow */
	0,		/* Page Up */
	'-',	
	0,		/* Left Arrow */
	0,	
	0,		/* Right Arrow */
	'+',	
	0,		/* 79 - End key*/
	0,		/* Down Arrow */
	0,		/* Page Down */
	0,		/* Insert Key */
	0,		/* Delete Key */
	0,   0,   0,
	0,		/* F11 Key */
	0,		/* F12 Key */
	0,		/* All other keys are undefined */
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void __LiBOS_EvaluateArrowKeys(void)
{
	UINT_32 i;
	UINT_8* vid = ( UINT_8* )0xb8000;
	UINT_8* displays = Disps();
	if(!global_OS_keyboard.arrow_keys)
		return;
	if(global_OS_keyboard.arrow_keys == ARROW_DOWN)
	{
		for(i=0;i<160*25;i++)
			vid[i] = displays[i+( (DisplayMovedDown + 1)*160 )];
		DisplayMovedDown++;
	}
	else if(global_OS_keyboard.arrow_keys == ARROW_UP && DisplayMovedDown > 0)
	{
		for(i=0;i<160*25;i++)
			vid[i] = displays[i+( (DisplayMovedDown - 1)*160 )];
		DisplayMovedDown--;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 RegisterKeyboard(KEYBOARD* keyboard)
{
	if(!keyboard)
	{
		panic("Invalid keyboard object passed\n");
		return 0;
	}
	
	Port_8_Register( &keyboard->KBD_STATUS_PORT );
	Port_8_Register( &keyboard->KBD_DATA_PORT );
	keyboard->last_registered_key = 0;
	keyboard->arrow_keys = 0;
	
	global_OS_keyboard = *keyboard;
	
	irq_install_handler(1, &keyboard_handler);
	//.printk( "        >>> Keyboard registered successfully <<<\n");
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void keyboard_handler(REGS* r)
{
	UINT_8 scancode, status;
	
	/* write EOI */
	global_OS_keyboard.KBD_STATUS_PORT.Write(0x20, 0x20);
	global_OS_keyboard.arrow_keys = 0;
	status = global_OS_keyboard.KBD_STATUS_PORT.Read(0x64);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01)
	{
		scancode = global_OS_keyboard.KBD_DATA_PORT.Read(0x60);
		global_OS_keyboard.last_registered_key = KEYBOARD_SCANCODES[scancode];
        
		if(global_OS_keyboard.last_registered_key == 27) // ESC pressed for restart
		{
			clear_screen();
			go_to_reset();
		}
        
		if (scancode & 0x80)
		{
			if(scancode == 170 || scancode == 182) // shift released
			{
				SHIFT_PRESSED = 0;
				return;
			}
		}
		else
		{
			if(scancode == 42 || scancode == 54)
			{
				SHIFT_PRESSED = 1;
				return;
			}
			else
			{
				if(SHIFT_PRESSED)
				{
					global_OS_keyboard.last_registered_key = KEYBOARD_SCANCODES[scancode] - 32;
					//print_char( KEYBOARD_SCANCODES[scancode] - 32, 2 );
				}
				else
				{
					global_OS_keyboard.last_registered_key = KEYBOARD_SCANCODES[scancode];
					//print_char( KEYBOARD_SCANCODES[scancode], 2 );
					if(scancode == ARROW_UP)
						global_OS_keyboard.arrow_keys = ARROW_UP;
					else if(scancode  == ARROW_DOWN)
						global_OS_keyboard.arrow_keys = ARROW_DOWN;
					else if(scancode  == ARROW_RIGHT)
						global_OS_keyboard.arrow_keys = ARROW_RIGHT;
					else if(scancode  == ARROW_LEFT)
						global_OS_keyboard.arrow_keys = ARROW_LEFT;
				}
			}
		}
	}
	__LiBOS_EvaluateArrowKeys();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void getch(UINT_8 keycode_to_break)
{
	_STI();
	while(1)
	{
		if(global_OS_keyboard.last_registered_key == keycode_to_break)
		{
		
			printk("Brocken\n");
			return;
		}
	}
	_CLI();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PM_set_keyboard(void)
{
	irq_install_handler(1, &keyboard_handler);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
