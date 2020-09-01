ENTRY(LOADER)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

MEMORY
{
	rom  (rx)  : ORIGIN = 0,          LENGTH = 512K
	ram  (!rx) : ORIGIN = 0x00100000, LENGTH = 13M
}

SECTIONS
{
	.text :
	{
		code = .;
		_LiBOS_text_section_begin = .;
		*(.text)
		*(.rodata) 
		_LiBOS_text_section_end = .;
		. = ALIGN(4096);
	} > ram
	_LiBOS_text_section_size = SIZEOF(.text);
	.data : 
	{
		data = .;
		_LiBOS_data_section_begin = .;
		*(.data)
		_LiBOS_data_section_end = .;
		. = ALIGN(4096);
	} > ram
	_LiBOS_data_section_size = SIZEOF(.data);
	.bss : 
	{
		bss = .;
		_LiBOS_bss_section_begin = .;
		*(.bss)
		_LiBOS_bss_section_end = .;
		. = ALIGN(4096);
	} > ram
	_LiBOS_bss_section_size = SIZEOF(.bss);
	end = .;
}
