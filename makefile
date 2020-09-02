GCCPARAMS  = -m32 -Iinclude -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings -fstrength-reduce -nostdinc -ffreestanding -mno-red-zone -nostartfiles -nodefaultlibs
LDPARAMS   = -melf_i386
NASMPARAMS = -f elf

objects =		                          \
		obj/LiBOS_LOADER.o                \
		obj/PRINT.o                       \
		obj/GDT.o                         \
		obj/ACPI.o                        \
		obj/CPU.o                         \
		obj/DEBUGGER.o                    \
		obj/MP.o                          \
		obj/PORT.o                        \
		obj/PAGING.o                      \
		obj/PROCESS.o                     \
		obj/_ASM_PAGING.o                 \
		obj/IDT.o                         \
		obj/ISRs_IRQs.o                   \
		obj/MULTI_PROCESSOR.o             \
		obj/IMAGE_LOADER.o                \
		obj/I_ASM.o                       \
		obj/PMODE_BIOS_CALL.o             \
		obj/_ASM_VIDEO_PLAYER.o           \
		obj/_ASM_WINDOW.o                 \
		obj/_ASM_FONT.o                   \
		obj/_ASM_SSE.o                    \
		obj/FONT.o                        \
		obj/VIDEO_PLAYER.o                \
		obj/WINDOW.o                      \
		obj/TIMER.o                       \
		obj/PCI.o                         \
		obj/AHCI.o                        \
		obj/FILE.o                        \
		obj/FILESYSTEM.o                  \
		obj/DOSSPEC.o                     \
		obj/XHCI.o                        \
		obj/SVGA.o                        \
		obj/KEYBOARD.o                    \
		obj/MOUSE.o                       \
		obj/MEMORY.o                      \
		obj/KERNEL.o

%.o: ./src/%.c
	gcc $(GCCPARAMS) -c -o ./obj/$@ $<

%.o: ./src/x86/%.asm
	@nasm $(NASMPARAMS) -o ./obj/$@ $<

LiBOS.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o ./bin/$@ $(objects)

copy_to_boot: ./bin/LiBOS.bin
	sudo cp $< /boot/LiBOS.bin

run_qemu:
	#qemu-system-i386 -device nec-usb-xhci,p2=8,p3=8,id=xhci -device usb-tablet,bus=xhci.0 -kernel ./bin/LiBOS.bin 
	qemu-system-i386 -m 3000M -kernel ./bin/LiBOS.bin 

.PHONY: run
run: 
	@make LiBOS_LOADER.o
	@make MULTI_PROCESSOR.o
	@make CPU.o
	@make DEBUGGER.o
	@make ISRs_IRQs.o
	@make PRINT.o
	@make GDT.o
	@make PORT.o
	@make PAGING.o
	@make PROCESS.o
	@make _ASM_PAGING.o
	@make MULTI_PROCESSOR.o
	@make PMODE_BIOS_CALL.o
	@make ACPI.o
	@make MP.o
	@make _ASM_VIDEO_PLAYER.o
	@make _ASM_WINDOW.o
	@make _ASM_FONT.o
	@make _ASM_SSE.o
	@make VIDEO_PLAYER.o
	@make WINDOW.o
	@make FONT.o
	@make SVGA.o
	@make IDT.o
	@make TIMER.o
	@make PCI.o
	@make FILE.o
	@make FILESYSTEM.o
	@make IMAGE_LOADER.o
	@make I_ASM.o
	@make DOSSPEC.o
	@make AHCI.o
	@make XHCI.o
	@make KEYBOARD.o
	@make MOUSE.o
	@make MEMORY.o
	@make KERNEL.o
	@make LiBOS.bin
	@make copy_to_boot
	@make run_qemu
