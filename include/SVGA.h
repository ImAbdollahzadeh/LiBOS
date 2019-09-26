#ifndef _SVGA__H__
#define _SVGA__H__

#include "LiBOS_CORE.h"

#define SEG(FP) ((UINT_16)(FP >> 4))
#define OFF(FP) ((UINT_16)(FP & 0xF))

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _VBE_INFO {
	INT_8   signature[4];             // must be "VESA" to indicate valid VBE support
	UINT_16 version;                  // VBE version; high byte is major version, low byte is minor version
	UINT_32 oem;                      // segment:offset pointer to OEM
	UINT_32 capabilities;             // bitfield that describes card capabilities
	UINT_32 video_modes;              // segment:offset pointer to list of supported video modes
	UINT_16 video_memory;             // amount of video memory in 64KB blocks
	UINT_16 software_rev;             // software revision
	UINT_32 vendor;                   // segment:offset to card vendor string
	UINT_32 product_name;             // segment:offset to card model name
	UINT_32 product_rev;              // segment:offset pointer to product revision
	INT_8   reserved[222];            // reserved for future expansion
	INT_8   oem_data[256];            // OEM BIOSes store their strings in this area
} __attribute__((packed)) VBE_INFO;

typedef struct _VBE_MODE_INFO {
	UINT_16 attributes;               // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	UINT_8  window_a;                 // deprecated
	UINT_8  window_b;                 // deprecated
	UINT_16 granularity;              // deprecated; used while calculating bank numbers
	UINT_16 window_size;
	UINT_16 segment_a;
	UINT_16 segment_b;
	UINT_32 win_func_ptr;             // deprecated; used to switch banks from protected mode without returning to real mode
	UINT_16 pitch;                    // number of bytes per horizontal line
	UINT_16 width;                    // width in pixels
	UINT_16 height;                   // height in pixels
	UINT_8  w_char;                   // unused...
	UINT_8  y_char;                   // ...
	UINT_8  planes;
	UINT_8  bpp;                      // bits per pixel in this mode
	UINT_8  banks;                    // deprecated; total number of banks in this mode
	UINT_8  memory_model;             // model 6 means direct 15/16/24. model 0 means text mode. model 1 means CGA.
	UINT_8  bank_size;                // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	UINT_8  image_pages;
	UINT_8  reserved0;
	UINT_8  red_mask;
	UINT_8  red_position;
	UINT_8  green_mask;
	UINT_8  green_position;
	UINT_8  blue_mask;
	UINT_8  blue_position;
	UINT_8  reserved_mask;
	UINT_8  reserved_position;
	UINT_8  direct_color_attributes;
	UINT_32 framebuffer;              // physical address of the linear frame buffer; write here to draw to the screen (LFB)
	UINT_32 off_screen_mem_off;
	UINT_16 off_screen_mem_size;      // size of memory in the framebuffer but not being displayed on the screen
	UINT_8  reserved1[206];
} __attribute__ ((packed)) VBE_MODE_INFO;

typedef struct _SVGA {
	VBE_INFO      vbe_info;
	VBE_MODE_INFO vbe_mode_info;
	UINT_32       capabilities;                   // bitfield that describes card capabilities
	UINT_32       pointer_to_list_of_video_modes; // segment:offset pointer to list of supported video modes
	UINT_16       video_memory;                   // amount of video memory in 64KB blocks
	UINT_16       software_rev;                   // software revision
	UINT_32       vendor;                         // segment:offset to card vendor string
	UINT_32       product_name;                   // segment:offset to card model name
	UINT_16       bytes_per_line;                 // number of bytes per horizontal line
	UINT_16       width;                          // width in pixels
	UINT_16       height;                         // height in pixels
	UINT_8        bits_per_pixel;
	UINT_8*       LFB;
} __attribute ((packed)) SVGA;

typedef struct _SVGA_REGS_16_BIT {
	UINT_16 di;
	UINT_16 si;
	UINT_16 bp;
	UINT_16 sp;
	UINT_16 bx;
	UINT_16 dx;
	UINT_16 cx;
	UINT_16 ax;
	UINT_16 gs;
	UINT_16 fs;
	UINT_16 es;
	UINT_16 ds;
	UINT_16 ef;
} __attribute__ ((packed)) SVGA_REGS_16_BIT;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/* GET_SVGA_STUFF: 0xC5, GET_INFO: BIT(0), GET_MODE_INFO: BIT(1), SET_MODE: BIT(2) */
typedef enum _LIBOS_VBE {
	LIBOS_GET_VBE_INFO      = 0xC501,
	LIBOS_GET_VBE_MODE_INFO = 0xC502,
	LIBOS_SET_MODE          = 0xC504
} LIBOS_VBE;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// main bios call: return to RealMode and gets back to ProtectedMode again
extern void    __LiBOS_BiosCall          (UINT_8 call_number, SVGA_REGS_16_BIT* register_set);

       UINT_32 RegisterSVGA              (SVGA* svga);
       SVGA*   get_svga_instance         (void);
       void    svga_report_vbe_info      (SVGA* svga);
       void    svga_report_vbe_mode_info (SVGA* svga);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _SVGA__H__
