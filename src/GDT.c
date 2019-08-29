#include "../include/GDT.h"
#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

GDT_POINTER gdt_pointer;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static SEGMENT_DESCRIPTOR Register_segment_descriptor(unsigned int base, unsigned int limit, unsigned char access, unsigned char gran)
{
	SEGMENT_DESCRIPTOR seg_desc;
	seg_desc.base_low     = (base & 0xFFFF);
	seg_desc.base_middle  = (base >> 16) & 0xFF;
	seg_desc.base_high    = (base >> 24) & 0xFF;
	seg_desc.limit_low    = (limit & 0xFFFF);
	seg_desc.granularity  = (limit >> 16) & 0x0F;
	seg_desc.granularity |= (gran & 0xF0);
	seg_desc.access       = access;
	return seg_desc;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int Register_gdt(GDT* gdt)
{
	unsigned int status = 0;	
	
	if(!gdt)
		return 0;
	gdt->null_segment_selector = Register_segment_descriptor(0, 0, 0, 0);
    gdt->code_segment_selector = Register_segment_descriptor(0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt->data_segment_selector = Register_segment_descriptor(0, 0xFFFFFFFF, 0x92, 0xCF);
	
	//printk( "    In GDT: NullSegmentSelector created\n");
	//printk( "    In GDT: CodeSegmentSelector created\n");
	//printk( "    In GDT: DataSegmentSelector created\n");
	
    gdt_pointer.size = (sizeof(SEGMENT_DESCRIPTOR)*3) - 1;
    gdt_pointer.base = (unsigned int)((void*)gdt);
	if(!gdt_pointer.size || !gdt_pointer.base)
	{
		panic("GDT pointer Failed\n");
		return 0;
	}
	
	status = Gdt_load();
	if (!status)
	{
		panic("GDT pointer loading Failed\n");
		return 0;
	}
	
	//printk( "        >>> GDT registered successfully <<<\n");
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned short Gdt_code_segment_selector(GDT* gdt)
{
	if(!gdt)
		return 0;
	return (unsigned char*)&gdt->code_segment_selector - (unsigned char*)gdt;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned short Gdt_data_segment_selector(GDT* gdt)
{
	if(!gdt)
		return 0;
	return (unsigned char*)&gdt->data_segment_selector - (unsigned char*)gdt;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int Segment_descriptor_base(SEGMENT_DESCRIPTOR* segment_descriptor)
{
	unsigned char* target = (unsigned char*)segment_descriptor;
    unsigned int result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int Segment_descriptor_limit(SEGMENT_DESCRIPTOR* segment_descriptor)
{
	unsigned char* target = (unsigned char*)segment_descriptor;
    unsigned int result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];
    if((target[6] & 0xC0) == 0xC0)
        result = (result << 12) | 0xFFF;
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
