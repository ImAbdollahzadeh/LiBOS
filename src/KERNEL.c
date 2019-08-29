#include "../include/GDT.h"
#include "../include/PORT.h"
#include "../include/IDT.h"
#include "../include/TIMER.h"
#include "../include/PRINT.h"
#include "../include/PCI.h"
#include "../include/AHCI.h"
#include "../include/XHCI.h"
#include "../include/FILESYSTEM.h"
#include "../include/FILE.h"
#include "../include/KEYBOARD.h"
#include "../include/MOUSE.h"
#include "../include/MEMORY.h"
#include "../include/VESA.h"
#include "../include/DOSSPEC.h"

void Imos_kernel_entry(void)
{
	unsigned int status; 
	clear_screen();
	//printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	//printk("                      Welcome to IMAN ABDOLLAHZADEH OS                        \n");
	//printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	
	GDT gdt;
	status = Register_gdt(&gdt);
	if(!status)
	{
		panic( "GDT registration failed\n" );
		return;
	}
	//printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	
	status = RegisterIDT();
	if(!status)
	{
		panic( "IDT registration failed\n" );
		return;
	}
	//printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	
	TIMER timer;
	status = RegisterTimer(&timer, OS_DEFAULT_TIMER_TICK);
	if(!status)
	{
		panic( "TIMER registration failed\n" );
		return;
	}
	//printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");

	PCI  pci;
	SATA sata;
	XHCI x;
	status = RegisterPCI(&pci, &sata, &x);
	if(!status)
	{
		panic( "PCI registration failed\n" );
		return;
	}
	//printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	
	KEYBOARD keyboard;
	status = RegisterKeyboard(&keyboard);
	if(!status)
	{
		panic( "KEYBOARD registration failed\n" );
		return;
	}
	//printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	
	_STI();
	while(1);
}