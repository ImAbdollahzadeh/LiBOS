#include "../include/FILE.h"
#include "../include/PRINT.h"
#include "../include/DOSSPEC.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

DESCRIPTOR   OpenOperation(HDPARAMS DriveVolume, char* DosPath);
unsigned int ReadOperation(HDPARAMS DriveVolume, PDESCRIPTOR descriptor, unsigned char* Buffer, unsigned int Bytes);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BINPAPER PaperOpen(char* name_address)
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
	char* convertedCStringToDosString = 0;
	HDPARAMS Drive = DriveEntry();
	
	convertedCStringToDosString = CStringAddressToDosString( name_address );
	if(!convertedCStringToDosString)
	{
		panic("Converting CString to DosString failed\n");
		return paper;
	}
	
	desc = OpenOperation(Drive, convertedCStringToDosString);
	unsigned long long tree = desc.tree;
	unsigned char descriptor_tree_count = (unsigned char)(tree >> 56);
	unsigned char must_be_zero = 0x00;
	unsigned int  constructed_validity_descriptor = ((unsigned int)(0xE1) << 24) | ((unsigned int)(must_be_zero) << 16) | ((unsigned int)(descriptor_tree_count) << 8) | (0x88);
	
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

unsigned int PaperClose(PBINPAPER paper)
{
	if (!paper)
	{
		panic("Invalid paper pointer\n");
		return 0;
	}
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int PaperRead(PBINPAPER paper, unsigned char* Buffer, unsigned int Bytes)
{
	unsigned int status = 0;
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
	status = ReadOperation(Drive, &paper->paperDescriptor, Buffer, Bytes);
	if (!status)
	{
		panic("Paper read failed\n");
		return 0;
	}
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
