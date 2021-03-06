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
#include "../include/VIDEO_PLAYER.h"
#include "../include/DOSSPEC.h"
#include "../include/WINDOW.h"
#include "../include/FONT.h"
#include "../include/IMAGE_LOADER.h"
#include "../include/I_ASM.h"
#include "../include/ACPI.h"
#include "../include/MP.h"
#include "../include/SSE.h"
#include "../include/PAGING.h"
#include "../include/PROCESS.h"
#include "../include/USER_MODE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ START OF KERNEL

void LiBOS_kernel_main(void)
{
	/* start LiBOS kernel */
	UINT_32 status; 
	clear_screen();
	printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	printk("             Welcome to LiBOS               \n");
	printk("       Written by Iman Abdollahzadeh        \n");
	printk("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");

	/* start LiBOS GDT and TSS for BSP CPU */
	GDT gdt;
	status = RegisterGDT(&gdt);
	if(!status)
	{
		panic( "kernel GDT registration failed\n" );
		return;
	}
	
	/* start LiBOS IDT, interrupt vectors, and usermode setup */
	status = RegisterIDT();
	if(!status)
	{
		panic( "kernel IDT registration failed\n" );
		return;
	}
	
	/* enable PIT as the LiBOS main timer for all logical CPUs */
	TIMER timer;
	status = RegisterTimer(&timer, OS_DEFAULT_TIMER_TICK);
	if(!status)
	{
		panic( "kernel timer registration failed\n" );
		return;
	}
	
	/* enable keyboard for all logical CPUs */
	KEYBOARD keyboard;
	status = RegisterKeyboard(&keyboard);
	if(!status)
	{
		panic( "kernel keyboard registration failed\n" );
		return;
	}
	
	/* initialize the non-paged physical memory allocator (for SYS space devices) */
	initialize_non_page_allocator();
	
	/* start paging with the entire 4GB address space as identity mapping 
	   and setting the iBOS main page directory available for all logical CPUs */
	if( !start_paging() )
	{
		panic( "kernel paging initiation failed\n" );
		return;
	}
	
	/* start svga and enabling PMODE_BIOS_CALLs for all logical CPUs */
	SVGA svga;
	status = RegisterSVGA(&svga);
	if(!status)
	{
		panic( "SVGA registration failed\n" );
		return;
	}
	
	/* enable all AP CPUs and bring them on halt state waiting for threads to run */
	MP_FLOATING_POINTER mpfp;
	if( query_multiprocessing(&mpfp) )
	{
		//-__LiBOS_ChrDump (mpfp.signature, 4);
		//-printk("MP features 1=^\n", mpfp.mp_features_1);
		//-printk("MP config pointer=^\n", mpfp.mp_config_pointer);
		MP_configuration_table(&mpfp);
		query_libos_cpus();
		query_libos_ioapics();
		start_multiprocessing();
	}
	
	/* start pci configuration, detecting sata, ehci, and xhci */
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
	
	/* start process and threads (i.e. tasks) to be distributed accross all logical CPUs */
	if( !initialize_process() )
	{
		panic( "kernel process initiation failed\n" );
		return;
	}
	
	
	/* start acpi configuration (if any) */
	//.RSDP_Descriptor_2_0 rsdp;
	//.if( query_rsdp(&rsdp) )
	//.{
	//.	UINT_32 rsdt = (rsdp.rsdp_1_0.revision != 0) ? PHYSICAL_ADDRESS((UINT_32)rsdp.xsdt_address) : PHYSICAL_ADDRESS(rsdp.rsdp_1_0.rsdt_address);
	//.	void* fadt   = find_FACP(rsdt);
	//.	if(fadt)
	//.		printk("FADT:^\n", fadt);
	//.}
	
	/* start FAT32 filesystem and files on sata */
	//FILESYSTEM filesystem;
	//RegisterFilesystem(&filesystem, &sata);
	//------if(!status)
	//------{
	//------	panic( "FILESYSTEM registration failed\n" );
	//------	return;
	//------}
	
	//,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,//----ect_execution("ECT/__IMG.ect");
	
	/* start the graphical desktop mode and leaving the debug, consule mode */
	//-clear_screen();
	
	//.UINT_8* fb = svga.LFB;
	//.init_desktop();
	//.
	//./* start the windows */
	//.WINDOW wnd;
	//.POINT wnd_orig = {50, 50};
	//.register_window(&wnd, "first_window", 400, 200, &wnd_orig);    
	//.const INT_8* test  = "ImanAbdollahzadeh";
	//.const INT_8* test2 = "abcdefghijklmnopqrstuvwxyz";
	//.draw_string(test,  &wnd, 20, 180, LiBOS_WINDOW_BODY_COLOR);
	//.draw_string(test2, &wnd, 20, 160, LiBOS_WINDOW_BODY_COLOR);
	//.
	//.WINDOW wnd2;
	//.POINT wnd_orig2 = {80, 70};
	//.register_window(&wnd2, "second_window", 200, 100, &wnd_orig2);
	//.WINDOW wnd3;
	//.POINT wnd_orig3 = {110, 35};
	//.WINDOW_OBJECT obj;
	//.obj.object_identifier = OBJECT_BUTTON;
	//.register_window(&wnd3, "third_window", 200, 100, &wnd_orig3); 
	//.register_object(&wnd3, &obj);
	//.WINDOW wnd4;
	//.POINT wnd_orig4 = {430, 240};
	//.register_window(&wnd4, "fourth_window", 80, 90, &wnd_orig4);
	//.
	//.draw_window(&wnd , fb);
	//.draw_window(&wnd2, fb);
	//.draw_window(&wnd3, fb);
	//.draw_window(&wnd4, fb);
	
	//-/* start communication with xhci-driven USB mouse */
	//-USB_MOUSE usb_mouse;
	//-status = RegisterMouse(&usb_mouse);
	//-if(!status)
	//-{
	//-	panic( "USB MOUSE registration failed\n" );
	//-	return;
	//-}
	
	/* start activation of SSE and AVX on all logical CPUs */
	//,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,_activate_sse();
	
	//_STI();
	
	/* start activation of SSE and AVX on all logical CPUs */
	//.VIDEO_PLAYER video_player;
	//.
	//.status = regiser_video_player(&video_player, 0);//*/fb);
	//.if(!status)
	//.{
	//.	panic( "VIDEO_PLAYER registration failed\n" );
	//.	return;
	//.}
	
	//.UINT_32 l = 0;
	//.UINT_32 h = 0;
	//.start_user_timer();
	//.
	//.play(&video_player, "test/__PDL.imn");
	//.
	//.end_user_timer(&h, &l);
	//.printk("Timer test: HIGH packet = %(sec), LOW packet = %(msec)\n", h, l);
	
	//.__LiBOS_report_binary_image_sections();
	
	
	//----/* only EXT stuff */
	//----load_ext_loader("_EXTS/EXTLDR.EXT");
	//----load_ext_image ("_EXTS/_TST.EXT");
	//----load_ext_image ("_EXTS/FKE.EXT");
	//----
	//----dump_global_ext_table();
	
	//....const char* i_asm_file = "[I_ASM 32-bit]\n[ORIGIN 0xFFFFFFFF]\n";
	//....printk(i_asm_file);
	//....printk("\n");
	//....enter_I_ASM(i_asm_file);
	//....
	//....
	//....debug();
	
	/* enter user mode as the no-go destination */
	//----------initiate_usermode();
	/* FROM NOW ON WE ARE IN USERMODE -> NO PRIVILEGE INSTRUCTION CAN BE ISSUED ANYMORE */
	_STI();
	while(1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ END OF KERNEL