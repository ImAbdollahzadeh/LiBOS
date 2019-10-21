#include "../include/LiBOS_CORE.h"
#include "../include/LiBOS_LOGO.h"
#include "../include/GDT.h"
#include "../include/PORT.h"
#include "../include/IDT.h"
#include "../include/TIMER.h"
#include "../include/PRINT.h"
#include "../include/PCI.h"
#include "../include/AHCI.h"
#include "../include/EHCI.h"
#include "../include/XHCI.h"
#include "../include/FILESYSTEM.h"
#include "../include/FILE.h"
#include "../include/KEYBOARD.h"
#include "../include/MOUSE.h"
#include "../include/MEMORY.h"
#include "../include/SVGA.h"
#include "../include/DOSSPEC.h"

void movie_player(char* addr, UINT_8* framebuffer, UINT_8* fp, UINT_32 width)
{
	UINT_32 i = 0, j = 0;
	BINPAPER f = PaperOpen(addr);
	PaperRead(&f, fp, 1024*1024);
	for(j=0; j<300; j++)
	{
		UINT_32 k = 0;
		for(i=0; i<3*400; i+=3)
		{
			framebuffer[(j*(width * 4)) + k + i]     = fp[(j * 400*3) + i];
			framebuffer[(j*(width * 4)) + k + i + 1] = fp[(j * 400*3) + i + 1];
			framebuffer[(j*(width * 4)) + k + i + 2] = fp[(j * 400*3) + i + 2];
			k++;
		}
	}
}

void KERNEL_MAIN_ENTRY(void)
{
	UINT_32 status; 
	clear_screen();
	//printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	//printk("    Welcome to IMAN ABDOLLAHZADEH OS        \n");
	//printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	
	//.SVGA svga;
	//.status = RegisterSVGA(&svga);
	//.if(!status)
	//.{
	//.	panic( "SVGA registration failed\n" );
	//.	return;
	//.}
	//.
	
	GDT gdt;
	status = RegisterGDT(&gdt);
	if(!status)
	{
		panic( "GDT registration failed\n" );
		return;
	}
	//printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	
	status = RegisterIDT();
	if(!status)
	{
		panic( "IDT registration failed\n" );
		return;
	}
	//printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	
	TIMER timer;
	status = RegisterTimer(&timer, OS_DEFAULT_TIMER_TICK);
	if(!status)
	{
		panic( "TIMER registration failed\n" );
		return;
	}
	//printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	
	KEYBOARD keyboard;
	status = RegisterKeyboard(&keyboard);
	if(!status)
	{
		panic( "KEYBOARD registration failed\n" );
		return;
	}
	
	PCI  pci;
	SATA sata;
	EHCI ehci;
	XHCI x;
	status = RegisterPCI(&pci, &sata, &ehci, &x);
	if(!status)
	{
		panic( "PCI registration failed\n" );
		return;
	}
	
	FILESYSTEM filesystem;
	RegisterFilesystem(&filesystem, &sata);
	if(!status)
	{
		panic( "FILESYSTEM registration failed\n" );
		return;
	}
	
	
	UINT_8* buff = (UINT_8*)( Alloc((1024*1024), 1, 1) );
	__LiBOS_MemZero((void*)buff, 1024*1024);
	
	//__LiBOS_HexDump((void*)(&buff[32*1024]), 1026, "");
	WaitSecond(5);
	SVGA svga;
	status = RegisterSVGA(&svga);
	if(!status)
	{
		panic( "SVGA registration failed\n" );
		return;
	}
	
	clear_screen();
	UINT_8* fb = svga.LFB;
	UINT_32 i = 0, j = 0;
	//.BOOL dir = FALSE;
	//.int gradient = 0;
	//.int gradient_width = 4;
	//.for(j=0;j <svga.height; j++)
	//.{
	//.	for(i=0; i<4*svga.width; i += 4)
	//.	{
	//.		/* my desktop solid color */
	//.		fb[(j*(svga.width * 4)) + i]     = (0xff >> gradient);
	//.		fb[(j*(svga.width * 4)) + i + 1] = 0x00;
	//.		fb[(j*(svga.width * 4)) + i + 2] = (0x66 >> gradient);
	//.	}
	//.	
	//.	if(!dir)
	//.	{
	//.		gradient_width--;
	//.		if(!gradient_width)
	//.		{
	//.			gradient++;
	//.			gradient_width = 4;
	//.		}
	//.	}
	//.	else 
	//.	{
	//.		gradient_width--;
	//.		if(!gradient_width)
	//.		{
	//.			gradient--;
	//.			gradient_width = 4;
	//.		}
	//.	}
	//.	if(gradient == -1 || gradient == 9)
	//.	{
	//.		if(dir) dir = FALSE;
	//.		else    dir = TRUE;
	//.		if(gradient == -1) gradient = 0;
	//.		if(gradient == 9)  gradient = 8;
	//.	}
	//.}
	while(1)
	{
		movie_player("_0.uLB", fb, buff, svga.width);
		movie_player("_1.uLB", fb, buff, svga.width);
		movie_player("_2.uLB", fb, buff, svga.width);
		movie_player("_3.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_4.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_5.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_6.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_7.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_8.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_9.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_10.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_11.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_12.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_13.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_14.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_15.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_16.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_17.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_18.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_19.uLB", fb, buff, svga.width);
		//.movie_player("scenes/_20.uLB", fb, buff, svga.width);
	}
	
	//.
	//.USB_MOUSE usb_mouse;
	//.status = RegisterMouse(&usb_mouse);
	//.if(!status)
	//.{
	//.	panic( "USB MOUSE registration failed\n" );
	//.	return;
	//.}
	
	
	//UINT_8* PAGE0 = (UINT_8*)( Alloc(3*1024*1024, 1, 1) ); 
	//
	//__LiBOS_HexDump((void*)file_buffer, 16, "frame 1");	
	
	//UINT_8* PAGE0  = fb;
	//UINT_8* PAGE1  = (UINT_8*)((void*)(PHYSICAL_ADDRESS(fb) + ( 1024*768*4 ))); 
	
	
	
	
	//while(1)
	//{
	//	//page_flip(0);
	//	BINPAPER file1 = PaperOpen("LIBOS_TEST_WITH_LARGE_FILES/fframe_4.imn");
	//	PaperRead(&file1, PAGE0, 2*1024*1024);
	//	//.WaitMiliSecond(100);
	//	//__LiBOS_HexDump((void*)PAGE0, 24, "frame 2");
	//	//__LiBOS_MemZero((void*)PAGE0, 100);
	//	
	//	//page_flip(1024*768*16);
	//	//__LiBOS_HexDump((void*)PAGE0, 24, "frame 1");
	//	BINPAPER file2 = PaperOpen("LIBOS_TEST_WITH_LARGE_FILES/fframe_3.imn");
	//	PaperRead(&file2, PAGE0, 2*1024*1024);
	//	//.WaitMiliSecond(100);
	//}
	
	
	//int i = 0;
	///**/UINT_32 l = 0;
	///**/UINT_32 h = 0;
	///**/start_user_timer();
	//
	///**/end_user_timer(&h, &l);
	///**/printk("Timer test: HIGH packet = %, LOW packet = %\n", h, l);
	//__LiBOS_HexDump ((void*)file_buffer, 34, "Test file reading");
	_STI();
	while(1);
}