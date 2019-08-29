#ifndef _GDT__H__
#define _GDT__H__

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _SEGMEN_TDESCRIPTOR {
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char  base_middle;
	unsigned char  access;
	unsigned char  granularity;
	unsigned char  base_high;
} __attribute__ ((packed)) SEGMENT_DESCRIPTOR;

typedef struct _GDT {
	SEGMENT_DESCRIPTOR null_segment_selector;
	SEGMENT_DESCRIPTOR code_segment_selector;
	SEGMENT_DESCRIPTOR data_segment_selector;
} __attribute__ ((packed)) GDT;

typedef struct _GDT_POINTER {
    unsigned short size;
    unsigned int   base;
} __attribute__((packed)) GDT_POINTER;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern unsigned int Gdt_load                 (void);
unsigned int        Segment_descriptor_base  (SEGMENT_DESCRIPTOR* segment_descriptor);
unsigned int        Segment_descriptor_limit (SEGMENT_DESCRIPTOR* segment_descriptor);
unsigned int        Register_gdt             (GDT* gdt);
unsigned short      Gdt_code_segment_selector(GDT* gdt);
unsigned short      Gdt_data_segment_selector(GDT* gdt);
    
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	
#endif //! _GDT__H__
