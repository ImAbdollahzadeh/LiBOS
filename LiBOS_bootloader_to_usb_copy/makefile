# you have to plug your USB stick and with GParted, format it to 'cleared', then from Device->CreatePartitionTable, set tomsdos and done.
# this would be the command to be given in terminal
# make install. usb_device=/dev/sdb(c or any other letters) bootsector=BTLDR1.SYS second_stage_bootloader=BTLDR2.SYS kernel=LIBKRN.SYS

GCCPARAMS  = -m32 -Iinclude -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings -fstrength-reduce -nostdinc -ffreestanding -mno-red-zone -nostartfiles -nodefaultlibs

install.:
	@#==========>  FIRST CHECK IF THE USB HAS BEEN MOUNTED BEFORE, IF SO UNMOUNT IT OTHERWISE DO NOTHING <===========   
	@(mount | grep $(usb_device) > /dev/null) && (sudo umount $(usb_device)) || echo "" > /dev/null
	@#==========>  FORMAT THE USB AS A FAT32 DRIVE  <=========== 
	@sudo mkfs.vfat -F32 $(usb_device) -I
	@#==========>  CREATE OBJECT AND BINARY FILES  <===========
	@gcc $(GCCPARAMS) -c -o ckernel.o ckernel.c
	@nasm -f bin -o $(bootsector) boot1.asm
	@nasm -f bin -o $(second_stage_bootloader) boot2.asm
	@nasm -f elf -o asmkernel.o kernel.asm
	@ld -melf_i386 -T kernel.ld -o $(kernel) asmkernel.o ckernel.o
	@#==========>  CREATE A BLANK FLOPPY IMAGE FILE AS BIG AS 1.44 MB  <===========
	@sudo dd bs=512 count=2880 if=/dev/zero of=LiBOS_FLOPPY.img
	@#==========>  FORMAT THE IMAGE FILE  <===========
	@sudo mkfs.msdos LiBOS_FLOPPY.img
	@#==========>  WRITE THE 1st BOOTLOADER INTO THE BOOTSECTOR OF THE IMAGE FILE. THE notrunc IS VERY IMPORTANT  <===========
	@sudo dd if=$(bootsector) of=LiBOS_FLOPPY.img bs=512 count=1 conv=notrunc
	@#==========>  CREATE A DIRECTORY, MOUNT THE IMAGEFILE TO THIS DIRECTORY VIA LOOP DEVICES  <===========
	@sudo mkdir /media/__LIBOS_virtual_floppy_drive_version_0xAABBCCDD/
	@sudo mount -o loop LiBOS_FLOPPY.img /media/__LIBOS_virtual_floppy_drive_version_0xAABBCCDD/
	@#==========>  COPY THE 2nd BOOTLOADER AND KERNEL FILES TO THE FLOPPY DRIVE (DIRECTORY)  <===========
	@sudo cp $(second_stage_bootloader) /media/__LIBOS_virtual_floppy_drive_version_0xAABBCCDD/
	@sudo cp $(kernel) /media/__LIBOS_virtual_floppy_drive_version_0xAABBCCDD/
	@#==========>  UNMOUNT THE FLOPPY DIRECTORY  <===========
	@sudo umount /media/__LIBOS_virtual_floppy_drive_version_0xAABBCCDD/
	@#==========>  WRITE THE IMAGE FILE INTO YOUR USB  <===========
	@sudo dd if=LiBOS_FLOPPY.img of=$(usb_device) bs=512 count=2880
	@#==========>  CLEAN  <=========== 
	@sudo rm -R /media/__LIBOS_virtual_floppy_drive_version_0xAABBCCDD/
	@sudo rm LiBOS_FLOPPY.img
	@sudo rm $(kernel) 
	@sudo rm $(second_stage_bootloader) 
	@sudo rm $(bootsector) 
	@sudo rm asmkernel.o
	@sudo rm ckernel.o
	@#==========>  REBOOT THE COMPUTER  <=========== 
	@#reboot
