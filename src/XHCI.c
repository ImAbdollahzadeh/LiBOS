#include "../include/XHCI.h"
#include "../include/PCI.h"
#include "../include/PRINT.h"
#include "../include/TIMER.h"
#include "../include/MEMORY.h"
#include "../include/IDT.h"
#include "../include/PORT.h"

#define xDEBUG 0
extern void CHECK_DS(unsigned int* ds);

bit32u base            = 0;
bit32u cap_base        = 0;
bit8u  caplength       = 0;
bit32u op_base         = 0;
bit32u runtime         = 0;
bit32u doorbell        = 0;
bit32u port_reg_set    = 0;
bit32u ext_cap         = 0;
bit32u context_size    = 0;
bit32u page_size       = 0;
bit32u hccparams1      = 0;
bit32u hccparams2      = 0;
bit32u hcsparams1      = 0;
bit32u hcsparams2      = 0;
bit32u cur_cmnd_ring_addr = 0;
bit32u dcbaap          = 0; // it is 64 bit
bit32u crcr            = 0; // it is 64 bit
bit32u irq             = 0;
bit32u max_intr        = 0;
bit32u cur_event_ring_addr = 0;
void* ers              = 0;
void* erst             = 0;
volatile void* pci              = 0;
bit16u bus = 0, device = 0, function = 0;

volatile unsigned int tagged_port_number = 255;
volatile xHCI_PORT_INFO port_info[255];

void xhci_interrupt_handler(PREGS r);

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

void xhci_print_on_screen(XHCI* x)
{
	printk("XHCI \n");
	printk("  base address(high)(low) = (^)(^)\n", x->base_address_hi, x->base_address_lo);
	printk("  caplength               = ^\n", (unsigned int)x->cap->caplength);
	printk("  hciversion              = ^\n", (unsigned int)x->cap->hciversion);
	printk("  hcsparam1               = ^\n", x->cap->hcsparam1);
	printk("      no. slot            = ^\n", (unsigned int)(x->cap->hcsparam1 & 0xFF));
	printk("      no. interrupter     = ^\n", (unsigned int)((x->cap->hcsparam1 & 0x7FF00)>>8));
	printk("      no. port            = ^\n", (unsigned int)((x->cap->hcsparam1 & 0xFF000000)>>24));
	printk("  hcsparam2               = ^\n", x->cap->hcsparam2);
	printk("  hcsparam3               = ^\n", x->cap->hcsparam3);
	printk("  hccparam1               = ^\n", x->cap->hccparam1);
    printk("      use %-byte context data structure\n", ((x->cap->hccparam1 & 0x4)>>2) == 0 ? 32: 64);
	printk("      each port is powered "); printk(((x->cap->hccparam1 & 0x8)>>3 == 0) ? "always\n": "its own\n");
	printk("  dboff                   = ^\n", x->cap->dboff);
	printk("  rtsoff                  = ^\n", x->cap->rtsoff);
	printk("  hccparam2               = ^\n", x->cap->hccparam2);
	
	printk("  usbcmd                  = ^\n", x->oper->usbcmd);
	printk("  usbsts                  = ^\n", x->oper->usbsts);
	printk("  pagesize                = %\n", ((x->oper->pagesize & 0xFFFF) << 12));
	printk("  dnctrl                  = ^\n", x->oper->dnctrl);
	printk("  dcbaap(high)(low)       = (^)(^)\n", (unsigned int)((x->oper->dcbaap) >> 32), (unsigned int)(x->oper->dcbaap));
	printk("  crcr  (high)(low)       = (^)(^)\n", (unsigned int)((x->oper->crcr)   >> 32), (unsigned int)(x->oper->crcr));
	printk("  xhci irq number         = %\n", (unsigned int)x->irq_number);
}

bit8u __port_speed(bit16u portSpeed)
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

void __read_port_register_set(XHCI* x, bit8u port)
{
	bit8u maxPort = (bit8u)((x->cap->hcsparam1 & 0xFF000000)>>24);
	bit16u speed;
	PxHCI_PORT_REGISTER_SET prs = (PxHCI_PORT_REGISTER_SET)((void*)((bit32u)((void*)x->port_reg_set) + (16 * port)));
	if(port > maxPort)
	{
		printk("the requested port number is greater than supported maxPort oh XHCI\n");
		return;
	}
	speed = (bit16u)(((prs->portsc) & xPORT_SPD) >> 10);
	printk("port %\n", port);
	printk("  portsc    = ^\n", prs->portsc);

	bit8u tag = __port_speed(speed);
	if(tag)
		tagged_port_number = (unsigned int)port;
	
	//.printk("  portpmsc  = ^\n", prs->portpmsc);
	//.printk("  portli    = ^\n", prs->portli);
	//.printk("  porthlpmc = ^\n", prs->porthlpmc);
	
	return;
}

void __init_port_power_on(bit8u port)
{
    PxHCI_PORT_REGISTER_SET prs = (PxHCI_PORT_REGISTER_SET)((void*)port_reg_set + (16 * port));
       
    prs->portsc |= xPORT_POW;
    WaitMiliSecond(20);
    
    bit32u timing = 1000;
    while ( (prs->portsc & xPORT_POW) == 0 ) 
	{
	  	prs->portsc |= xPORT_POW;
	  	WaitMiliSecond(1);
	  	if (timing == 0)
		{
	    	printk("port % set power failed\n", port);
			return;
		}
        timing--;
	}
}

bool __xhci_reset_port(XHCI* x, unsigned int port) 
{
	PxHCI_PORT_REGISTER_SET prs = (PxHCI_PORT_REGISTER_SET)((void*)((bit32u)((void*)x->port_reg_set) + (16 * port)));
	volatile bit32u val;
	
	// power the port?
	if ( prs->portsc & xPORT_POW == 0 ) 
	{
	  	prs->portsc |= xPORT_POW;
	  	WaitMiliSecond(20);
		
	  	if ( prs->portsc & xPORT_POW == 0 )
		{
	    	printk("port % bad reset\n", port);
			return FALSE;  // return bad reset.
		}
	}
	
	// we need to make sure that the status change bits are clear
	prs->portsc |= xPORT_CHANGE_BITS;
	
	// set bit 4 (USB2) or 31 (USB3) to reset the port
	prs->portsc |= xPORT_RESET;
	
	//.if (xHCI_IS_USB3_PORT(port))
	//.	*(bit32u*)((void*)prs->portsc) = (BIT(9) | BIT(31));
	//.else
	//.	*(bit32u*)((void*)prs->portsc) = (BIT(9) | BIT(4));
	
	// wait for bit 21 to set
	unsigned int timeout = 1000;
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
			return 1;
		}
	} 
	else
	{
		printk("port % reset timed out\n", port);
		return 0;
	}
	
	//.// if we have a successful USB2 reset, we need to make sure this port is marked active, and if it has a paired port, it is marked inactive
	//.if (ret == TRUE && xHCI_IS_USB2_PORT(port)) 
	//.{
	//.	port_info[port].flags |= xHCI_PROTO_ACTIVE;
	//.	if (port_info[port].flags & xHCI_PROTO_HAS_PAIR)
	//.		port_info[port_info[port].other_port_num].flags &= ~xHCI_PROTO_ACTIVE;
	//.}
	//.
	//.// if error resetting USB3 protocol, deactivate this port and activate the paired USB2 port. it will be paired since all USB3 ports must be USB2 compatible.
	//.if (ret == FALSE && xHCI_IS_USB3_PORT(port)) 
	//.{
	//.	port_info[port].flags &= ~xHCI_PROTO_ACTIVE;
	//.	port_info[port_info[port].other_port_num].flags |= xHCI_PROTO_ACTIVE;
	//.}
	printk("port % reset \n", port);
}

void __read_extended_capabilities(XHCI* x, unsigned int* legacy_entry)
{
	//bit8u maxPort = (bit8u)((x->cap->hcsparam1 & 0xFF000000)>>24);
	bit8u maxPort  = (bit8u)  ((hcsparams1 & 0xFF000000) >> 24);
	bit32u ext_off = (bit32u)(((hccparams1 & 0xFFFF0000) >> 16)*4);
	PxHCI_EXTENDED_CAPABILITY ext_cap = (PxHCI_EXTENDED_CAPABILITY)((void*)(base + ext_off));
	if(ext_cap == 0)
	{
		printk("error: zero xECP\n");
		return;
	}
	bit32u next = 0;
	unsigned int i = 0, k = 0, entries = 0;
	
	char str[5];
	for(i=0;i<5;i++)
		str[i] = 0;
	
	while(entries < maxPort)
	{
		if((bit8u)(ext_cap->DWORD0 & 0xFF) == 0x01)
		{
			*legacy_entry = entries;
			printk("found xECP with ID = 0x01 @ entry=%\n", entries);
		}
		//.printk("xECP ID=% @ entry=%\n", (unsigned int)( (bit8u)(ext_cap->DWORD0 & 0xFF) ), entries);
		if((bit8u)(ext_cap->DWORD0 & 0xFF) == 0x02)
		{
			printk("found xECP with ID = 0x02 @ entry=%\n", entries);
			for(i=0;i<4;i++)
				str[i] = (char)((ext_cap->DWORD1 >> (i*8)) & 0xFF);
			
			printk("protocol = ");
			printk((char*)str);
			printk("%.% -> ", (unsigned int)(((ext_cap->DWORD0)&0xFF000000)>>24), (unsigned int)(((ext_cap->DWORD0)&0x00FF0000)>>16));
			printk("% ports @ offset %\n", (unsigned int)(((ext_cap->DWORD2)&0x0000FF00)>>8), (unsigned int)(((ext_cap->DWORD2)&0x000000FF)));
			
			bit8u portCount = ((ext_cap->DWORD2)&0x0000FF00)>>8;
			bit8u offset    = (ext_cap->DWORD2)&0x000000FF;
			bit8u usb2_or_usb3 = ((ext_cap->DWORD0)&0xFF000000)>>24;
			bit32u ports_usb2 = 0;
			bit32u ports_usb3 = 0;
			
			// .... for (i=0; i<portCount; i++) 
			// .... {
			// .... 	port_info[offset + i].offset = (usb2_or_usb3 == 0x03) ? ports_usb3++ : ports_usb2++;
			// .... 	port_info[offset + i].flags  = (usb2_or_usb3 == 0x03) ? xHCI_PROTO_USB3 : xHCI_PROTO_USB2;
			// .... }
			// .... 
			// .... // pair up each USB3 port with it's companion USB2 port
			// .... for (i=0; i<maxPort; i++) 
			// .... {
			// .... 	for (k=0; k<maxPort; k++) 
			// .... 	{
			// .... 		if ((port_info[k].offset == port_info[i].offset) && ((port_info[k].flags & xHCI_PROTO_INFO) != (port_info[i].flags & xHCI_PROTO_INFO))) 
			// .... 		{
			// .... 			port_info[i].other_port_num = k;
			// .... 			port_info[i].flags |= xHCI_PROTO_HAS_PAIR;
			// .... 			port_info[k].other_port_num = i;
			// .... 			port_info[k].flags |= xHCI_PROTO_HAS_PAIR;
			// .... 		}
			// .... 	}
			// .... }
			// .... 
			// .... // mark all USB3 ports and any USB2 only ports as active, deactivating any USB2 ports that have a USB3 companion
			// .... for (i=0; i<maxPort; i++) 
			// .... {
			// .... 	if (xHCI_IS_USB3_PORT(i) || (xHCI_IS_USB2_PORT(i) && !xHCI_HAS_PAIR(i)))
			// .... 		port_info[i].flags |= xHCI_PROTO_ACTIVE;
			// .... }
		}
		
		if(((ext_cap->DWORD0 & 0xFF00)>>8) == 0)
		{
			printk("end of xECP list reached @ entry=%\n", entries);
			return;
		}		
		next = 4*((ext_cap->DWORD0 & 0xFF00)>>8);
		ext_cap = (PxHCI_EXTENDED_CAPABILITY)((void*)((bit32u)((void*)ext_cap) + next));
		entries++;
	}
	
	return;
}


void xhci_get_trb(xHCI_TRB* trb, bit32u address) 
{
	trb->param =   (bit64u)(mread(address, 0));
	trb->status =  mread(address,  8);
	trb->command = mread(address, 12);
}

void xhci_set_trb(xHCI_TRB* trb, bit32u address) 
{
	mwrite(address, 0, (bit32u)trb->param);
	mwrite(address,  8, trb->status);
	mwrite(address, 12, trb->command);
}

void xhci_write_doorbell(XHCI* x, const bit32u slot_id, const bit32u val) 
{
	mwrite(doorbell, slot_id, val);
}
static bit8u signal_from_noop_to_irq = 0;
static bit8u signal_from_irq_to_noop = 0;

void ___test_NO_OP_TRB___(XHCI* x)
{
	xHCI_TRB trb = { .param = 0ULL, .status = 0, .command = 0x00002000 };
	mwrite(cur_cmnd_ring_addr, 0x00, trb.param); 
	mwrite(cur_cmnd_ring_addr, 0x04, 0);  
	mwrite(cur_cmnd_ring_addr, 0x08, trb.status);                       
	mwrite(cur_cmnd_ring_addr, 0x0C, (trb.command | TRB_CYCLE_ON));
	cur_cmnd_ring_addr += sizeof(xHCI_TRB);
	
	//.bit32u cmnd = mread(command_trb_address_32, 0x0C);
	//.if (TRB_GET_TYPE(cmnd) == xLINK) 
	//.{
	//.	mwrite(command_trb_address_32, 0x0C, ((cmnd & ~1) | command_trb_cycle));
	//.	command_trb_address_32 = command_ring_address_32;
	//.	command_trb_cycle ^= 1;
	//.}

	xhci_write_doorbell(x, 0, 0);
	
	//.unsigned int timer = 2000;
	//.signal_from_noop_to_irq = 1;
	//.while (timer) 
	//.{
	//.	volatile bit32u tmp = signal_from_irq_to_noop;
	//.	if(tmp == 1)
	//.	{
	//.		printk("NOOP succeeded\n");
	//.		signal_from_irq_to_noop = 0;
	//.		signal_from_noop_to_irq = 0;
	//.		break;
	//.	}
	//.	WaitMiliSecond(1);
	//.	timer--;
	//.}
	//.if(timer == 0)
	//.	printk("NOOP time out\n");
}



bool start_xhci_controller(XHCI* x)
{
	bit32u i;
	bit32u j;
	bit8u  portMax;	
	unsigned int legacy_entry = 257;
	__IMOS_XHCI_MemZero((void*)(&port_info[0]), 255 * sizeof(xHCI_PORT_INFO));
	
    unsigned int ds = 0;
    CHECK_DS(&ds);
    if(!ds)
    {
        printk("DS register is wrong!\n");
        return FALSE;
    }

	base         = (bit32u)x->base_address_lo;
	irq          = x->irq_number;
	cap_base     = base;
	caplength    = (bit8u)( mread(cap_base, 0x00) & 0xFF );
	op_base      = base + (bit32u)caplength;
    port_reg_set = op_base + 0x400;
	pci          = x->pci;
	bus          = x->bus;
	device       = x->device;
	function     = x->function;
    
	//. bit16u pci_status = (bit16u)( Read(pci, bus, device, function, 0x04) >> 16 );
    //. if(pci_status & BIT(4))
    //.     printk("xHCI has the CapabilityPointer\n");                                 

	unsigned int timeout = 50;
    bit32u cmd = mread(op_base, 0x00);
    cmd |= ~(xCMD_RUN);
	mwrite(op_base, 0x00, cmd);	
    while((mread(op_base, 0x04) & xSTS_HCH) == 0)
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
    cmd = mread(op_base, 0x00);
    cmd |= xCMD_RESET;
	mwrite(op_base, 0x00, cmd);	
	while((mread(op_base, 0x00) & xCMD_RESET) != 0)
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
	while((mread(op_base, 0x04) & xSTS_CNR) != 0)
	{
		if(timeout == 0)
		{
			printk("xHC is not ready after 1 sec\n");
			return FALSE;
		}
		WaitMiliSecond(1);
		timeout--;
	}
	
	printk("XHCI reset successful\n");	
    
    //.// to make sure that PME# doesn't generate any interrupt
    //.__stop_PME(pci, bus, device, function);
    
	runtime         = base + mread(base, 0x18);
	doorbell        = base + mread(base, 0x14);
	hccparams1      = mread(base, 0x10);
	hccparams2      = mread(base, 0x1C);
	hcsparams1      = mread(base, 0x04);
	hcsparams2      = mread(base, 0x08);
	ext_cap         = (((hccparams1 & 0xFFFF0000) >> 16) * 4);
	context_size    = (hccparams1 & BIT(2)) ? 64 : 32;
	page_size       = (mread(op_base, 0x00000008) & 0x0000FFFF) << 12;

	bit32u maxSlots = (hcsparams1 & 0x000000FF);
    //.printk("max device slots is %\n", maxSlots);
	//.__IMOS_XHCI_MemZero((void*)doorbell, 4 * (maxSlots + 1)); // 1 is for the presence of the command ring doorbell
    
	// Device Notification Control (only bit 1 is allowed)
	mwrite(op_base, 0x14, BIT(1));
	
    // Configure Register
	mwrite(op_base, 0x38, maxSlots);
    
    // allocate the dcbaa, crcr and the slot contexts.
    void* mdcbaap = xhci_alloc_memory(2048, 64, page_size);
	void* mcrcr   = xhci_alloc_memory((CMND_RING_TRBS * sizeof(xHCI_TRB)), 64, 65536);
	__IMOS_XHCI_MemZero(mdcbaap, 2048);               
	__IMOS_XHCI_MemZero(mcrcr, CMND_RING_TRBS * sizeof(xHCI_TRB));
	 
	// setting up the scratchpad buffer
	//bit8u MaxScratchpadBuffers = ((hcsparams2 >> 27) & 0x1F) | ((hcsparams2 >> 16) & 0xE0);
	//if(MaxScratchpadBuffers > 0)
	//{
	//	if(xDEBUG)
	//		printk("scratchpad buffers number = %\n", (unsigned int)MaxScratchpadBuffers);
	//	
	//	bit64u* ScratchpadBuffersPtr = (bit64u*)(xhci_alloc_memory(sizeof(bit64u) * MaxScratchpadBuffers, 64, page_size));
	//	__IMOS_XHCI_MemZero((void*)ScratchpadBuffersPtr, sizeof(bit64u) * MaxScratchpadBuffers);		   		   
	//	for(i=0; i<MaxScratchpadBuffers; i++)
	//	{
	//		bit32u tmp = (bit32u)( xhci_alloc_memory(page_size, page_size, page_size) );
	//		__IMOS_XHCI_MemZero((void*)tmp, page_size);
	//		ScratchpadBuffersPtr[i] = (bit64u)tmp;
	//	}
	//	
	//	mwrite((bit32u)mdcbaap, 0x00, (bit32u)((void*)ScratchpadBuffersPtr));	
	//}
    
    // setting up device contexts to maxSlot  
	//for(i=1; i<maxSlots + 1; i++)
	//{
	//	bit64u* DevCntx = (bit64u*)(xhci_alloc_memory(sizeof(bit64u), 64, page_size));
    //    __IMOS_XHCI_MemZero((void*)DevCntx, sizeof(bit64u));
    //    
    //    bit64u* DevCntxSlots = (bit64u*)(xhci_alloc_memory(32, 64, page_size));
	//	__IMOS_XHCI_MemZero((void*)DevCntxSlots, 32);
    //    xHCI_SLOT_CONTEXT* slot = (xHCI_SLOT_CONTEXT*)((void*)DevCntxSlots);
    //    slot->entries    = 32;
    //    slot->slot_state = 1;
    //    mwrite((bit32u)((void*)DevCntx), 0x00, (bit32u)((void*)DevCntxSlots));
    //    
    //    mwrite((bit32u)mdcbaap, (0x08 * i), (bit64u)((bit32u)((void*)DevCntx)));	
	//}	
    
	// 64bit write
	mwrite(op_base, 0x30, (bit32u)mdcbaap);	
	mwrite(op_base, 0x34, 0x00);
	dcbaap = (bit32u)mdcbaap;
	
	//command ring Link TRB
	bit32u pos = (bit32u)mcrcr + ((CMND_RING_TRBS - 1) * sizeof(xHCI_TRB));
	mwrite(pos, 0x00, (bit32u)mcrcr);                 // param
	mwrite(pos, 0x04, 0);
	mwrite(pos, 0x08, 0);                             // status
	mwrite(pos, 0x0C, TRB_LINK_CMND);                 // command 
	
	// 64bit write
    mwrite(op_base, 0x1C, 0x00);
	mwrite(op_base, 0x18, ((bit32u)mcrcr | TRB_CYCLE_ON));	
	cur_cmnd_ring_addr =  (bit32u)mcrcr;
	
	if(xDEBUG)
	{
		printk("base:^, cap:^, op:^, capl:^\n", base, cap_base, op_base, caplength);
		printk("rtsoff:^, dboff:^, hcsp1:^, hcsp2:^\n", runtime, doorbell, hcsparams1, hcsparams2);
		printk("hccp1:^, hccp2:^\n", hccparams1, hccparams2);
		printk("pgsz:^, cntxsz:^, extcap:^, portoff:^\n", page_size, context_size, ext_cap, port_reg_set);
		printk("dcbaap:^, crcr:^\n", dcbaap, cur_cmnd_ring_addr);
		printk("reading back crcr:^, irq:%\n", mread(op_base, 0x18), irq);
	}

	// Initialize the interrupters
	ers                  = xhci_alloc_memory(256 * sizeof(xHCI_TRB), 64, 65536);
	erst                 = xhci_alloc_memory(sizeof(xHCI_EVENT_SEG_TABLE), 64, 65536);
	cur_event_ring_addr  = (bit32u)ers;
	__IMOS_XHCI_MemZero(ers,  256 * sizeof(xHCI_TRB));
	__IMOS_XHCI_MemZero(erst, sizeof(xHCI_EVENT_SEG_TABLE));
	
	xHCI_EVENT_SEG_TABLE* segment_table = (xHCI_EVENT_SEG_TABLE*)erst;
	segment_table->addr                 = (bit64u)(0ULL | (bit32u)ers);
	segment_table->size                 = 256;
	segment_table->resv                 = 0;
	
	bit32u interrupter0 = runtime + 0x20;
        
	if(xDEBUG)
		printk("pri. interrupter:^\n", interrupter0);
	
	mwrite(interrupter0, xHC_INTERRUPTER_ERSTSZ,     1);                           
	mwrite(interrupter0, xHC_INTERRUPTER_ERDP,       (bit32u)segment_table->addr | BIT(3)); 
	mwrite(interrupter0, xHC_INTERRUPTER_ERDP + 4,   0); 
	mwrite(interrupter0, xHC_INTERRUPTER_ERSTBA,     (bit32u)erst); 
	mwrite(interrupter0, xHC_INTERRUPTER_ERSTBA + 4, 0);
    mwrite(interrupter0, xHC_INTERRUPTER_IMOD,       4000); 
	mwrite(interrupter0, xHC_INTERRUPTER_IMAN,       (xHC_IMAN_IE | xHC_IMAN_IP)); 
    
    WaitMiliSecond(100);
    
    // clear the status register
    bit32u st = mread(op_base, 4);
    st |= (BIT(2) | BIT(3) | BIT(4) | BIT(10));
    mwrite(op_base, 4, st); 
    
    // start the interrupter
	__irq_install_handler(irq, &xhci_interrupt_handler);  

	// start the controller
    cmd = 0;
	cmd = mread(op_base, 0x00);
	cmd |= (xCMD_RUN | xCMD_INTE | xCMD_HSEE);   
	mwrite(op_base, 0x00, cmd);
	WaitMiliSecond(100);
	
	timeout = 50;
	while((mread(op_base, 0x04) & xSTS_HCH) != 0)
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
	while((mread(op_base, 0x04) & xSTS_CNR) != 0)
	{
		if(timeout == 0)
		{
			printk("xHC is not ready after 1 sec\n");
			return FALSE;
		}
		WaitMiliSecond(1);
		timeout--;
	}
    
    //portMax = (bit8u)((x->cap->hcsparam1 & 0xFF000000)>>24);
    //for(i = 0; i < portMax; i++)
    //    __init_port_power_on(i);

	//.xhci_print_on_screen(x);	
	
	__read_extended_capabilities(x, &legacy_entry);
	//
	//_print_port_info_list_( (bit8u)((x->cap->hcsparam1 & 0xFF000000)>>24) );
	//if(legacy_entry == 257)
	//	printk("The controller does not support bios legacy mode\n");
	//else
	//{
	//	bool sts = __stop_legacy_mode(x, legacy_entry);
	//	if(!sts)
	//	{
	//		printk("error: stop bios legacy timed out \n");
	//		return FALSE;
	//	}
	//}
	//
	//portMax = (bit8u)((x->cap->hcsparam1 & 0xFF000000)>>24);
	//for(i=0;i<portMax;i++)
	//	__read_port_register_set(x, i);
	//
	//// now the USB2 ports
	//for (i=0; i<portMax; i++) 
	//{
	//	if(i == tagged_port_number)
	//	{
	//		// power and reset the port
	//    	if(__xhci_reset_port(x, i))
	//		{
	//    		printk("port % reset succeeded\n", i);
	//			xhci_get_descriptor(x, i);
	//		}
	//	}
	//}
	
	//printk("NOOP:\n");
	___test_NO_OP_TRB___(x);
	___test_NO_OP_TRB___(x);
	___test_NO_OP_TRB___(x);
	
	return TRUE;
}

void* xhci_alloc_memory(bit32u byte, bit32u alignment, bit32u page_boundary)
{
	return Alloc(byte, alignment, page_boundary);
}

void xhci_free_memory(void* pointer)
{
	Free(pointer);
}

void xhci_interrupt_handler(PREGS r)
{
	volatile bit32u status = mread(op_base, 4);
	mwrite(op_base, 4, status);

	volatile bit32u interrupter0 = runtime + 0x20;
	bit32u iman         = mread(interrupter0, xHC_INTERRUPTER_IMAN);

    printk("xhci interrupt: status ^, IMAN ^\n", status, iman);
    
    iman |= (xHC_IMAN_IE | xHC_IMAN_IP);
    mwrite(interrupter0, xHC_INTERRUPTER_IMAN, iman);    
    WaitMiliSecond(100);    

	cur_event_ring_addr += sizeof(xHCI_TRB);
	mwrite(interrupter0, xHC_INTERRUPTER_ERDP, cur_event_ring_addr | BIT(3));
    mwrite(interrupter0, xHC_INTERRUPTER_ERDP + 4, 0);
	
    return;
}