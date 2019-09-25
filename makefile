GCCPARAMS  = -m32 -Iinclude -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings -fstrength-reduce -nostdinc -ffreestanding -mno-red-zone -nostartfiles -nodefaultlibs
LDPARAMS   = -melf_i386
NASMPARAMS = -f elf

objects =   obj/LOADER.o          \
            obj/PRINT.o           \
	    obj/GDT.o             \
            obj/PORT.o            \
            obj/IDT.o             \
            obj/EXTENDEDENTRIES.o \
            obj/TIMER.o           \
            obj/PCI.o             \
	    obj/AHCI.o            \
	    obj/XHCI.o            \
	    obj/KEYBOARD.o        \
	    obj/MOUSE.o           \
	    obj/MEMORY.o          \
            obj/KERNEL.o

%.o: ./src/%.c
	gcc $(GCCPARAMS) -c -o ./obj/$@ $<

%.o: ./src/%.asm
	@nasm $(NASMPARAMS) -o ./obj/$@ $<

LiBOS.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o ./bin/$@ $(objects)

copy_to_boot: ./bin/LiBOS.bin
	sudo cp $< /boot/LiBOS.bin

run_qemu:
	qemu-system-i386 -device nec-usb-xhci,p2=8,p3=8,id=xhci -device usb-tablet,bus=xhci.0 -kernel ./bin/LiBOS.bin 

run: 
	@make LOADER.o
	@make PRINT.o
	@make GDT.o
	@make PORT.o
	@make EXTENDEDENTRIES.o
	@make IDT.o
	@make TIMER.o
	@make PCI.o
	@make AHCI.o
	@make XHCI.o
	@make KEYBOARD.o
	@make MOUSE.o
	@make MEMORY.o
	@make KERNEL.o
	@make LiBOS.bin
	@make copy_to_boot
	@make run_qemu
