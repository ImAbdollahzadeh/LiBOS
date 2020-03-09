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

extern void _LiBOS_text_section_size;
extern void _LiBOS_data_section_size;
extern void _LiBOS_bss_section_size;
extern void _LiBOS_text_section_begin;
extern void _LiBOS_text_section_end;
extern void _LiBOS_data_section_begin;
extern void _LiBOS_data_section_end;
extern void _LiBOS_bss_section_begin;
extern void _LiBOS_bss_section_end;

static void __LiBOS_report_binary_image_sections(void)
{
	UINT_32* text_sz = (UINT_32*)(&_LiBOS_text_section_size);
	UINT_32* data_sz = (UINT_32*)(&_LiBOS_data_section_size);
	UINT_32* bss_sz  = (UINT_32*)(&_LiBOS_bss_section_size);
	UINT_32* text_bg = (UINT_32*)(&_LiBOS_text_section_begin);
	UINT_32* text_en = (UINT_32*)(&_LiBOS_text_section_end);
	UINT_32* data_bg = (UINT_32*)(&_LiBOS_data_section_begin);
	UINT_32* data_en = (UINT_32*)(&_LiBOS_data_section_end);
	UINT_32* bss_bg  = (UINT_32*)(&_LiBOS_bss_section_begin);
	UINT_32* bss_en  = (UINT_32*)(&_LiBOS_bss_section_end);
	
	printk("text begin=^, text end=^, text size=%\n", (UINT_32)text_bg, (UINT_32)text_en, (UINT_32)text_sz);
	printk("data begin=^, data end=^, data size=%\n", (UINT_32)data_bg, (UINT_32)data_en, (UINT_32)data_sz);
	printk("bss  begin=^, bss  end=^, bss  size=%\n", (UINT_32)bss_bg,  (UINT_32)bss_en,  (UINT_32)bss_sz);
}


void movie_player(char* addr, UINT_8* framebuffer, UINT_8* fp, UINT_32 width)
{
	UINT_32 i = 0, j = 0;
	BINPAPER f = PaperOpen(addr);
	PaperRead(&f, fp, 1024*1024, 0);
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
	
	//----ect_execution("ECT/__IMG.ect");
	
	//SVGA svga;
	//status = RegisterSVGA(&svga);
	//if(!status)
	//{
	//	panic( "SVGA registration failed\n" );
	//	return;
	//}
	//
	//clear_screen();
	//UINT_8* fb = svga.LFB;
	//
	//init_desktop();
	//
	//WINDOW wnd;
    //POINT wnd_orig = {50, 50};
	//register_window(&wnd, "first_window", 400, 200, &wnd_orig);    
	//const INT_8* test  = "ImanAbdollahzadeh";
	//const INT_8* test2 = "abcdefghijklmnopqrstuvwxyz";
	//draw_string(test,  &wnd, 20, 180, LiBOS_WINDOW_BODY_COLOR);
	//draw_string(test2, &wnd, 20, 160, LiBOS_WINDOW_BODY_COLOR);
	//
    //WINDOW wnd2;
    //POINT wnd_orig2 = {80, 70};
	//register_window(&wnd2, "second_window", 200, 100, &wnd_orig2);
    //WINDOW wnd3;
    //POINT wnd_orig3 = {110, 35};
	//WINDOW_OBJECT obj;
	//obj.object_identifier = OBJECT_BUTTON;
	//register_window(&wnd3, "third_window", 200, 100, &wnd_orig3); 
	//register_object(&wnd3, &obj);
    //WINDOW wnd4;
    //POINT wnd_orig4 = {430, 240};
	//register_window(&wnd4, "fourth_window", 80, 90, &wnd_orig4);
	//
	//draw_window(&wnd , fb);
	//draw_window(&wnd2, fb);
	//draw_window(&wnd3, fb);
	//draw_window(&wnd4, fb);
	//
	//USB_MOUSE usb_mouse;
	//status = RegisterMouse(&usb_mouse);
	//if(!status)
	//{
	//	panic( "USB MOUSE registration failed\n" );
	//	return;
	//}
	
	//._activate_sse();
	
	_STI();
	
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
	
	
	/* only EXT stuff */
	load_ext_loader("_EXTS/EXTLDR.EXT");
	load_ext_image ("_EXTS/_TST.EXT");
	load_ext_image ("_EXTS/FKE.EXT");
	
	dump_global_ext_table();
	
	while(1);
}