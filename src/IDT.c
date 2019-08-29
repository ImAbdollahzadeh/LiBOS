#include "../include/IDT.h"
#include "../include/PRINT.h"
#include "../include/PORT.h"
#include "../include/TIMER.h"
#include "../include/XHCI.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

void*      __irq_routines[256];
IDTPOINTER idt_pointer;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void SetIDTEntry(unsigned char Interrupt, unsigned long HandlerAddress, unsigned short CodeSegmentSelectorOffset, unsigned char flags)
{
	idt[Interrupt].handlerAddressLowBits   = (((unsigned int)HandlerAddress) & 0xFFFF);
    idt[Interrupt].handlerAddressHighBits  = (((unsigned int)HandlerAddress) >> 16) & 0xFFFF;
    idt[Interrupt].gdt_codeSegmentSelector = CodeSegmentSelectorOffset;
    idt[Interrupt].reserved                = 0;
    idt[Interrupt].access                  = flags;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int RegisterIDT(void)
{
	unsigned int status = 0, i;	
	
	for(i=255;i>0;i--)
	{
		SetIDTEntry(i, (unsigned long)(void*)ISR_DEFAULT,  0x08, 0x8E);
		__irq_routines[i] = 0;
	}
	    SetIDTEntry(0, (unsigned long)(void*)ISR_DEFAULT,  0x08, 0x8E);
	    __irq_routines[0] = 0;


    	SetIDTEntry(0,  (unsigned long)(void*)ISR0,  0x08, 0x8E);
    	SetIDTEntry(1,  (unsigned long)(void*)ISR1,  0x08, 0x8E);
    	SetIDTEntry(2,  (unsigned long)(void*)ISR2,  0x08, 0x8E);
    	SetIDTEntry(3,  (unsigned long)(void*)ISR3,  0x08, 0x8E);
    	SetIDTEntry(4,  (unsigned long)(void*)ISR4,  0x08, 0x8E);
   	 SetIDTEntry(5,  (unsigned long)(void*)ISR5,  0x08, 0x8E);
    	SetIDTEntry(6,  (unsigned long)(void*)ISR6,  0x08, 0x8E);
    	SetIDTEntry(7,  (unsigned long)(void*)ISR7,  0x08, 0x8E);
    	SetIDTEntry(8,  (unsigned long)(void*)ISR8,  0x08, 0x8E);
    	SetIDTEntry(9,  (unsigned long)(void*)ISR9,  0x08, 0x8E);
    	SetIDTEntry(10, (unsigned long)(void*)ISR10, 0x08, 0x8E);
    	SetIDTEntry(11, (unsigned long)(void*)ISR11, 0x08, 0x8E);
    	SetIDTEntry(12, (unsigned long)(void*)ISR12, 0x08, 0x8E);
    	SetIDTEntry(13, (unsigned long)(void*)ISR13, 0x08, 0x8E);
	SetIDTEntry(14, (unsigned long)(void*)ISR14, 0x08, 0x8E);
    	SetIDTEntry(15, (unsigned long)(void*)ISR15, 0x08, 0x8E);
    	SetIDTEntry(16, (unsigned long)(void*)ISR16, 0x08, 0x8E);
    	SetIDTEntry(17, (unsigned long)(void*)ISR17, 0x08, 0x8E);
	SetIDTEntry(18, (unsigned long)(void*)ISR18, 0x08, 0x8E);
    	SetIDTEntry(19, (unsigned long)(void*)ISR19, 0x08, 0x8E);
	SetIDTEntry(20, (unsigned long)(void*)ISR20, 0x08, 0x8E);
    	SetIDTEntry(21, (unsigned long)(void*)ISR21, 0x08, 0x8E);
    	SetIDTEntry(22, (unsigned long)(void*)ISR22, 0x08, 0x8E);
    	SetIDTEntry(23, (unsigned long)(void*)ISR23, 0x08, 0x8E);
	SetIDTEntry(24, (unsigned long)(void*)ISR24, 0x08, 0x8E);
    	SetIDTEntry(25, (unsigned long)(void*)ISR25, 0x08, 0x8E);
    	SetIDTEntry(26, (unsigned long)(void*)ISR26, 0x08, 0x8E);
    	SetIDTEntry(27, (unsigned long)(void*)ISR27, 0x08, 0x8E);
	SetIDTEntry(28, (unsigned long)(void*)ISR28, 0x08, 0x8E);
    	SetIDTEntry(29, (unsigned long)(void*)ISR29, 0x08, 0x8E);
	SetIDTEntry(30, (unsigned long)(void*)ISR30, 0x08, 0x8E);
    	SetIDTEntry(31, (unsigned long)(void*)ISR31, 0x08, 0x8E);
	
	//printk("    In IDT: 32 ISRs registered\n");

	__irq_remap();
    	SetIDTEntry(32, (unsigned long)(void*)IRQ0,  0x08, 0x8E);
	SetIDTEntry(33, (unsigned long)(void*)IRQ1,  0x08, 0x8E);
	SetIDTEntry(34, (unsigned long)(void*)IRQ2,  0x08, 0x8E);
	SetIDTEntry(35, (unsigned long)(void*)IRQ3,  0x08, 0x8E);
	SetIDTEntry(36, (unsigned long)(void*)IRQ4,  0x08, 0x8E);
	SetIDTEntry(37, (unsigned long)(void*)IRQ5,  0x08, 0x8E);
	SetIDTEntry(38, (unsigned long)(void*)IRQ6,  0x08, 0x8E);
	SetIDTEntry(39, (unsigned long)(void*)IRQ7,  0x08, 0x8E);
	SetIDTEntry(40, (unsigned long)(void*)IRQ8,  0x08, 0x8E);
	SetIDTEntry(41, (unsigned long)(void*)IRQ9,  0x08, 0x8E);
	SetIDTEntry(42, (unsigned long)(void*)IRQ10, 0x08, 0x8E);
	SetIDTEntry(43, (unsigned long)(void*)IRQ11, 0x08, 0x8E);
	SetIDTEntry(44, (unsigned long)(void*)IRQ12, 0x08, 0x8E);
	SetIDTEntry(45, (unsigned long)(void*)IRQ13, 0x08, 0x8E);
	SetIDTEntry(46, (unsigned long)(void*)IRQ14, 0x08, 0x8E);
    	SetIDTEntry(47, (unsigned long)(void*)IRQ15, 0x08, 0x8E);
	
	//printk("    In IDT: 16 IRQs registered\n");

	idt_pointer.size = (sizeof(GATEDESCRIPTOR) * 256) - 1;
    	idt_pointer.base = (unsigned int)((void*)(&idt[0]));
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
	
	//printk("    In IDT: IDT pointer created\n");
	
	//printk( "        >>> IDT registered successfully <<<\n");

	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

char* __exception_messages[32] = 
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

static unsigned short __pic_get_irq_reg(int ocw3)
{
    	PORT_8 p;
	Port_8_Register(&p);	
    	p.Write(PIC2_CMD, ocw3);
    	return (p.Read(PIC2_CMD) << 8);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned short pic_get_irr(void)
{
    return __pic_get_irq_reg(PIC_READ_IRR);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned short pic_get_isr(void)
{
    return __pic_get_irq_reg(PIC_READ_ISR);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void FAULT_HANDLER(PREGS r)
{
    if (r->int_no < 32)
    {
        panic(__exception_messages[r->int_no]);
        panic(" Exception. System Halted!\n");
        while(1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
unsigned int irq11_counter = 0;
void IRQ_HANDLER(PREGS r)
{
    	void (*handler)(PREGS r);
	PORT_8 p;
	Port_8_Register(&p);
	
    	handler = __irq_routines[r->int_no - 32];
    	if (handler)
    		handler(r);
	
	//if (r->int_no == 43 && irq11_counter == 0)
	//{
	//	_CLI();
	//	xhci_set_pending_status();
	//	irq11_counter++;
	//}
	
	/* IRQ8 - 15: need to send an EOI to the slave PIC */
    	if (r->int_no >= 40)
    		p.Write(0xA0, 0x20);
	
   	 /* In either case: the EOI to the master PIC */
    	p.Write(0x20, 0x20);
	
	//// this is before critical irq11
	//if (r->int_no == 43)
	//{
	//	printk("end of irq 11\n");
	//}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void IRQ_set_mask(unsigned char IRQline) 
{
    	unsigned short port;
    	unsigned char  value;
 
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

void IRQ_clear_mask(unsigned char IRQline) 
{
    	unsigned short port;
    	unsigned char  value;
 
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

void __irq_install_handler(int irq, void (*handler)(PREGS r))
{
	__irq_routines[irq] = handler;
	
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

void __irq_uninstall_handler(int irq)
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
	unsigned char* vid = Disps();
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

void CRITICAL_XHCI_HANDLER(void)
{
	printk("critical xhci handler1\n");
	xhci_clear_pending_status();
	printk("critical xhci handler2\n");
}
