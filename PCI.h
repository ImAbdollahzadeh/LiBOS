#ifndef _PCI__H__
#define _PCI__H__
#include "../include/PORT.h"
#include "../include/AHCI.h"
#include "../include/XHCI.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define FNC_BAR_0 0x10
#define FNC_BAR_1 0x14
#define FNC_BAR_2 0x18
#define FNC_BAR_3 0x1C
#define FNC_BAR_4 0x20
#define FNC_BAR_5 0x24

typedef enum _BASEADDRESSREGISTERTYPE {
    MEMORYMAPPED = 0x00,
    INPUTOUTPUT  = 0x01
} BASEADDRESSREGISTERTYPE;
		
typedef struct _BASEADDRESSREGISTER {
	unsigned int            prefetchable;
	unsigned char*          address;
	unsigned int            size;
	BASEADDRESSREGISTERTYPE type;
} __attribute__ ((packed))
BASEADDRESSREGISTER, *PBASEADDRESSREGISTER;

typedef struct _PCIDESCRIPTOR {
	unsigned int   portBase;
	unsigned int   interrupt;
	unsigned short bus;
	unsigned short device;
	unsigned short function;
	unsigned short vendor_id;
	unsigned short device_id;
	unsigned char  class_id;
	unsigned char  subclass_id;
	unsigned char  interface_id;
	unsigned char  revision;
} __attribute__ ((packed))
PCIDESCRIPTOR, *PPCIDESCRIPTOR;

typedef struct _PCI {
	PORT_32 DataPort;
	PORT_32 CommandPort;
} __attribute__ ((packed))
PCI, *PPCI;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int        RegisterPCI           (PPCI pci, PSATA sata, XHCI* x);
unsigned int        Read                  (PPCI pci, unsigned short bus, unsigned short device, unsigned short function, unsigned int registeroffset);
void                Write                 (PPCI pci, unsigned short bus, unsigned short device, unsigned short function, unsigned int registeroffset, unsigned int value);
unsigned int        DeviceHasFunctions    (PPCI pci, unsigned short bus, unsigned short device);
BASEADDRESSREGISTER GetBASEADDRESSREGISTER(PPCI pci, unsigned short bus, unsigned short device, unsigned short function, unsigned short bar);
PCIDESCRIPTOR       GetPCIDESCRIPTOR      (PPCI pci, unsigned short bus, unsigned short device, unsigned short function);
void                SetBusMaster          (PPCI pci, unsigned short bus, unsigned short device, unsigned short function);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			
#endif //! _PCI__H__
