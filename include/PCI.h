#ifndef _PCI__H__
#define _PCI__H__

#include "LiBOS_CORE.h"
#include "PORT.h"
#include "AHCI.h"
#include "EHCI.h"
#include "XHCI.h"

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
	UINT_32                 prefetchable;
	UINT_8*                 address;
	UINT_32                 size;
	BASEADDRESSREGISTERTYPE type;
} __attribute__ ((packed)) BASEADDRESSREGISTER;

typedef struct _PCIDESCRIPTOR {
	UINT_32 portBase;
	UINT_32 interrupt;
	UINT_16 bus;
	UINT_16 device;
	UINT_16 function;
	UINT_16 vendor_id;
	UINT_16 device_id;
	UINT_8  class_id;
	UINT_8  subclass_id;
	UINT_8  interface_id;
	UINT_8  revision;
} __attribute__ ((packed)) PCIDESCRIPTOR;

typedef struct _PCI {
	PORT_32 DataPort;
	PORT_32 CommandPort;
} __attribute__ ((packed)) PCI;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32             RegisterPCI           (PCI* pci, SATA* sata, EHCI* ehci, XHCI* x);
UINT_32             Read                  (PCI* pci, UINT_16 bus, UINT_16 device, UINT_16 function, UINT_32 registeroffset);
void                Write                 (PCI* pci, UINT_16 bus, UINT_16 device, UINT_16 function, UINT_32 registeroffset, UINT_32 value);
UINT_32             DeviceHasFunctions    (PCI* pci, UINT_16 bus, UINT_16 device);
BASEADDRESSREGISTER GetBASEADDRESSREGISTER(PCI* pci, UINT_16 bus, UINT_16 device, UINT_16 function, UINT_16 bar);
PCIDESCRIPTOR       GetPCIDESCRIPTOR      (PCI* pci, UINT_16 bus, UINT_16 device, UINT_16 function);
void                SetBusMaster          (PCI* pci, UINT_16 bus, UINT_16 device, UINT_16 function);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			
#endif //! _PCI__H__
