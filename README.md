# LiBOS
A 32-bit operating system implemented fully in assembly and C languages.<br/>
Support for multi-processor (logical CPUs) implementation.<br/>
It is gifted to my beloved gold fishes, Lila and Bob, who made the name of the OS to be originated from.<br/>
LiBOS: ***Li***la and ***B***ob ***OS***

# Aims:
## learn how an OS services by touching everything from scratch
It is a 32-bit protected mode OS enabling BIOS calls by switching into real mode and back again to protected mode
## implementation of USB 3.0 support via xHCI controller
The goal was primarily to support usb HIDs (mouse and keyboard), but finally I fell in love with the whole xHCI implementation.
 Thanks to Ben Lunt
## SVGA double buffering implementation for later implementation of custom MoviePlayer based on it
It has always been and still is my primarily goal to learn and start deep thinking of the OS working principles. 
I would love to implement fast, assembly-optimized graphic applications (from kernel, driver, and graphic adapter point of view). No doubt the best resource is "zen of assembly" and "zen of graphic" by M. Abrash
## Multi processor implementation for parallel multithreading
The legacy PIC was disabled and APIC, instead, enabled to allow waking up of all logical CPUs.
The concept of paging, threads, and processes were implemented to allow the kernel's scheduler to distribute threads accreoss various logical CPUs.
## Contact me:
email: [iman_ab2013[at]yahoo[DoT]com](mailto:iman_ab2013@yahoo.com)
or find me [here](https://forum.osdev.org/memberlist.php?mode=viewprofile&u=19122) in OSDev.rog
<br/>Basically I'm always available
## Wonderful open and close resources:
1. Zen of assembly language<br/>
2. Zen of Graphics Programming<br/>
3. bkerndev: Bran's Kernel Development<br/>
4. BrokenThorn Entertainment: Operating System Development Series<br/>
5. Transition from Protected Mode to Real Mode: by Bob Smith<br/>
6. Operating System Design Book Series: by Ben Lunt<br/>
7. SAVING AND RESTORING VGA SCREENS: Dr Dobb's<br/>
8. AHCI: in Github: thimble/ahci.c<br/>
9. MBR/EBR Partition Tables: by Daniel B. Sedory<br/>
10. eXtensible Host Controller Interface for Universal Serial Bus (xHCI) spec: by Intel<br/>
11. Universal Serial Bus 2.0 Specification: by Intel<br/>
12. Universal Serial Bus 3.0 Specification: by Intel<br/>
13. Intel® 7 Series / C216 Chipset Family Platform Controller Hub (PCH) Datasheet: by Intel<br/>
14. USB Made Simple: A Series of Articles on USB: Copyright © 2006-2008 MQP Electronics Ltd<br/>
15. USB Device Class Definition for Human Interface Devices (HID) Version 1.11<br/>
16. VESA BIOS EXTENSION (VBE); Core Functions; Standard Version: 2.0 Document Revision: 1.1<br/>
17. Modern X86 Assembly Language Programming: 32-bit, 64-bit, SSE, and AVX; by D. Kusswurm<br/>
18. Multiprocessing Support for Hobby OSes Explained by Ben L. Titzer<br/>
19. Intel® 64 and IA-32 Architectures Software Developer’s Manual Volume 3A<br/>
