#include "../include/XHCI.h"
#include "../include/PCI.h"
#include "../include/PRINT.h"
#include "../include/TIMER.h"
#include "../include/MEMORY.h"
#include "../include/IDT.h"
#include "../include/PORT.h"
bit32u prot = 0;
xHCI_SLOT_CONTEXT g_slot;
xHCI_EP_CONTEXT g_ep;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

#define xDEBUG 0
#define xINTERRUPT_HANDLER_DEBUG 1
#define PHYS_ADDRESS(addr) (bit32u)((void*)(addr))
extern void CHECK_DS                        (unsigned int* ds);
       void xhci_interrupt_handler          (PREGS r);
       void handle_device_attachment_to_port(XHCI* x, bit32u port);
XHCI* xx                 = 0x00;
bit8u usb_sockets_status = 0x00;
bit8u sockets_flags      = 0x00;
bit8u speed_flags        = 0x00;
bit8u slot_ep_flags      = 0x00;

bit8u critical_event = 0x00;

static xHCI_SLOT_CONTEXT slot;

static TRB_TYPES trb_types[64] =  {
	{0, "Reserved"},
	{1, "Normal"}, 
    {2, "Setup Stage"},
    {3, "Data Stage"},
    {4, "Status Stage"},
    {5, "Isochronous"},
    {6, "Link"},
    {7, "Event Data"},
    {8, "No-Op"},
    {9, "Enable Slot"},
    {10, "Disable Slot"},
    {11, "Address Device"},
    {12, "Configure Endpoint"},
    {13, "Evaluate Context"},
    {14, "Reset Endpoint"},
    {15, "Stop Endpoint"},
    {16, "Set TR Dequeue Pointer"},
    {17, "Reset Device"},
    {18, "Force Event"},
    {19, "Negotiate Bandwidth"},
    {20, "Set Latency Tolerance Value"},
    {21, "Get Port Bandwidth"},
    {22, "Force Header"},
    {23, "No-Op Command"},
    {24, "Reserved"},
    {25, "Reserved"},
    {26, "Reserved"},
    {27, "Reserved"},
    {28, "Reserved"},
    {29, "Reserved"},
    {30, "Reserved"},
    {31, "Reserved"},
    {32, "Transfer Event"},
    {33, "Command Completion Event"},
    {34, "Port Status Change Event"},
    {35, "Bandwidth Request Event"},
    {36, "Doorbell Event"},
    {37, "Host Controller Event"},
    {38, "Device Notification Event"},
    {39, "MFINDEX Wrap Event"},
    {40, "Reserved"},
    {41, "Reserved"},
    {42, "Reserved"},
    {43, "Reserved"},
    {44, "Reserved"},
    {45, "Reserved"},
    {46, "Reserved"},
    {47, "Reserved"},
    {48, "Vendor Defined"},
    {49, "Vendor Defined"},
    {50, "Vendor Defined"},
    {51, "Vendor Defined"},
    {52, "Vendor Defined"},
    {53, "Vendor Defined"},
    {54, "Vendor Defined"},
    {55, "Vendor Defined"},
    {56, "Vendor Defined"},
    {57, "Vendor Defined"},
    {58, "Vendor Defined"},
    {59, "Vendor Defined"},
    {60, "Vendor Defined"},
    {61, "Vendor Defined"},
    {62, "Vendor Defined"},
    {63, "Vendor Defined"}
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void __IMOS_XHCI_MemZero(void* mem, unsigned int bytes)
{
    unsigned int        blocks = bytes >> 3;
    unsigned char       remain = bytes &  7;
    unsigned long long* dst    = (unsigned long long*)mem; 
    unsigned char*      ddst   = 0; 
    while(blocks--) { *dst++ = 0ULL; }
    ddst = (unsigned char*)dst;
    while(remain--) { *ddst++ = 0; } 
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_print_on_screen(XHCI* x)
{
	printk("XHCI \n");
	printk("  base address(high)(low) = (^)(^)\n", x->base_address_hi, x->base_address_lo);
	printk("  caplength               = ^\n", (unsigned int)x->cap->caplength);
	printk("  hciversion              = ^\n", (unsigned int)x->cap->hciversion);
	printk("  hcsparam1               = ^\n", x->cap->hcsparams1);
	printk("      no. slot            = ^\n", (unsigned int)(x->cap->hcsparams1 & 0xFF));
	printk("      no. interrupter     = ^\n", (unsigned int)((x->cap->hcsparams1 & 0x7FF00)>>8));
	printk("      no. port            = ^\n", (unsigned int)((x->cap->hcsparams1 & 0xFF000000)>>24));
	printk("  hcsparam2               = ^\n", x->cap->hcsparams2);
	printk("  hcsparam3               = ^\n", x->cap->hcsparams3);
	printk("  hccparam1               = ^\n", x->cap->hccparams1);
    printk("      use %-byte context data structure\n", ((x->cap->hccparams1 & 0x4)>>2) == 0 ? 32: 64);
	printk("      each port is powered "); printk(((x->cap->hccparams1 & 0x8)>>3 == 0) ? "always\n": "its own\n");
	printk("  dboff                   = ^\n", x->cap->dboff);
	printk("  rtsoff                  = ^\n", x->cap->rtsoff);
	printk("  hccparam2               = ^\n", x->cap->hccparams2);
	
	printk("  usbcmd                  = ^\n", x->oper->usbcmd);
	printk("  usbsts                  = ^\n", x->oper->usbsts);
	printk("  pagesize                = %\n", ((x->oper->pagesize & 0xFFFF) << 12));
	printk("  dnctrl                  = ^\n", x->oper->dnctrl);
	printk("  dcbaap(high)(low)       = (^)(^)\n", (unsigned int)((x->oper->dcbaap) >> 32), (unsigned int)(x->oper->dcbaap));
	printk("  crcr  (high)(low)       = (^)(^)\n", (unsigned int)((x->oper->crcr)   >> 32), (unsigned int)(x->oper->crcr));
	printk("  xhci irq number         = %\n", (unsigned int)x->irq_number);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bit8u port_speed(bit16u portSpeed)
{
	unsigned char tag = 0;
	switch (portSpeed)
	{
		case 0:
			printk("undefined\n");
			break;
		case 1:
			printk("full-speed - 12MiB/s - USB 2.0\n");
			tag=1;
			break;
		case 2:
			printk("low-speed - 1.5Mib/s - USB 2.0\n");
			tag=2;
			break;
		case 3:
			printk("high-speed - 480Mib/s - USB 2.0\n");
			tag=3;
			break;
		case 4:
			printk("super-speed - 5Gib/s - USB 3.0\n");
			tag=4;
			break;
	}
	return tag;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void read_port_register_set(XHCI* x, bit32u port, bit8u* spd)
{
	bit8u  maxPort = (bit8u)((x->cap->hcsparams1 & 0xFF000000)>>24);
	bit16u speed;
	xHCI_PORT_REGISTER_SET* prs = (xHCI_PORT_REGISTER_SET*)((void*)((bit32u)((void*)x->port_reg) + (16 * port)));
	if(port > maxPort)
	{
		printk("the requested port number is greater than supported maxPort oh XHCI\n");
		return;
	}
	speed = (bit16u)(((prs->portsc) & xPORT_SPD) >> 10);
	//.printk("*** port % ***\n", port);
	//.printk("  portsc    = ^\n", prs->portsc);

	port_speed(speed);
	*spd = (bit8u)speed;
    
	//.printk("  portpmsc  = ^\n", prs->portpmsc);
	//.printk("  portli    = ^\n", prs->portli);
	//.printk("  porthlpmc = ^\n", prs->porthlpmc);
	
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bool reset_port(XHCI* x, bit32u port) 
{
    	xHCI_PORT_REGISTER_SET* tmp_prs = x->port_reg;
    	xHCI_PORT_REGISTER_SET* prs     =  (xHCI_PORT_REGISTER_SET*)((void*)((bit32u)((void*)tmp_prs) + (16 * port)));
	volatile bit32u val;
	
	// power the port?
    	prs->portsc |= xPORT_POW;
	WaitMiliSecond(20);
	if ( (prs->portsc & xPORT_POW) == 0 ) 
	{
	  	prs->portsc |= xPORT_POW;
	  	WaitMiliSecond(20);
	  	if ( (prs->portsc & xPORT_POW) == 0 )
		{
	    	printk("port % bad reset\n", port);
			return FALSE;
		}
	}
	
	// we need to make sure that the status change bits are clear
	prs->portsc |= xPORT_CHANGE_BITS;
	
	// set bit 4 (USB2) or 31 (USB3) to reset the port
	prs->portsc |= xPORT_RESET;

	// wait for bit 21 to set
	bit32u timeout = 500;
    
    	/* 	control the "usb_sockets_status" to make sure we have no device connected so far
       		if so, there is no need for reset, and we must clear the bit */
    	bit8u bit = (bit8u)port;
    	if( usb_sockets_status & BIT(bit) )
    	{
        	usb_sockets_status &= ~BIT(bit);
        	printk("Device at port % disconnected\n", port);
        	return FALSE;
    	}
    	usb_sockets_status |= BIT(bit);
    
	while (timeout) 
	{
		val = prs->portsc;
	  	if (val & xPORT_PRC)
	    	break;
		timeout--;
		WaitMiliSecond(1);
	}
	
	if (timeout > 0) 
	{
		// reset recovery time
		WaitMiliSecond(3);
		
		// if after the reset, the enable bit is non zero, there was a successful reset/enable
		val = prs->portsc;
		if (val & xPORT_ED) 
        {
			// clear the status change bit(s)
			prs->portsc |= xPORT_CHANGE_BITS;
            goto RESET_DONE;   
        }
	} 
	else
		goto TI_TUSB7340_QUIRK;
	
TI_TUSB7340_QUIRK:
    prs->portsc |= (xPORT_CHANGE_BITS | xPORT_WRC);
	prs->portsc |= xPORT_RESET;
	timeout = 500;
	while (timeout) 
	{
		val = prs->portsc;
	  	if (val & xPORT_PRC)
	    	break;
		timeout--;
		WaitMiliSecond(1);
	}
	if (timeout > 0) 
	{
		WaitMiliSecond(3);
		val = prs->portsc;
		if (val & xPORT_ED)
			prs->portsc |= (xPORT_CHANGE_BITS | xPORT_WRC);
	} 
	else
	{
        printk("port % reset timed out\n", port);
		return FALSE;
	}
    
RESET_DONE: 
    printk("Device at port % connected\n", port);
	//.printk("port % reset \n", port);
    return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int cnt = 0;
void read_extended_capabilities(XHCI* x)
{
	bit8u  maxPort                    = (bit8u)  ((x->cap->hcsparams1 & 0xFF000000) >> 24);
    bit32u extended_capability_offset = (bit32u)(((x->cap->hccparams1 & 0xFFFF0000) >> 16) * 4);
	xHCI_EXTENDED_CAPABILITY* ex_cap  = (xHCI_EXTENDED_CAPABILITY*)((void*)(x->base_address_lo + extended_capability_offset));
	if(ex_cap == 0)
	{
		printk("error: zero xECP\n");
		return;
	}
	bit32u next = 0, i = 0, k = 0, entries = 0;
	
	char str[5];
	for(i=0;i<5;i++)
		str[i] = 0;
	
	while(entries < maxPort)
	{
		if((bit8u)(ex_cap->DWORD0 & 0xFF) == 0x01)
			printk("found xECP with ID = 0x01 @ entry=%\n", entries);
		
		//printk("xECP ID=% @ entry=%\n", (unsigned int)( (bit8u)(ex_cap->DWORD0 & 0xFF) ), entries);
		if((bit8u)(ex_cap->DWORD0 & 0xFF) == 0x02)
		{
			//printk("found xECP with ID = 0x02 @ entry=%\n", entries);
			for(i=0;i<4;i++)
				str[i] = (char)((ex_cap->DWORD1 >> (i*8)) & 0xFF);
			
			//printk("protocol = ");
			//printk((char*)str);
			//printk("%.% -> ", (unsigned int)(((ex_cap->DWORD0)&0xFF000000)>>24), (unsigned int)(((ex_cap->DWORD0)&0x00FF0000)>>16));
			//printk("% ports @ offset %\n", (unsigned int)(((ex_cap->DWORD2)&0x0000FF00)>>8), (unsigned int)(((ex_cap->DWORD2)&0x000000FF)));
			
			if(cnt == 0) {
				prot = ex_cap->DWORD3 & 0x0F;
				//.printk("protocol: ^\n", prot);
				cnt++;
			}
				
			bit8u portCount = ((ex_cap->DWORD2)&0x0000FF00)>>8;
			bit8u offset    = (ex_cap->DWORD2)&0x000000FF;
			bit8u usb2_or_usb3 = ((ex_cap->DWORD0)&0xFF000000)>>24;
			bit32u ports_usb2 = 0;
			bit32u ports_usb3 = 0;
		}
		
		if(((ex_cap->DWORD0 & 0xFF00)>>8) == 0)
		{
			//printk("end of xECP list reached @ entry=%\n", entries);
			return;
		}		
		next = 4*((ex_cap->DWORD0 & 0xFF00)>>8);
		ex_cap = (xHCI_EXTENDED_CAPABILITY*)((void*)((bit32u)((void*)ex_cap) + next));
		entries++;
	}
	
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.bool __stop_legacy_mode(XHCI* x, unsigned int entry)
//.{
//.	unsigned int i = 0, next = 0;
//.	PxHCI_EXTENDED_CAPABILITY ext_cap = x->ext_cap;
//.	
//.	while(i != entry) 
//.	{
//.		next = 4*((ext_cap->DWORD0 & 0xFF00)>>8);
//.		ext_cap = (PxHCI_EXTENDED_CAPABILITY)((void*)((bit32u)ext_cap + next));
//.		i++;
//.	}
//.	
//.	bit32u bios = *(bit32u*)((void*)ext_cap);
//.	bios       |= xHC_xECP_LEGACY_OS_OWNED;
//.	*(bit32u*)((void*)ext_cap) = bios;
//.
//.	unsigned int timeout = 50;
//.	while (timeout--) 
//.	{
//.		if ((*(bit32u*)((void*)ext_cap) & xHC_xECP_LEGACY_OWNED_MASK) == xHC_xECP_LEGACY_OS_OWNED)
//.			return TRUE;
//.		WaitMiliSecond(1);
//.	}
//.	return FALSE;
//.}
//.
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_get_trb(xHCI_TRB* trb, bit32u address) 
{
	trb->param   = (bit64u)(mread(address, 0x00));
	trb->status  =          mread(address, 0x08);
	trb->command =          mread(address, 0x0C);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void xhci_set_trb(xHCI_TRB* trb, bit32u address) 
//.{
//.	mwrite(address, 0, (bit32u)trb->param);
//.	mwrite(address,  8, trb->status);
//.	mwrite(address, 12, trb->command);
//.}
//.
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_write_doorbell(XHCI* x, bit32u slot_id, bit32u val) 
{
	volatile bit32u doorbell = x->base_address_lo + x->cap->dboff;
    mwrite(doorbell, (slot_id * 4), val);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

volatile bit32u signal_from_noop_to_irq                     = 0;
volatile bit32u signal_from_irq_to_noop                     = 0;
volatile bit32u signal_from_irq_to_slot_configuration       = 0;
volatile bit32u signal_from_slot_configuration_to_irq       = 0;
volatile bit32u signal_from_slot_set_address_to_irq         = 0;
volatile bit32u signal_from_irq_to_slot_set_address         = 0;
volatile bit32u signal_from_setup_data_status_stages_to_irq = 0;
volatile bit32u signal_from_irq_to_setup_data_status_stages = 0;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void NO_OP_test(XHCI* x)
{
	xHCI_TRB trb = { .param = 0ULL, .status = 0, .command = 0x00005C00 };
    
	mwrite(x->command_ring_Enqueue, 0x00, trb.param); 
	mwrite(x->command_ring_Enqueue, 0x04, 0x00);  
	mwrite(x->command_ring_Enqueue, 0x08, trb.status);                       
	mwrite(x->command_ring_Enqueue, 0x0C, (trb.command | x->command_ring_PCS));
	x->command_ring_Enqueue += sizeof(xHCI_TRB);

	bit32u cmnd = mread(x->command_ring_Enqueue, 0x0C);
	if (TRB_GET_TYPE(cmnd) == xLINK) 
	{
		mwrite(x->command_ring_Enqueue, 0x0C, ((cmnd & ~1) | x->command_ring_PCS));
		x->command_ring_Enqueue = x->driver_crcr;
		x->command_ring_PCS ^= 1;
	}
    
	signal_from_noop_to_irq = 1;
	xhci_write_doorbell(x, 0, 0);
	
   //unsigned int timer = 5000;
	//while (timer) 
	//{
	//	if(signal_from_irq_to_noop == 1)
	//	{
	//		signal_from_irq_to_noop = 0;
	//		signal_from_noop_to_irq = 0;
	//		break;
	//	}
	//	WaitMiliSecond(1);
	//	timer--;
	//}
	//if(timer == 0)
	//	printk("NOOP time out\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bool start_xhci_controller(XHCI* x)
{
	bit32u i;
    	bit32u base = x->base_address_lo;
    	xx = x;                  

	x->usb_device->connected         = FALSE;
	x->usb_device->speed             = 0;
	x->usb_device->slot_configuration = FALSE;
	x->usb_device->slot_configuration_command = FALSE;
	x->usb_device->slot_set_address = FALSE;
	x->usb_device->slot_id           = 255;
	x->usb_device->slot_address      = 0;
	x->usb_device->ep0_address       = 0;
	
	unsigned int timeout = 50;
    	bit32u cmd = mread(PHYS_ADDRESS(x->oper), 0x00);
    	cmd |= ~(xCMD_RUN);
	mwrite(PHYS_ADDRESS(x->oper), 0x00, cmd);	
    	while((mread(PHYS_ADDRESS(x->oper), 0x04) & xSTS_HCH) == 0)
	{
		if(timeout == 0)
		{
			printk("xHC did not stop in 50 msec\n");
			return FALSE;
		}
		WaitMiliSecond(1);
		timeout--;
	}              
    
	timeout = 1000;
    	cmd = mread(PHYS_ADDRESS(x->oper), 0x00);
    	cmd |= xCMD_RESET;
	mwrite(PHYS_ADDRESS(x->oper), 0x00, cmd);	
	while((mread(PHYS_ADDRESS(x->oper), 0x00) & xCMD_RESET) != 0)
	{
		if(timeout == 0)
		{
			printk("xHC did not reset in 1 sec\n");
			return FALSE;
		}
		WaitMiliSecond(1);
		timeout--;
	}
    
    	timeout = 1000;
	while((mread(PHYS_ADDRESS(x->oper), 0x04) & xSTS_CNR) != 0)
	{
		if(timeout == 0)
		{
			printk("xHC is not ready after 1 sec\n");
			return FALSE;
		}
		WaitMiliSecond(1);
		timeout--;
	}
    
    	// PANTHER POINT SWITCH
    	if(
    	    ((Read((PCI*)x->pci, x->bus, x->device, x->function, 0) & 0xFFFF) == 0x8086) &&
    	    ((Read((PCI*)x->pci, x->bus, x->device, x->function, 2) & 0xFFFF) == 0x1E31) &&
    	    ((Read((PCI*)x->pci, x->bus, x->device, x->function, 8) & 0xFF)   == 4)
    	)
    	{
    	    if(xDEBUG)
    	        printk("Panther Point controller detected !!\n");
    	    Write((PCI*)x->pci, x->bus, x->device, x->function, 0xD8, 0xFFFFFFFF);
    	    Write((PCI*)x->pci, x->bus, x->device, x->function, 0xD0, 0xFFFFFFFF);
    	}
    
    	// set FLADJ
    	bit32u fladj = Read((PCI*)x->pci, x->bus, x->device, x->function, 0x61);
    	fladj &= ~0xFF;
    	fladj |= 0x20;
    	Write((PCI*)x->pci, x->bus, x->device, x->function, 0x61, fladj);

	// set bus master
    	bit32u pciCommandRegister = Read((PCI*)x->pci, x->bus, x->device, x->function, 0x04);
    	pciCommandRegister &= ~(BIT(2) | BIT(4));
    	pciCommandRegister |=  (BIT(2) | BIT(4));
    	Write((PCI*)x->pci, x->bus, x->device, x->function, 0x04, pciCommandRegister);  
	
	if(xDEBUG)
        printk("XHCI reset successful\n");

	x->ex_cap          = (((x->cap->hccparams1 & 0xFFFF0000) >> 16) * 4);
	x->context_size    = (x->cap->hccparams1 & BIT(2)) ? 64 : 32;
	x->page_size       = (x->oper->pagesize & 0x0000FFFF) << 12;

	bit32u maxSlots    = (x->cap->hcsparams1 & 0x000000FF);

	// Device Notification Control (only bit 1 is allowed)
	mwrite(PHYS_ADDRESS(x->oper), 0x14, BIT(1));
	
    	// Configure Register
	mwrite(PHYS_ADDRESS(x->oper), 0x38, maxSlots);
    
    	// allocate the dcbaa, crcr and the slot contexts.
    	void* mdcbaap = xhci_alloc_memory(1024, 64, x->page_size);
	void* mcrcr   = xhci_alloc_memory((CMND_RING_TRBS * sizeof(xHCI_TRB)), 64, 65536);
	__IMOS_XHCI_MemZero(mdcbaap, 1024);               
	__IMOS_XHCI_MemZero(mcrcr, CMND_RING_TRBS * sizeof(xHCI_TRB));
	 
	// setting up the scratchpad buffer
	bit8u MaxScratchpadBuffers = ((x->cap->hcsparams2 >> 27) & 0x1F) | ((x->cap->hcsparams2 >> 16) & 0xE0);
	if(MaxScratchpadBuffers > 0)
	{
		if(xDEBUG)
			printk("scratchpad buffers number = %\n", (unsigned int)MaxScratchpadBuffers);
		
		bit64u* ScratchpadBuffersPtr = (bit64u*)(xhci_alloc_memory(248, 64, x->page_size));
		__IMOS_XHCI_MemZero((void*)ScratchpadBuffersPtr, 248);		   		   
		for(i=0; i<MaxScratchpadBuffers; i++)
		{
			bit32u tmp = (bit32u)( xhci_alloc_memory(x->page_size, x->page_size, x->page_size) );
			__IMOS_XHCI_MemZero((void*)tmp, x->page_size);
			ScratchpadBuffersPtr[i] = (bit64u)tmp;
		}
		
		mwrite((bit32u)mdcbaap, 0x00, (bit32u)((void*)ScratchpadBuffersPtr));	
	}
    
    	// setting up device contexts to maxSlot  
	for(i=1; i<(maxSlots+1); i++)
	{
		//void* DevCntxSlots = xhci_alloc_memory(32 * x->context_size, 64, x->page_size);
		//__IMOS_XHCI_MemZero(DevCntxSlots, 32 * x->context_size);
        //mwrite((bit32u)mdcbaap, (0x08 * i), (bit32u)DevCntxSlots);
		mwrite((bit32u)mdcbaap, (0x08 * i), 0);
	}	
    
	// 64bit write
	mwrite(PHYS_ADDRESS(x->oper), 0x30, (bit32u)mdcbaap);	
	mwrite(PHYS_ADDRESS(x->oper), 0x34, 0x00);

	//command ring Link TRB
	bit32u pos = (bit32u)mcrcr + ((CMND_RING_TRBS - 1) * sizeof(xHCI_TRB));
	mwrite(pos, 0x00, (bit32u)mcrcr);                 // param
	mwrite(pos, 0x04, 0);
	mwrite(pos, 0x08, 0);                             // status
	mwrite(pos, 0x0C, TRB_LINK_CMND);                 // command 
	
	// 64bit write
	mwrite(PHYS_ADDRESS(x->oper), 0x18, ((bit32u)mcrcr | TRB_CYCLE_ON));
    	mwrite(PHYS_ADDRESS(x->oper), 0x1C, 0x00);
	x->driver_crcr             = (bit32u)mcrcr;
    	x->command_ring_Enqueue    = (bit32u)mcrcr;
    	x->command_ring_PCS        = TRB_CYCLE_ON;
        
	if(xDEBUG)
	{
		xhci_print_on_screen(x);
        //printk("base:^, cap:^, op:^, capl:^\n", x->base, x->cap_base, x->oper, x->caplength);
		//printk("rtsoff:^, dboff:^, hcsp1:^, hcsp2:^\n", x->runtime, x->doorbell, x->hcsparams1, x->hcsparams2);
		//printk("hccp1:^, hccp2:^\n", x->hccparams1, x->hccparams2);
		//printk("pgsz:^, cntxsz:^, extcap:^, portoff:^\n", x->page_size, x->context_size, x->ext_cap, x->port_reg_set);
		//printk("dcbaap:^, crcr:^\n", x->dcbaap, x->crcr);
		//printk("reading back crcr:^, irq:%\n", pr_crcr, x->irq);
	}

	// Initialize the interrupters
	x->ers                  = xhci_alloc_memory(256 * sizeof(xHCI_TRB), 64, 65536);
	x->erst                 = xhci_alloc_memory(sizeof(xHCI_EVENT_SEG_TABLE), 64, 65536);
	x->current_event_ring_address  = (bit32u)x->ers;
	__IMOS_XHCI_MemZero(x->ers,  256 * sizeof(xHCI_TRB));
	__IMOS_XHCI_MemZero(x->erst, sizeof(xHCI_EVENT_SEG_TABLE));
    
	xHCI_EVENT_SEG_TABLE* segment_table = (xHCI_EVENT_SEG_TABLE*)x->erst;
	segment_table->addr                 = (bit64u)(0ULL | (bit32u)x->ers);
	segment_table->size                 = 256;
	segment_table->resv                 = 0;
	x->current_event_ring_cycle         = 1;
	
	volatile bit32u interrupter0 = base + x->cap->rtsoff + 0x20;
        
	if(xDEBUG)
		printk("pri. interrupter:^\n", interrupter0); 
    
    	mwrite(interrupter0, xHC_INTERRUPTER_IMAN,       (xHC_IMAN_IE | xHC_IMAN_IP)); 
	mwrite(interrupter0, xHC_INTERRUPTER_ERSTSZ,     1); 
    	mwrite(interrupter0, xHC_INTERRUPTER_ERSTBA,     (bit32u)x->erst); 
	mwrite(interrupter0, xHC_INTERRUPTER_ERSTBA + 4, 0);
	mwrite(interrupter0, xHC_INTERRUPTER_ERDP,       ((bit32u)segment_table->addr | BIT(3))); 
	mwrite(interrupter0, xHC_INTERRUPTER_ERDP + 4,   0); 
    	mwrite(interrupter0, xHC_INTERRUPTER_IMOD,       4000); 
    	WaitMiliSecond(100);
        
    cmd = 0;
    cmd = mread(PHYS_ADDRESS(x->oper), 0x00);
    cmd |= xCMD_INTE;
    mwrite(PHYS_ADDRESS(x->oper), 0x00, cmd);
    WaitMiliSecond(100);
    
    cmd = 0;
    cmd = mread(PHYS_ADDRESS(x->oper), 0x00);
    cmd |= xCMD_HSEE;
    mwrite(PHYS_ADDRESS(x->oper), 0x00, cmd);
    WaitMiliSecond(100);
    
    // clear the status register
    bit32u st = mread(PHYS_ADDRESS(x->oper), 4);
    st |= (BIT(2) | BIT(3) | BIT(4) | BIT(10));
    mwrite(PHYS_ADDRESS(x->oper), 4, st); 
    
    	// start the interrupter
	__irq_install_handler(x->irq_number, &xhci_interrupt_handler);
    
	// start the controller
    	cmd = 0;
	cmd = mread(PHYS_ADDRESS(x->oper), 0x00);
	cmd |= xCMD_RUN;   
	mwrite(PHYS_ADDRESS(x->oper), 0x00, cmd);
	WaitMiliSecond(100);
	
	timeout = 50;
	while((mread(PHYS_ADDRESS(x->oper), 0x04) & xSTS_HCH) != 0)
	{
		if(timeout == 0)
		{
			printk("xHC did not started in 50 ms\n");
			return FALSE;
		}
		WaitMiliSecond(1);
		timeout--;
	}

    	timeout = 1000;
	while((mread(PHYS_ADDRESS(x->oper), 0x04) & xSTS_CNR) != 0)
	{
		if(timeout == 0)
		{
			printk("xHC is not ready after 1 sec\n");
			return FALSE;
		}
		WaitMiliSecond(1);
		timeout--;
	}
	
	read_extended_capabilities(x);

    	//NO_OP_test(x);
    
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void* xhci_alloc_memory(bit32u byte, bit32u alignment, bit32u page_boundary)
{
	return Alloc(byte, alignment, page_boundary);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_free_memory(void* pointer)
{
	Free(pointer);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void report_commmand_completion_trb(xHCI_TRB* trb)
{
    printk("command completion TRB report\n");
    printk("  command trb pointer low: ^, command trb pointer high:^\n", (unsigned int)(trb->param & 0xFFFFFFF0), (unsigned int)(trb->param >> 32));
    printk("  completion code:%\n", (unsigned int)((trb->status & 0xFF000000) >> 24));
    printk("  status:^\n", trb->status);
    printk("  cycle bit:%\n", (unsigned int)(trb->command & BIT(0)));
    printk("  TRB type is ");
    bit32u trb_type_number = (bit32u)((trb->command & 0xFC00) >> 10);
    printk(trb_types[trb_type_number].message_type);
    printk(" with id number %\n", trb_type_number);
    printk("  Slot id: %\n", (unsigned int)(trb->command >> 24));
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_interrupt_handler(PREGS r)
{   
    	volatile bit32u sts           = 0;
    	volatile bit32u timeout       = 2000;
    	volatile bit32u base          = xx->base_address_lo;
   	volatile bit32u interrupter0  = base + xx->cap->rtsoff + 0x20;
	volatile bit32u iman          = mread(interrupter0, xHC_INTERRUPTER_IMAN);
    	volatile bit32u driver_status = 0;
    
    	if((iman & 3) != 3)
        	return;
    	if(xINTERRUPT_HANDLER_DEBUG) printk("****************************************\n");    
	    while(TRUE)
	    {
		sts = mread(PHYS_ADDRESS(xx->oper), 0x04);
		if(sts != 0)
		{
		    if(xINTERRUPT_HANDLER_DEBUG) printk("interrupt 0: sts:^, IMAN:^\n", sts, iman);
		    driver_status = sts;
		    mwrite(PHYS_ADDRESS(xx->oper), 0x04, sts); 
			WaitMiliSecond(500);
		    iman |= (xHC_IMAN_IE | xHC_IMAN_IP);
		    mwrite(interrupter0, xHC_INTERRUPTER_IMAN, iman);
		    WaitMiliSecond(100);
		    break;
		}
		WaitMiliSecond(1);
		timeout--;
		if(timeout == 0)
		    break;
	    }
    
	// do the work
	xHCI_TRB trb;
    	xhci_get_trb(&trb, xx->current_event_ring_address);
    	if(xDEBUG) report_commmand_completion_trb(&trb);
    
	sts = mread(PHYS_ADDRESS(xx->oper), 0x04);
	iman  = mread(interrupter0, xHC_INTERRUPTER_IMAN);
    	mwrite(PHYS_ADDRESS(xx->oper), 0x04, sts);
    	mwrite(interrupter0, xHC_INTERRUPTER_IMAN, (iman | (xHC_IMAN_IE | xHC_IMAN_IP)));
	if(xINTERRUPT_HANDLER_DEBUG) printk("interrupt 1: sts:^, IMAN:^\n", sts, iman);
	
	// Did we detected a device attach?
    if(driver_status & xSTS_PCD)
    {
		// which port? get it from trb's PORT_ID (a.k.a. bits 24-31 of DWORD0). The reported port is one-based number i.e. 1, 2, 3 and should be converted to 0, 1, 2
        	bit32u attached_port = ((bit32u)trb.param) >> 24;
        	attached_port--;
        	if(xINTERRUPT_HANDLER_DEBUG) printk("Port % activated\n", attached_port);  
        	handle_device_attachment_to_port(xx, attached_port);

		sts = mread(PHYS_ADDRESS(xx->oper), 0x04);
		iman  = mread(interrupter0, xHC_INTERRUPTER_IMAN);
    		mwrite(PHYS_ADDRESS(xx->oper), 0x04, sts);
    		mwrite(interrupter0, xHC_INTERRUPTER_IMAN, (iman | (xHC_IMAN_IE | xHC_IMAN_IP)));
		if(xINTERRUPT_HANDLER_DEBUG) printk("interrupt 2: sts:^, IMAN:^\n", sts, iman);

		xx->current_event_ring_address += sizeof(xHCI_TRB);
    		mwrite(interrupter0, xHC_INTERRUPTER_ERDP, (xx->current_event_ring_address | BIT(3))); 
		mwrite(interrupter0, xHC_INTERRUPTER_ERDP + 4, 0);
		
		sts = mread(PHYS_ADDRESS(xx->oper), 0x04);
		iman  = mread(interrupter0, xHC_INTERRUPTER_IMAN);
    		mwrite(PHYS_ADDRESS(xx->oper), 0x04, sts);
    		mwrite(interrupter0, xHC_INTERRUPTER_IMAN, (iman | (xHC_IMAN_IE | xHC_IMAN_IP)));
		if(xINTERRUPT_HANDLER_DEBUG) printk("interrupt 3: sts:^, IMAN:^\n", sts, iman);

		xx->usb_device->slot_configuration_command = TRUE;
		// for disconnection
		if((sts == 0x00) && (iman == 0x02))
		{
			xx->usb_device->port      = 0;
			xx->usb_device->speed     = 0;
			xx->usb_device->connected = FALSE;
			xx->usb_device->slot_id   = 255;
			critical_event = 0;
			goto XHCI_INTERRUPT_HANDLER_END;
		}		
		xx->current_event_ring_address += sizeof(xHCI_TRB);
    		mwrite(interrupter0, xHC_INTERRUPTER_ERDP, (xx->current_event_ring_address | BIT(3))); 
		mwrite(interrupter0, xHC_INTERRUPTER_ERDP + 4, 0); 
		
		sts = mread(PHYS_ADDRESS(xx->oper), 0x04);
		iman  = mread(interrupter0, xHC_INTERRUPTER_IMAN);
    		mwrite(PHYS_ADDRESS(xx->oper), 0x04, sts);
    		mwrite(interrupter0, xHC_INTERRUPTER_IMAN, (iman | (xHC_IMAN_IE | xHC_IMAN_IP)));
		if(xINTERRUPT_HANDLER_DEBUG) printk("interrupt 4: sts:^, IMAN:^\n", sts, iman);
		critical_event = 1; // keep it for slot_configuration set
    	}
	else if((driver_status & xSTS_EINT))
	{
		if(signal_from_slot_configuration_to_irq == 1)
		{
			signal_from_irq_to_slot_configuration = 1;
			xx->usb_device->slot_id = (bit8u)(trb.command >> 24);
			critical_event = 0; // keep it for slot_set_address set
		}
		
		else if(signal_from_slot_set_address_to_irq == 1)
		{
			signal_from_irq_to_slot_set_address = 1;
			critical_event = 0;
		}
		
		else if(signal_from_noop_to_irq == 1)
		{
			signal_from_irq_to_noop = 1;
			critical_event = 0;
		}
		
		else if(signal_from_setup_data_status_stages_to_irq == 1)
		{
			signal_from_irq_to_setup_data_status_stages = 1;
			critical_event = 0;
		}
		
		xhci_get_trb(&trb, xx->current_event_ring_address);
        	report_commmand_completion_trb(&trb);		
		
		xx->current_event_ring_address += sizeof(xHCI_TRB);
    		mwrite(interrupter0, xHC_INTERRUPTER_ERDP, (xx->current_event_ring_address | BIT(3))); 
		mwrite(interrupter0, xHC_INTERRUPTER_ERDP + 4, 0); 
	}
	else
	{
		printk("another interrupt: ^\n", driver_status);
	}
	
XHCI_INTERRUPT_HANDLER_END:
	if(xINTERRUPT_HANDLER_DEBUG) printk("****************************************\n");
    return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void handle_device_attachment_to_port(XHCI* x, bit32u port)
{
    if( reset_port(x, port) )
    {
        bit8u speed;
        read_port_register_set(x, port, &speed);
	x->usb_device->connected = TRUE;
	x->usb_device->port      = port;
	x->usb_device->speed     = speed;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bit32u cur_ep_ring_ptr   = 0;
bit32u cur_ep_ring_cycle = 0;

void xhci_slot_configuration(XHCI* x, bit32u port, bit32u speed)
{
	bit8u i;
	
	x->usb_device->slot_configuration_command = FALSE;
	if(xINTERRUPT_HANDLER_DEBUG) printk("slot config for port %, speed %\n", port, speed);
	xHCI_TRB trb = { .param = 0ULL, .status = 0, .command = (9 << 10)};
    	mwrite(x->command_ring_Enqueue, 0x00, trb.param); 
	mwrite(x->command_ring_Enqueue, 0x04, 0x00);  
	mwrite(x->command_ring_Enqueue, 0x08, trb.status);                       
	mwrite(x->command_ring_Enqueue, 0x0C, (trb.command | x->command_ring_PCS));
	x->command_ring_Enqueue += sizeof(xHCI_TRB);
	bit32u cmnd = mread(x->command_ring_Enqueue, 0x0C);
	if (TRB_GET_TYPE(cmnd) == xLINK) 
	{
		mwrite(x->command_ring_Enqueue, 0x0C, ((cmnd & ~1) | x->command_ring_PCS));
		x->command_ring_Enqueue = x->driver_crcr;
		x->command_ring_PCS ^= 1;
	}
    
    	signal_from_slot_configuration_to_irq = 1;
	xhci_write_doorbell(x, 0, 0);
    	bit32u timing = 2000;
    	while(timing)
    	{
    	    volatile bit32u tmp = signal_from_irq_to_slot_configuration;
    	    if(tmp == 1)
    	    {
    	        signal_from_irq_to_slot_configuration = 0;
    	        signal_from_slot_configuration_to_irq = 0;
    	        break;
    	    }
    	    WaitMiliSecond(1);
    	    timing--;
    	    if(timing == 0)
    	    {
    	        printk("SLOT_CONFIG timed out\n");
				signal_from_slot_configuration_to_irq = 0;
    	        break;
    	    }
    	}

    	// at context slot-id < is 1-based and not 0-based! >
	volatile bit32u mdcbaap = mread(PHYS_ADDRESS(x->oper), 0x30);
	void* ctx1_buffer = xhci_alloc_memory(2 * x->context_size, 32, x->page_size);
	__IMOS_XHCI_MemZero(ctx1_buffer, 2 * x->context_size);
	mwrite(mdcbaap, (0x08 * (x->usb_device->slot_id)), (bit32u)ctx1_buffer);
	//void* ctx1_buffer = (void*)((bit32u)dcbaap[x->usb_device->slot_id]);
	
    	xHCI_SLOT_CONTEXT* slot = (xHCI_SLOT_CONTEXT*)ctx1_buffer;
    	xHCI_EP_CONTEXT*   ep   = (xHCI_EP_CONTEXT*) ((void*)((bit32u)ctx1_buffer + x->context_size));

	g_slot.DWORD0               = ((1 << 27) | (speed << 20)); // i.e. 1 endpoint (EP0) + speed     
    	g_slot.root_hub_port_number = port + 1;                    // root hub port number this device is attached to < values are 1 to MaxPort >
    	g_slot.max_exit_latency     = 0;                           // calculated later
	g_slot.number_of_ports      = 0;    
	g_slot.TT_hub_slot_id       = 0;    
	g_slot.TT_port_number       = 0;    
    	g_slot.WORD2                = 0;                           // interrupter 0
    	g_slot.WORD_BYTE3[1]        = 0;
	g_slot.WORD_BYTE3[2]        = 0;
	g_slot.WORD_BYTE3[3]        = 0;
    	g_slot.usb_device_address   = 0;
	*slot = g_slot;
	// allocate the EP's Transfer Ring
	void* ep_tr_pointer = xhci_alloc_memory((256 * sizeof(xHCI_TRB)), 16, 65536);
	__IMOS_XHCI_MemZero(ep_tr_pointer, (256 * sizeof(xHCI_TRB)));
	
	// make the last one a link TRB to point to the first one
	volatile bit32u pos = (bit32u)ep_tr_pointer + ((255) * sizeof(xHCI_TRB));
	mwrite(pos, 0x00, (bit32u)ep_tr_pointer);  
	mwrite(pos, 0x04, 0x00); 
	mwrite(pos, 0x08, 0x00);
	mwrite(pos, 0x0C, TRB_LINK_CMND);
	
  	g_ep.DWORD2  = ((bit32u)ep_tr_pointer | TRB_CYCLE_ON); // TR dequeue pointer + dequeue pointer's CS
	g_ep.TR_dequeue_pointer_hi = 0x00;
	
	cur_ep_ring_ptr        = (bit32u)(g_ep.DWORD2 & (~1));
	cur_ep_ring_cycle      = (bit32u)((g_ep.DWORD2 & BIT(0)));
	
  	// set the initial values
  	g_ep.WORD0  = (0 << 10) | (0 << 15) | (0 << 0);
  	g_ep.BYTE1  = (3 << 1) | (4 << 3) | BIT(7);
  	g_ep.average_trb_length = 8;  // All CONTROL EP's shall have '8' (page 325)
  	g_ep.max_burst_size = 0;
	g_ep.max_packet_size = (speed == 2) ? 8 : (speed == 1) ? 64 : (speed == 3) ? 64 : 512;
  	g_ep.interval = 0;
	*ep = g_ep;
	
	x->usb_device->slot_address = (void*)slot;
	x->usb_device->ep0_address  = (void*)ep;
    	if(xINTERRUPT_HANDLER_DEBUG) printk("slot address for configuration:^\n", (bit32u)x->usb_device->slot_address);
    	if(xINTERRUPT_HANDLER_DEBUG) printk("ep   address for configuration:^\n", (bit32u)x->usb_device->ep0_address);	

	x->usb_device->slot_configuration = TRUE;
	
	xhci_slot_set_address(x);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_slot_release(XHCI* x, bit32u port, bit32u speed)
{
	printk("slot release\n");
	critical_event = 0;
}

void xhci_configure_endpoint(XHCI* x, bit32u inp)
{
	xHCI_TRB trb = { .param = (bit64u)inp, .status = 0, .command = (((x->usb_device->slot_id) << 24) | (12 << 10))};
	mwrite(x->command_ring_Enqueue, 0x00, trb.param); 
	mwrite(x->command_ring_Enqueue, 0x04, 0x00);  
	mwrite(x->command_ring_Enqueue, 0x08, trb.status);                       
	mwrite(x->command_ring_Enqueue, 0x0C, (trb.command | x->command_ring_PCS));
	x->command_ring_Enqueue += sizeof(xHCI_TRB);
	xhci_write_doorbell(x, 0, 0);
	WaitSecond(1);
}

void xhci_soft_reset_endpoint(XHCI* x)
{
	xHCI_TRB trb = { .param = 0ULL, .status = 0, .command = ((1 << 24) | (1 << 16) | (14 << 10) | (1 << 9))};
	mwrite(x->command_ring_Enqueue, 0x00, trb.param); 
	mwrite(x->command_ring_Enqueue, 0x04, 0x00);  
	mwrite(x->command_ring_Enqueue, 0x08, trb.status);                       
	mwrite(x->command_ring_Enqueue, 0x0C, (trb.command | x->command_ring_PCS));
	x->command_ring_Enqueue += sizeof(xHCI_TRB);
	xhci_write_doorbell(x, 0, 0);
	WaitSecond(1);
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_setup_stage(XHCI* x, REQUEST_PACKET* request, bit8u dir) 
{
	if(xINTERRUPT_HANDLER_DEBUG) printk("inside setup stage: ep_TR_ring ^\n",  cur_ep_ring_ptr);
	if(xINTERRUPT_HANDLER_DEBUG) printk("inside setup stage: ep_TR_cycle %\n", cur_ep_ring_cycle);

	//bit64u param = (bit64u)(((request->value << 16) | (request->request << 8) | request->request_type) | ((bit64u)request->length << 48) | ((bit64u)request->index << 32));
	mwrite(cur_ep_ring_ptr, 0x00, 0x01000680);
	mwrite(cur_ep_ring_ptr, 0x04, 0x00080000);
	mwrite(cur_ep_ring_ptr, 0x08, 0x08);
	mwrite(cur_ep_ring_ptr, 0x0C, ((3 << 16) | (2 << 10) | (1 << 6) | (0 << 5) | cur_ep_ring_cycle));
	cur_ep_ring_ptr += sizeof(xHCI_TRB);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_data_stage(XHCI* x, bit32u addr, bit8u trb_type, bit32u size, bit8u direction, bit16u max_packet, bit32u status_addr)
{
	mwrite(cur_ep_ring_ptr, 0x00, addr); 
	mwrite(cur_ep_ring_ptr, 0x04, 0);    
	mwrite(cur_ep_ring_ptr, 0x08, (0 << 17) | (8 << 0));
	mwrite(cur_ep_ring_ptr, 0x0C, ((1 << 16) | (3 << 10) | cur_ep_ring_cycle));
	cur_ep_ring_ptr += sizeof(xHCI_TRB);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_status_stage(XHCI* x, bit8u dir, bit32u status_addr) 
{
	mwrite(cur_ep_ring_ptr, 0x00, 0x00);
	mwrite(cur_ep_ring_ptr, 0x04, 0x00);
	mwrite(cur_ep_ring_ptr, 0x08, (0 << 22));
	mwrite(cur_ep_ring_ptr, 0x0C, ((0 << 16) | (4 << 10) | (1 << 5) | cur_ep_ring_cycle));
	cur_ep_ring_ptr += sizeof(xHCI_TRB);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_setup_data_status_stages(XHCI* x, void* target, bit32u length, bit32u max_packet) 
{
	bit32u status_addr = (bit32u)(xhci_alloc_memory(4, 16, 16));    // we need a dword status buffer with a physical address
	bit32u buffer_addr = (bit32u)(xhci_alloc_memory(18, 64, 65536));   // get a physical address buffer and then copy from it later
	__IMOS_XHCI_MemZero((void*)status_addr, 4);
	__IMOS_XHCI_MemZero((void*)buffer_addr, 18);

	REQUEST_PACKET packet = { STDRD_GET_REQUEST, GET_DESCRIPTOR, ((DEVICE << 8) | 0), 0, 0 };
	packet.length = length;
	
	xhci_setup_stage (x, &packet, xHCI_DIR_IN);
	xhci_data_stage  (x, buffer_addr, xDATA_STAGE, length, xHCI_DIR_IN_B, max_packet, status_addr);
	xhci_status_stage(x, (xHCI_DIR_IN_B ^ 1), status_addr);

	signal_from_setup_data_status_stages_to_irq = 1;
	xhci_write_doorbell(x, x->usb_device->slot_id, xHCI_CONTROL_EP);
	WaitSecond(1);
	
	bit32u timing = 2000;
	while(timing)
    	{
    	    volatile bit32u tmp = signal_from_irq_to_setup_data_status_stages;
    	    if(tmp == 1)
    	    {
    	        signal_from_irq_to_setup_data_status_stages = 0;
    	        signal_from_setup_data_status_stages_to_irq = 0;
    	        break;
    	    }
    	    WaitMiliSecond(1);
    	    timing--;
    	    if(timing == 0)
    	    {
    	        printk("SETUP_DATA_STAUS_STAGES timed out\n");
				signal_from_setup_data_status_stages_to_irq = 0;
    	        break;
    	    }
    	}

	x->usb_device->setup_data_status_stages = TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_slot_set_address(XHCI* x)
{
	printk("slot set address\n");
	bit32u input_context = (bit32u)( xhci_alloc_memory(x->context_size * 32, 64, x->page_size) );
	__IMOS_XHCI_MemZero((void*)input_context, (x->context_size * 32));
	mwrite(input_context, 0x00, ~0x03); // Drops bits
	mwrite(input_context, 0x04,  0x03); // Adds  bits < only bits 0 and 1 are set >

    	xHCI_SLOT_CONTEXT* slot = (xHCI_SLOT_CONTEXT*)x->usb_device->slot_address;
    	xHCI_EP_CONTEXT*   ep   = (xHCI_EP_CONTEXT*)  x->usb_device->ep0_address;
	
	xHCI_SLOT_CONTEXT* input_context_slot = (xHCI_SLOT_CONTEXT*)((void*)(input_context + x->context_size));
	xHCI_EP_CONTEXT*   input_context_ep   = (xHCI_EP_CONTEXT*)  ((void*)(input_context + x->context_size + x->context_size));
	*input_context_slot = g_slot; // copy of slot context into input_context
	*input_context_ep   = g_ep;   // copy of endpoint context into input_context

	xHCI_TRB trb = { .param = (bit64u)(input_context), .status = 0, .command = ((11 << 10) | ((x->usb_device->slot_id) << 24) | (1 << 9)) };
	
    	mwrite(x->command_ring_Enqueue, 0x00, trb.param); 
	mwrite(x->command_ring_Enqueue, 0x04, (bit32u)(trb.param >> 32));  
	mwrite(x->command_ring_Enqueue, 0x08, trb.status);                       
	mwrite(x->command_ring_Enqueue, 0x0C, (trb.command | x->command_ring_PCS));
	x->command_ring_Enqueue += sizeof(xHCI_TRB);
	bit32u cmnd = mread(x->command_ring_Enqueue, 0x0C);
	if (TRB_GET_TYPE(cmnd) == xLINK) 
	{
		mwrite(x->command_ring_Enqueue, 0x0C, ((cmnd & ~1) | x->command_ring_PCS));
		x->command_ring_Enqueue = x->driver_crcr;
		x->command_ring_PCS ^= 1;
	}
    
    	signal_from_slot_set_address_to_irq = 1;
	xhci_write_doorbell(x, 0, 0);
    	bit32u timing = 2000;
    	while(timing)
    	{
        	volatile bit32u tmp = signal_from_irq_to_slot_set_address;
        	if(tmp == 1)
        	{
            	signal_from_irq_to_slot_set_address = 0;
            	signal_from_slot_set_address_to_irq = 0;
            	break;
        	}
        	WaitMiliSecond(1);
        	timing--;
        	if(timing == 0)
        	{
            		printk("SLOT_SET_ADDRESS timed out\n");
			signal_from_slot_set_address_to_irq = 0;
            		break;
        	}
    	}
	
	x->usb_device->slot_set_address = TRUE;
	
	xHCI_SLOT_CONTEXT* copy_slot = &g_slot;
    	xHCI_EP_CONTEXT*   copy_ep   = &g_ep;
	
	xHCI_SLOT_CONTEXT tmp_slot;
    	xHCI_EP_CONTEXT   tmp_ep;
	xHCI_SLOT_CONTEXT* ptr_tmp_slot = &tmp_slot;
    	xHCI_EP_CONTEXT*   ptr_tmp_ep = &tmp_ep;
	*ptr_tmp_slot = *slot;
    	*ptr_tmp_ep   = *ep;
	
	g_slot.DWORD0 = ptr_tmp_slot->DWORD0;
	g_slot.usb_device_address = ptr_tmp_slot->usb_device_address;
    	g_ep.WORD0   = ptr_tmp_ep->WORD0;
	g_ep.max_packet_size   = ptr_tmp_ep->max_packet_size;
	
	DEVICE_DESC dev_desc;
	xhci_setup_data_status_stages(x, (void*)(&dev_desc), 8, ep->max_packet_size);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

XHCI* xhci_instance_to_idt(void)
{
	return xx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bit8u xhci_get_critical_event(void)
{
	return critical_event ;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
