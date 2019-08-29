#ifndef _IDT__H__
#define _IDT__H__

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _REGS {
    unsigned int gs, fs, es, ds;                          /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;                        /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;            /* pushed by the processor automatically */ 
} __attribute__((packed))
REGS, *PREGS;

typedef struct _GATEDESCRIPTOR {
    unsigned short handlerAddressLowBits;
    unsigned short gdt_codeSegmentSelector;
    unsigned char  reserved;
    unsigned char  access;
    unsigned short handlerAddressHighBits;
} __attribute__((packed))
GATEDESCRIPTOR, *PGATEDESCRIPTOR;

GATEDESCRIPTOR idt[256];

typedef struct _IDTPOINTER {
    unsigned short size;
    unsigned int   base;
} __attribute__((packed))
IDTPOINTER;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void __go_to_reset(void);
void SetIDTEntry(unsigned char Interrupt, unsigned long HandlerAddress, unsigned short CodeSegmentSelectorOffset, unsigned char flags);
unsigned int RegisterIDT(void);
void FAULT_HANDLER(PREGS r);
void IRQ_HANDLER(PREGS r);
void __irq_install_handler(int irq, void (*handler)(PREGS r));
void __irq_uninstall_handler(int irq);
void __irq_remap(void);
extern unsigned int IDTLoad();
extern void ISR_DEFAULT();
extern void ISR0();
extern void ISR1();
extern void ISR2();
extern void ISR3();
extern void ISR4();
extern void ISR5();
extern void ISR6();
extern void ISR7();
extern void ISR8();
extern void ISR9();
extern void ISR10();
extern void ISR11();
extern void ISR12();
extern void ISR13();
extern void ISR14();
extern void ISR15();
extern void ISR16();
extern void ISR17();
extern void ISR18();
extern void ISR19();
extern void ISR20();
extern void ISR21();
extern void ISR22();
extern void ISR23();
extern void ISR24();
extern void ISR25();
extern void ISR26();
extern void ISR27();
extern void ISR28();
extern void ISR29();
extern void ISR30();
extern void ISR31();
extern void IRQ0();
extern void IRQ1();
extern void IRQ2();
extern void IRQ3();
extern void IRQ4();
extern void IRQ5();
extern void IRQ6();
extern void IRQ7();
extern void IRQ8();
extern void IRQ9();
extern void IRQ10();
extern void IRQ11();
extern void IRQ12();
extern void IRQ13();
extern void IRQ14();
extern void IRQ15();

extern void _CLI();
extern void _STI();
void set_lapic_to_idt(unsigned int*);
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _IDT__H__
