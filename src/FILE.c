#include "../include/FILE.h"
#include "../include/PRINT.h"
#include "../include/DOSSPEC.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

DESCRIPTOR OpenOperation(HDPARAMS DriveVolume, INT_8* DosPath);
UINT_32    ReadOperation(HDPARAMS DriveVolume, DESCRIPTOR* descriptor, UINT_8* Buffer, UINT_32 Bytes, UINT_8 ect_size_operation);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BINPAPER PaperOpen(INT_8* name_address)
{
	BINPAPER paper = {
		.name            = 0,
		.flags           = 0, 
		.fileLength      = 0,
		.id              = 0,
		.eof             = 0,
		.position        = 0,
		.currentCluster  = 0,
		.device          = 0,
		.paperDescriptor = 0
	};
	DESCRIPTOR desc;
	INT_8* convertedCStringToDosString = 0;
	HDPARAMS Drive = DriveEntry();
	
	convertedCStringToDosString = CStringAddressToDosString( name_address );
	if(!convertedCStringToDosString)
	{
		panic("Converting CString to DosString failed\n");
		return paper;
	}
	
	desc = OpenOperation(Drive, convertedCStringToDosString);
	UINT_64 tree = desc.tree;
	UINT_8 descriptor_tree_count = (UINT_8)(tree >> 56);
	UINT_8 must_be_zero = 0x00;
	UINT_32  constructed_validity_descriptor = ((UINT_32)(0xE1) << 24) | ((UINT_32)(must_be_zero) << 16) | ((UINT_32)(descriptor_tree_count) << 8) | (0x88);
	
	if(desc.validity != constructed_validity_descriptor)
	{
		panic("Invalid descriptor passed from OpenOperation ");
		printk("^\n", constructed_validity_descriptor);
		return paper;
	}
	else
		paper.paperDescriptor = desc;
	
	return paper;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 PaperClose(BINPAPER* paper)
{
	if (!paper)
	{
		panic("Invalid paper pointer\n");
		return 0;
	}
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 PaperRead(BINPAPER* paper, UINT_8* Buffer, UINT_32 Bytes, UINT_8 ect_size_operation)
{
	UINT_32 status = 0;
	HDPARAMS Drive;
	
	if (!paper)
	{
		panic("Invalid paper pointer\n");
		return 0;
	}
	
	if (!Buffer)
	{
		panic("Invalid Buffer\n");
		return 0;
	}
	
	if (!Bytes)
	{
		panic("Cannot read the content of the paper pointer into a zero bytes buffer\n");
		return 0;
	}
	
	Drive = DriveEntry();
	status = ReadOperation(Drive, &paper->paperDescriptor, Buffer, Bytes, ect_size_operation);
	if (!status)
	{
		panic("Paper read failed\n");
		return 0;
	}
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
