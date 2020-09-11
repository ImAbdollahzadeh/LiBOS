#ifndef _GDT__H__
#define _GDT__H__

#include "LiBOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _SEGMENTDESCRIPTOR {
	UINT_16 limit_low;
	UINT_16 base_low;
	UINT_8  base_middle;
	UINT_8  access;
	UINT_8  granularity;
	UINT_8  base_high;
} __attribute__ ((packed)) SEGMENTDESCRIPTOR;

typedef struct _GDT {
	SEGMENTDESCRIPTOR NullSegmentSelector;
	SEGMENTDESCRIPTOR CodeSegmentSelector;
	SEGMENTDESCRIPTOR DataSegmentSelector;
	SEGMENTDESCRIPTOR CodeSegmentSelector16;
	SEGMENTDESCRIPTOR DataSegmentSelector16;
	SEGMENTDESCRIPTOR UserCodeSegmentSelector;
	SEGMENTDESCRIPTOR UserDataSegmentSelector;
	SEGMENTDESCRIPTOR TSS_BSP;
	SEGMENTDESCRIPTOR TSS_AP1;
	SEGMENTDESCRIPTOR TSS_AP2;
	SEGMENTDESCRIPTOR TSS_AP3;
} __attribute__ ((packed)) GDT;

typedef struct _GDTPOINTER {
    UINT_16 size;
    UINT_32 base;
} __attribute__((packed)) GDTPOINTER;

typedef struct _TSS {
	UINT_32 prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
	UINT_32 esp0;       // The stack pointer to load when we change to kernel mode.
	UINT_32 ss0;        // The stack segment to load when we change to kernel mode.
	UINT_32 esp1;       // everything below here is unused now. 
	UINT_32 ss1;
	UINT_32 esp2;
	UINT_32 ss2;
	UINT_32 cr3;
	UINT_32 eip;
	UINT_32 eflags;
	UINT_32 eax;
	UINT_32 ecx;
	UINT_32 edx;
	UINT_32 ebx;
	UINT_32 esp;
	UINT_32 ebp;
	UINT_32 esi;
	UINT_32 edi;
	UINT_32 es;         
	UINT_32 cs;        
	UINT_32 ss;        
	UINT_32 ds;        
	UINT_32 fs;       
	UINT_32 gs;         
	UINT_32 ldt;      
	UINT_16 trap;
	UINT_16 iomap_base;
} __attribute__ ((packed)) TSS;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern UINT_32 get_kernel_stack      (void);
extern void    activate_tss0         (void);
extern void    activate_tss1         (void);
extern void    activate_tss2         (void);
extern void    activate_tss3         (void);
extern UINT_32 GDTLoad               (void);
       UINT_32 SegmentDescriptorBase (SEGMENTDESCRIPTOR* segmentdescriptor);
       UINT_32 SegmentDescriptorLimit(SEGMENTDESCRIPTOR* segmentdescriptor);
       UINT_32 RegisterGDT           (GDT* gdt);
       UINT_16 GDTCodeSegmentSelector(GDT* gdt);
       UINT_16 GDTDataSegmentSelector(GDT* gdt);
       void    set_TSS_esp           (UINT_32 esp);
       UINT_32 get_TSS_esp           (void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _GDT__H__
