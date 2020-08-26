#include "../include/GDT.h"
#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

GDTPOINTER gdt_pointer;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

SEGMENTDESCRIPTOR __RegisterSegmentDescriptor(UINT_32 base, UINT_32 limit, UINT_8 access, UINT_8 gran)
{
	SEGMENTDESCRIPTOR seg_desc;
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

UINT_32 RegisterGDT(GDT* gdt)
{
	UINT_32 status = 0;
	
	if(!gdt)
		return 0;
	gdt->NullSegmentSelector   = __RegisterSegmentDescriptor(0, 0, 0, 0);
	gdt->CodeSegmentSelector   = __RegisterSegmentDescriptor(0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt->DataSegmentSelector   = __RegisterSegmentDescriptor(0, 0xFFFFFFFF, 0x92, 0xCF);
	gdt->CodeSegmentSelector16 = __RegisterSegmentDescriptor(0, 0xFFFFFFFF, 0x9A, 0x0F);
	gdt->DataSegmentSelector16 = __RegisterSegmentDescriptor(0, 0xFFFFFFFF, 0x92, 0x0F);
	
	gdt_pointer.size = (sizeof(SEGMENTDESCRIPTOR)*5) - 1;
	gdt_pointer.base = (UINT_32)((void*)gdt);
	
	if(!gdt_pointer.size || !gdt_pointer.base)
	{
		panic("GDT pointer Failed\n");
		return 0;
	}
	
	status = GDTLoad();
	if (!status)
	{
		panic("GDT pointer loading Failed\n");
		return 0;
	}
	
	//.printk( "        >>> GDT registered successfully <<<\n");
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_16 GDTCodeSegmentSelector(GDT* gdt)
{
	if(!gdt)
		return 0;
	return (UINT_8*)&gdt->CodeSegmentSelector - (UINT_8*)gdt;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_16 GDTDataSegmentSelector(GDT* gdt)
{
	if(!gdt)
		return 0;
	return (UINT_8*)&gdt->DataSegmentSelector - (UINT_8*)gdt;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 SegmentDescriptorBase(SEGMENTDESCRIPTOR* segmentdescriptor)
{
	UINT_8* target = (UINT_8*)segmentdescriptor;
	UINT_32 result = target[7];
	result = (result << 8) + target[4];
	result = (result << 8) + target[3];
	result = (result << 8) + target[2];
	return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 SegmentDescriptorLimit(SEGMENTDESCRIPTOR* segmentdescriptor)
{
	UINT_8* target = (UINT_8*)segmentdescriptor;
    UINT_32 result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];
    if((target[6] & 0xC0) == 0xC0)
        result = (result << 12) | 0xFFF;
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
