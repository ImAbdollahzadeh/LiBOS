***Please do not try the provided 'makefile' unless you know what you are doing***

# Aim:
Making a bootable USB stick with LiBOS

This is the triple-stage bootloader file. 
There is the bootsector (which is loaded into the USB by DD utility), second_stage_bootloader and the LiBOS binary file.

The first stage bootloader is labeled as 'BTLDR1.SYS', the second stage bootloader is labeled as 'BTLDR2.SYS'
and the actual kernel binary file is labeled as 'LIBKRN.SYS'. 
