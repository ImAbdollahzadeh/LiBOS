GCCPARAMS  = -m32 -Iinclude -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings -fstrength-reduce -nostdinc
LDPARAMS   = -melf_i386
NASMPARAMS = -f elf

objects    = obj/LOADER.o          \
             obj/PRINT.o           \
			 obj/GDT.o             \
             obj/PORT.o            \
             obj/IDT.o             \
             obj/EXTENDEDENTRIES.o \
             obj/TIMER.o           \
             obj/PCI.o             \
			 obj/AHCI.o            \
			 obj/EHCI.o            \
			 obj/FILESYSTEM.o      \
			 obj/VESA.o            \
			 obj/FILE.o            \
			 obj/KEYBOARD.o        \
			 obj/MOUSE.o           \
			 obj/MEMORY.o          \
			 obj/DOSSPEC.o         \
             obj/KERNEL.o

%.o: ./src/%.c
	gcc $(GCCPARAMS) -c -o ./obj/$@ $<

%.o: ./src/%.asm
	@nasm $(NASMPARAMS) -o ./obj/$@ $<

ThirdOS.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o ./bin/$@ $(objects)

copy_to_boot: ./bin/ThirdOS.bin
	sudo cp $< /boot/ThirdOS.bin

run_qemu:
	qemu-system-i386 -kernel ./bin/ThirdOS.bin 

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
	@make EHCI.o
	@make FILESYSTEM.o
	@make VESA.o
	@make FILE.o
	@make KEYBOARD.o
	@make MOUSE.o
	@make MEMORY.o
	@make DOSSPEC.o
	@make KERNEL.o
	@make ThirdOS.bin
	@make copy_to_boot
	@make run_qemu