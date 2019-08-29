#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

unsigned int counter = 0;
unsigned int line = 1;
int          ARGS_LIST[64];
int*         PARGS_LIST = ARGS_LIST;
void print_char( char id, unsigned char print_color );
void print_number( unsigned int n );
void print_hex_number( unsigned int n );
unsigned char displays[160*25*4];

void __IMSO_Blit(unsigned char* src) 
{
	int i;
	unsigned char* vid = (unsigned char*)0xb8000;
	for(i=0;i<160*25;i++)
		vid[i]=src[i];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void clear_screen( void )
{
	unsigned char* vid = ( unsigned char* )0xb8000;
	int i;
	for(i=0; i<160*50; i+=2) {
		vid[i]   = ' ';
		vid[i+1] = 0;
	}
	counter = 0;
	line = 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void printk_impl(char* fmt, unsigned int color)
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
            print_char((char)*fmt, color);
        fmt++;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void panic( char* id )
{
	unsigned char panic_color = 4;
	printk_impl(id, panic_color);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void print_char( char id, unsigned char print_color )
{
	unsigned char* vid = (unsigned char*)displays;
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

void print_number( unsigned int n )
{
    unsigned int  m, i=1, nn = n, bk_i;
    unsigned char lut[0x0A] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    unsigned char buf[0x20];
    
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

void print_hex_number( unsigned int n )
{
    unsigned int  m, i=1, nn = n, bk_i;
    unsigned char lut[0x10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    unsigned char buf[0x20];
    
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

unsigned char* Disps(void)
{
	return displays;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+