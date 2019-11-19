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

	while(1)
	{
		movie_player("_0.uLB", fb, buff, svga.width);
		movie_player("_1.uLB", fb, buff, svga.width);
		movie_player("_2.uLB", fb, buff, svga.width);
		movie_player("_3.uLB", fb, buff, svga.width);
	}

	_STI();
	while(1);
}
