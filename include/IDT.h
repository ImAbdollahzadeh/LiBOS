#ifndef _IDT__H__
#define _IDT__H__

#include "LiBOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _REGS {
	UINT_32 gs;        /* pushed the segs last */
	UINT_32 fs;        /* pushed the segs last */
	UINT_32 es;        /* pushed the segs last */
	UINT_32 ds;        /* pushed the segs last */
	UINT_32 edi;       /* pushed by 'pusha' */
	UINT_32 esi;       /* pushed by 'pusha' */
	UINT_32 ebp;       /* pushed by 'pusha' */
	UINT_32 esp;       /* pushed by 'pusha' */
	UINT_32 ebx;       /* pushed by 'pusha' */
	UINT_32 edx;       /* pushed by 'pusha' */
	UINT_32 ecx;       /* pushed by 'pusha' */
	UINT_32 eax;       /* pushed by 'pusha' */
	UINT_32 int_no;    /* 'push byte #' */
	UINT_32 err_code;  /* 'push byte #' */
	UINT_32 eip;       /* pushed by the processor automatically */
	UINT_32 cs;        /* pushed by the processor automatically */
	UINT_32 eflags;    /* pushed by the processor automatically */
	UINT_32 user_esp;  /* pushed by the processor automatically */
	UINT_32 ss;        /* pushed by the processor automatically */
} __attribute__ ((packed)) REGS;

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

       void      irq_set_mask             (UINT_8 irq_line);
       void      irq_clear_mask           (UINT_8 irq_line);
       void      go_to_reset              (void);
       void      SetIDTEntry              (UINT_8 Interrupt, UINT_32 HandlerAddress, UINT_16 CodeSegmentSelectorOffset, UINT_8 flags);
       UINT_32   RegisterIDT              (void);
       void      FAULT_HANDLER            (REGS* r);
       void      IRQ_HANDLER              (REGS* r);
       void      irq_install_handler      (INT_32 irq, void (*handler)(REGS* r));
       void      irq_install_user_handler (void (*handler)(REGS* r)); // only 0x80 reserved for usermode
       void      irq_uninstall_handler    (INT_32 irq);
       void      irq_remap                (void);
extern void      set_MSR                  (UINT_32 base_addr, UINT_32 phys_addr_lo, UINT_32 phys_addr_hi);
extern void      get_MSR                  (UINT_32 base_addr, UINT_32* addr_lo, UINT_32* addr_hi);
extern UINT_32   IDTLoad                  (void);
extern void      set_irq_0x10_to_0xff     (UINT_8 irq);
extern void      query_cr2                (UINT_32* cr2);
extern void      ISR_DEFAULT           (void);
extern void      ISR1                  (void);
extern void      ISR2                  (void);
extern void      ISR3                  (void);
extern void      ISR4                  (void);
extern void      ISR0                  (void);
extern void      ISR5                  (void);
extern void      ISR6                  (void);
extern void      ISR7                  (void);
extern void      ISR8                  (void);
extern void      ISR9                  (void);
extern void      ISR10                 (void);
extern void      ISR11                 (void);
extern void      ISR12                 (void);
extern void      ISR13                 (void);
extern void      ISR14                 (void);
extern void      ISR15                 (void);
extern void      ISR16                 (void);
extern void      ISR17                 (void);
extern void      ISR18                 (void);
extern void      ISR19                 (void);
extern void      ISR20                 (void);
extern void      ISR21                 (void);
extern void      ISR22                 (void);
extern void      ISR23                 (void);
extern void      ISR24                 (void);
extern void      ISR25                 (void);
extern void      ISR26                 (void);
extern void      ISR27                 (void);
extern void      ISR28                 (void);
extern void      ISR29                 (void);
extern void      ISR30                 (void);
extern void      ISR31                 (void);
extern void      IRQ0                  (void); // PIT timer
extern void      IRQ1                  (void); // PS/2 keyboard
extern void      IRQ2                  (void);
extern void      IRQ3                  (void);
extern void      IRQ4                  (void);
extern void      IRQ5                  (void);
extern void      IRQ6                  (void);
extern void      IRQ7                  (void); // MASTER PIC spurious
extern void      IRQ8                  (void);
extern void      IRQ9                  (void);
extern void      IRQ10                 (void);
extern void      IRQ11                 (void); // XHCI controller
extern void      IRQ12                 (void);
extern void      IRQ13                 (void);
extern void      IRQ14                 (void);
extern void      IRQ15                 (void); // SLAVE PIC spurious
extern void      IRQ16                 (void); // APIC ERROR VECTOR
extern void      IRQ17                 (void);
extern void      IRQ18                 (void);
extern void      IRQ19                 (void);
extern void      IRQ20                 (void);
extern void      IRQ21                 (void);
extern void      IRQ22                 (void);
extern void      IRQ23                 (void);
extern void      IRQ24                 (void);
extern void      IRQ25                 (void);
extern void      IRQ26                 (void);
extern void      IRQ27                 (void);
extern void      IRQ28                 (void);
extern void      IRQ29                 (void);
extern void      IRQ30                 (void);
extern void      IRQ31                 (void);
extern void      IRQ32                 (void);
extern void      IRQ33                 (void);
extern void      IRQ34                 (void);
extern void      IRQ35                 (void);
extern void      IRQ36                 (void);
extern void      IRQ37                 (void);
extern void      IRQ38                 (void);
extern void      IRQ39                 (void);
extern void      IRQ40                 (void);
extern void      IRQ41                 (void);
extern void      IRQ42                 (void);
extern void      IRQ43                 (void);
extern void      IRQ44                 (void);
extern void      IRQ45                 (void);
extern void      IRQ46                 (void);
extern void      IRQ47                 (void); // APIC spurious
extern void      IRQ48                 (void);
extern void      IRQ49                 (void);
extern void      IRQ40                 (void);
extern void      IRQ41                 (void);
extern void      IRQ42                 (void);
extern void      IRQ43                 (void);
extern void      IRQ44                 (void);
extern void      IRQ45                 (void);
extern void      IRQ46                 (void);
extern void      IRQ47                 (void);
extern void      IRQ48                 (void);
extern void      IRQ49                 (void);
extern void      IRQ50                 (void);
extern void      IRQ51                 (void);
extern void      IRQ52                 (void);
extern void      IRQ53                 (void);
extern void      IRQ54                 (void);
extern void      IRQ55                 (void);
extern void      IRQ56                 (void);
extern void      IRQ57                 (void);
extern void      IRQ58                 (void);
extern void      IRQ59                 (void);
extern void      IRQ60                 (void);
extern void      IRQ61                 (void);
extern void      IRQ62                 (void);
extern void      IRQ63                 (void);
extern void      IRQ64                 (void);
extern void      IRQ65                 (void);
extern void      IRQ66                 (void);
extern void      IRQ67                 (void);
extern void      IRQ68                 (void);
extern void      IRQ69                 (void);
extern void      IRQ70                 (void); // BSP CPU (CPU 0) LAPIC timer
extern void      IRQ71                 (void); // AP  CPU (CPU 1) LAPIC timer
extern void      IRQ72                 (void); // AP  CPU (CPU 2) LAPIC timer
extern void      IRQ73                 (void); // AP  CPU (CPU 3) LAPIC timer
extern void      IRQ74                 (void);
extern void      IRQ75                 (void);
extern void      IRQ76                 (void);
extern void      IRQ77                 (void);
extern void      IRQ78                 (void);
extern void      IRQ79                 (void);
extern void      IRQ80                 (void);
extern void      IRQ81                 (void);
extern void      IRQ82                 (void);
extern void      IRQ83                 (void);
extern void      IRQ84                 (void);
extern void      IRQ85                 (void);
extern void      IRQ86                 (void);
extern void      IRQ87                 (void);
extern void      IRQ88                 (void);
extern void      IRQ89                 (void);
extern void      IRQ90                 (void);
extern void      IRQ91                 (void);
extern void      IRQ92                 (void);
extern void      IRQ93                 (void);
extern void      IRQ94                 (void);
extern void      IRQ95                 (void);
extern void      IRQ96                 (void);
extern void      IRQ97                 (void);
extern void      IRQ98                 (void);
extern void      IRQ99                 (void);
extern void      IRQ100                (void);
extern void      IRQ101                (void);
extern void      IRQ102                (void);
extern void      IRQ103                (void);
extern void      IRQ104                (void);
extern void      IRQ105                (void);
extern void      IRQ106                (void);
extern void      IRQ107                (void);
extern void      IRQ108                (void);
extern void      IRQ109                (void);
extern void      IRQ110                (void);
extern void      IRQ111                (void);
extern void      IRQ112                (void);
extern void      IRQ113                (void);
extern void      IRQ114                (void);
extern void      IRQ115                (void);
extern void      IRQ116                (void);
extern void      IRQ117                (void);
extern void      IRQ118                (void);
extern void      IRQ119                (void);
extern void      IRQ120                (void);
extern void      IRQ121                (void);
extern void      IRQ122                (void);
extern void      IRQ123                (void);
extern void      IRQ124                (void);
extern void      IRQ125                (void);
extern void      IRQ126                (void);
extern void      IRQ127                (void);
extern void      IRQ128                (void);
extern void      IRQ129                (void);
extern void      IRQ130                (void);
extern void      IRQ131                (void);
extern void      IRQ132                (void);
extern void      IRQ133                (void);
extern void      IRQ134                (void);
extern void      IRQ135                (void);
extern void      IRQ136                (void);
extern void      IRQ137                (void);
extern void      IRQ138                (void);
extern void      IRQ139                (void);
extern void      IRQ140                (void);
extern void      IRQ141                (void);
extern void      IRQ142                (void);
extern void      IRQ143                (void);
extern void      IRQ144                (void);
extern void      IRQ145                (void);
extern void      IRQ146                (void);
extern void      IRQ147                (void);
extern void      IRQ148                (void);
extern void      IRQ149                (void);
extern void      IRQ140                (void);
extern void      IRQ141                (void);
extern void      IRQ142                (void);
extern void      IRQ143                (void);
extern void      IRQ144                (void);
extern void      IRQ145                (void);
extern void      IRQ146                (void);
extern void      IRQ147                (void);
extern void      IRQ148                (void);
extern void      IRQ149                (void);
extern void      IRQ150                (void);
extern void      IRQ151                (void);
extern void      IRQ152                (void);
extern void      IRQ153                (void);
extern void      IRQ154                (void);
extern void      IRQ155                (void);
extern void      IRQ156                (void);
extern void      IRQ157                (void);
extern void      IRQ158                (void);
extern void      IRQ159                (void);
extern void      IRQ160                (void);
extern void      IRQ161                (void);
extern void      IRQ162                (void);
extern void      IRQ163                (void);
extern void      IRQ164                (void);
extern void      IRQ165                (void);
extern void      IRQ166                (void);
extern void      IRQ167                (void);
extern void      IRQ168                (void);
extern void      IRQ169                (void);
extern void      IRQ170                (void);
extern void      IRQ171                (void);
extern void      IRQ172                (void);
extern void      IRQ173                (void);
extern void      IRQ174                (void);
extern void      IRQ175                (void);
extern void      IRQ176                (void);
extern void      IRQ177                (void);
extern void      IRQ178                (void);
extern void      IRQ179                (void);
extern void      IRQ180                (void);
extern void      IRQ181                (void);
extern void      IRQ182                (void);
extern void      IRQ183                (void);
extern void      IRQ184                (void);
extern void      IRQ185                (void);
extern void      IRQ186                (void);
extern void      IRQ187                (void);
extern void      IRQ188                (void);
extern void      IRQ189                (void);
extern void      IRQ190                (void);
extern void      IRQ191                (void);
extern void      IRQ192                (void);
extern void      IRQ193                (void);
extern void      IRQ194                (void);
extern void      IRQ195                (void);
extern void      IRQ196                (void);
extern void      IRQ197                (void);
extern void      IRQ198                (void);
extern void      IRQ199                (void);
extern void      IRQ200                (void);
extern void      IRQ201                (void);
extern void      IRQ202                (void);
extern void      IRQ203                (void);
extern void      IRQ204                (void);
extern void      IRQ205                (void);
extern void      IRQ206                (void);
extern void      IRQ207                (void);
extern void      IRQ208                (void);
extern void      IRQ209                (void);
extern void      IRQ210                (void);
extern void      IRQ211                (void);
extern void      IRQ212                (void);
extern void      IRQ213                (void);
extern void      IRQ214                (void);
extern void      IRQ215                (void);
extern void      IRQ216                (void);
extern void      IRQ217                (void);
extern void      IRQ218                (void);
extern void      IRQ219                (void);
extern void      IRQ220                (void);
extern void      IRQ221                (void);
extern void      IRQ222                (void);
extern void      IRQ223                (void);
extern void      _CLI                  (void);
extern void      _STI                  (void);
extern void      debug                 (void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _IDT__H__
