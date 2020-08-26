OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386)

SECTIONS
{
    . = 0x00100000;
    .text   : { _ro_start = .;   *(.text*)                 KEEP(*(.text))   }
    .rodata : {                  *(.rodata*) _ro_end = .;  KEEP(*(.rodata)) }
    .data   : {                  *(.data*)                 KEEP(*(.data))   }
    .bss    : { _bss_start = .;  *(.bss*);   _bss_end = .; KEEP(*(.bss))    }
}
