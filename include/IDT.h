#ifndef _IDT__H__
#define _IDT__H__

#include "LiBOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _REGS {
    UINT_32 gs, fs, es, ds;                          /* pushed the segs last */
    UINT_32 edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    UINT_32 int_no, err_code;                        /* our 'push byte #' and ecodes do this */
    UINT_32 eip, cs, eflags, useresp, ss;            /* pushed by the processor automatically */ 
} __attribute__((packed)) REGS;

typedef struct _GATEDESCRIPTOR {
    UINT_16 handlerAddressLowBits;
    UINT_16 gdt_codeSegmentSelector;
    UINT_8  reserved;
    UINT_8  access;
    UINT_16 handlerAddressHighBits;
} __attribute__((packed)) GATEDESCRIPTOR;

GATEDESCRIPTOR idt[256];

typedef struct _IDTPOINTER {
    UINT_16 size;
    UINT_32 base;
} __attribute__((packed)) IDTPOINTER;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void             IRQ_set_mask  (UINT_8 IRQline);
void             IRQ_clear_mask(UINT_8 IRQline);
void             __go_to_reset(void);
void             SetIDTEntry(UINT_8 Interrupt, UINT_32 HandlerAddress, UINT_16 CodeSegmentSelectorOffset, UINT_8 flags);
UINT_32          RegisterIDT(void);
void             FAULT_HANDLER(REGS* r);
void             IRQ_HANDLER(REGS* r);
void             __irq_install_handler(INT_32 irq, void (*handler)(REGS* r));
void             __irq_uninstall_handler(INT_32 irq);
void             __irq_remap(void);
extern void      disable_PIC(void);
extern void      set_MSR(UINT_32 base_addr, UINT_32 phys_addr_lo, UINT_32 phys_addr_hi);
extern UINT_32   IDTLoad ();
extern void ISR_DEFAULT();
extern void ISR1();
extern void ISR2();
extern void ISR3();
extern void ISR4();
extern void ISR0();
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
extern void IRQ16();
extern void _CLI();
extern void _STI();
extern void WM();

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _IDT__H__
