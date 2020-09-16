#include "../include/IDT.h"
#include "../include/PRINT.h"
#include "../include/PORT.h"
#include "../include/TIMER.h"
#include "../include/XHCI.h"
#include "../include/KEYBOARD.h"
#include "../include/MP.h"
#include "../include/PAGING.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

#define PIC1_CMD     0x20  //MASTER
#define PIC1_DATA    0x21  //MASTER
#define PIC2_CMD     0xA0  //SLAVE
#define PIC2_DATA    0xA1  //SLAVE
#define PIC_READ_IRR 0x0a
#define PIC_READ_ISR 0x0b

void*      __irq_routines[256];
IDTPOINTER idt_pointer;

void (*handler)(REGS* r);

void LiBOS_DEFAULT_HANLER (REGS* r)
{ ; }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void SetIDTEntry(UINT_8 Interrupt, UINT_32 HandlerAddress, UINT_16 CodeSegmentSelectorOffset, UINT_8 flags)
{
	idt[Interrupt].handlerAddressLowBits   = (((UINT_32)HandlerAddress) & 0xFFFF);
	idt[Interrupt].handlerAddressHighBits  = (((UINT_32)HandlerAddress) >> 16) & 0xFFFF;
	idt[Interrupt].gdt_codeSegmentSelector = CodeSegmentSelectorOffset;
	idt[Interrupt].reserved                = 0;
	idt[Interrupt].access                  = flags | 0x60;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 RegisterIDT(void)
{
	UINT_32 status = 0, i;
	
	for(i=255;i>0;i--)
	{
		SetIDTEntry(i, (UINT_32)(void*)ISR_DEFAULT, 0x08, 0x8E);
		__irq_routines[i] = 0;
	}
		SetIDTEntry(0, (UINT_32)(void*)ISR_DEFAULT, 0x08, 0x8E);
		__irq_routines[0] = 0;


	SetIDTEntry(0,  (UINT_32)(void*)ISR0, 0x08, 0x8E);
	SetIDTEntry(1,  (UINT_32)(void*)ISR1, 0x08, 0x8E);
	SetIDTEntry(2,  (UINT_32)(void*)ISR2, 0x08, 0x8E);
	SetIDTEntry(3,  (UINT_32)(void*)ISR3, 0x08, 0x8E);
	SetIDTEntry(4,  (UINT_32)(void*)ISR4, 0x08, 0x8E);
	SetIDTEntry(5,  (UINT_32)(void*)ISR5, 0x08, 0x8E);
	SetIDTEntry(6,  (UINT_32)(void*)ISR6, 0x08, 0x8E);
	SetIDTEntry(7,  (UINT_32)(void*)ISR7, 0x08, 0x8E);
	SetIDTEntry(8,  (UINT_32)(void*)ISR8, 0x08, 0x8E);
	SetIDTEntry(9,  (UINT_32)(void*)ISR9, 0x08, 0x8E);
	SetIDTEntry(10, (UINT_32)(void*)ISR10, 0x08, 0x8E);
	SetIDTEntry(11, (UINT_32)(void*)ISR11, 0x08, 0x8E);
	SetIDTEntry(12, (UINT_32)(void*)ISR12, 0x08, 0x8E);
	SetIDTEntry(13, (UINT_32)(void*)ISR13, 0x08, 0x8E);
	SetIDTEntry(14, (UINT_32)(void*)ISR14, 0x08, 0x8E);
	SetIDTEntry(15, (UINT_32)(void*)ISR15, 0x08, 0x8E);
	SetIDTEntry(16, (UINT_32)(void*)ISR16, 0x08, 0x8E);
	SetIDTEntry(17, (UINT_32)(void*)ISR17, 0x08, 0x8E);
	SetIDTEntry(18, (UINT_32)(void*)ISR18, 0x08, 0x8E);
	SetIDTEntry(19, (UINT_32)(void*)ISR19, 0x08, 0x8E);
	SetIDTEntry(20, (UINT_32)(void*)ISR20, 0x08, 0x8E);
	SetIDTEntry(21, (UINT_32)(void*)ISR21, 0x08, 0x8E);
	SetIDTEntry(22, (UINT_32)(void*)ISR22, 0x08, 0x8E);
	SetIDTEntry(23, (UINT_32)(void*)ISR23, 0x08, 0x8E);
	SetIDTEntry(24, (UINT_32)(void*)ISR24, 0x08, 0x8E);
	SetIDTEntry(25, (UINT_32)(void*)ISR25, 0x08, 0x8E);
	SetIDTEntry(26, (UINT_32)(void*)ISR26, 0x08, 0x8E);
	SetIDTEntry(27, (UINT_32)(void*)ISR27, 0x08, 0x8E);
	SetIDTEntry(28, (UINT_32)(void*)ISR28, 0x08, 0x8E);
	SetIDTEntry(29, (UINT_32)(void*)ISR29, 0x08, 0x8E);
	SetIDTEntry(30, (UINT_32)(void*)ISR30, 0x08, 0x8E);
	SetIDTEntry(31, (UINT_32)(void*)ISR31, 0x08, 0x8E);
	
	//.printk("    In IDT: 32 ISRs registered\n");

	irq_remap();
	SetIDTEntry(32,  (UINT_32)(void*)IRQ0,   0x08, 0x8E);
	SetIDTEntry(33,  (UINT_32)(void*)IRQ1,   0x08, 0x8E);
	SetIDTEntry(34,  (UINT_32)(void*)IRQ2,   0x08, 0x8E);
	SetIDTEntry(35,  (UINT_32)(void*)IRQ3,   0x08, 0x8E);
	SetIDTEntry(36,  (UINT_32)(void*)IRQ4,   0x08, 0x8E);
	SetIDTEntry(37,  (UINT_32)(void*)IRQ5,   0x08, 0x8E);
	SetIDTEntry(38,  (UINT_32)(void*)IRQ6,   0x08, 0x8E);
	SetIDTEntry(39,  (UINT_32)(void*)IRQ7,   0x08, 0x8E);
	SetIDTEntry(40,  (UINT_32)(void*)IRQ8,   0x08, 0x8E);
	SetIDTEntry(41,  (UINT_32)(void*)IRQ9,   0x08, 0x8E);
	SetIDTEntry(42,  (UINT_32)(void*)IRQ10,  0x08, 0x8E);
	SetIDTEntry(43,  (UINT_32)(void*)IRQ11,  0x08, 0x8E);
	SetIDTEntry(44,  (UINT_32)(void*)IRQ12,  0x08, 0x8E);
	SetIDTEntry(45,  (UINT_32)(void*)IRQ13,  0x08, 0x8E);
	SetIDTEntry(46,  (UINT_32)(void*)IRQ14,  0x08, 0x8E);
	SetIDTEntry(47,  (UINT_32)(void*)IRQ15,  0x08, 0x8E);
	SetIDTEntry(48,  (UINT_32)(void*)IRQ16,  0x08, 0x8E);
	SetIDTEntry(49,  (UINT_32)(void*)IRQ17,  0x08, 0x8E);
	SetIDTEntry(50,  (UINT_32)(void*)IRQ18,  0x08, 0x8E);
	SetIDTEntry(51,  (UINT_32)(void*)IRQ19,  0x08, 0x8E);
	SetIDTEntry(52,  (UINT_32)(void*)IRQ20,  0x08, 0x8E);
	SetIDTEntry(53,  (UINT_32)(void*)IRQ21,  0x08, 0x8E);
	SetIDTEntry(54,  (UINT_32)(void*)IRQ22,  0x08, 0x8E);
	SetIDTEntry(55,  (UINT_32)(void*)IRQ23,  0x08, 0x8E);
	SetIDTEntry(56,  (UINT_32)(void*)IRQ24,  0x08, 0x8E);
	SetIDTEntry(57,  (UINT_32)(void*)IRQ25,  0x08, 0x8E);
	SetIDTEntry(58,  (UINT_32)(void*)IRQ26,  0x08, 0x8E);
	SetIDTEntry(59,  (UINT_32)(void*)IRQ27,  0x08, 0x8E);
	SetIDTEntry(60,  (UINT_32)(void*)IRQ28,  0x08, 0x8E);
	SetIDTEntry(61,  (UINT_32)(void*)IRQ29,  0x08, 0x8E);
	SetIDTEntry(62,  (UINT_32)(void*)IRQ30,  0x08, 0x8E);
	SetIDTEntry(63,  (UINT_32)(void*)IRQ31,  0x08, 0x8E);
	SetIDTEntry(64,  (UINT_32)(void*)IRQ32,  0x08, 0x8E);
	SetIDTEntry(65,  (UINT_32)(void*)IRQ33,  0x08, 0x8E);
	SetIDTEntry(66,  (UINT_32)(void*)IRQ34,  0x08, 0x8E);
	SetIDTEntry(67,  (UINT_32)(void*)IRQ35,  0x08, 0x8E);
	SetIDTEntry(68,  (UINT_32)(void*)IRQ36,  0x08, 0x8E);
	SetIDTEntry(69,  (UINT_32)(void*)IRQ37,  0x08, 0x8E);
	SetIDTEntry(70,  (UINT_32)(void*)IRQ38,  0x08, 0x8E);
	SetIDTEntry(71,  (UINT_32)(void*)IRQ39,  0x08, 0x8E);
	SetIDTEntry(72,  (UINT_32)(void*)IRQ40,  0x08, 0x8E);
	SetIDTEntry(73,  (UINT_32)(void*)IRQ41,  0x08, 0x8E);
	SetIDTEntry(74,  (UINT_32)(void*)IRQ42,  0x08, 0x8E);
	SetIDTEntry(75,  (UINT_32)(void*)IRQ43,  0x08, 0x8E);
	SetIDTEntry(76,  (UINT_32)(void*)IRQ44,  0x08, 0x8E);
	SetIDTEntry(77,  (UINT_32)(void*)IRQ45,  0x08, 0x8E);
	SetIDTEntry(78,  (UINT_32)(void*)IRQ46,  0x08, 0x8E);
	SetIDTEntry(79,  (UINT_32)(void*)IRQ47,  0x08, 0x8E);
	SetIDTEntry(80,  (UINT_32)(void*)IRQ48,  0x08, 0x8E);
	SetIDTEntry(81,  (UINT_32)(void*)IRQ49,  0x08, 0x8E);
	SetIDTEntry(82,  (UINT_32)(void*)IRQ50,  0x08, 0x8E);
	SetIDTEntry(83,  (UINT_32)(void*)IRQ51,  0x08, 0x8E);
	SetIDTEntry(84,  (UINT_32)(void*)IRQ52,  0x08, 0x8E);
	SetIDTEntry(85,  (UINT_32)(void*)IRQ53,  0x08, 0x8E);
	SetIDTEntry(86,  (UINT_32)(void*)IRQ54,  0x08, 0x8E);
	SetIDTEntry(87,  (UINT_32)(void*)IRQ55,  0x08, 0x8E);
	SetIDTEntry(88,  (UINT_32)(void*)IRQ56,  0x08, 0x8E);
	SetIDTEntry(89,  (UINT_32)(void*)IRQ57,  0x08, 0x8E);
	SetIDTEntry(90,  (UINT_32)(void*)IRQ58,  0x08, 0x8E);
	SetIDTEntry(91,  (UINT_32)(void*)IRQ59,  0x08, 0x8E);
	SetIDTEntry(92,  (UINT_32)(void*)IRQ60,  0x08, 0x8E);
	SetIDTEntry(93,  (UINT_32)(void*)IRQ61,  0x08, 0x8E);
	SetIDTEntry(94,  (UINT_32)(void*)IRQ62,  0x08, 0x8E);
	SetIDTEntry(95,  (UINT_32)(void*)IRQ63,  0x08, 0x8E);
	SetIDTEntry(96,  (UINT_32)(void*)IRQ64,  0x08, 0x8E);
	SetIDTEntry(97,  (UINT_32)(void*)IRQ65,  0x08, 0x8E);
	SetIDTEntry(98,  (UINT_32)(void*)IRQ66,  0x08, 0x8E);
	SetIDTEntry(99,  (UINT_32)(void*)IRQ67,  0x08, 0x8E);
	SetIDTEntry(100, (UINT_32)(void*)IRQ68,  0x08, 0x8E);
	SetIDTEntry(101, (UINT_32)(void*)IRQ69,  0x08, 0x8E);
	SetIDTEntry(102, (UINT_32)(void*)IRQ70,  0x08, 0x8E);
	SetIDTEntry(103, (UINT_32)(void*)IRQ71,  0x08, 0x8E);
	SetIDTEntry(104, (UINT_32)(void*)IRQ72,  0x08, 0x8E);
	SetIDTEntry(105, (UINT_32)(void*)IRQ73,  0x08, 0x8E);
	SetIDTEntry(106, (UINT_32)(void*)IRQ74,  0x08, 0x8E);
	SetIDTEntry(107, (UINT_32)(void*)IRQ75,  0x08, 0x8E);
	SetIDTEntry(108, (UINT_32)(void*)IRQ76,  0x08, 0x8E);
	SetIDTEntry(109, (UINT_32)(void*)IRQ77,  0x08, 0x8E);
	SetIDTEntry(110, (UINT_32)(void*)IRQ78,  0x08, 0x8E);
	SetIDTEntry(111, (UINT_32)(void*)IRQ79,  0x08, 0x8E);
	SetIDTEntry(112, (UINT_32)(void*)IRQ80,  0x08, 0x8E);
	SetIDTEntry(113, (UINT_32)(void*)IRQ81,  0x08, 0x8E);
	SetIDTEntry(114, (UINT_32)(void*)IRQ82,  0x08, 0x8E);
	SetIDTEntry(115, (UINT_32)(void*)IRQ83,  0x08, 0x8E);
	SetIDTEntry(116, (UINT_32)(void*)IRQ84,  0x08, 0x8E);
	SetIDTEntry(117, (UINT_32)(void*)IRQ85,  0x08, 0x8E);
	SetIDTEntry(118, (UINT_32)(void*)IRQ86,  0x08, 0x8E);
	SetIDTEntry(119, (UINT_32)(void*)IRQ87,  0x08, 0x8E);
	SetIDTEntry(120, (UINT_32)(void*)IRQ88,  0x08, 0x8E);
	SetIDTEntry(121, (UINT_32)(void*)IRQ89,  0x08, 0x8E);
	SetIDTEntry(122, (UINT_32)(void*)IRQ90,  0x08, 0x8E);
	SetIDTEntry(123, (UINT_32)(void*)IRQ91,  0x08, 0x8E);
	SetIDTEntry(124, (UINT_32)(void*)IRQ92,  0x08, 0x8E);
	SetIDTEntry(125, (UINT_32)(void*)IRQ93,  0x08, 0x8E);
	SetIDTEntry(126, (UINT_32)(void*)IRQ94,  0x08, 0x8E);
	SetIDTEntry(127, (UINT_32)(void*)IRQ95,  0x08, 0x8E);
	SetIDTEntry(128, (UINT_32)(void*)IRQ96,  0x08, 0x8E);
	SetIDTEntry(129, (UINT_32)(void*)IRQ97,  0x08, 0x8E);
	SetIDTEntry(130, (UINT_32)(void*)IRQ98,  0x08, 0x8E);
	SetIDTEntry(131, (UINT_32)(void*)IRQ99,  0x08, 0x8E);
	SetIDTEntry(132, (UINT_32)(void*)IRQ100, 0x08, 0x8E);
	SetIDTEntry(133, (UINT_32)(void*)IRQ101, 0x08, 0x8E);
	SetIDTEntry(134, (UINT_32)(void*)IRQ102, 0x08, 0x8E);
	SetIDTEntry(135, (UINT_32)(void*)IRQ103, 0x08, 0x8E);
	SetIDTEntry(136, (UINT_32)(void*)IRQ104, 0x08, 0x8E);
	SetIDTEntry(137, (UINT_32)(void*)IRQ105, 0x08, 0x8E);
	SetIDTEntry(138, (UINT_32)(void*)IRQ106, 0x08, 0x8E);
	SetIDTEntry(139, (UINT_32)(void*)IRQ107, 0x08, 0x8E);
	SetIDTEntry(140, (UINT_32)(void*)IRQ108, 0x08, 0x8E);
	SetIDTEntry(141, (UINT_32)(void*)IRQ109, 0x08, 0x8E);
	SetIDTEntry(142, (UINT_32)(void*)IRQ110, 0x08, 0x8E);
	SetIDTEntry(143, (UINT_32)(void*)IRQ111, 0x08, 0x8E);
	SetIDTEntry(144, (UINT_32)(void*)IRQ112, 0x08, 0x8E);
	SetIDTEntry(145, (UINT_32)(void*)IRQ113, 0x08, 0x8E);
	SetIDTEntry(146, (UINT_32)(void*)IRQ114, 0x08, 0x8E);
	SetIDTEntry(147, (UINT_32)(void*)IRQ115, 0x08, 0x8E);
	SetIDTEntry(148, (UINT_32)(void*)IRQ116, 0x08, 0x8E);
	SetIDTEntry(149, (UINT_32)(void*)IRQ117, 0x08, 0x8E);
	SetIDTEntry(150, (UINT_32)(void*)IRQ118, 0x08, 0x8E);
	SetIDTEntry(151, (UINT_32)(void*)IRQ119, 0x08, 0x8E);
	SetIDTEntry(152, (UINT_32)(void*)IRQ120, 0x08, 0x8E);
	SetIDTEntry(153, (UINT_32)(void*)IRQ121, 0x08, 0x8E);
	SetIDTEntry(154, (UINT_32)(void*)IRQ122, 0x08, 0x8E);
	SetIDTEntry(155, (UINT_32)(void*)IRQ123, 0x08, 0x8E);
	SetIDTEntry(156, (UINT_32)(void*)IRQ124, 0x08, 0x8E);
	SetIDTEntry(157, (UINT_32)(void*)IRQ125, 0x08, 0x8E);
	SetIDTEntry(158, (UINT_32)(void*)IRQ126, 0x08, 0x8E);
	SetIDTEntry(159, (UINT_32)(void*)IRQ127, 0x08, 0x8E);
	SetIDTEntry(160, (UINT_32)(void*)IRQ128, 0x08, 0x8E);
	SetIDTEntry(161, (UINT_32)(void*)IRQ129, 0x08, 0x8E);
	SetIDTEntry(162, (UINT_32)(void*)IRQ130, 0x08, 0x8E);
	SetIDTEntry(163, (UINT_32)(void*)IRQ131, 0x08, 0x8E);
	SetIDTEntry(164, (UINT_32)(void*)IRQ132, 0x08, 0x8E);
	SetIDTEntry(165, (UINT_32)(void*)IRQ133, 0x08, 0x8E);
	SetIDTEntry(166, (UINT_32)(void*)IRQ134, 0x08, 0x8E);
	SetIDTEntry(167, (UINT_32)(void*)IRQ135, 0x08, 0x8E);
	SetIDTEntry(168, (UINT_32)(void*)IRQ136, 0x08, 0x8E);
	SetIDTEntry(169, (UINT_32)(void*)IRQ137, 0x08, 0x8E);
	SetIDTEntry(170, (UINT_32)(void*)IRQ138, 0x08, 0x8E);
	SetIDTEntry(171, (UINT_32)(void*)IRQ139, 0x08, 0x8E);
	SetIDTEntry(172, (UINT_32)(void*)IRQ140, 0x08, 0x8E);
	SetIDTEntry(173, (UINT_32)(void*)IRQ141, 0x08, 0x8E);
	SetIDTEntry(174, (UINT_32)(void*)IRQ142, 0x08, 0x8E);
	SetIDTEntry(175, (UINT_32)(void*)IRQ143, 0x08, 0x8E);
	SetIDTEntry(176, (UINT_32)(void*)IRQ144, 0x08, 0x8E);
	SetIDTEntry(177, (UINT_32)(void*)IRQ145, 0x08, 0x8E);
	SetIDTEntry(178, (UINT_32)(void*)IRQ146, 0x08, 0x8E);
	SetIDTEntry(179, (UINT_32)(void*)IRQ147, 0x08, 0x8E);
	SetIDTEntry(180, (UINT_32)(void*)IRQ148, 0x08, 0x8E);
	SetIDTEntry(181, (UINT_32)(void*)IRQ149, 0x08, 0x8E);
	SetIDTEntry(182, (UINT_32)(void*)IRQ150, 0x08, 0x8E);
	SetIDTEntry(183, (UINT_32)(void*)IRQ151, 0x08, 0x8E);
	SetIDTEntry(184, (UINT_32)(void*)IRQ152, 0x08, 0x8E);
	SetIDTEntry(185, (UINT_32)(void*)IRQ153, 0x08, 0x8E);
	SetIDTEntry(186, (UINT_32)(void*)IRQ154, 0x08, 0x8E);
	SetIDTEntry(187, (UINT_32)(void*)IRQ155, 0x08, 0x8E);
	SetIDTEntry(188, (UINT_32)(void*)IRQ156, 0x08, 0x8E);
	SetIDTEntry(189, (UINT_32)(void*)IRQ157, 0x08, 0x8E);
	SetIDTEntry(190, (UINT_32)(void*)IRQ158, 0x08, 0x8E);
	SetIDTEntry(191, (UINT_32)(void*)IRQ159, 0x08, 0x8E);
	SetIDTEntry(192, (UINT_32)(void*)IRQ160, 0x08, 0x8E);
	SetIDTEntry(193, (UINT_32)(void*)IRQ161, 0x08, 0x8E);
	SetIDTEntry(194, (UINT_32)(void*)IRQ162, 0x08, 0x8E);
	SetIDTEntry(195, (UINT_32)(void*)IRQ163, 0x08, 0x8E);
	SetIDTEntry(196, (UINT_32)(void*)IRQ164, 0x08, 0x8E);
	SetIDTEntry(197, (UINT_32)(void*)IRQ165, 0x08, 0x8E);
	SetIDTEntry(198, (UINT_32)(void*)IRQ166, 0x08, 0x8E);
	SetIDTEntry(199, (UINT_32)(void*)IRQ167, 0x08, 0x8E);
	SetIDTEntry(200, (UINT_32)(void*)IRQ168, 0x08, 0x8E);
	SetIDTEntry(201, (UINT_32)(void*)IRQ169, 0x08, 0x8E);
	SetIDTEntry(202, (UINT_32)(void*)IRQ170, 0x08, 0x8E);
	SetIDTEntry(203, (UINT_32)(void*)IRQ171, 0x08, 0x8E);
	SetIDTEntry(204, (UINT_32)(void*)IRQ172, 0x08, 0x8E);
	SetIDTEntry(205, (UINT_32)(void*)IRQ173, 0x08, 0x8E);
	SetIDTEntry(206, (UINT_32)(void*)IRQ174, 0x08, 0x8E);
	SetIDTEntry(207, (UINT_32)(void*)IRQ175, 0x08, 0x8E);
	SetIDTEntry(208, (UINT_32)(void*)IRQ176, 0x08, 0x8E);
	SetIDTEntry(209, (UINT_32)(void*)IRQ177, 0x08, 0x8E);
	SetIDTEntry(210, (UINT_32)(void*)IRQ178, 0x08, 0x8E);
	SetIDTEntry(211, (UINT_32)(void*)IRQ179, 0x08, 0x8E);
	SetIDTEntry(212, (UINT_32)(void*)IRQ180, 0x08, 0x8E);
	SetIDTEntry(213, (UINT_32)(void*)IRQ181, 0x08, 0x8E);
	SetIDTEntry(214, (UINT_32)(void*)IRQ182, 0x08, 0x8E);
	SetIDTEntry(215, (UINT_32)(void*)IRQ183, 0x08, 0x8E);
	SetIDTEntry(216, (UINT_32)(void*)IRQ184, 0x08, 0x8E);
	SetIDTEntry(217, (UINT_32)(void*)IRQ185, 0x08, 0x8E);
	SetIDTEntry(218, (UINT_32)(void*)IRQ186, 0x08, 0x8E);
	SetIDTEntry(219, (UINT_32)(void*)IRQ187, 0x08, 0x8E);
	SetIDTEntry(220, (UINT_32)(void*)IRQ188, 0x08, 0x8E);
	SetIDTEntry(221, (UINT_32)(void*)IRQ189, 0x08, 0x8E);
	SetIDTEntry(222, (UINT_32)(void*)IRQ190, 0x08, 0x8E);
	SetIDTEntry(223, (UINT_32)(void*)IRQ191, 0x08, 0x8E);
	SetIDTEntry(224, (UINT_32)(void*)IRQ192, 0x08, 0x8E);
	SetIDTEntry(225, (UINT_32)(void*)IRQ193, 0x08, 0x8E);
	SetIDTEntry(226, (UINT_32)(void*)IRQ194, 0x08, 0x8E);
	SetIDTEntry(227, (UINT_32)(void*)IRQ195, 0x08, 0x8E);
	SetIDTEntry(228, (UINT_32)(void*)IRQ196, 0x08, 0x8E);
	SetIDTEntry(229, (UINT_32)(void*)IRQ197, 0x08, 0x8E);
	SetIDTEntry(230, (UINT_32)(void*)IRQ198, 0x08, 0x8E);
	SetIDTEntry(231, (UINT_32)(void*)IRQ199, 0x08, 0x8E);
	SetIDTEntry(232, (UINT_32)(void*)IRQ200, 0x08, 0x8E);
	SetIDTEntry(233, (UINT_32)(void*)IRQ201, 0x08, 0x8E);
	SetIDTEntry(234, (UINT_32)(void*)IRQ202, 0x08, 0x8E);
	SetIDTEntry(235, (UINT_32)(void*)IRQ203, 0x08, 0x8E);
	SetIDTEntry(236, (UINT_32)(void*)IRQ204, 0x08, 0x8E);
	SetIDTEntry(237, (UINT_32)(void*)IRQ205, 0x08, 0x8E);
	SetIDTEntry(238, (UINT_32)(void*)IRQ206, 0x08, 0x8E);
	SetIDTEntry(239, (UINT_32)(void*)IRQ207, 0x08, 0x8E);
	SetIDTEntry(240, (UINT_32)(void*)IRQ208, 0x08, 0x8E);
	SetIDTEntry(241, (UINT_32)(void*)IRQ209, 0x08, 0x8E);
	SetIDTEntry(242, (UINT_32)(void*)IRQ210, 0x08, 0x8E);
	SetIDTEntry(243, (UINT_32)(void*)IRQ211, 0x08, 0x8E);
	SetIDTEntry(244, (UINT_32)(void*)IRQ212, 0x08, 0x8E);
	SetIDTEntry(245, (UINT_32)(void*)IRQ213, 0x08, 0x8E);
	SetIDTEntry(246, (UINT_32)(void*)IRQ214, 0x08, 0x8E);
	SetIDTEntry(247, (UINT_32)(void*)IRQ215, 0x08, 0x8E);
	SetIDTEntry(248, (UINT_32)(void*)IRQ216, 0x08, 0x8E);
	SetIDTEntry(249, (UINT_32)(void*)IRQ217, 0x08, 0x8E);
	SetIDTEntry(250, (UINT_32)(void*)IRQ218, 0x08, 0x8E);
	SetIDTEntry(251, (UINT_32)(void*)IRQ219, 0x08, 0x8E);
	SetIDTEntry(252, (UINT_32)(void*)IRQ220, 0x08, 0x8E);
	SetIDTEntry(253, (UINT_32)(void*)IRQ221, 0x08, 0x8E);
	SetIDTEntry(254, (UINT_32)(void*)IRQ222, 0x08, 0x8E);
	SetIDTEntry(255, (UINT_32)(void*)IRQ223, 0x08, 0x8E);
	
	//.printk("    In IDT: 16 IRQs registered\n");

	idt_pointer.size = (sizeof(GATEDESCRIPTOR) * 256) - 1;
	idt_pointer.base = (UINT_32)((void*)(&idt[0]));
	
	if(!idt_pointer.size || !idt_pointer.base)
	{
		panic("IDT pointer Failed\n");
		return 0;
	}
	
	status = IDTLoad();
	if (!status)
	{
		panic("IDT pointer loading Failed\n");
		return 0;
	}
	
	//.printk("    In IDT: IDT pointer created\n");
	
	//.printk( "        >>> IDT registered successfully <<<\n");

	irq_install_handler(7,  &LiBOS_DEFAULT_HANLER); // 0x07 pic mster spurious
	irq_install_handler(15, &LiBOS_DEFAULT_HANLER); // 0x0F pic slave spurious
	irq_install_handler(47, &LiBOS_DEFAULT_HANLER); // 0x2F lapic spurious
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

INT_8* __exception_messages[32] = 
{
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",
	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ LiBOS actual_page_fault_handling

/* this function takes the physical address where a page fault happened
   and passes the address to PAGING routines */
static BOOL actual_page_fault_handling(UINT_32 virtual_address)
{
	BOOL status = FALSE; 
	if( !(status = ask_for_page(virtual_address)) )
		panic("LiBOS could not get a valid page\n");
	return status;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ PIC functions

static UINT_16 pic_get_irq_reg(INT_32 ocw3)
{
	PORT_8 p;
	Port_8_Register(&p);

	p.Write(PIC2_CMD, ocw3);
	return (p.Read(PIC2_CMD) << 8);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_16 pic_get_irr(void)
{
	return pic_get_irq_reg(PIC_READ_IRR);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_16 pic_get_isr(void)
{
	return pic_get_irq_reg(PIC_READ_ISR);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ LiBOS fault handler

void LiBOS_fault_handler(REGS* r)
{
	if (r->int_no < 32)
	{
		panic(__exception_messages[r->int_no]);
		panic(" Exception. System Halted!\n");
		printk("eax:^, ebx:^, ecx:^, edx:^\n",  r->eax, r->ebx, r->ecx, r->edx);
		printk("esi:^, edi:^, esp:^, ebp:^\n",  r->esi, r->edi, r->esp, r->ebp);
		printk("eip:^, cs:^, ss:^, eflags:^\n", r->eip, r->cs, r->ss, r->eflags);
		printk("err_code:^\n", r->err_code);
		
		/* if page_fault, query cr2 register */
		if(r->int_no == 14)
		{
			UINT_32 cr2 = 0;
			query_cr2(&cr2);
			printk("CR2 = ^\n", cr2);
			
			/* handle the page fault and safely return */
			if( !actual_page_fault_handling(cr2) )
				goto hang_forever;
			return;
		}
		
		/* for INT 0x03 (a.k.a. DEBUG_HANDLER) we need a keyboard code to continue */
		if (r->int_no == 0x03)
		{
			// we need space-bar to continue
			getch(' ');
			return;
		}
hang_forever:
		while(1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ LiBOS irq handler

void LiBOS_irq_handler(REGS* r)
{
	PORT_8 p;
	Port_8_Register(&p);

	handler = __irq_routines[r->int_no - 32];
	if (handler)
		handler(r);
	
	if( apic_mode() )
	{
		apic_eoi();
		goto TEMP_APIC_MODE;
	}
	
	/* If the IDT entry that was invoked was greater than 40 (meaning IRQ8 - 15), then we need to send an EOI to the slave controller */
	if (r->int_no >= 40)
		p.Write(0xA0, 0x20);
	/* In either case, we need to send an EOI to the master interrupt controller too */
	p.Write(0x20, 0x20);

	if(r->int_no == 43) // XHCI must be followed continously
	{
		XHCI* x = xhci_instance_to_idt();
		if(!( xhci_get_critical_event() ))
			return;
		else
		{
			if(!(x->usb_device->slot_configuration))
			{
				if(x->usb_device->slot_configuration_command)
					xhci_slot_configuration(x, x->usb_device->port, x->usb_device->speed);
			}
		}
	}
	
TEMP_APIC_MODE:
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void irq_set_mask(UINT_8 irq_line) 
{
	UINT_16 port;
	UINT_8  value;
	
	PORT_8 p;
	Port_8_Register(&p);
	
	if(irq_line < 8) 
		port = PIC1_DATA;
	else 
	{
		port      = PIC2_DATA;
		irq_line -= 8;
	}
	value = p.Read(port) | (1 << irq_line);
	p.Write(port, value);        
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void irq_clear_mask(UINT_8 irq_line) 
{
	UINT_16 port;
	UINT_8  value;
	
	PORT_8 p;
	Port_8_Register(&p);
	
	if(irq_line < 8) 
		port = PIC1_DATA;
	else 
	{
		port      = PIC2_DATA;
		irq_line -= 8;
	}
	value = p.Read(port) & ~(1 << irq_line);
	p.Write(port, value);        
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void irq_install_handler(INT_32 irq, void (*__handler)(REGS* r))
{
	__irq_routines[irq] = __handler;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void irq_uninstall_handler(INT_32 irq)
{
	__irq_routines[irq] = 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void irq_remap(void)
{
	PORT_8 p;
	Port_8_Register(&p);
	
	p.Write(0x20, 0x11);
	p.Write(0xA0, 0x11);
	p.Write(0x21, 0x20);
	p.Write(0xA1, 0x28);
	p.Write(0x21, 0x04);
	p.Write(0xA1, 0x02);
	p.Write(0x21, 0x01);
	p.Write(0xA1, 0x01);
	p.Write(0x21, 0x00);
	p.Write(0xA1, 0x00);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void go_to_reset(void)
{
	/*  
		We have nothing to lose at this point,
		since the KILL command has been requested
		perform a triple fault on purpose by destroying the whole IDT table 
	*/
	INT_32 i;
	UINT_8* vid = Disps();
	for(i =0; i<50*160; i+=2)
	{
		vid[i]   = ' ';
		vid[i+1] = 0;
	}
	
	panic("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	panic("                         REBOOT                         \n");
	panic("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	WaitSecond(1);
	_CLI();
	idt_pointer.size = 0;
	idt_pointer.base = 0;
	IDTLoad();
	_STI();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void disable_pic(void)
{
	PORT_8 p;
	Port_8_Register(&p);
	
	irq_remap();
	p.Write(0x21, 0xFF);
	p.Write(0xA1, 0xFF);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+