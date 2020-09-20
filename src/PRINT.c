#include "../include/PRINT.h"
#include "../include/SVGA.h"
#include "../include/FONT.h"
#include "../include/KEYBOARD.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

UINT_8 global_character_sets[0xFF] = {
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', ' ', '!', 
	'"', '#', '$', '%', '&', '.', '(', ')', '*', '+', ',', '-', '.', '/',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', 
	'=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 
	'Y', 'Z', '[', '\\', ']', '^', '_', '.', 'a', 'b', 'c', 'd', 'e', 'f', 
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 
	'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', '.', '.', '.', '.', '.', 
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
	'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 
	'.', '.', '.'
};

UINT_32 counter    = 0;
UINT_32 line       = 1;
INT_32  ARGS_LIST[64];
INT_32* PARGS_LIST = ARGS_LIST;
void print_char      ( INT_8 id, UINT_8 print_color );
void print_number    ( UINT_32 n );
void print_hex_number( UINT_32 n );
UINT_8 displays[160*25*4];

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ END OF TEXT CONSULE 
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ START OF GRAPHIC CONSULE

       BOOL     LiBOS_graphic_mode    = FALSE; // we are in consule debugger mode
static UINT_32* graphic_mode_displays = 0;     // we have only access to 2 graphic mode displays
static UINT_32  key_stroke            = 0;

/* VERY IMPORTANT */
static UINT_32 consule_x = 0;
static UINT_32 consule_y = 0;

/* main framebuffer pointer */
UINT_32* fb = 0;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void consule_blit(UINT_8* src) 
{
	UINT_32 i;
	UINT_8* vid = ( UINT_8* )0xb8000;
	for(i=0; i<160*25; i++)
		vid[i] = src[i];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void clear_screen( void )
{
	UINT_8* vid = ( UINT_8* )0xb8000;
	int i;
	for(i=0; i<160*50; i+=2) {
		vid[i]   = ' ';
		vid[i+1] = 0;
	}
	counter = 0;
	line = 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void printk_impl(INT_8* fmt, UINT_32 color)
{
	INT_32 ccounter = 1;
	while (*fmt)
	{
		if (*fmt == '%')
		{
			print_number(ARGS_LIST[ccounter]);
			ccounter++;
		}
		else if (*fmt == '^')
		{
			print_hex_number(ARGS_LIST[ccounter]);
			ccounter++;
		}
		else
			print_char((INT_8)*fmt, color);
		fmt++;
	}
	
	if( graphic_mode() )
	{
		/* do the actual blit */
		//void*    pointer_inside_displays = (void*)(PHYSICAL_ADDRESS(graphic_mode_displays));// + (4096 * key_stroke));
		__LiBOS_MemCopy(graphic_mode_displays /*pointer_inside_displays*/, fb, MEGA_BYTE(3));
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void panic( INT_8* id )
{
	UINT_8 panic_color = 4;
	printk_impl(id, panic_color);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void print_char( INT_8 id, UINT_8 print_color )
{
	/* check if we are in graphic mode */
	if( graphic_mode() )
	{
		consule_draw_on_framebuffer(id);
		return;
	}
	
	/* we are still in text-consule-debugger mode */
	UINT_8* vid = (UINT_8*)displays;
	if ( id == '\n' )
	{
		counter += ( line * 160 - counter );
		id++;
		line++;
	}
	else 
	{
		vid[counter]     = id;
		vid[counter + 1] = print_color;
		counter         += 2;
		id++;
	}
	consule_blit(vid);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void print_number( UINT_32 n )
{
	UINT_32  m, i=1, nn = n, bk_i;
	UINT_8 lut[0x0A] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	UINT_8 buf[0x20];
	
	/* get size */
	while(nn >= 10)
	{
		i++;
		nn/=10;
	}
	bk_i = i;
	buf[i] = 'j';
	i--;
	nn = n;
	while(nn >= 10)
	{
		m = nn%10;
		buf[i] = lut[m];
		i--;
		nn/=10;
	}
	buf[i] = lut[nn];
	for(i = 0; i < bk_i; i++)
		print_char(buf[i], 2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void print_hex_number( UINT_32 n )
{
	UINT_32  m, i=1, nn = n, bk_i;
	UINT_8 lut[0x10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	UINT_8 buf[0x20];
	
	/* get size */
	while(nn >= 16)
	{
		i++;
		nn/=16;
	}
	bk_i = i;
	buf[i] = 'j';
	i--;
	nn = n;
	while(nn >= 16)
	{
		m = nn%16;
		buf[i] = lut[m];
		i--;
		nn/=16;
	}
	buf[i] = lut[nn];
	
	print_char('0', 2);
	print_char('x', 2);
	
	for(i = 0; i < bk_i; i++)
		print_char(buf[i], 2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_8* Disps(void)
{
	return displays;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void __LiBOS_HexDump(void* pointer_to_buffer, UINT_32 bytes, INT_8* begin_message)
{
	if (!pointer_to_buffer)
		return;

	if (!bytes)
		return;
		
	if (begin_message)
	{
		printk(begin_message);
		printk(" hex dump:\n");
	}

	UINT_32 i;
	UINT_32 word_counter = 0;
	UINT_8* ptr          = (UINT_8*)pointer_to_buffer;

	for (i = 0; i < bytes; i++)
	{
		if (word_counter < 15)
		{
			printk("^ ", (UINT_32)ptr[i]);
			word_counter++;
		}
		else
		{
			printk("\n");
			word_counter = 0;
			printk("^ ", (UINT_32)ptr[i]);
		}
	}
	
	printk("\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void __LiBOS_ChrDump (void* pointer_to_buffer, UINT_32 bytes)
{
	if (!pointer_to_buffer)
		return;

	if (!bytes)
		return;
	
	UINT_32 i;
	UINT_8* ptr = (UINT_8*)pointer_to_buffer;

	for (i = 0; i < bytes; i++)
	{
		print_char(global_character_sets[(ptr[i])], 2);
	}
	
	printk("\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GRAPHIC MODE DEBUGGING

void consule_draw_on_framebuffer(INT_8 ch)
{
	UINT_32* letter = 0;
	switch (ch)
	{
		case 'A': letter = get_A_data     (); break;
		case 'B': letter = get_B_data     (); break;
		case 'C': letter = get_C_data     (); break;
		case 'D': letter = get_D_data     (); break;
		case 'E': letter = get_E_data     (); break;
		case 'F': letter = get_F_data     (); break;
		case 'G': letter = get_G_data     (); break;
		case 'H': letter = get_H_data     (); break;
		case 'I': letter = get_I_data     (); break;
		case 'J': letter = get_J_data     (); break;
		case 'K': letter = get_K_data     (); break;
		case 'L': letter = get_L_data     (); break;
		case 'M': letter = get_M_data     (); break;
		case 'N': letter = get_N_data     (); break;
		case 'O': letter = get_O_data     (); break;
		case 'P': letter = get_P_data     (); break;
		case 'Q': letter = get_Q_data     (); break;
		case 'R': letter = get_R_data     (); break;
		case 'S': letter = get_S_data     (); break;
		case 'T': letter = get_T_data     (); break;
		case 'U': letter = get_U_data     (); break;
		case 'V': letter = get_V_data     (); break;
		case 'W': letter = get_W_data     (); break;
		case 'X': letter = get_X_data     (); break;
		case 'Y': letter = get_Y_data     (); break;
		case 'Z': letter = get_Z_data     (); break;
		case 'a': letter = get_a_data     (); break;
		case 'b': letter = get_b_data     (); break;
		case 'c': letter = get_c_data     (); break;
		case 'd': letter = get_d_data     (); break;
		case 'e': letter = get_e_data     (); break;
		case 'f': letter = get_f_data     (); break;
		case 'g': letter = get_g_data     (); break;
		case 'h': letter = get_h_data     (); break;
		case 'i': letter = get_i_data     (); break;
		case 'j': letter = get_j_data     (); break;
		case 'k': letter = get_k_data     (); break;
		case 'l': letter = get_l_data     (); break;
		case 'm': letter = get_m_data     (); break;
		case 'n': letter = get_n_data     (); break;
		case 'o': letter = get_o_data     (); break;
		case 'p': letter = get_p_data     (); break;
		case 'q': letter = get_q_data     (); break;
		case 'r': letter = get_r_data     (); break;
		case 's': letter = get_s_data     (); break;
		case 't': letter = get_t_data     (); break;
		case 'u': letter = get_u_data     (); break;
		case 'v': letter = get_v_data     (); break;
		case 'w': letter = get_w_data     (); break;
		case 'x': letter = get_x_data     (); break;
		case 'y': letter = get_y_data     (); break;
		case 'z': letter = get_z_data     (); break;
		case '0': letter = get_0_data     (); break;
		case '1': letter = get_1_data     (); break;
		case '2': letter = get_2_data     (); break;
		case '3': letter = get_3_data     (); break;
		case '4': letter = get_4_data     (); break;
		case '5': letter = get_5_data     (); break;
		case '6': letter = get_6_data     (); break;
		case '7': letter = get_7_data     (); break;
		case '8': letter = get_8_data     (); break;
		case '9': letter = get_9_data     (); break;
		case '.': letter = get_dot_data   (); break;
		case ',': letter = get_comma_data (); break;
		case ':': letter = get_coloum_data(); break;
		case '*': letter = get_star_data  (); break;
		case '-': letter = get_minus_data (); break;
		case '+': letter = get_plus_data  (); break;
		case '=': letter = get_equal_data (); break;
		case '!': letter = get_danger_data(); break;
		case ' ': letter = get_space_data (); break;
		case '\n':
		{
			consule_x = 0;
			consule_y += 14;
			return;
		}
	}
	
	UINT_32 i, j;
	for(j=0; j<12; j++)
		for(i=0; i<9; i++)
			graphic_mode_displays[((consule_y + j) * 1024) + (consule_x + i)] = ~(letter[(9 * j) + i]);
	
	consule_x += 9;
	
	/* sanity check */
	if(consule_x >= 1024)
	{
		consule_x = 0;
		consule_y += 14;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/* 
   This is very small graphic mode debugger.
   Only upper/lower case letters, numbers, and these special characters can be given:
   ' ', '.', '+', '-', '*', ':', '=', and ','
 */
void graphic_print(const INT_8* str)
{
	INT_8* ch = 0;
	while(ch = *str)
	{
		consule_draw_on_framebuffer(ch);
		str++;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL graphic_mode(void)
{
	return LiBOS_graphic_mode;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void handle_key_events_in_graphical_displays(KEYBOARD* kb)
{
	/* sanity check */
	if(!kb->arrow_keys)
		return;
	
	/* set a pointer */
	void* pointer_inside_displays = 0;
	
	if(kb->arrow_keys == ARROW_DOWN) /* move framebuffer down */
	{
		if(key_stroke >= 768)
		{
			/* we reached the end of displays! */
			return;
		}
		
		key_stroke+=14;
		pointer_inside_displays = (void*)(PHYSICAL_ADDRESS(graphic_mode_displays) + (4096 * key_stroke));
		
		/* do the blit */
		__LiBOS_MemCopy(pointer_inside_displays, fb, MEGA_BYTE(3));
	}
	else if(kb->arrow_keys == ARROW_UP) /* move framebuffer up */
	{
		if(key_stroke <= 0)
		{
			/* we reached the end of displays! */
			return;
		}
		
		key_stroke-=14;
		pointer_inside_displays = (void*)(PHYSICAL_ADDRESS(graphic_mode_displays) + (4096 * key_stroke));
		
		/* do the blit */
		__LiBOS_MemCopy(pointer_inside_displays, fb, MEGA_BYTE(3));
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void graphical_reboot(void)
{
	/* destroy all displays */
	__LiBOS_MemZero(graphic_mode_displays, (2* MEGA_BYTE(3)));
	key_stroke = consule_x = consule_y = 0;
	
	printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	printk("                         REBOOT                         \n");
	printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	
	__LiBOS_MemCopy(graphic_mode_displays, fb, MEGA_BYTE(3));
	kernel_free(graphic_mode_displays);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void allocate_graphic_mode_displays(void)
{
	/* get the framebuffer pointer */
	fb = (UINT_32*)(get_svga_instance()->LFB);
	
	/* allocate 2 displays (each 3MB) */
	graphic_mode_displays = (UINT_32*)( kernel_alloc(2 * MEGA_BYTE(3), 1, 0xFFFFFFFF) );
	__LiBOS_MemZero(graphic_mode_displays, (2 * MEGA_BYTE(3)));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+