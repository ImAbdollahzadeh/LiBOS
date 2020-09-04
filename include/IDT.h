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

void             irq_set_mask          (UINT_8 irq_line);
void             irq_clear_mask        (UINT_8 irq_line);
void             go_to_reset           (void);
void             SetIDTEntry           (UINT_8 Interrupt, UINT_32 HandlerAddress, UINT_16 CodeSegmentSelectorOffset, UINT_8 flags);
UINT_32          RegisterIDT           (void);
void             FAULT_HANDLER         (REGS* r);
void             IRQ_HANDLER           (REGS* r);
void             irq_install_handler   (INT_32 irq, void (*handler)(REGS* r));
void             irq_uninstall_handler (INT_32 irq);
void             irq_remap             (void);
extern void      set_MSR               (UINT_32 base_addr, UINT_32 phys_addr_lo, UINT_32 phys_addr_hi);
extern void      get_MSR               (UINT_32 base_addr, UINT_32* addr_lo, UINT_32* addr_hi);
extern UINT_32   IDTLoad               (void);
extern void      set_irq_0x10_to_0xff  (UINT_8 irq);
extern void      query_cr2             (UINT_32* cr2);
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
extern void IRQ0(); // PIT timer
extern void IRQ1(); // PS/2 keyboard
extern void IRQ2();
extern void IRQ3();
extern void IRQ4();
extern void IRQ5();
extern void IRQ6();
extern void IRQ7(); // MASTER PIC spurious
extern void IRQ8();
extern void IRQ9();
extern void IRQ10();
extern void IRQ11(); // XHCI controller
extern void IRQ12();
extern void IRQ13();
extern void IRQ14();
extern void IRQ15(); // SLAVE PIC spurious
extern void IRQ16(); // APIC ERROR VECTOR
extern void IRQ17();
extern void IRQ18();
extern void IRQ19();
extern void IRQ20();
extern void IRQ21();
extern void IRQ22();
extern void IRQ23();
extern void IRQ24();
extern void IRQ25();
extern void IRQ26();
extern void IRQ27();
extern void IRQ28();
extern void IRQ29();
extern void IRQ30();
extern void IRQ31();
extern void IRQ32();
extern void IRQ33();
extern void IRQ34();
extern void IRQ35();
extern void IRQ36();
extern void IRQ37();
extern void IRQ38();
extern void IRQ39();
extern void IRQ40();
extern void IRQ41();
extern void IRQ42();
extern void IRQ43();
extern void IRQ44();
extern void IRQ45();
extern void IRQ46();
extern void IRQ47(); // APIC spurious
extern void IRQ48();
extern void IRQ49();
extern void IRQ40();
extern void IRQ41();
extern void IRQ42();
extern void IRQ43();
extern void IRQ44();
extern void IRQ45();
extern void IRQ46();
extern void IRQ47();
extern void IRQ48();
extern void IRQ49();
extern void IRQ50();
extern void IRQ51();
extern void IRQ52();
extern void IRQ53();
extern void IRQ54();
extern void IRQ55();
extern void IRQ56();
extern void IRQ57();
extern void IRQ58();
extern void IRQ59();
extern void IRQ60();
extern void IRQ61();
extern void IRQ62();
extern void IRQ63();
extern void IRQ64();
extern void IRQ65();
extern void IRQ66();
extern void IRQ67();
extern void IRQ68();
extern void IRQ69();
extern void IRQ70(); // BSP CPU (CPU 0) LAPIC timer
extern void IRQ71(); // AP  CPU (CPU 1) LAPIC timer
extern void IRQ72(); // AP  CPU (CPU 2) LAPIC timer
extern void IRQ73(); // AP  CPU (CPU 3) LAPIC timer
extern void IRQ74();
extern void IRQ75();
extern void IRQ76();
extern void IRQ77();
extern void IRQ78();
extern void IRQ79();
extern void IRQ80();
extern void IRQ81();
extern void IRQ82();
extern void IRQ83();
extern void IRQ84();
extern void IRQ85();
extern void IRQ86();
extern void IRQ87();
extern void IRQ88();
extern void IRQ89();
extern void IRQ90();
extern void IRQ91();
extern void IRQ92();
extern void IRQ93();
extern void IRQ94();
extern void IRQ95();
extern void IRQ96();
extern void IRQ97();
extern void IRQ98();
extern void IRQ99();
extern void IRQ100();
extern void IRQ101();
extern void IRQ102();
extern void IRQ103();
extern void IRQ104();
extern void IRQ105();
extern void IRQ106();
extern void IRQ107();
extern void IRQ108();
extern void IRQ109();
extern void IRQ110();
extern void IRQ111();
extern void IRQ112();
extern void IRQ113();
extern void IRQ114();
extern void IRQ115();
extern void IRQ116();
extern void IRQ117();
extern void IRQ118();
extern void IRQ119();
extern void IRQ120();
extern void IRQ121();
extern void IRQ122();
extern void IRQ123();
extern void IRQ124();
extern void IRQ125();
extern void IRQ126();
extern void IRQ127();
extern void IRQ128();
extern void IRQ129();
extern void IRQ130();
extern void IRQ131();
extern void IRQ132();
extern void IRQ133();
extern void IRQ134();
extern void IRQ135();
extern void IRQ136();
extern void IRQ137();
extern void IRQ138();
extern void IRQ139();
extern void IRQ140();
extern void IRQ141();
extern void IRQ142();
extern void IRQ143();
extern void IRQ144();
extern void IRQ145();
extern void IRQ146();
extern void IRQ147();
extern void IRQ148();
extern void IRQ149();
extern void IRQ140();
extern void IRQ141();
extern void IRQ142();
extern void IRQ143();
extern void IRQ144();
extern void IRQ145();
extern void IRQ146();
extern void IRQ147();
extern void IRQ148();
extern void IRQ149();
extern void IRQ150();
extern void IRQ151();
extern void IRQ152();
extern void IRQ153();
extern void IRQ154();
extern void IRQ155();
extern void IRQ156();
extern void IRQ157();
extern void IRQ158();
extern void IRQ159();
extern void IRQ160();
extern void IRQ161();
extern void IRQ162();
extern void IRQ163();
extern void IRQ164();
extern void IRQ165();
extern void IRQ166();
extern void IRQ167();
extern void IRQ168();
extern void IRQ169();
extern void IRQ170();
extern void IRQ171();
extern void IRQ172();
extern void IRQ173();
extern void IRQ174();
extern void IRQ175();
extern void IRQ176();
extern void IRQ177();
extern void IRQ178();
extern void IRQ179();
extern void IRQ180();
extern void IRQ181();
extern void IRQ182();
extern void IRQ183();
extern void IRQ184();
extern void IRQ185();
extern void IRQ186();
extern void IRQ187();
extern void IRQ188();
extern void IRQ189();
extern void IRQ190();
extern void IRQ191();
extern void IRQ192();
extern void IRQ193();
extern void IRQ194();
extern void IRQ195();
extern void IRQ196();
extern void IRQ197();
extern void IRQ198();
extern void IRQ199();
extern void IRQ200();
extern void IRQ201();
extern void IRQ202();
extern void IRQ203();
extern void IRQ204();
extern void IRQ205();
extern void IRQ206();
extern void IRQ207();
extern void IRQ208();
extern void IRQ209();
extern void IRQ210();
extern void IRQ211();
extern void IRQ212();
extern void IRQ213();
extern void IRQ214();
extern void IRQ215();
extern void IRQ216();
extern void IRQ217();
extern void IRQ218();
extern void IRQ219();
extern void IRQ220();
extern void IRQ221();
extern void IRQ222();
extern void IRQ223();

extern void _CLI();
extern void _STI();
extern void debug(void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _IDT__H__
