#include "../include/IDT.h"
#include "../include/PRINT.h"
#include "../include/PORT.h"
#include "../include/TIMER.h"
#include "../include/XHCI.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

void*      __irq_routines[256];
IDTPOINTER idt_pointer;
void (*handler)(REGS* r);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void SetIDTEntry(UINT_8 Interrupt, UINT_32 HandlerAddress, UINT_16 CodeSegmentSelectorOffset, UINT_8 flags)
{
	idt[Interrupt].handlerAddressLowBits   = (((UINT_32)HandlerAddress) & 0xFFFF);
	idt[Interrupt].handlerAddressHighBits  = (((UINT_32)HandlerAddress) >> 16) & 0xFFFF;
	idt[Interrupt].gdt_codeSegmentSelector = CodeSegmentSelectorOffset;
	idt[Interrupt].reserved                = 0;
	idt[Interrupt].access                  = flags;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 RegisterIDT(void)
{
	UINT_32 status = 0, i;	
	
	for(i=255;i>0;i--)
	{
		SetIDTEntry(i, (UINT_32)(void*)ISR_DEFAULT,  0x08, 0x8E);
		__irq_routines[i] = 0;
	}
	    SetIDTEntry(0, (UINT_32)(void*)ISR_DEFAULT,  0x08, 0x8E);
	    __irq_routines[0] = 0;


	SetIDTEntry(0,  (UINT_32)(void*)ISR0,  0x08, 0x8E);
    SetIDTEntry(1,  (UINT_32)(void*)ISR1,  0x08, 0x8E);
    SetIDTEntry(2,  (UINT_32)(void*)ISR2,  0x08, 0x8E);
    SetIDTEntry(3,  (UINT_32)(void*)ISR3,  0x08, 0x8E);
	SetIDTEntry(4,  (UINT_32)(void*)ISR4,  0x08, 0x8E);
    SetIDTEntry(5,  (UINT_32)(void*)ISR5,  0x08, 0x8E);
    SetIDTEntry(6,  (UINT_32)(void*)ISR6,  0x08, 0x8E);
    SetIDTEntry(7,  (UINT_32)(void*)ISR7,  0x08, 0x8E);
	SetIDTEntry(8,  (UINT_32)(void*)ISR8,  0x08, 0x8E);
    SetIDTEntry(9,  (UINT_32)(void*)ISR9,  0x08, 0x8E);
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

	__irq_remap();
    SetIDTEntry(32, (UINT_32)(void*)IRQ0,  0x08, 0x8E);
	SetIDTEntry(33, (UINT_32)(void*)IRQ1,  0x08, 0x8E);
	SetIDTEntry(34, (UINT_32)(void*)IRQ2,  0x08, 0x8E);
	SetIDTEntry(35, (UINT_32)(void*)IRQ3,  0x08, 0x8E);
	SetIDTEntry(36, (UINT_32)(void*)IRQ4,  0x08, 0x8E);
	SetIDTEntry(37, (UINT_32)(void*)IRQ5,  0x08, 0x8E);
	SetIDTEntry(38, (UINT_32)(void*)IRQ6,  0x08, 0x8E);
	SetIDTEntry(39, (UINT_32)(void*)IRQ7,  0x08, 0x8E);
	SetIDTEntry(40, (UINT_32)(void*)IRQ8,  0x08, 0x8E);
	SetIDTEntry(41, (UINT_32)(void*)IRQ9,  0x08, 0x8E);
	SetIDTEntry(42, (UINT_32)(void*)IRQ10, 0x08, 0x8E);
	SetIDTEntry(43, (UINT_32)(void*)IRQ11, 0x08, 0x8E);
	SetIDTEntry(44, (UINT_32)(void*)IRQ12, 0x08, 0x8E);
	SetIDTEntry(45, (UINT_32)(void*)IRQ13, 0x08, 0x8E);
	SetIDTEntry(46, (UINT_32)(void*)IRQ14, 0x08, 0x8E);
    SetIDTEntry(47, (UINT_32)(void*)IRQ15, 0x08, 0x8E);
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define PIC1_CMD     0x20  //MASTER
#define PIC1_DATA    0x21  //MASTER
#define PIC2_CMD     0xA0  //SLAVE
#define PIC2_DATA    0xA1  //SLAVE
#define PIC_READ_IRR 0x0a
#define PIC_READ_ISR 0x0b

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static UINT_16 __pic_get_irq_reg(INT_32 ocw3)
{
    PORT_8 p;
	Port_8_Register(&p);	

    p.Write(PIC2_CMD, ocw3);
    return (p.Read(PIC2_CMD) << 8);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_16 pic_get_irr(void)
{
    return __pic_get_irq_reg(PIC_READ_IRR);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_16 pic_get_isr(void)
{
    return __pic_get_irq_reg(PIC_READ_ISR);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void FAULT_HANDLER(REGS* r)
{
    if (r->int_no < 32)
    {
        panic(__exception_messages[r->int_no]);
        panic(" Exception. System Halted!\n");
        while(1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void IRQ_HANDLER(REGS* r)
{
	PORT_8 p;
	Port_8_Register(&p);
	
    handler = __irq_routines[r->int_no - 32];
    if (handler)
    	handler(r);

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
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void IRQ_set_mask(UINT_8 IRQline) 
{
    UINT_16 port;
    UINT_8  value;
 
	PORT_8 p;
	Port_8_Register(&p);
	
    if(IRQline < 8) 
        port = PIC1_DATA;
	else 
	{
        port     = PIC2_DATA;
        IRQline -= 8;
    }
    value = p.Read(port) | (1 << IRQline);
    p.Write(port, value);        
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void IRQ_clear_mask(UINT_8 IRQline) 
{
    UINT_16 port;
    UINT_8  value;
 
	PORT_8 p;
	Port_8_Register(&p);
 
    if(IRQline < 8) 
        port = PIC1_DATA;
	else 
	{
        port     = PIC2_DATA;
        IRQline -= 8;
    }
    value = p.Read(port) & ~(1 << IRQline);
    p.Write(port, value);        
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void __irq_install_handler(INT_32 irq, void (*__handler)(REGS* r))
{
	_CLI();
	__irq_routines[irq] = __handler;
	// master PIC
	IRQ_set_mask(3);
	IRQ_set_mask(4);
	IRQ_set_mask(5);
	IRQ_set_mask(6);
	IRQ_set_mask(7);
	// slave PIC
	IRQ_set_mask(8);
	IRQ_set_mask(9);
	IRQ_set_mask(10);
	IRQ_set_mask(12);
	IRQ_set_mask(13);
	IRQ_set_mask(14);
	IRQ_set_mask(15);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void __irq_uninstall_handler(INT_32 irq)
{
    __irq_routines[irq] = 0;
	
	// master PIC
	IRQ_clear_mask(3);
	IRQ_clear_mask(4);
	IRQ_clear_mask(5);
	IRQ_clear_mask(6);
	IRQ_clear_mask(7);
	
	// slave PIC
	IRQ_clear_mask(8);
	IRQ_clear_mask(9);
	IRQ_clear_mask(10);
	IRQ_clear_mask(12);
	IRQ_clear_mask(13);
	IRQ_clear_mask(14);
	IRQ_clear_mask(15);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void __irq_remap(void)
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

void __go_to_reset(void)
{
	/*  
		We have nothing to lose at this point,
		since the KILL command has been requested
		perform a triple fault on purpose by destroying the whole IDT table 
	*/
	int i;
	UINT_8* vid = Disps();
	for(i =0; i<50*160; i+=2)
	{
		vid[i]   = ' ';
		vid[i+1] = 0;
	}
	
    panic("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+\n");
	panic("                         REBOOT                        \n");
	panic("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+\n");
	WaitSecond(1);
	_CLI();
	idt_pointer.size = 0;
    idt_pointer.base = 0;	
    IDTLoad();
	_STI();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+