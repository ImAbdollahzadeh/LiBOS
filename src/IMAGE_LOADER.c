#include "../include/IMAGE_LOADER.h"
#include "../include/FILE.h"
#include "../include/MEMORY.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void load_ext_loader(char* path)
{
	/*
		1- put 1 in address 0x0104837C
		2- zero 0x837C bytes at address 0x01040000 (initialize global_ext_table)
		3- put address of table (i.e. 0x01040000) in address 0x01048380
	*/
	*(UINT_32*)((void*)0x0104837C) = 0x00000001;
	__LiBOS_MemZero((void*)0x01040000, 0x837C);
	*(UINT_32*)((void*)0x01048380) = 0x01040000;
	
	/* first entry is for EXTLDR.EXT */
	char* trg = (char*)0x01040000;
	char* nm = path;
	while(*nm)
	{
		*trg = *nm;
		nm++;
		trg++;
	}
	mwrite(0x01040080, 0x00, 0x01040000);
	
	UINT_8 ext_loader_buff[0x410];
	__LiBOS_MemZero(ext_loader_buff, 0x410);
	
	unsigned int ext_loader_size = 0;
	BINPAPER bp = PaperOpen(path);
	PaperRead(&bp, (UINT_8*)(&ext_loader_size), 1, 1);
	printk("sizeof EXT LOADER: %\n", ext_loader_size);
	PaperRead(&bp, (UINT_8*)ext_loader_buff, ext_loader_size, 0);
	
	unsigned int i;
	UINT_8* target = (UINT_8*)0x01000000;
	for(i=0;i<0x400;i++)
		target[i] = ext_loader_buff[i];
	
	//__LiBOS_HexDump((void*)0x01000000, 0x400, "EXT LOADER");
	//__LiBOS_HexDump((void*)0x0104837C, 4, "GLOBAL_EXT_TABLE_COUNT");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void load_ext_image(char* path)
{
	UINT_8 ext_loader_buff[0x410];
	__LiBOS_MemZero(ext_loader_buff, 0x410);
	
	unsigned int ext_loader_size = 0;
	BINPAPER bp = PaperOpen(path);
	PaperRead(&bp, (UINT_8*)(&ext_loader_size), 1, 1);
	printk("sizeof IMAGE: %\n", ext_loader_size);
	PaperRead(&bp, (UINT_8*)ext_loader_buff, ext_loader_size, 0);
	//__LiBOS_HexDump(ext_loader_buff, 0x400, path);
	
	_LiBOSASM_load_ext_image(path, ext_loader_buff);
	/* 
		if the NASM call executes and successfully returns
		check the PHYSICAL_ADDRESS(GLOB_EXT_TAB) to see if
		it worked as expected
	*/
	//char* nm          = (char*) ((void*)0x01040084);
	//unsigned int addr = *(unsigned int*)((void*)(0x01040084 + 0x80));
	//printk(nm);
	//printk("\n");
	//__LiBOS_HexDump(&addr, 4, "LOADED ADDRESS");
	
	//_LiBOSASM_load_ext_image("libos_fake", ext_loader_buff);

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void dump_global_ext_table(void)
{
	UINT_32 count = mread(0x0104837C, 0x00);
	UINT_32 _tab  = mread(0x01048380, 0x00);
	
	char* tab = (char*)_tab;
	UINT_32 i = 0;
	while(i < count)
	{
		printk("entry %; name = ", i);
		printk(tab);
		printk("; address = ^\n", *(UINT_32*)(tab + 0x80));
		i++;
		tab += 0x84;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+












//..#define CODE_BASE_HOLDER 0xBF500004
//..#define DATA_BASE_HOLDER 0xBF500000
//..
//..extern void _execute_ect(void);
//..
//..void ect_execution(const INT_8* file_name)
//..{
//..	UINT_32 status = 0, size_of_paper = 0;
//..	BINPAPER paper = PaperOpen( (INT_8*)file_name );
//..	status = PaperRead(&paper, (UINT_8*)(&size_of_paper), 1, 0x01);
//..	if(status)
//..	{
//..		if(!size_of_paper)
//..		{
//..			panic("ERROR: size of ect file is zero\n");
//..			return;
//..		}
//..		status = 0;
//..		
//..		*(UINT_32*)((void*)CODE_BASE_HOLDER) = 0x08000000;
//..		*(UINT_32*)((void*)DATA_BASE_HOLDER) = 0x09000000;
//..		
//..		const char* print_ect = "ECT/__PRNT.ect";
//..		BINPAPER p = PaperOpen( (INT_8*)print_ect );
//..		status = PaperRead(&p, (UINT_8*)0x07000000, 512, 0);
//..		if(!status)
//..		{
//..			panic("ERROR: could not load the ECT/__PRNT.ect\n");
//..			return;
//..		}
//..		status = 0;
//..		const char* name_of_print_ect = "libos_lex_print";
//..		*(UINT_32*)((void*)0xBF50000C)       = (void*)name_of_print_ect; // pointer to the string
//..		*(UINT_32*)((void*)0xBF500010)       = 0x07000000; // address of print_ect
//..		UINT_32 code_base = *(UINT_32*)((void*)CODE_BASE_HOLDER);
//..		UINT_32 data_base = *(UINT_32*)((void*)DATA_BASE_HOLDER);
//..		
//..		status = PaperRead(&paper, (UINT_8*)code_base, size_of_paper, 0);
//..		if(!status)
//..		{
//..			panic("ERROR: ect file did not load\n");
//..			return;
//..		}
//..
//..		printk("ect file loaded successfully at ^\n"        , code_base);
//..		printk("ect file DATA_SPACE is at ^\n"              , data_base);
//..		printk("pre-compiled file loaded at ^ with name: " , *(UINT_32*)((void*)0xBF500010));
//..		INT_8* str = (INT_8*)(*(UINT_32*)((void*)0xBF50000C));
//..		printk(str);
//..		printk("\n");
//..		WaitSecond(2);
//..		clear_screen();
//..		_execute_ect();
//..		return;
//..	}
//..	printk("ERROR: could not read the paper at all\n");
//..	return;
//..}