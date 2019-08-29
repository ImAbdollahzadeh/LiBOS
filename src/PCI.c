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

void __IMOS_PCI_MemZero(void* mem, unsigned int bytes)
{
    unsigned int        blocks = bytes >> 3;
    unsigned char       remain = bytes &  7;
    unsigned long long* dst    = (unsigned long long*)mem; 
    unsigned char*      ddst   = 0; 
    while(blocks--) { *dst++ = 0ULL; }
    ddst = (unsigned char*)dst;
    while(remain--) { *ddst++ = 0; } 
}

unsigned int RegisterPCI(PPCI pci, PSATA sata, XHCI* x)
{
	unsigned short bus, device, function, barNum;
	
	if(!pci)
		return 0;
	
	Port_32_Register( &(pci->DataPort) );
	Port_32_Register( &(pci->CommandPort) );
	
	for(bus = 0; bus < 8; bus++)
    {
        for(device = 0; device < 32; device++)
        {
            int numFunctions = DeviceHasFunctions(pci, bus, device) ? 8 : 1;
            for(function = 0; function < numFunctions; function++)
            {
                PCIDESCRIPTOR dev = GetPCIDESCRIPTOR(pci, bus, device, function);
                if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
                    continue;     
                for(barNum = 0; barNum < 6; barNum++)
                {
                    BASEADDRESSREGISTER bar = GetBASEADDRESSREGISTER(pci, bus, device, function, barNum);
                    if(bar.address && (bar.type == INPUTOUTPUT))
                        dev.portBase = (unsigned int)bar.address;
                }            
				
				if(dev.class_id == 0x01 && dev.subclass_id == 0x06)
				{
            	}
				if(dev.class_id == 0x0C && dev.subclass_id == 0x03 && dev.interface_id == 0x20)
				{
            	}
                if(dev.class_id == 0x0C && dev.subclass_id == 0x03 && dev.interface_id == 0x30)
				{
                    __IMOS_PCI_MemZero((void*)x, sizeof(XHCI));
                    unsigned int bar0 = (Read(pci, bus, device, function, FNC_BAR_0) & 0xFFFFFFF0);
                    unsigned int bar1 =  Read(pci, bus, device, function, FNC_BAR_1);
                    x->base_address_lo = (void*)bar0;
	                x->base_address_hi = (void*)bar1;
                    
                    unsigned int headertype = (Read(pci, bus, device, function, 0x0C) & 0x00FF0000) >> 16;
                    //printk("HeaderType=%\n", headertype);
                    
                    x->bus      = bus;
                    x->device   = device;
                    x->function = function;
                    x->pci      = (void*)pci;
                                                        
                    x->cap          = (PxHCI_CAPABILITY)                           x->base_address_lo; 
                    x->oper         = (PxHCI_OPERATION)           ((void*)((bit32u)x->base_address_lo + (bit32u)x->cap->caplength)); 
                    x->port_reg_set = (PxHCI_PORT_REGISTER_SET)   ((void*)((bit32u)x->oper + 0x0400));
                    x->ext_cap      = 0;                    

                    // set bus master
                    bit32u pciCommandRegister = Read(pci, bus, device, function, 0x04);
                    pciCommandRegister &= ~(BIT(2) | BIT(4) );
                    pciCommandRegister |=  (BIT(2) | BIT(4));
                    Write(pci, bus, device, function, 0x04, pciCommandRegister);                    

                    x->irq_number = (unsigned char)(Read(pci, bus, device, function, 0x3C));
                    
					start_xhci_controller(x);
                }
                
				//printk("    In PCI: bus %, device(slot) %, function % = vendor %\n", (bus & 0xFF), (device & 0xFF), (function & 0xFF), ((dev.vendor_id & 0xFF00) >> 8));
            }
        }
    }
	//printk( "        >>> PCI registered successfully <<<\n");
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int Read(PPCI pci, unsigned short bus, unsigned short device, unsigned short function, unsigned int registeroffset)
{
	unsigned int id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	
	if(!pci)
	{
		panic("trying to read from an unregistered pci\n");
		return 0;
	}
	
    pci->CommandPort.Write( COMMANDPORTVALUE, id );
    unsigned int result = pci->DataPort.Read( DATAPORTVALUE );
    return result >> (8 * (registeroffset % 4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void Write(PPCI pci, unsigned short bus,unsigned short device, unsigned short function, unsigned int registeroffset, unsigned int value)
{
	unsigned int id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (registeroffset & 0xFC);
	
	if(!pci)
	{
		panic("trying to write to an unregistered pci\n");
		return;
	}
	
    pci->CommandPort.Write( COMMANDPORTVALUE, id );
    pci->DataPort.Write   ( DATAPORTVALUE, value ); 
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int DeviceHasFunctions(PPCI pci, unsigned short bus, unsigned short device)
{
	if(!pci)
	{
		panic("trying to read from an unregistered pci\n");
		return 0;
	}	
	return Read(pci, bus, device, 0, 0x0E) & (1<<7);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BASEADDRESSREGISTER GetBASEADDRESSREGISTER(PPCI pci, unsigned short bus,unsigned short device, unsigned short function, unsigned short bar)
{
	BASEADDRESSREGISTER baseaddressregister;
    
	if(!pci)
	{
		panic("trying to get base address of an unregistered pci\n");
		return baseaddressregister;
	} 
	
    unsigned int headertype = Read(pci, bus, device, function, 0x0E) & 0x7F;
    int maxBARs = 6 - (4 * headertype);
    if(bar >= maxBARs)
        return baseaddressregister;    
    
    unsigned int bar_value = Read(pci, bus, device, function, 0x10 + 4*bar);
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
        baseaddressregister.address = (unsigned char*)(bar_value & ~0x3);
        baseaddressregister.prefetchable = 0;
    }
	return baseaddressregister;
}
	
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

PCIDESCRIPTOR GetPCIDESCRIPTOR(PPCI pci, unsigned short bus, unsigned short device, unsigned short function)
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

void SetBusMaster(PPCI pci, unsigned short bus, unsigned short device, unsigned short function)
{
    Write(pci, bus, device, function, 0x04, 0x06);
}