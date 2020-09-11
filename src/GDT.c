#include "../include/GDT.h"
#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

GDTPOINTER gdt_pointer;
TSS tss0, tss1, tss2, tss3; // 4 TSS's for all 4 logical CPUs

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

static void install_all_TSSs(GDT* gdt)
{
	/* First compute the base and limit of our entry into the GDT */
	UINT_32 base  = PHYSICAL_ADDRESS(&tss0);
	UINT_32 limit = sizeof(TSS);
	
	/* add our tss address to the GDT */
	gdt->TSS_BSP = __RegisterSegmentDescriptor(base, base + limit, 0xE9, 0x0);
	
	/* TSS is initially zero */
	__LiBOS_MemZero(&tss0, sizeof(TSS));
	
	/* set the SS:ESP of the kernel (BSP CPU) */
	tss0.ss0  = 0x10;
	tss0.esp0 = 0; //get_kernel_stack();
	tss0.cs   = 0x0B;
	tss0.ss   = 0x13;
	tss0.es   = 0x13;
	tss0.ds   = 0x13;
	tss0.fs   = 0x13;
	tss0.gs   = 0x13;
	
	//----/* ----  AND THE SAME FOR ALL AP CPUS ---- */
	//----base  = PHYSICAL_ADDRESS(&tss1);
	//----__LiBOS_MemZero(&tss1, sizeof(TSS));
	//----tss1.ss0  = 0x10;
	//----tss1.esp0 = __LiBOS_Main_Kernel_Stack;
	//----gdt->TSS_AP1 = __RegisterSegmentDescriptor(base, limit, 0xE9, 0x40);
	//----
	//----base  = PHYSICAL_ADDRESS(&tss2);
	//----__LiBOS_MemZero(&tss2, sizeof(TSS));
	//----tss2.ss0  = 0x10;
	//----tss2.esp0 = __LiBOS_Main_Kernel_Stack;
	//----gdt->TSS_AP2 = __RegisterSegmentDescriptor(base, limit, 0xE9, 0x40);
	//----
	//----base  = PHYSICAL_ADDRESS(&tss3);
	//----__LiBOS_MemZero(&tss3, sizeof(TSS));
	//----tss3.ss0  = 0x10;
	//----tss3.esp0 = __LiBOS_Main_Kernel_Stack;
	//----gdt->TSS_AP3 = __RegisterSegmentDescriptor(base, limit, 0xE9, 0x40);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void set_TSS_esp(UINT_32 esp)
{
	tss0.esp0 = esp;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 get_TSS_esp(void)
{
	return tss0.esp0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 RegisterGDT(GDT* gdt)
{
	UINT_32 status = 0;
	
	if(!gdt)
		return 0;
	gdt->NullSegmentSelector     = __RegisterSegmentDescriptor(0, 0, 0, 0);
	gdt->CodeSegmentSelector     = __RegisterSegmentDescriptor(0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt->DataSegmentSelector     = __RegisterSegmentDescriptor(0, 0xFFFFFFFF, 0x92, 0xCF);
	gdt->CodeSegmentSelector16   = __RegisterSegmentDescriptor(0, 0xFFFFFFFF, 0x9A, 0x0F);
	gdt->DataSegmentSelector16   = __RegisterSegmentDescriptor(0, 0xFFFFFFFF, 0x92, 0x0F);
	gdt->UserCodeSegmentSelector = __RegisterSegmentDescriptor(0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdt->UserDataSegmentSelector = __RegisterSegmentDescriptor(0, 0xFFFFFFFF, 0xF2, 0xCF);
	
	/* install 4 TSS entries for 4 CPUs of LiBOS */
	install_all_TSSs(gdt);
	
	gdt_pointer.size = (sizeof(SEGMENTDESCRIPTOR) * 8 /* actually 11 */) - 1;
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
	
	/* activate TSR in hardware */
	activate_tss0();
	
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
