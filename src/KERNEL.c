#include "../include/LiBOS_CORE.h"
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
	//.unsigned char* fb = svga.LFB;
	//.int i = 0;
	//.int j = 0;
	//.for(j=0;j <svga.height; j++){
	//.	for(i=0; i<4*svga.width; i++)
	//.		fb[(j*(svga.width * 4)) + i] = 255;
	//.}
	
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
	
	USB_MOUSE usb_mouse;
	status = RegisterMouse(&usb_mouse);
	if(!status)
	{
		panic( "USB MOUSE registration failed\n" );
		return;
	}
	
	SVGA svga;
	status = RegisterSVGA(&svga);
	if(!status)
	{
		panic( "SVGA registration failed\n" );
		return;
	}
	
	_STI();
	while(1);
}