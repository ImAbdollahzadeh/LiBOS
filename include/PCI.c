#include "../include/PCI.h"
#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

#define DATAPORTVALUE    0xCFC
#define COMMANDPORTVALUE 0xCF8
#define  pciDEBUG        0

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ PCI SPACE PICTURE

/* when you enumerated and found a function (fnc):

fnc
    0x00                0x0F   0x10                0x1F   0x20                0x2F   0x30                0x3F
     ---- ---- ---- ----        ---- ---- ---- ----        ---- ---- ---- ----        ---- ---- ---- ----
    |d e s c r i p t o r|      |bar0|bar1|bar2|bar3|      |bar4|bar5|    |    |      |    |    |    |    |
     ---- ---- ---- ----        ---- ---- ---- ----        ---- ---- ---- ----        ---- ---- ---- ----  */

 
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 RegisterPCI(PCI* pci, SATA* sata, EHCI* ehci, XHCI* x)
{
	UINT_16 bus, device, function, barNum;
	
	if(!pci)
		return 0;
	
	Port_32_Register( &(pci->DataPort) );
	Port_32_Register( &(pci->CommandPort) );
	
	for(bus = 0; bus < 8; bus++)
	{
		for(device = 0; device < 32; device++)
		{
			INT_32 numFunctions = DeviceHasFunctions(pci, bus, device) ? 8 : 1;
			for(function = 0; function < numFunctions; function++)
			{
				PCIDESCRIPTOR dev = GetPCIDESCRIPTOR(pci, bus, device, function);
				if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
				    continue;     
				for(barNum = 0; barNum < 6; barNum++)
				{
					BASEADDRESSREGISTER bar = GetBASEADDRESSREGISTER(pci, bus, device, function, barNum);
					if(bar.address && (bar.type == INPUTOUTPUT))
						dev.portBase = (UINT_32)bar.address;
				}            

				if(dev.class_id == 0x01 && dev.subclass_id == 0x06)
				{
					UINT_32 BAR5 = Read(pci, bus, device, function, FNC_BAR_5);
					printk("    In PCI: AHCI SATA with Bar 5 = ^ found\n", BAR5);
					
					sata->valid            = 1;
					sata->bus              = bus;
					sata->device           = device;
					sata->bar5             = (void*)BAR5;
					sata->abar             = 0;
					sata->sata_port_number = 0;
					
					AhciBegin( sata );
				}
				if(dev.class_id == 0x0C && dev.subclass_id == 0x03 && dev.interface_id == 0x20)
				{
					//.UINT_32 BAR0 = Read(pci, bus, device, function, FNC_BAR_0);
					//.printk("    In PCI: EHCI USB with Bar 0 = ^ found\n", BAR0);

					if(
						((Read(pci, bus, device, function, 0) & 0xFFFF) == 0x8086) &&
						((Read(pci, bus, device, function, 2) & 0xFFFF) == 0x1E31) &&
						((Read(pci, bus, device, function, 8) & 0xFF)   == 4)
					)
					{
						if(pciDEBUG)
							printk("Panther Point controller detected !!\n");
						Write(pci, bus, device, function, 0xD8, 0xFFFFFFFF);
						Write(pci, bus, device, function, 0xD0, 0xFFFFFFFF);
					}
					//. SetBusMaster(pci, bus, device, function);                    
					//.ehci->bus         = bus;
					//.ehci->device      = device;
					//.ehci->function    = function;
					//.ehci->bar0        = (void*)BAR0;
					//.ehci->pci_pointer = (void*)pci;
					//.process_ehci( ehci );
				}
				if(dev.class_id == 0x0C && dev.subclass_id == 0x03 && dev.interface_id == 0x30)
				{
					__LiBOS_MemZero((void*)x, sizeof(XHCI));
					
					UINT_32 bar0 = (Read(pci, bus, device, function, FNC_BAR_0) & 0xFFFFFFF0);
					UINT_32 bar1 =  Read(pci, bus, device, function, FNC_BAR_1);
					x->base_address_lo = bar0;
					x->base_address_hi = bar1;
					
					UINT_32 headertype = (Read(pci, bus, device, function, 0x0C) & 0x00FF0000) >> 16;
					if(pciDEBUG)
						printk("xhci_HeaderType=%\n", headertype);
					
					x->bus      = bus;
					x->device   = device;
					x->function = function;
					x->pci      = (void*)pci;                                                        
					x->cap      = (xHCI_CAPABILITY*)                   x->base_address_lo; 
					x->oper     = (xHCI_OPERATION*)           ((void*)(x->base_address_lo + (UINT_32)x->cap->caplength)); 
					x->port_reg = (xHCI_PORT_REGISTER_SET*)   ((void*)((UINT_32)((void*)x->oper) + 0x0400));
					x->ex_cap   = 0;                    
					
					// set bus master
					UINT_32 pciCommandRegister = Read(pci, bus, device, function, 0x04);
					pciCommandRegister &= ~(BIT(2) | BIT(4));
					pciCommandRegister |=  (BIT(2) | BIT(4));
					Write(pci, bus, device, function, 0x04, pciCommandRegister);  

					x->irq_number   = (UINT_8)(Read(pci, bus, device, function, 0x3C) & 0xFF);
					
					xhci_start_controller(x);
				}
			    //printk("    In PCI: bus %, device(slot) %, function % = vendor %\n", (bus & 0xFF), (device & 0xFF), (function & 0xFF), ((dev.vendor_id & 0xFF00) >> 8));
			}
		}
	}
	//.printk( "        >>> PCI registered successfully <<<\n");
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 Read(PCI* pci, UINT_16 bus, UINT_16 device, UINT_16 function, UINT_32 registeroffset)
{
	UINT_32 id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	
	if(!pci)
	{
		panic("trying to read from an unregistered pci\n");
		return 0;
	}
	
	pci->CommandPort.Write( COMMANDPORTVALUE, id );
	UINT_32 result = pci->DataPort.Read( DATAPORTVALUE );
	return result >> (8 * (registeroffset % 4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void Write(PCI* pci, UINT_16 bus,UINT_16 device, UINT_16 function, UINT_32 registeroffset, UINT_32 value)
{
	UINT_32 id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	
	if(!pci)
	{
		panic("trying to write to an unregistered pci\n");
		return;
	}
	
	pci->CommandPort.Write( COMMANDPORTVALUE, id );
	pci->DataPort.Write   ( DATAPORTVALUE, value ); 
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 DeviceHasFunctions(PCI* pci, UINT_16 bus, UINT_16 device)
{
	if(!pci)
	{
		panic("trying to read from an unregistered pci\n");
		return 0;
	}
	return Read(pci, bus, device, 0, 0x0E) & (1<<7);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BASEADDRESSREGISTER GetBASEADDRESSREGISTER(PCI* pci, UINT_16 bus,UINT_16 device, UINT_16 function, UINT_16 bar)
{
	BASEADDRESSREGISTER baseaddressregister;
    
	if(!pci)
	{
		panic("trying to get base address of an unregistered pci\n");
		return baseaddressregister;
	} 
	
	UINT_32 headertype = Read(pci, bus, device, function, 0x0E) & 0x7F;
	int maxBARs = 6 - (4 * headertype);
	if(bar >= maxBARs)
		return baseaddressregister;    
	
	UINT_32 bar_value = Read(pci, bus, device, function, 0x10 + 4*bar);
	baseaddressregister.type = (bar_value & 0x1) ? INPUTOUTPUT : MEMORYMAPPED;
	
	if(baseaddressregister.type == MEMORYMAPPED)
	{
		switch((bar_value >> 1) & 0x3)
		{
			case 0: // 32 Bit Mode 
			case 1: // 20 Bit Mode
			case 2: // 64 Bit Mode
				break;
		}
	}
	else // INPUTOUTPUT
	{
	    baseaddressregister.address = (UINT_8*)(bar_value & ~0x3);
	    baseaddressregister.prefetchable = 0;
	}
	return baseaddressregister;
}
	
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

PCIDESCRIPTOR GetPCIDESCRIPTOR(PCI* pci, UINT_16 bus, UINT_16 device, UINT_16 function)
{
	PCIDESCRIPTOR result;
	
	if(!pci)
	{
		panic("trying to get PCI descriptor of an unregistered pci\n");
		result.bus          = 0;
		result.device       = 0;
		result.function     = 0;
		result.vendor_id    = 0;
		result.device_id    = 0;
		result.class_id     = 0;
		result.subclass_id  = 0;
		result.interface_id = 0;
		result.revision     = 0;
		result.interrupt    = 0;
		return result;
	}

	result.bus          = bus;
	result.device       = device;
	result.function     = function;
	result.vendor_id    = Read(pci, bus, device, function, 0x00);
	result.device_id    = Read(pci, bus, device, function, 0x02);
	result.class_id     = Read(pci, bus, device, function, 0x0b);
	result.subclass_id  = Read(pci, bus, device, function, 0x0a);
	result.interface_id = Read(pci, bus, device, function, 0x09);
	result.revision     = Read(pci, bus, device, function, 0x08);
	result.interrupt    = Read(pci, bus, device, function, 0x3c);
	return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void SetBusMaster(PCI* pci, UINT_16 bus, UINT_16 device, UINT_16 function)
{
    Write(pci, bus, device, function, 0x04, 0x06);
}