#include "../include/SVGA.h"
#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

static SVGA* global_svga = 0;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//
static BOOL BiosCall(UINT_8 call_number, UINT_32 query_mode, SVGA* svga)
{
	SVGA_REGS_16_BIT r;
	
	UINT_16 mode = 0x0118;
	
	// hard coded below 1MiB area allocation
	VBE_INFO*      vbe_info      = (VBE_INFO*)0x0600;  
	VBE_MODE_INFO* vbe_mode_info = (VBE_MODE_INFO*)0x0700;
	
	vbe_info->signature[0] = 'V';
	vbe_info->signature[1] = 'B';
	vbe_info->signature[2] = 'E';
	vbe_info->signature[3] = '2';
	
	UINT_32 vbe_info_ptr      = PHYSICAL_ADDRESS(vbe_info);
	UINT_32 vbe_mode_info_ptr = PHYSICAL_ADDRESS(vbe_mode_info);
	
	switch(query_mode)
	{
		case LIBOS_GET_VBE_INFO:
			r.ax = 0x4F00;
			r.es = SEG(vbe_info_ptr);
			r.di = OFF(vbe_info_ptr);
			__LiBOS_BiosCall(call_number, &r);
			if (r.ax != 0x004F)
			{
				printk("getting svga vbe info failed from bios call\n");
				return FALSE;
			}
			svga->vbe_info                       = *vbe_info;
			svga->capabilities                   = vbe_info->capabilities;
			svga->pointer_to_list_of_video_modes = vbe_info->video_modes;
			svga->video_memory                   = vbe_info->video_memory;
			svga->software_rev                   = vbe_info->software_rev;
			svga->vendor                         = vbe_info->vendor;
			svga->product_name                   = vbe_info->product_name;
			return TRUE;
			
		case LIBOS_GET_VBE_MODE_INFO:
			r.ax = 0x4F01;
			r.cx = 0x0118;
			r.es = SEG(vbe_mode_info_ptr);
			r.di = OFF(vbe_mode_info_ptr);
			__LiBOS_BiosCall(call_number, &r);
			if (r.ax != 0x004F)
			{
				printk("getting svga vbe mode info failed from bios call\n");
				return FALSE;
			}
			svga->vbe_mode_info  = *vbe_mode_info;
			svga->bytes_per_line = vbe_mode_info->pitch;
			svga->width          = vbe_mode_info->width;
			svga->height         = vbe_mode_info->height;
			svga->bits_per_pixel = vbe_mode_info->bpp;
			svga->LFB            = (UINT_8*)((void*)vbe_mode_info->framebuffer);
			
			return TRUE;
			
		case LIBOS_SET_MODE:
			r.ax = 0x4F02;
			r.bx = mode;
			__LiBOS_BiosCall(call_number, &r);
			if (r.ax != 0x004F)
			{
				printk("setting svga mode failed from bios call\n");
				return FALSE;
			}
			
			return TRUE;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 RegisterSVGA(SVGA* svga)
{
	if(!svga)
	{
		panic("Invalid svga object passed\n");
		return 0;
	}
	
	BOOL status = FALSE;
	status      = BiosCall(0x10, LIBOS_GET_VBE_INFO, svga);
	if(!status)
	{
		printk("get_vba_info failed\n");
		return 0;
	}
	else
		svga_report_vbe_info(svga);
	
	status = BiosCall(0x10, LIBOS_GET_VBE_MODE_INFO, svga);
	if(!status)
	{
		printk("get_vba_mode_info failed\n");
		return 0;
	}
	else
		svga_report_vbe_mode_info(svga);
	
	//status = BiosCall(0x10, LIBOS_SET_MODE, svga);
	//if(!status)
	//{
	//	printk("svga set_mode failed\n");
	//	return 0;
	//}
	
	global_svga = svga;
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define HIGH_WORD(U32) ((UINT_16)(U32 >> 16))
#define  LOW_WORD(U32) ((UINT_16)(U32 & 0x0000FFFF))

void svga_report_vbe_info(SVGA* svga)
{
	printk("signature:^ ^ ^ ^\n",          svga->vbe_info.signature[0], svga->vbe_info.signature[1], svga->vbe_info.signature[2], svga->vbe_info.signature[3]);
	printk("version:^\n",                  svga->vbe_info.version);
	printk("capabilities bitfield:^\n",    svga->vbe_info.capabilities);
	printk("pointer to list of modes:^\n", svga->vbe_info.video_modes);
	printk("video_memory in 64K:^\n",      svga->vbe_info.video_memory);
	printk("vendor:^\n",                   svga->vbe_info.vendor);
	
	UINT_32 mode_list = PHYSICAL_ADDRESS( svga->vbe_info.video_modes );
	UINT_16* modes = (UINT_16*)( (void*)((HIGH_WORD(mode_list)*16) + LOW_WORD(mode_list)) );
	UINT_32 mode_counts = 0;
	while(*modes != 0xFFFF)
	{
		mode_counts++;
		printk("mode % = ^\n", mode_counts, *modes);
		modes++;
	}
	
	UINT_32 vendor_pointer = PHYSICAL_ADDRESS( svga->vbe_info.vendor );
	UINT_8* vendor = (UINT_8*)( (void*)((HIGH_WORD(vendor_pointer)*16) + LOW_WORD(vendor_pointer)) );
	printk(vendor);
	printk("\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void svga_report_vbe_mode_info(SVGA* svga)
{
	printk("pitch->number of bytes per horiz line:^\n",   svga->vbe_mode_info.pitch);
	printk("width in pixel:^\n",                          svga->vbe_mode_info.width);
	printk("height in pixel:^\n",                         svga->vbe_mode_info.height);
	printk("bit per pixel:^\n",                           svga->vbe_mode_info.bpp);
	printk("linear framebuffer:^\n",                      svga->vbe_mode_info.framebuffer);
	printk("memsize LFB but not displayed on screen:^\n", svga->vbe_mode_info.off_screen_mem_size);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL svga_set_mode(UINT_16 mode)
{
	SVGA_REGS_16_BIT r;
	r.ax = 0x4F02;
	r.bx = mode;
	
    __LiBOS_BiosCall(0x10, &r);
	
    if (r.ax != 0x004F)
	{
		printk("setting svga mode failed from bios call\n");
		return FALSE;
	}
	
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

SVGA* get_svga_instance(void)
{
	return global_svga;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

