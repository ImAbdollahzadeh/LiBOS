#include "../include/ACPI.h"
#include "../include/PRINT.h"
#include "../include/MEMORY.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL acpi_strcmp(INT_8* src, INT_8* trg, UINT_32 bytes)
{
	UINT_32 i;
	for(i=0; i<bytes; i++)
		if(*src++ != *trg++)
			return FALSE;
	return TRUE;
}

static BOOL acpi_checksum_1(RSDP_Descriptor_2_0* r)
{
	INT_8 sum = 0, i;
	UINT_8 bytes_to_add = 20;
	for(i=0; i<bytes_to_add; i++)
		sum += ((char*)r)[i];
	if(!sum)
		return TRUE;
	return FALSE;
}

static BOOL acpi_checksum_2(RSDP_Descriptor_2_0* r)
{
	INT_8 sum = 0, i;
	UINT_8 bytes_to_add = 16;
	for(i=0; i<bytes_to_add; i++)
		sum += ((char*)(r + 20))[i];
	if(!sum)
		return TRUE;
	return FALSE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL query_rsdp(RSDP_Descriptor_2_0* rsdp)
{
	RSDP_Descriptor_2_0* r = 0;
	
	UINT_8* start = (UINT_8*)((*(UINT_16*)0x040E) >> 4);
	for (UINT_8* i = start; i < start + 1024; i += 16)
	{
		r = (RSDP_Descriptor_2_0*)i;
		UINT_8* mem = 0;
		if ( acpi_strcmp(r->rsdp_1_0.signature, (INT_8*)("RSD PTR "), 8) )
		{
			UINT_8 val = 0;
			void* end = i + 20;
			for (mem = i; mem != end; mem++) 
				val += *mem;
			if (!val && r->rsdp_1_0.signature != 0)
			{
			    end = i + sizeof(RSDP_Descriptor_2_0) - 3; // Current RDSP size - 3 reserved bytes
			    for (mem = i; mem != end; mem++)
					val += *mem;
				if (!val) 
					goto FOUND_RSDP;
			}
			else if (!val && r->rsdp_1_0.signature == 0)
				goto FOUND_RSDP;
		}
	}
	
	// first check out interval EDBA_BEGIN (aka 0x0009FC00) and EDBA_END (aka 0x000A0000)
	for (UINT_8* i = (UINT_8*)0x0009FC00; i < (UINT_8*)0x000A0000; i += 16)
	{
		r = (RSDP_Descriptor_2_0*)i;
		UINT_8* mem = 0;
		if ( acpi_strcmp(r->rsdp_1_0.signature, (INT_8*)("RSD PTR "), 8) )
		{
			UINT_8 val = 0;
			void* end = i + 20;
			for (mem = i; mem != end; mem++) 
				val += *mem;
			if (!val && r->rsdp_1_0.signature != 0)
			{
			    end = i + sizeof(RSDP_Descriptor_2_0) - 3; // Current RDSP size - 3 reserved bytes
			    for (mem = i; mem != end; mem++)
					val += *mem;
				if (!val) 
					goto FOUND_RSDP;
			}
			else if (!val && r->rsdp_1_0.signature == 0)
				goto FOUND_RSDP;
		}
	}
	
	// then check out interval BIOS_BEGIN (aka 0x000E0000) and BIOS_END (aka 0x00100000)
	for (UINT_8* i = (UINT_8*)0x000E0000; i < (UINT_8*)0x00100000; i += 16)
	{
		r = (RSDP_Descriptor_2_0*)i;
		UINT_8* mem = 0;
		if ( acpi_strcmp(r->rsdp_1_0.signature, (INT_8*)("RSD PTR "), 8) )
		{
			UINT_8 val = 0;
			void* end = i + 20;
			for (mem = i; mem != end; mem++) 
				val += *mem;
			if (!val && r->rsdp_1_0.signature != 0)
			{
			    end = i + sizeof(RSDP_Descriptor_2_0) - 3; // Current RDSP size - 3 reserved bytes
			    for (mem = i; mem != end; mem++)
					val += *mem;
				if (!val) 
					goto FOUND_RSDP;
			}
			else if (!val && r->rsdp_1_0.signature == 0)
				goto FOUND_RSDP;
		}
	}
	return FALSE;
FOUND_RSDP:
	*rsdp = *r;
	printk("RSDP OEM_id: ");
	__LiBOS_ChrDump (rsdp->rsdp_1_0.OEM_id, 6);
	printk("RSDP revision:^\n", rsdp->rsdp_1_0.revision);
	printk("RSDP rsdt: ^\n",    rsdp->rsdp_1_0.rsdt_address);
	
	if(rsdp->rsdp_1_0.revision != 0) // versions 2.0 or above
	{
		printk("RSDP length: ^\n", rsdp->length);
		printk("RSDP xsdt-low: ^\n", rsdp->xsdt_address);
		printk("RSDP xsdt-hi:  ^\n", (rsdp->xsdt_address >> 32));
	}
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void* find_FACP(UINT_32 rsdt)
{
	RSDT*    r               = (RSDT*)(rsdt);
	UINT_32  entries         = (r->h.length - sizeof(ACPI_SDT_Header)) >> 2;
	//printk("RSDT length=^ and entries=^\n", r->h.length, entries);
	UINT_32  i;
	for (i = 0; i < entries; i++)
	{
		ACPI_SDT_Header* h = (ACPI_SDT_Header*)(PHYSICAL_ADDRESS(r->pointer_to_otgher_SDT[i]));
		__LiBOS_ChrDump (h->signature, 4);
		if ( acpi_strcmp(h->signature, (INT_8*)("FACP"), 4) )
			return (void*)h;
	}
	return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

