#ifndef _PORT_H__
#define _PORT_H__

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline unsigned char __Read8(unsigned short port)
{
    unsigned char result;
    __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline void __Write8(unsigned short port, unsigned char data)
{
    __asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline unsigned short __Read16(unsigned short port)
{
    unsigned short result;
    __asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline void __Write16(unsigned short port, unsigned short data)
{
    __asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (port));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline unsigned int __Read32(unsigned short port)
{
    unsigned int result;
    __asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline void __Write32(unsigned short port, unsigned int data)
{
    __asm__ volatile("outl %0, %1" : : "a" (data), "Nd" (port));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _PORT_8 {
	unsigned char(*Read)(unsigned short port);
	void(*Write)(unsigned short port, unsigned char data);
} __attribute__ ((packed))
PORT_8;

typedef struct _PORT_16 {
	unsigned short(*Read)(unsigned short port);
	void(*Write)(unsigned short port, unsigned short data);
} __attribute__ ((packed))
PORT_16;

typedef struct _PORT_32 {
	unsigned int(*Read)(unsigned short port);
	void(*Write)(unsigned short port, unsigned int data);
} __attribute__ ((packed))
PORT_32;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void Port_8_Register (PORT_8*  port);
void Port_16_Register(PORT_16* port);
void Port_32_Register(PORT_32* port);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _PORT__H__
