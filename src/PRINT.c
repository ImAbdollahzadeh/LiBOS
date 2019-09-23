#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

//.INT_32 current_x_pos = 0;
//.INT_32 current_y_pos = 0;

UINT_32 counter    = 0;
UINT_32 line       = 1;
INT_32  ARGS_LIST[64];
INT_32* PARGS_LIST = ARGS_LIST;
void print_char      ( INT_8 id, UINT_8 print_color );
void print_number    ( UINT_32 n );
void print_hex_number( UINT_32 n );
UINT_8 displays[160*25*4];

void __IMSO_Blit(UINT_8* src) 
{
	int i;
	UINT_8* vid = (UINT_8*)0xb8000;
	for(i=0;i<160*25;i++)
		vid[i]=src[i];
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
    int ccounter = 1;
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
	__IMSO_Blit(vid);
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

void __IMOS_HexDump(void* pointer_to_buffer, UINT_32 bytes, INT_8* begin_message)
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

//#define SIGNED(UNSIGNED) (INT_8)((UINT_8)(UNSIGNED ^ 0xFF))
void mouse_pointer_update(INT_8* report_packet)
{
	//UINT_8* vid = (UINT_8*)0xb8000;
	//INT_8 x = report_packet[1];
	//INT_8 y = report_packet[2];
	//
	//if(x & 0x80)
	//	x = -SIGNED(x);
	//if(y & 0x80)
	//	y = -SIGNED(y);
	//
	//UINT_32 position = ((current_y_pos * 160) + (current_x_pos << 1));
	//vid[position]     = ' ';
	//vid[position + 1] = 0x00;
	//
	//current_x_pos += (x >> 3);
	//current_y_pos += (y >> 3);
	//
	//if(current_x_pos >= 79) current_x_pos = 79;
	//if(current_y_pos >= 24) current_y_pos = 24;
	//if(current_x_pos <= 0)  current_x_pos = 0;
	//if(current_y_pos <= 0)  current_y_pos = 0;
	//
	//position = ((current_y_pos * 160) + (current_x_pos << 1));
	//
	//vid[position]     = 219;
	//vid[position + 1] = 0x04;	
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+