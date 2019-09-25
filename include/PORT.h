#ifndef _PORT_H__
#define _PORT_H__

#include "LiBOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline UINT_8 __Read8(UINT_16 port)
{
    UINT_8 result;
    __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline void __Write8(UINT_16 port, UINT_8 data)
{
    __asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline UINT_16 __Read16(UINT_16 port)
{
    UINT_16 result;
    __asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline void __Write16(UINT_16 port, UINT_16 data)
{
    __asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (port));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline UINT_32 __Read32(UINT_16 port)
{
    UINT_32 result;
    __asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline void __Write32(UINT_16 port, UINT_32 data)
{
    __asm__ volatile("outl %0, %1" : : "a" (data), "Nd" (port));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _PORT_8 {
	UINT_8(*Read)(UINT_16 port);
	void(*Write)(UINT_16 port, UINT_8 data);
} __attribute__ ((packed)) PORT_8;

typedef struct _PORT_16 {
	UINT_16(*Read)(UINT_16 port);
	void(*Write)(UINT_16 port, UINT_16 data);
} __attribute__ ((packed)) PORT_16;

typedef struct _PORT_32 {
	UINT_32(*Read)(UINT_16 port);
	void(*Write)(UINT_16 port, UINT_32 data);
} __attribute__ ((packed)) PORT_32;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void Port_8_Register (PORT_8*  port);
void Port_16_Register(PORT_16* port);
void Port_32_Register(PORT_32* port);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _PORT__H__
