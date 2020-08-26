[Bits 32]
jmp KernelStart


KernelStart:
    mov esp, 0x600000

    mov eax, cr0
    and eax, 0x9FFFFFFF     ; Activate CPU cache
    mov cr0, eax

    push ebx                ; EBX points to the multiboot structure created by the bootloader and containing e.g. the address of the memory map

    extern main             ; entry point in ckernel.c
    call   main

    cli
    hlt
