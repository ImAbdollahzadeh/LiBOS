#include "../include/XHCI.h"
#include "../include/PCI.h"
#include "../include/PRINT.h"
#include "../include/TIMER.h"
#include "../include/MEMORY.h"
#include "../include/IDT.h"
#include "../include/PORT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

#define xDEBUG 0
static BOOL xINTERRUPT_HANDLER_DEBUG = FALSE;

extern void CHECK_DS                        (UINT_32* ds);
       void xhci_interrupt_handler          (REGS* r);
       void handle_device_attachment_to_port(XHCI* x, UINT_32 port);

// virtual usb 2.0 and usb 3.0 port connection status
static UINT_8 usb2_connection_ports = 0x00;
static UINT_8 usb3_connection_ports = 0x00;

// IRQ signals
volatile UINT_32 signal_from_noop_to_irq                     = 0;
volatile UINT_32 signal_from_irq_to_noop                     = 0;
volatile UINT_32 signal_from_irq_to_slot_configuration       = 0;
volatile UINT_32 signal_from_slot_configuration_to_irq       = 0;
volatile UINT_32 signal_from_slot_set_address_to_irq         = 0;
volatile UINT_32 signal_from_irq_to_slot_set_address         = 0;
volatile UINT_32 signal_from_setup_data_status_stages_to_irq = 0;
volatile UINT_32 signal_from_irq_to_setup_data_status_stages = 0;
volatile UINT_32 signal_from_irq_to_string_descriptor        = 0;
volatile UINT_32 signal_from_string_descriptor_to_irq        = 0;

// tmp
UINT_8 tmp_disconnection = TRUE;

// instance of the XHCI object
XHCI*  xx             = 0x00;

// entering critical event in IRQ_HANDLER
UINT_8 critical_event = 0x00;

// All TRB types
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

void xhci_print_on_screen(XHCI* x)
{
	printk("XHCI \n");
	printk("  base address(high)(low) = (^)(^)\n", x->base_address_hi, x->base_address_lo);
	printk("  caplength               = ^\n", (UINT_32)x->cap->caplength);
	printk("  hciversion              = ^\n", (UINT_32)x->cap->hciversion);
	printk("  hcsparam1               = ^\n", x->cap->hcsparams1);
	printk("      no. slot            = ^\n", (UINT_32)(x->cap->hcsparams1 & 0xFF));
	printk("      no. interrupter     = ^\n", (UINT_32)((x->cap->hcsparams1 & 0x7FF00)>>8));
	printk("      no. port            = ^\n", (UINT_32)((x->cap->hcsparams1 & 0xFF000000)>>24));
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
	printk("  dcbaap(high)(low)       = (^)(^)\n", (UINT_32)((x->oper->dcbaap) >> 32), (UINT_32)(x->oper->dcbaap));
	printk("  crcr  (high)(low)       = (^)(^)\n", (UINT_32)((x->oper->crcr)   >> 32), (UINT_32)(x->oper->crcr));
	printk("  xhci irq number         = %\n", (UINT_32)x->irq_number);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_8 port_speed(UINT_16 portSpeed)
{
	UINT_8 tag = 0;
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

void read_port_register_set(XHCI* x, UINT_32 port, UINT_8* spd)
{
	UINT_8  maxPort = (UINT_8)((x->cap->hcsparams1 & 0xFF000000)>>24);
	UINT_16 speed;
	xHCI_PORT_REGISTER_SET* prs = (xHCI_PORT_REGISTER_SET*)((void*)((UINT_32)((void*)x->port_reg) + (16 * port)));
	if(port > maxPort)
	{
		printk("the requested port number is greater than supported maxPort oh XHCI\n");
		return;
	}
	speed = (UINT_16)(((prs->portsc) & xPORT_SPD) >> 10);
	//.printk("*** port % ***\n", port);
	//.printk("  portsc    = ^\n", prs->portsc);

	port_speed(speed);
	*spd = (UINT_8)speed;
    
	//.printk("  portpmsc  = ^\n", prs->portpmsc);
	//.printk("  portli    = ^\n", prs->portli);
	//.printk("  porthlpmc = ^\n", prs->porthlpmc);
	
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL reset_port(XHCI* x, UINT_32 port) 
{
             UINT_8  connection_status = 0;
	         UINT_32 prs               = ( PHYSICAL_ADDRESS(x->port_reg) + (16 * port) );
	volatile UINT_32 val               = mread(prs, 0x00);
	
	switch(port)
	{
		case 0: connection_status = (usb2_connection_ports & PORT_0) >> 0; break;
		case 1: connection_status = (usb2_connection_ports & PORT_1) >> 2; break;
		case 2: connection_status = (usb2_connection_ports & PORT_2) >> 4; break;
		case 3: connection_status = (usb2_connection_ports & PORT_3) >> 6; break;
	}
	
	switch(connection_status)
	{
		case USB2_CONNECTION_PORT_DISCONNECT:
			// go to full reset and set connection status to connect 1
			switch(port)
			{
				case 0: usb2_connection_ports = (USB2_CONNECTION_PORT_CONNECT_01 << 0); break;
				case 1: usb2_connection_ports = (USB2_CONNECTION_PORT_CONNECT_01 << 2); break;
				case 2: usb2_connection_ports = (USB2_CONNECTION_PORT_CONNECT_01 << 4); break;
				case 3: usb2_connection_ports = (USB2_CONNECTION_PORT_CONNECT_01 << 6); break;
			}
			goto FULL_RESET;			
		case USB2_CONNECTION_PORT_CONNECT_01:
			// go to full reset and set connection status to connect 2
			switch(port)
			{
				case 0: usb2_connection_ports = (USB2_CONNECTION_PORT_CONNECT_02 << 0); break;
				case 1: usb2_connection_ports = (USB2_CONNECTION_PORT_CONNECT_02 << 2); break;
				case 2: usb2_connection_ports = (USB2_CONNECTION_PORT_CONNECT_02 << 4); break;
				case 3: usb2_connection_ports = (USB2_CONNECTION_PORT_CONNECT_02 << 6); break;
			}
			goto FULL_RESET;			
		case USB2_CONNECTION_PORT_CONNECT_02:
			// go to partial reset and set connection status to disconnect
			switch(port)
			{
				case 0: usb2_connection_ports = (USB2_CONNECTION_PORT_DISCONNECT << 0); break;
				case 1: usb2_connection_ports = (USB2_CONNECTION_PORT_DISCONNECT << 2); break;
				case 2: usb2_connection_ports = (USB2_CONNECTION_PORT_DISCONNECT << 4); break;
				case 3: usb2_connection_ports = (USB2_CONNECTION_PORT_DISCONNECT << 6); break;
			}
			goto FULL_RESET;//PARTIAL_RESET;			
		case USB2_CONNECTION_PORT_INVALID:
			// go to error report and set connection status to disconnect
			switch(port)
			{
				case 0: usb2_connection_ports = (USB2_CONNECTION_PORT_DISCONNECT << 0); break;
				case 1: usb2_connection_ports = (USB2_CONNECTION_PORT_DISCONNECT << 2); break;
				case 2: usb2_connection_ports = (USB2_CONNECTION_PORT_DISCONNECT << 4); break;
				case 3: usb2_connection_ports = (USB2_CONNECTION_PORT_DISCONNECT << 6); break;
			}
			goto ERROR_REPORT;
	}
	
FULL_RESET:	
	// power the port?
	val |= xPORT_POW;
    mwrite(prs, 0x00, val);
	WaitMiliSecond(20);
	if ( mread(prs, 0x00) == 0 ) 
	{
	  	val = mread(prs, 0x00);
		val |= xPORT_POW;
    	mwrite(prs, 0x00, val);
	  	WaitMiliSecond(20);
	  	if ( mread(prs, 0x00) == 0 )
		{
	    	printk("port % bad reset\n", port);
			return FALSE;
		}
	}
	
	// we need to make sure that the status change bits are clear
	val = mread(prs, 0x00);
	val |= (xPORT_CHANGE_BITS | xPORT_POW);
    mwrite(prs, 0x00, val);
	
	// clear BIT(21) before the reset to prevent premature reset
	val = mread(prs, 0x00);
	val |= (~xPORT_PRC);
	val |= BIT(1); // clear ENABLE bit to disable state
    mwrite(prs, 0x00, val);
	WaitMiliSecond(50);
	
	// set bit 4 (USB2) or 31 (USB3) to reset the port
	val = mread(prs, 0x00);
	val |= xPORT_RESET;
    mwrite(prs, 0x00, val);

	// wait for bit 21 to set
	UINT_32 timeout = 500;
	
	val = mread(prs, 0x00);
	while (timeout) 
	{
		val = mread(prs, 0x00);
	  	if (val & xPORT_PRC)
	    	break;
		timeout--;
		WaitMiliSecond(1);
	}
	
	val = mread(prs, 0x00);
	if (timeout > 0) 
	{
		// reset recovery time
		WaitMiliSecond(100);
		
		// if after the reset, the enable bit is non zero, there was a successful reset/enable
		val = mread(prs, 0x00);
		if (val & xPORT_ED) 
        {
			WaitMiliSecond(100);
			
			// clear the status change bit(s)
			//.val = mread(prs, 0x00);
			//.val |= (xPORT_CHANGE_BITS | xPORT_POW);
    		//.mwrite(prs, 0x00, val);
			
			printk("Device at port % connected\n", port);
            goto SUCCESSFUL;   
        }
		else 
		{
			printk("Port is not Enabled\n");
			goto ERROR_REPORT;   
		}
	} 
	
	goto SUCCESSFUL;
	
PARTIAL_RESET:
	// partial reset code
	printk("Device at port % disconnected\n", port);
	goto SUCCESSFUL;
	
ERROR_REPORT:
	return FALSE;
	
SUCCESSFUL:
    return TRUE;

	         
	
	
//.	else
//.		goto TI_TUSB7340_QUIRK;
//.	
//.TI_TUSB7340_QUIRK:
//.	val = mread(prs, 0x00);
//.	val |= (xPORT_CHANGE_BITS | xPORT_WRC | xPORT_POW);
//.	val |= xPORT_RESET;
//.    mwrite(prs, 0x00, val);
//.
//.	timeout = 500;
//.	while (timeout) 
//.	{
//.		val = mread(prs, 0x00);
//.	  	if (val & xPORT_PRC)
//.	    	break;
//.		timeout--;
//.		WaitMiliSecond(1);
//.	}
//.	if (timeout > 0) 
//.	{
//.		WaitMiliSecond(3);
//.		val = mread(prs, 0x00);
//.		if (val & xPORT_ED)
//.		{
//.			val = mread(prs, 0x00);
//.			val |= (xPORT_CHANGE_BITS | xPORT_WRC | xPORT_POW);
//.			val |= xPORT_RESET;
//.    		mwrite(prs, 0x00, val);
//.		}
//.	} 
//.	else
//.	{
//.        printk("port % reset timed out\n", port);
//.		return FALSE;
//.	}

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int cnt = 0;
void read_extended_capabilities(XHCI* x)
{
	UINT_8  maxPort                    = (UINT_8)  ((x->cap->hcsparams1 & 0xFF000000) >> 24);
    UINT_32 extended_capability_offset = (UINT_32)(((x->cap->hccparams1 & 0xFFFF0000) >> 16) * 4);
	xHCI_EXTENDED_CAPABILITY* ex_cap  = (xHCI_EXTENDED_CAPABILITY*)((void*)(x->base_address_lo + extended_capability_offset));
	if(ex_cap == 0)
	{
		printk("error: zero xECP\n");
		return;
	}
	UINT_32 next = 0, i = 0, k = 0, entries = 0;
	
	char str[5];
	for(i=0;i<5;i++)
		str[i] = 0;
	
	while(entries < maxPort)
	{
		if((UINT_8)(ex_cap->DWORD0 & 0xFF) == 0x01)
			printk("found xECP with ID = 0x01 @ entry=%\n", entries);
		
		//printk("xECP ID=% @ entry=%\n", (UINT_32)( (UINT_8)(ex_cap->DWORD0 & 0xFF) ), entries);
		if((UINT_8)(ex_cap->DWORD0 & 0xFF) == 0x02)
		{
			//printk("found xECP with ID = 0x02 @ entry=%\n", entries);
			for(i=0;i<4;i++)
				str[i] = (char)((ex_cap->DWORD1 >> (i*8)) & 0xFF);
			
			//printk("protocol = ");
			//printk((char*)str);
			//printk("%.% -> ", (UINT_32)(((ex_cap->DWORD0)&0xFF000000)>>24), (UINT_32)(((ex_cap->DWORD0)&0x00FF0000)>>16));
			//printk("% ports @ offset %\n", (UINT_32)(((ex_cap->DWORD2)&0x0000FF00)>>8), (UINT_32)(((ex_cap->DWORD2)&0x000000FF)));
				
			UINT_8 portCount = ((ex_cap->DWORD2)&0x0000FF00)>>8;
			UINT_8 offset    = (ex_cap->DWORD2)&0x000000FF;
			UINT_8 usb2_or_usb3 = ((ex_cap->DWORD0)&0xFF000000)>>24;
			UINT_32 ports_usb2 = 0;
			UINT_32 ports_usb3 = 0;
		}
		
		if(((ex_cap->DWORD0 & 0xFF00)>>8) == 0)
		{
			//printk("end of xECP list reached @ entry=%\n", entries);
			return;
		}		
		next = 4*((ex_cap->DWORD0 & 0xFF00)>>8);
		ex_cap = (xHCI_EXTENDED_CAPABILITY*)((void*)((UINT_32)((void*)ex_cap) + next));
		entries++;
	}
	
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.BOOL __stop_legacy_mode(XHCI* x, UINT_32 entry)
//.{
//.	UINT_32 i = 0, next = 0;
//.	PxHCI_EXTENDED_CAPABILITY ext_cap = x->ext_cap;
//.	
//.	while(i != entry) 
//.	{
//.		next = 4*((ext_cap->DWORD0 & 0xFF00)>>8);
//.		ext_cap = (PxHCI_EXTENDED_CAPABILITY)((void*)((UINT_32)ext_cap + next));
//.		i++;
//.	}
//.	
//.	UINT_32 bios = *(UINT_32*)((void*)ext_cap);
//.	bios       |= xHC_xECP_LEGACY_OS_OWNED;
//.	*(UINT_32*)((void*)ext_cap) = bios;
//.
//.	UINT_32 timeout = 50;
//.	while (timeout--) 
//.	{
//.		if ((*(UINT_32*)((void*)ext_cap) & xHC_xECP_LEGACY_OWNED_MASK) == xHC_xECP_LEGACY_OS_OWNED)
//.			return TRUE;
//.		WaitMiliSecond(1);
//.	}
//.	return FALSE;
//.}
//.
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_get_trb(xHCI_TRB* trb, UINT_32 address) 
{
	trb->param   = (UINT_64)(mread(address, 0x00));
	trb->status  =          mread(address, 0x08);
	trb->command =          mread(address, 0x0C);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//.
//.void xhci_set_trb(xHCI_TRB* trb, UINT_32 address) 
//.{
//.	mwrite(address, 0, (UINT_32)trb->param);
//.	mwrite(address,  8, trb->status);
//.	mwrite(address, 12, trb->command);
//.}
//.
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_write_doorbell(XHCI* x, UINT_32 slot_id, UINT_32 val) 
{
	volatile UINT_32 doorbell = x->base_address_lo + x->cap->dboff;
    mwrite(doorbell, (slot_id * 4), val);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void NO_OP_test(XHCI* x)
{
	xHCI_TRB trb = { .param = 0ULL, .status = 0, .command = 0x00005C00 };
    
	mwrite(x->command_ring_Enqueue, 0x00, trb.param); 
	mwrite(x->command_ring_Enqueue, 0x04, 0x00);  
	mwrite(x->command_ring_Enqueue, 0x08, trb.status);                       
	mwrite(x->command_ring_Enqueue, 0x0C, (trb.command | x->command_ring_PCS));
	x->command_ring_Enqueue += sizeof(xHCI_TRB);

	UINT_32 cmnd = mread(x->command_ring_Enqueue, 0x0C);
	if (TRB_GET_TYPE(cmnd) == xLINK) 
	{
		mwrite(x->command_ring_Enqueue, 0x0C, ((cmnd & ~1) | x->command_ring_PCS));
		x->command_ring_Enqueue = x->driver_crcr;
		x->command_ring_PCS ^= 1;
	}
    
	signal_from_noop_to_irq = 1;
	xhci_write_doorbell(x, 0, 0);
	
	UINT_32 timer = 5000;
	while (timer) 
	{
		if(signal_from_irq_to_noop == 1)
		{
			signal_from_irq_to_noop = 0;
			signal_from_noop_to_irq = 0;
			break;
		}
		WaitMiliSecond(1);
		timer--;
	}
	if(timer == 0)
		printk("NOOP time out\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL start_xhci_controller(XHCI* x)
{
	UINT_32 i;
    UINT_32 base = x->base_address_lo;
    xx = x;                  

	x->usb_device->connected                  = FALSE;
	x->usb_device->speed                      = 0;
	x->usb_device->slot_configuration         = FALSE;
	x->usb_device->slot_configuration_command = FALSE;
	x->usb_device->slot_set_address           = FALSE;
	x->usb_device->slot_id                    = 255;
	x->usb_device->slot_address               = 0;
	x->usb_device->ep0_address                = 0;
	
	UINT_32 timeout = 50;
    UINT_32 cmd = mread(PHYSICAL_ADDRESS(x->oper), 0x00);
    cmd |= ~(xCMD_RUN);
	mwrite(PHYSICAL_ADDRESS(x->oper), 0x00, cmd);	
    while((mread(PHYSICAL_ADDRESS(x->oper), 0x04) & xSTS_HCH) == 0)
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
    cmd = mread(PHYSICAL_ADDRESS(x->oper), 0x00);
    cmd |= xCMD_RESET;
	mwrite(PHYSICAL_ADDRESS(x->oper), 0x00, cmd);	
	while((mread(PHYSICAL_ADDRESS(x->oper), 0x00) & xCMD_RESET) != 0)
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
	while((mread(PHYSICAL_ADDRESS(x->oper), 0x04) & xSTS_CNR) != 0)
	{
		if(timeout == 0)
		{
			printk("xHC is not ready after 1 sec\n");
			return FALSE;
		}
		WaitMiliSecond(1);
		timeout--;
	}
    
	// set bus master
    UINT_32 pciCommandRegister = Read((PCI*)x->pci, x->bus, x->device, x->function, 0x04);
    pciCommandRegister &= ~(BIT(2) | BIT(4));
    pciCommandRegister |=  (BIT(2) | BIT(4));
    Write((PCI*)x->pci, x->bus, x->device, x->function, 0x04, pciCommandRegister);
	
    // PANTHER POINT SWITCH
    if(
        ((Read((PCI*)x->pci, x->bus, x->device, x->function, 0) & 0xFFFF) == 0x8086) &&
        ((Read((PCI*)x->pci, x->bus, x->device, x->function, 2) & 0xFFFF) == 0x1E31) &&
        ((Read((PCI*)x->pci, x->bus, x->device, x->function, 8) & 0xFF)   == 4)
    )
    {
        if(xDEBUG)
            printk("Panther Point controller detected !!\n");
        Write((PCI*)x->pci, x->bus, x->device, x->function, 0xD4, 0x0000000F);
        Write((PCI*)x->pci, x->bus, x->device, x->function, 0xD0, 0x0000000F);
		Write((PCI*)x->pci, x->bus, x->device, x->function, 0xDC, 0x0000000F);
        Write((PCI*)x->pci, x->bus, x->device, x->function, 0xD8, 0x0000000F);
    }
    
    // set FLADJ
    //UINT_32 fladj = Read((PCI*)x->pci, x->bus, x->device, x->function, 0x61);
    //fladj &= ~0xFF;
    //fladj |= 0x20;
    Write((PCI*)x->pci, x->bus, x->device, x->function, 0x61, 0x20);

	if(xDEBUG)
        printk("XHCI reset successful\n");

	x->ex_cap          = (((x->cap->hccparams1 & 0xFFFF0000) >> 16) * 4);
	x->context_size    = (x->cap->hccparams1 & BIT(2)) ? 64 : 32;
	x->page_size       = (x->oper->pagesize & 0x0000FFFF) << 12;

	UINT_32 maxSlots    = (x->cap->hcsparams1 & 0x000000FF);

    // allocate the dcbaa, crcr and the slot contexts.
    void* mdcbaap = xhci_alloc_memory(4096, 64, x->page_size);
	void* mcrcr   = xhci_alloc_memory((CMND_RING_TRBS * sizeof(xHCI_TRB)), 64, 65536);
	__IMOS_MemZero(mdcbaap, 4096);               
	__IMOS_MemZero(mcrcr, CMND_RING_TRBS * sizeof(xHCI_TRB));
	 
	// setting up the scratchpad buffer
	UINT_8 MaxScratchpadBuffers = ((x->cap->hcsparams2 >> 27) & 0x1F) | ((x->cap->hcsparams2 >> 16) & 0xE0);
	if(MaxScratchpadBuffers > 0)
	{
		if(xDEBUG)
			printk("scratchpad buffers number = %\n", (UINT_32)MaxScratchpadBuffers);
		
		UINT_64* ScratchpadBuffersPtr = (UINT_64*)(xhci_alloc_memory(32*32, 64, x->page_size));
		__IMOS_MemZero((void*)ScratchpadBuffersPtr, 32*32);		   		   
		for(i=0; i<MaxScratchpadBuffers; i++)
		{
			UINT_32 tmp = (UINT_32)( xhci_alloc_memory(x->page_size, x->page_size, x->page_size) );
			__IMOS_MemZero((void*)tmp, x->page_size);
			ScratchpadBuffersPtr[i] = (UINT_64)tmp;
		}
		
		mwrite((UINT_32)mdcbaap, 0x00, (UINT_32)((void*)ScratchpadBuffersPtr));	
	}
    
    // setting up device contexts to maxSlot  
	for(i=1; i<(maxSlots+1); i++)
	{
		void* DevCntxSlots = xhci_alloc_memory(32 * x->context_size, 256, x->page_size);
		__IMOS_MemZero(DevCntxSlots, 32 * x->context_size);
        mwrite((UINT_32)mdcbaap, (0x08 * i), (UINT_32)DevCntxSlots);
	}
    
	// 64bit write
	mwrite(PHYSICAL_ADDRESS(x->oper), 0x30, (UINT_32)mdcbaap);	
	mwrite(PHYSICAL_ADDRESS(x->oper), 0x34, 0x00);

	//command ring Link TRB
	UINT_32 pos = (UINT_32)mcrcr + ((CMND_RING_TRBS - 1) * sizeof(xHCI_TRB));
	mwrite(pos, 0x00, (UINT_32)mcrcr);                 // param
	mwrite(pos, 0x04, 0);
	mwrite(pos, 0x08, 0);                             // status
	mwrite(pos, 0x0C, TRB_LINK_CMND);                 // command 
	
	// 64bit write
	mwrite(PHYSICAL_ADDRESS(x->oper), 0x18, ((UINT_32)mcrcr | TRB_CYCLE_ON));
    mwrite(PHYSICAL_ADDRESS(x->oper), 0x1C, 0x00);
	x->driver_crcr             = (UINT_32)mcrcr;
    x->command_ring_Enqueue    = (UINT_32)mcrcr;
    x->command_ring_PCS        = TRB_CYCLE_ON;
		
    // Configure Register
	mwrite(PHYSICAL_ADDRESS(x->oper), 0x38, maxSlots);
	
	// Device Notification Control (only bit 1 is allowed)
	mwrite(PHYSICAL_ADDRESS(x->oper), 0x14, BIT(1));
        
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
	x->current_event_ring_address  = (UINT_32)x->ers;
	__IMOS_MemZero(x->ers,  256 * sizeof(xHCI_TRB));
	__IMOS_MemZero(x->erst, sizeof(xHCI_EVENT_SEG_TABLE));
    
	xHCI_EVENT_SEG_TABLE* segment_table = (xHCI_EVENT_SEG_TABLE*)x->erst;
	segment_table->addr                 = (UINT_64)(0ULL | (UINT_32)x->ers);
	segment_table->size                 = 256;
	segment_table->resv                 = 0;
	x->current_event_ring_cycle         = 1;
	
	volatile UINT_32 interrupter0 = base + x->cap->rtsoff + 0x20;
        
	if(xDEBUG)
		printk("pri. interrupter:^\n", interrupter0); 
    
    mwrite(interrupter0, xHC_INTERRUPTER_IMAN,       (xHC_IMAN_IE | xHC_IMAN_IP)); 
	mwrite(interrupter0, xHC_INTERRUPTER_ERSTSZ,     1); 
    mwrite(interrupter0, xHC_INTERRUPTER_ERSTBA,     (UINT_32)x->erst); 
	mwrite(interrupter0, xHC_INTERRUPTER_ERSTBA + 4, 0);
	mwrite(interrupter0, xHC_INTERRUPTER_ERDP,       ((UINT_32)segment_table->addr | BIT(3))); 
	mwrite(interrupter0, xHC_INTERRUPTER_ERDP + 4,   0); 
    mwrite(interrupter0, xHC_INTERRUPTER_IMOD,       4000); 
    WaitMiliSecond(100);
        
    cmd = 0;
    cmd = mread(PHYSICAL_ADDRESS(x->oper), 0x00);
    cmd |= xCMD_INTE;
    mwrite(PHYSICAL_ADDRESS(x->oper), 0x00, cmd);
    WaitMiliSecond(100);
    
    cmd = 0;
    cmd = mread(PHYSICAL_ADDRESS(x->oper), 0x00);
    cmd |= xCMD_HSEE;
    mwrite(PHYSICAL_ADDRESS(x->oper), 0x00, cmd);
    WaitMiliSecond(100);
    
    // clear the status register
    UINT_32 st = mread(PHYSICAL_ADDRESS(x->oper), 4);
    st |= (BIT(2) | BIT(3) | BIT(4) | BIT(10));
    mwrite(PHYSICAL_ADDRESS(x->oper), 4, st); 
    
    // start the interrupter
	__irq_install_handler(x->irq_number, &xhci_interrupt_handler);
    
	// start the controller
    cmd = 0;
	cmd = mread(PHYSICAL_ADDRESS(x->oper), 0x00);
	cmd |= xCMD_RUN;   
	mwrite(PHYSICAL_ADDRESS(x->oper), 0x00, cmd);
	WaitMiliSecond(100);
	
	timeout = 50;
	while((mread(PHYSICAL_ADDRESS(x->oper), 0x04) & xSTS_HCH) != 0)
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
	while((mread(PHYSICAL_ADDRESS(x->oper), 0x04) & xSTS_CNR) != 0)
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

void* xhci_alloc_memory(UINT_32 byte, UINT_32 alignment, UINT_32 page_boundary)
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
    printk("  command trb pointer low: ^, command trb pointer high:^\n", (UINT_32)(trb->param & 0xFFFFFFF0), (UINT_32)(trb->param >> 32));
    printk("  completion code:% ", (UINT_32)((trb->status & 0xFF000000) >> 24));
	//.printk("  status:^\n", trb->status);
    printk("cycle bit:%\n", (UINT_32)(trb->command & BIT(0)));
    printk("  TRB type is ");
    UINT_32 trb_type_number = (UINT_32)((trb->command & 0xFC00) >> 10);
    printk(trb_types[trb_type_number].message_type);
    printk(" with id number %", trb_type_number);
	printk(", Slot id: %\n", (UINT_32)(trb->command >> 24));
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static BOOL this_TRB_to_print = FALSE;

void xhci_interrupt_handler(REGS* r)
{   
    volatile UINT_32 sts           = 0;
    volatile UINT_32 timeout       = 2000;
    volatile UINT_32 base          = xx->base_address_lo;
    volatile UINT_32 interrupter0  = base + xx->cap->rtsoff + 0x20;
	volatile UINT_32 iman          = mread(interrupter0, xHC_INTERRUPTER_IMAN);
    volatile UINT_32 driver_status = 0;
    
    if((iman & 3) != 3)
        return;
    if(xINTERRUPT_HANDLER_DEBUG) 
		printk("****************************************\n");    
    while(TRUE)
    {
        sts = mread(PHYSICAL_ADDRESS(xx->oper), 0x04);
        if(sts != 0)
        {
            if(xINTERRUPT_HANDLER_DEBUG) 
				printk("interrupt 0: sts:^, IMAN:^\n", sts, iman);
            driver_status = sts;
            mwrite(PHYSICAL_ADDRESS(xx->oper), 0x04, sts); 
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
    //report_commmand_completion_trb(&trb);
    
	sts = mread(PHYSICAL_ADDRESS(xx->oper), 0x04);
	iman  = mread(interrupter0, xHC_INTERRUPTER_IMAN);
    mwrite(PHYSICAL_ADDRESS(xx->oper), 0x04, sts);
    mwrite(interrupter0, xHC_INTERRUPTER_IMAN, (iman | (xHC_IMAN_IE | xHC_IMAN_IP)));
	if(xINTERRUPT_HANDLER_DEBUG) 
		printk("interrupt 1: sts:^, IMAN:^\n", sts, iman);
	
	// Did we detected a device attach?
    if(driver_status & xSTS_PCD)
    {
		// which port? get it from trb's PORT_ID (a.k.a. bits 24-31 of DWORD0). The reported port is one-based number i.e. 1, 2, 3 and should be converted to 0, 1, 2
        UINT_32 attached_port = ((UINT_32)trb.param) >> 24;
        attached_port--;
        if(xINTERRUPT_HANDLER_DEBUG) 
			printk("Port % activated\n", attached_port);  
        if(tmp_disconnection)
		{
			handle_device_attachment_to_port(xx, attached_port);
			tmp_disconnection = FALSE;
		}

		sts = mread(PHYSICAL_ADDRESS(xx->oper), 0x04);
		iman  = mread(interrupter0, xHC_INTERRUPTER_IMAN);
    	mwrite(PHYSICAL_ADDRESS(xx->oper), 0x04, sts);
    	mwrite(interrupter0, xHC_INTERRUPTER_IMAN, (iman | (xHC_IMAN_IE | xHC_IMAN_IP)));
		if(xINTERRUPT_HANDLER_DEBUG) 
			printk("interrupt 2: sts:^, IMAN:^\n", sts, iman);

		xx->current_event_ring_address += sizeof(xHCI_TRB);
    	mwrite(interrupter0, xHC_INTERRUPTER_ERDP, (xx->current_event_ring_address | BIT(3))); 
		mwrite(interrupter0, xHC_INTERRUPTER_ERDP + 4, 0);

		sts = mread(PHYSICAL_ADDRESS(xx->oper), 0x04);
		iman  = mread(interrupter0, xHC_INTERRUPTER_IMAN);
    	mwrite(PHYSICAL_ADDRESS(xx->oper), 0x04, sts);
    	mwrite(interrupter0, xHC_INTERRUPTER_IMAN, (iman | (xHC_IMAN_IE | xHC_IMAN_IP)));
		if(xINTERRUPT_HANDLER_DEBUG) 
			printk("interrupt 3: sts:^, IMAN:^\n", sts, iman);

		xx->usb_device->slot_configuration_command = TRUE;
		
		// for disconnection
		UINT_8 connection_status = 0;
		switch(xx->usb_device->port)
		{
			case 0: connection_status = (usb2_connection_ports & PORT_0) >> 0; break;
			case 1: connection_status = (usb2_connection_ports & PORT_1) >> 2; break;
			case 2: connection_status = (usb2_connection_ports & PORT_2) >> 4; break;
			case 3: connection_status = (usb2_connection_ports & PORT_3) >> 6; break;
		}
		switch(connection_status)
		{		
			case USB2_CONNECTION_PORT_CONNECT_02:
				goto XHCI_INTERRUPT_HANDLER_END;
		}
		// !
		xx->current_event_ring_address += sizeof(xHCI_TRB);
    	mwrite(interrupter0, xHC_INTERRUPTER_ERDP, (xx->current_event_ring_address | BIT(3))); 
		mwrite(interrupter0, xHC_INTERRUPTER_ERDP + 4, 0); 
		
		sts = mread(PHYSICAL_ADDRESS(xx->oper), 0x04);
		iman  = mread(interrupter0, xHC_INTERRUPTER_IMAN);
    	mwrite(PHYSICAL_ADDRESS(xx->oper), 0x04, sts);
    	mwrite(interrupter0, xHC_INTERRUPTER_IMAN, (iman | (xHC_IMAN_IE | xHC_IMAN_IP)));
		if(xINTERRUPT_HANDLER_DEBUG) 
			printk("interrupt 4: sts:^, IMAN:^\n", sts, iman);
		critical_event = 1; // keep it for slot_configuration set
    }
	else if((driver_status & xSTS_EINT))
	{
		if(signal_from_slot_configuration_to_irq == 1)
		{
			signal_from_irq_to_slot_configuration = 1;
			xx->usb_device->slot_id = (UINT_8)(trb.command >> 24);
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
		
		else if(signal_from_string_descriptor_to_irq == 1)
		{
			this_TRB_to_print = TRUE;
			signal_from_irq_to_string_descriptor = 1;
			critical_event = 0;
		}
		
		xhci_get_trb(&trb, xx->current_event_ring_address);
        if(this_TRB_to_print)
		{
			this_TRB_to_print = FALSE;
			report_commmand_completion_trb(&trb);	
		}
		
		xx->current_event_ring_address += sizeof(xHCI_TRB);
    	mwrite(interrupter0, xHC_INTERRUPTER_ERDP, (xx->current_event_ring_address | BIT(3))); 
		mwrite(interrupter0, xHC_INTERRUPTER_ERDP + 4, 0); 
	}
	else
	{
		printk("another interrupt: ^\n", driver_status);
	}
	
XHCI_INTERRUPT_HANDLER_END:
	if(xINTERRUPT_HANDLER_DEBUG) 
		printk("****************************************\n");
    return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void handle_device_attachment_to_port(XHCI* x, UINT_32 port)
{
	if( reset_port(x, port) )
    {
		WaitMiliSecond(150);
		UINT_8 speed;
        read_port_register_set(x, port, &speed);
		x->usb_device->connected = TRUE;
		x->usb_device->port      = port;
		x->usb_device->speed     = speed;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 cur_ep_ring_ptr   = 0;
UINT_32 cur_ep_ring_cycle = 0;
xHCI_SLOT_CONTEXT gslot;
xHCI_EP_CONTEXT   gep;

void xhci_slot_configuration(XHCI* x, UINT_32 port, UINT_32 speed)
{
	UINT_8 i;
	
	x->usb_device->slot_configuration_command = FALSE;
	//.if(xINTERRUPT_HANDLER_DEBUG) printk("slot config for port %, speed %\n", port, speed);
	xHCI_TRB trb = { .param = 0ULL, .status = 0, .command = (9 << 10)};
    mwrite(x->command_ring_Enqueue, 0x00, trb.param); 
	mwrite(x->command_ring_Enqueue, 0x04, 0x00);  
	mwrite(x->command_ring_Enqueue, 0x08, trb.status);                       
	mwrite(x->command_ring_Enqueue, 0x0C, (trb.command | x->command_ring_PCS));
	x->command_ring_Enqueue += sizeof(xHCI_TRB);
	UINT_32 cmnd = mread(x->command_ring_Enqueue, 0x0C);
	if (TRB_GET_TYPE(cmnd) == xLINK) 
	{
		mwrite(x->command_ring_Enqueue, 0x0C, ((cmnd & ~1) | x->command_ring_PCS));
		x->command_ring_Enqueue = x->driver_crcr;
		x->command_ring_PCS ^= 1;
	}
    
    signal_from_slot_configuration_to_irq = 1;
	xhci_write_doorbell(x, 0, 0);
    UINT_32 timing = 2000;
    while(timing)
    {
        volatile UINT_32 tmp = signal_from_irq_to_slot_configuration;
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
	volatile UINT_32 mdcbaap = mread(PHYSICAL_ADDRESS(x->oper), 0x30);
	volatile UINT_64* dcbaap = (UINT_64*)((void*)mdcbaap);
	void* ctx1_buffer       = (void*)((UINT_32)dcbaap[x->usb_device->slot_id]);
	
    xHCI_SLOT_CONTEXT* slot = (xHCI_SLOT_CONTEXT*)ctx1_buffer;
    xHCI_EP_CONTEXT*   ep   = (xHCI_EP_CONTEXT*) ((void*)((UINT_32)ctx1_buffer + x->context_size));

	slot->DWORD0               = ((1 << 27) | (speed << 20)); // i.e. 1 endpoint (EP0) + speed     
    slot->root_hub_port_number = port + 1;                    // root hub port number this device is attached to < values are 1 to MaxPort >
    slot->max_exit_latency     = 0;                           // calculated later
	slot->number_of_ports      = 0;    
	slot->TT_hub_slot_id       = 0;    
	slot->TT_port_number       = 0;    
    slot->WORD2                = 0;                           // interrupter 0
    slot->WORD_BYTE3[1]        = 0;
	slot->WORD_BYTE3[2]        = 0;
	slot->WORD_BYTE3[3]        = 0;
    slot->usb_device_address   = 0;
	
	// allocate the EP's Transfer Ring
	void* ep_tr_pointer = xhci_alloc_memory((256 * sizeof(xHCI_TRB)), 256, 65536);
	__IMOS_MemZero(ep_tr_pointer, (256 * sizeof(xHCI_TRB)));
	
	// make the last one a link TRB to point to the first one
	volatile UINT_32 pos = (UINT_32)ep_tr_pointer + ((255) * sizeof(xHCI_TRB));
	mwrite(pos, 0x00, (UINT_32)ep_tr_pointer);  
	mwrite(pos, 0x04, 0x00); 
	mwrite(pos, 0x08, 0x00);
	mwrite(pos, 0x0C, TRB_LINK_CMND);
	
  	ep->DWORD2  = ((UINT_32)ep_tr_pointer | TRB_CYCLE_ON); // TR dequeue pointer + dequeue pointer's CS
	ep->TR_dequeue_pointer_hi = 0x00;
	
	cur_ep_ring_ptr        = (UINT_32)(ep->DWORD2 & (~1));
	cur_ep_ring_cycle      = (UINT_32)((ep->DWORD2 & BIT(0)));
	
  	// set the initial values
  	ep->WORD0  = (0 << 10) | (0 << 15) | (0 << 0);
  	ep->BYTE1  = (3 << 1) | (4 << 3) | BIT(7);
  	ep->average_trb_length = 8;  // All CONTROL EP's shall have '8' (page 325)
  	ep->max_burst_size = 0;
	ep->max_packet_size = ((speed == 2) ? 8 : (speed == 1) ? 64 : (speed == 3) ? 64 : 512);
  	ep->interval = 0;
	
	x->usb_device->slot_address = (void*)slot;
	x->usb_device->ep0_address  = (void*)ep;
	
	gslot.DWORD0 = slot->DWORD0;
    gslot.root_hub_port_number = slot->root_hub_port_number;
	gslot.max_exit_latency = slot->max_exit_latency;
	gslot.number_of_ports = slot->number_of_ports;
	gslot.TT_hub_slot_id = slot->TT_hub_slot_id;
	gslot.TT_port_number = slot->TT_port_number;
    gslot.WORD2         = slot->WORD2;
    gslot.WORD_BYTE3[1] = slot->WORD_BYTE3[1];
	gslot.WORD_BYTE3[2]      = slot->WORD_BYTE3[2] ;    
	gslot.WORD_BYTE3[3]      = slot->WORD_BYTE3[3];     
    gslot.usb_device_address = slot->usb_device_address;
  	gep.DWORD2 = ep->DWORD2;
	gep.TR_dequeue_pointer_hi = ep->TR_dequeue_pointer_hi;
  	gep.WORD0 = ep->WORD0;
  	gep.BYTE1 = ep->BYTE1;
  	gep.average_trb_length = ep->average_trb_length;
  	gep.max_burst_size = ep->max_burst_size;
	gep.max_packet_size = ep->max_packet_size;
  	gep.interval = ep->interval;
    //if(xINTERRUPT_HANDLER_DEBUG) printk("slot address for configuration:^\n", (UINT_32)x->usb_device->slot_address);
    //if(xINTERRUPT_HANDLER_DEBUG) printk("ep   address for configuration:^\n", (UINT_32)x->usb_device->ep0_address);	

	x->usb_device->slot_configuration = TRUE;
	
	xhci_slot_set_address(x, TRUE);
	
	if( !reset_port(x, x->usb_device->port) )
    {
		printk("second device reset failed\n");
		return;
    }
	
	xhci_slot_set_address(x, FALSE);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_slot_release(XHCI* x, UINT_32 port, UINT_32 speed)
{
	printk("slot release\n");
	critical_event = 0;
}

void xhci_configure_endpoint(XHCI* x, UINT_32 inp)
{
	xHCI_TRB trb = { .param = (UINT_64)inp, .status = 0, .command = (((x->usb_device->slot_id) << 24) | (12 << 10))};
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

void xhci_setup_stage(XHCI* x, REQUEST_PACKET* request, UINT_8 dir) 
{
	//.if(xINTERRUPT_HANDLER_DEBUG) printk("inside setup stage: ep_TR_ring ^\n",  cur_ep_ring_ptr);
	//.if(xINTERRUPT_HANDLER_DEBUG) printk("inside setup stage: ep_TR_cycle %\n", cur_ep_ring_cycle);
	UINT_32 param_lo = (UINT_32)((request->value << 16) | (request->request << 8) | request->request_type);
	UINT_32 param_hi = (UINT_32) (request->length << 16 | request->index);
	
	mwrite(cur_ep_ring_ptr, 0x00, param_lo);
	mwrite(cur_ep_ring_ptr, 0x04, param_hi);
	mwrite(cur_ep_ring_ptr, 0x08, 0x08);
	mwrite(cur_ep_ring_ptr, 0x0C, ((3 << 16) | (2 << 10) | (1 << 6) | (0 << 5) | cur_ep_ring_cycle));	
	cur_ep_ring_ptr += sizeof(xHCI_TRB);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_data_stage(XHCI* x, UINT_32 addr, UINT_8 trb_type, UINT_32 size, UINT_8 direction, UINT_16 max_packet, UINT_32 status_addr)
{
	mwrite(cur_ep_ring_ptr, 0x00, addr); 
	mwrite(cur_ep_ring_ptr, 0x04, 0);    
	mwrite(cur_ep_ring_ptr, 0x08, (0 << 17) | (size << 0));
	mwrite(cur_ep_ring_ptr, 0x0C, ((1 << 16) | (3 << 10) | cur_ep_ring_cycle));
	cur_ep_ring_ptr += sizeof(xHCI_TRB);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_status_stage(XHCI* x, UINT_8 dir, UINT_32 status_addr) 
{
	mwrite(cur_ep_ring_ptr, 0x00, 0x00);
	mwrite(cur_ep_ring_ptr, 0x04, 0x00);
	mwrite(cur_ep_ring_ptr, 0x08, (0 << 22));
	mwrite(cur_ep_ring_ptr, 0x0C, ((0 << 16) | (4 << 10) | (1 << 5) | cur_ep_ring_cycle));
	cur_ep_ring_ptr += sizeof(xHCI_TRB);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_setup_data_status_stages(XHCI* x, void* target, UINT_32 length, UINT_32 max_packet, BOOL first_or_second) 
{
	UINT_32 status_addr = (UINT_32)(xhci_alloc_memory(4, 16, 16));    // we need a dword status buffer with a physical address
	UINT_32 buffer_addr = (UINT_32)(xhci_alloc_memory(18, 64, 65536));   // get a physical address buffer and then copy from it later
	__IMOS_MemZero((void*)status_addr, 4);
	__IMOS_MemZero((void*)buffer_addr, 18);

	REQUEST_PACKET packet = { STDRD_GET_REQUEST, GET_DESCRIPTOR, ((DEVICE << 8) | 0), 0, length };
	
	xhci_setup_stage (x, &packet, xHCI_DIR_IN);
	xhci_data_stage  (x, buffer_addr, xDATA_STAGE, length, xHCI_DIR_IN_B, max_packet, status_addr);
	xhci_status_stage(x, (xHCI_DIR_IN_B ^ 1), status_addr);

	signal_from_setup_data_status_stages_to_irq = 1;
	xhci_write_doorbell(x, x->usb_device->slot_id, 1);
	
	UINT_32 timing = 2000;
	while(timing)
    {
        volatile UINT_32 tmp = signal_from_irq_to_setup_data_status_stages;
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
	
	if(first_or_second)
		__IMOS_HexDump((void*)buffer_addr, 0x08, "xhci_first_8_bytes_of_Device_Descriptor");
	else
		__IMOS_HexDump((void*)buffer_addr, 0x12, "xhci_second_18_bytes_of_Device_Descriptor");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_string_descriptor(XHCI* x, UINT_8 max_packet, UINT_8 level) 
{
	UINT_8  length_of_the_descriptor = 0;
	UINT_32 buffer_addr              = 0;
	switch(level)
	{
		case 0:
			goto DEFAULT_STRING_DESCRIPTOR;
		case 1:
			goto MANUFACTURER_STRING_DESCRIPTOR;
		case 2:
			goto PRODUCT_STRING_DESCRIPTOR;
	}
	
DEFAULT_STRING_DESCRIPTOR:
	buffer_addr = (UINT_32)(xhci_alloc_memory(255, 64, 65536));
	__IMOS_MemZero((void*)buffer_addr, 255);

	REQUEST_PACKET packet = { STDRD_GET_REQUEST, GET_DESCRIPTOR, ((STRING << 8) | 0), 0, 255 };
	
	xhci_setup_stage (x, &packet, xHCI_DIR_IN);
	xhci_data_stage  (x, buffer_addr, xDATA_STAGE, 255, xHCI_DIR_IN_B, max_packet, 0);
	xhci_status_stage(x, (xHCI_DIR_IN_B ^ 1), 0);

	signal_from_string_descriptor_to_irq = 1;
	xhci_write_doorbell(x, x->usb_device->slot_id, 1);
	
	UINT_32 timing = 2000;
	while(timing)
    {
        volatile UINT_32 tmp = signal_from_irq_to_string_descriptor;
        if(tmp == 1)
        {
            signal_from_irq_to_string_descriptor = 0;
            signal_from_string_descriptor_to_irq = 0;
            break;
        }
        WaitMiliSecond(1);
        timing--;
        if(timing == 0)
        {
            printk("STRING DESCRIPTOR timed out\n");
			signal_from_string_descriptor_to_irq = 0;
            break;
        }
    }
	
	length_of_the_descriptor = *(UINT_8*)((void*)buffer_addr);
	__IMOS_HexDump((void*)buffer_addr, length_of_the_descriptor, "xhci_string_Descriptor");
	goto SUCCESS_STRING_DESCRIPTOR;
	
MANUFACTURER_STRING_DESCRIPTOR:
	return;

PRODUCT_STRING_DESCRIPTOR:
	return;
	
SUCCESS_STRING_DESCRIPTOR:
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void xhci_slot_set_address(XHCI* x, BOOL BSR)
{
	//printk("slot set address\n");
	UINT_32 input_context = (UINT_32)( xhci_alloc_memory(x->context_size * 32, 64, x->page_size) );
	__IMOS_MemZero((void*)input_context, (x->context_size * 32));
	mwrite(input_context, 0x00, 0x00); // Drops bits
	mwrite(input_context, 0x04, 0x03); // Adds  bits < only bits 0 and 1 are set >

    xHCI_SLOT_CONTEXT* slot = (xHCI_SLOT_CONTEXT*)x->usb_device->slot_address;
    xHCI_EP_CONTEXT*   ep   = (xHCI_EP_CONTEXT*)  x->usb_device->ep0_address;
	
	xHCI_SLOT_CONTEXT* input_context_slot = (xHCI_SLOT_CONTEXT*)((void*)(input_context + x->context_size));
	xHCI_EP_CONTEXT*   input_context_ep   = (xHCI_EP_CONTEXT*)  ((void*)(input_context + x->context_size + x->context_size));

	*input_context_slot = gslot;
	*input_context_ep   = gep;
	
	xHCI_TRB trb = { .param = (UINT_64)(input_context), .status = 0, .command = ((11 << 10) | ((x->usb_device->slot_id) << 24) | (BSR << 9)) };
	
    mwrite(x->command_ring_Enqueue, 0x00, trb.param); 
	mwrite(x->command_ring_Enqueue, 0x04, (UINT_32)(trb.param >> 32));  
	mwrite(x->command_ring_Enqueue, 0x08, trb.status);                       
	mwrite(x->command_ring_Enqueue, 0x0C, (trb.command | x->command_ring_PCS));
	x->command_ring_Enqueue += sizeof(xHCI_TRB);
	UINT_32 cmnd = mread(x->command_ring_Enqueue, 0x0C);
	if (TRB_GET_TYPE(cmnd) == xLINK) 
	{
		mwrite(x->command_ring_Enqueue, 0x0C, ((cmnd & ~1) | x->command_ring_PCS));
		x->command_ring_Enqueue = x->driver_crcr;
		x->command_ring_PCS ^= 1;
	}
    
    signal_from_slot_set_address_to_irq = 1;
	xhci_write_doorbell(x, 0, 0);
    UINT_32 timing = 2000;
    while(timing)
    {
        volatile UINT_32 tmp = signal_from_irq_to_slot_set_address;
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

	gslot.DWORD0             = slot->DWORD0;
    gslot.usb_device_address = slot->usb_device_address;
  	gep.WORD0                = ep->WORD0;
	gep.max_packet_size      = ep->max_packet_size;
	
	DEVICE_DESC dev_desc;
	if (BSR)
		xhci_setup_data_status_stages(x, (void*)(&dev_desc), 8, ep->max_packet_size, TRUE);
	else 
	{
		xhci_setup_data_status_stages(x, (void*)(&dev_desc), 18, ep->max_packet_size, FALSE);
		xhci_string_descriptor(x, ep->max_packet_size, 0);
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

XHCI* xhci_instance_to_idt(void)
{
	return xx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_8 xhci_get_critical_event(void)
{
	return critical_event ;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//. RESET_ENDPOINT
//. 	xHCI_TRB trb = { .param = 0ULL, .status = 0, .command = ((1 << 24) | (1 << 16) | (14 << 10) | (0 << 9) )};
//.     mwrite(x->command_ring_Enqueue, 0x00, trb.param); 
//. 	mwrite(x->command_ring_Enqueue, 0x04, 0x00);  
//. 	mwrite(x->command_ring_Enqueue, 0x08, trb.status);                       
//. 	mwrite(x->command_ring_Enqueue, 0x0C, (trb.command | x->command_ring_PCS));
//. 	x->command_ring_Enqueue += sizeof(xHCI_TRB);
//. 	xhci_write_doorbell(x, 0, 0);
