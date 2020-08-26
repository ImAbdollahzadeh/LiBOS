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
} __attribute__ ((packed)) GDT;

typedef struct _GDTPOINTER {
    UINT_16 size;
    UINT_32 base;
} __attribute__((packed)) GDTPOINTER;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern UINT_32 GDTLoad               (void);
       UINT_32 SegmentDescriptorBase (SEGMENTDESCRIPTOR* segmentdescriptor);
       UINT_32 SegmentDescriptorLimit(SEGMENTDESCRIPTOR* segmentdescriptor);
       UINT_32 RegisterGDT           (GDT* gdt);
       UINT_16 GDTCodeSegmentSelector(GDT* gdt);
       UINT_16 GDTDataSegmentSelector(GDT* gdt);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _GDT__H__
