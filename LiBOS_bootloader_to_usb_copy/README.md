***Please do not try the provided 'makefile' unless you know what you are doing***

# Aim:
Making a bootable USB stick with LiBOS

This is the triple-stage bootloader file. 
There is the bootsector (which is loaded into the USB by **DD** utility), the 2nd stage bootloader file and the LiBOS binary file.

The first and second stage bootloaders are labeled as **BTLDR1.SYS** and **BTLDR2.SYS**, respectively.
The actual kernel binary file is labeled as **LIBKRN.SYS**. 
