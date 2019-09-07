#include "../include/PCI.h"
#include "../include/EHCI.h"
#include "../include/MEMORY.h"
#include "../include/TIMER.h"
#include "../include/PRINT.h"

#define HEAP_START                0x01000000           // at 16 meg must be a multiple of a meg
#define HEAP_SIZE                 ((1048576 * 16) - 1) // 16 meg (needs to be a multiple of 4k) (size needs to be total_size - 1)
#define SUCCESS                   0
#define ERROR_STALLED            -1
#define ERROR_DATA_BUFFER_ERROR  -2
#define ERROR_BABBLE_DETECTED    -3
#define ERROR_NAK                -4
#define ERROR_TIME_OUT          254
#define ERROR_UNKNOWN           255

#define DEV_TO_HOST     0x80
#define HOST_TO_DEV     0x00
#define REQ_TYPE_STNDRD 0x00
#define REQ_TYPE_CLASS  0x20
#define REQ_TYPE_VENDOR 0x40
#define REQ_TYPE_RESV   0x60
#define RECPT_DEVICE    0x00
#define RECPT_INTERFACE 0x01
#define RECPT_ENDPOINT  0x02
#define RECPT_OTHER     0x03
#define STDRD_GET_REQUEST   (DEV_TO_HOST | REQ_TYPE_STNDRD | RECPT_DEVICE)
#define STDRD_SET_REQUEST   (HOST_TO_DEV | REQ_TYPE_STNDRD | RECPT_DEVICE)
#define STDRD_SET_INTERFACE (HOST_TO_DEV | REQ_TYPE_STNDRD | RECPT_INTERFACE)


enum { GET_STATUS=0, CLEAR_FEATURE, SET_FEATURE=3, SET_ADDRESS=5, GET_DESCRIPTOR=6, SET_DESCRIPTOR,
        GET_CONFIGURATION, SET_CONFIGURATION,
        GET_INTERFACE, SET_INTERFACE,
        SYNCH_FRAME,
        GET_MAX_LUNS = 0xFE, BULK_ONLY_RESET
};

enum { 
  DEVICE=1, 
  CONFIG, 
  STRING, 
  INTERFACE, 
  ENDPOINT, 
  DEVICE_QUALIFIER,
  OTHER_SPEED_CONFIG, 
  INTERFACE_POWER, 
  OTG, 
  DEBUG, 
  INTERFACE_ASSOSIATION,
  HID=0x21,
  HID_REPORT, 
  HID_PHYSICAL, 
  INTERFACE_FUNCTION = 0x24,
  ENDPOINT_FUNCTION,
  HUB=0x29
};

typedef struct _DEVICE_DESC {
	UINT_8  len;
	UINT_8  type;
	UINT_16 usb_ver;
	UINT_8  _class;
	UINT_8  subclass;
	UINT_8  protocol;
	UINT_8  max_packet_size;
	UINT_16 vendorid;
	UINT_16 productid;
	UINT_16 device_rel;
	UINT_8  manuf_indx;   // index value
	UINT_8  prod_indx;    // index value
	UINT_8  serial_indx;  // index value
	UINT_8  configs;      // Number of configurations
} __attribute__ ((packed)) DEVICE_DESC;

typedef struct _REQUEST_PACKET {
	UINT_8  request_type;
	UINT_8  request;
	UINT_16 value;
	UINT_16 index;
	UINT_16 length;
} __attribute__ ((packed)) REQUEST_PACKET;

typedef struct _CONFIG_DESC {
	UINT_8  len;
	UINT_8  type;
	UINT_16 tot_len;
	UINT_8  num_interfaces;
	UINT_8  config_val;
	UINT_8  config_indx;
	UINT_8  bm_attrbs;
	UINT_8  max_power;
} __attribute__ ((packed)) CONFIG_DESC;

BOOL    process_ehci           (EHCI*);
void    ehci_init_stack_frame  (const UINT_32);
BOOL    ehci_stop_legacy       (EHCI*, const UINT_32);
BOOL    ehci_enable_async_list (const BOOL);
BOOL    ehci_handshake         (const UINT_32, const UINT_32, const UINT_32, UINT_32);
BOOL    ehci_reset_port        (const INT_32);
BOOL    ehci_get_descriptor    (const INT_32);
void    ehci_clear_phy_mem     (const UINT_32, const INT_32);
void    ehci_copy_to_phy_mem   (const UINT_32, void*, const INT_32);
void    ehci_copy_from_phy_mem (void*, const UINT_32, const INT_32);
void    ehci_write_phy_mem     (const UINT_32, UINT_32);
UINT_32 ehci_read_phy_mem      (const UINT_32);
BOOL    ehci_set_address       (const UINT_8, const UINT_8);
BOOL    ehci_control_in        (void*, const INT_32, const INT_32, const UINT_8);
void    ehci_queue             (UINT_32, const UINT_32, const UINT_8, const UINT_16, const UINT_8);
INT_32  ehci_setup_packet      (const UINT_32, UINT_32);
INT_32  ehci_packet            (UINT_32, const UINT_32, UINT_32, const UINT_32, const BOOL, UINT_8, const UINT_8, const UINT_16);
void    ehci_insert_queue      (UINT_32, const UINT_8);
BOOL    ehci_remove_queue      (UINT_32);
INT_32  ehci_wait_interrupt    (UINT_32, const UINT_32, BOOL *);
void    ehci_write_cap_reg     (const UINT_32, const UINT_32);
void    ehci_write_op_reg      (const UINT_32, const UINT_32);
UINT_32 ehci_read_cap_reg      (const UINT_32);
UINT_32 ehci_read_op_reg       (const UINT_32);


UINT_32 async_base;
UINT_32 op_base_off, hccparams, hcsparams;
UINT_8  num_ports;
void*  ehci_bar0 = 0;

BOOL process_ehci(PEHCI ehci)
{
	int timeout, i;
	
	// allow access to data (memmapped IO)
	PPCI pci = (PPCI)ehci->pci_pointer;
	Write(pci, ehci->bus, ehci->device, ehci->function, 0x04, 0x0006);
	
	ehci_bar0 = ehci->bar0;
	
	UINT_32 cap_len = *((UINT_32*)ehci_bar0);
	op_base_off = (UINT_32)((UINT_32)ehci_bar0 + cap_len);
	
	timeout = 500;
	ehci_write_op_reg(EHC_OPS_USBCommand, (1<<1));
	while (ehci_read_op_reg(EHC_OPS_USBCommand) & (1<<1))
	{
		WaitMiliSecond(1);
		if (timeout-- == 0)
			return FALSE;
	}
	
	/* if we get here, we have a valid EHCI controller, so set it up */
	//printk("A valid EHCI controller detected\n");
	
	hcsparams = *(UINT_32*)(ehci->bar0 + EHC_CAPS_HCSParams);
	hccparams = *(UINT_32*)(ehci->bar0 + EHC_CAPS_HCCParams);
	
	// Turn off legacy support for Keyboard and Mice
	if (!ehci_stop_legacy(ehci, hccparams)) 
	{
		printk("BIOS did not release Legacy support\n");
		return FALSE;
	}
	
	// get num_ports from EHCI's HCSPARAMS register
	num_ports = (UINT_8) (hcsparams & 0x0F);  // at least 1 and no more than 15
	//.printk("Found % root hub ports\n", num_ports);
	
	// allocate then initialize the async queue list (Control and Bulk TD's)
	async_base = (UINT_32)( Alloc(16 * EHCI_QUEUE_HEAD_SIZE, 32) );
	__IMOS_MemZero((void*)async_base, 16 * EHCI_QUEUE_HEAD_SIZE);
	ehci_init_stack_frame(async_base);
	
	// set and start the Host Controllers schedule
	if ((hccparams & (1<<0)) == 1)
		ehci_write_op_reg(EHC_OPS_CtrlDSSegemnt, 0x00000000); // we use only 32-bit addresses
	ehci_write_op_reg(EHC_OPS_PeriodicListBase, 0x00000000);  // physical address
	ehci_write_op_reg(EHC_OPS_AsyncListBase, async_base);     // physical address
	ehci_write_op_reg(EHC_OPS_FrameIndex, 0);                 // start at (micro)frame 0
	ehci_write_op_reg(EHC_OPS_USBInterrupt, 0);               // disallow interrupts
	ehci_write_op_reg(EHC_OPS_USBStatus, 0x3F);               // clear any pending interrupts
	
	// start the host controller: 8 micro-frames, start schedule (frame list size = 1024)
	ehci_write_op_reg(EHC_OPS_USBCommand, (8<<16) | (1<<0));
	
	// enable the asynchronous list
	
	UINT_32 cmd = ehci_read_op_reg(EHC_OPS_USBCommand);
	ehci_write_op_reg(EHC_OPS_USBCommand, 2 | cmd & ~((1<<5) | (1<<4) | (1<<0)));
	WaitMiliSecond(100);
	ehci_read_op_reg(EHC_OPS_USBCommand);
	while(ehci_read_op_reg(EHC_OPS_USBCommand) & 2);
	WaitMiliSecond(100);
	ehci_read_op_reg(EHC_OPS_USBCommand);
	while(!(ehci_read_op_reg(EHC_OPS_USBStatus) & (1<<12)));
	
	printk("EHCI reset...\n");
	
	// Setting bit 0 in the ConfigFlags reg tells all ports to use the EHCI controller.
	ehci_write_op_reg(EHC_OPS_ConfigFlag, 1);
	
	// if we have control to change the port power, we need to power each port to 1
	if (hcsparams & (1<<4))
		for (i=0; i<num_ports; i++)
			ehci_write_op_reg(EHC_OPS_PortStatus + (i * 4), ehci_read_op_reg(EHC_OPS_PortStatus + (i * 4)) | EHCI_PORT_PP);
	
	// after powering a port, we must wait 20mS before using it.
	WaitMiliSecond(25);
	
	// we should be ready to detect any ports that are occupied
	for (i=0; i<num_ports; i++) 
	{
		// power and reset the port
		if (ehci_reset_port(i))
		{
			printk("port % reset\n", i);
			
			// if the reset was good, get the descriptor
			ehci_get_descriptor(i);
		}
	}
	
	return 0;
}

BOOL ehci_reset_port(const int port) 
{
	//UINT_32* _pport = (UINT_32*)(op_base_off + (17 * 4));
	//UINT_32   pport = _pport[port];
//
    //UINT_32 status = pport;
    //status |= (1 << 8);
    //status &= ~((1<<1) | (1<<3) | (1<<5));
    //pport = status;
	//
	//WaitMiliSecond(50);
	//
    //status = pport;
    //status &= ~((1<<1) | (1<<3) | (1<<5));
    //status &= ~(1 << 8);
    //status |= ((1<<1) | (1<<3) | (1<<5)) & (1 << 8);
    //pport = status;
//
    //UINT_32 i;
    //for (i=0; i<10; i++)
    //{
    //    // Delay
    //    WaitMiliSecond(10);
//
    //    // Get current status
    //    status = pport;
//
    //    // Check if device is attached to port
    //    if (~status & (1 << 0))
    //        break;
//
    //    // Acknowledge change in status
    //    if (status & ((1<<3) | (1<<1)))
    //    {
	//		status = pport;
    //		status &= ~((1<<1) | (1<<3) | (1<<5));
    //		status &= ~((1<<1) | (1<<3));
    //		status |= ((1<<1) | (1<<3) | (1<<5)) & ((1<<1) | (1<<3));
    //		pport = status;		
    //        continue;
    //    }
//
    //    // Check if device is enabled
    //    if (status & (1<<2))
	//		break;
    //}
//
    //return status;
	
	BOOL ret = FALSE;
	UINT_32 HCPortStatusOff = EHC_OPS_PortStatus + (port * 4);
	UINT_32 dword;
	
	// Clear the enable bit and status change bits (making sure the PP is set)
	ehci_write_op_reg(HCPortStatusOff, EHCI_PORT_PP | EHCI_PORT_OVER_CUR_C | EHCI_PORT_ENABLE_C | EHCI_PORT_CSC);
	
	// read the port and see if a device is attached
	// if device attached and is a hs device, the controller will set the enable bit.
	// if the enable bit is not set, then there was an error or it is a low- or full-speed device.
	// if bits 11:10 = 01b, then it isn't a high speed device anyway, skip the reset.
	dword = ehci_read_op_reg(HCPortStatusOff);
	if ((dword & EHCI_PORT_CCS) && (((dword & EHCI_PORT_LINE_STATUS) >> 10) != 0x01)) 
	{
		// set bit 8 (writing a zero to bit 2)
		ehci_write_op_reg(HCPortStatusOff, EHCI_PORT_PP | EHCI_PORT_RESET);
		WaitMiliSecond(100);  // at least 50 ms for a root hub
	
		// clear the reset bit leaving the power bit set
		ehci_write_op_reg(HCPortStatusOff, EHCI_PORT_PP);
		WaitMiliSecond(100);
	}
	
	dword = ehci_read_op_reg(HCPortStatusOff);
	if (dword & EHCI_PORT_CCS) 
	{
		//..  // if after the reset, the enable bit is set, we have a high-speed device
		//..  if (dword & EHCI_PORT_ENABLED) 
		//..  {
		//..  	// Found a high-speed device.
		//..  	// clear the status change bit(s)
		//..  	ehci_write_op_reg(HCPortStatusOff, ehci_read_op_reg(HCPortStatusOff) & EHCI_PORT_WRITE_MASK);
		//..  	return TRUE;
		//..  } 
		//..  else 
		//..  {
		//..  	printk("Found a low- or full-speed device\n");
		//..  	// disable and power off the port
		//..  	ehci_write_op_reg(HCPortStatusOff, 0);
		//..  	WaitMiliSecond(10);
		//..  
		//..  	// the next two lines are not necassary for this utility, but they remain included
		//..  	//  to show what you would need to do to release ownership of the port.
		//..  	ehci_write_op_reg(HCPortStatusOff, EHCI_PORT_OWNER);
		//..  	// wait for the owner bit to actually be set, and the ccs bit to clear
		//..  	ehci_handshake(HCPortStatusOff, (EHCI_PORT_OWNER | EHCI_PORT_CCS), EHCI_PORT_OWNER, 25);
		//..  }
		
		ehci_write_op_reg(HCPortStatusOff, ehci_read_op_reg(HCPortStatusOff) & EHCI_PORT_WRITE_MASK);
		return TRUE;
	}
	return FALSE;
}

BOOL ehci_get_descriptor(const int port) 
{
	DEVICE_DESC dev_desc;
	__IMOS_MemZero(&dev_desc, 18);
	/*
	* Since most high-speed devices will only work with a max packet size of 64,
	*  we don't request the first 8 bytes, then set the address, and request
	*  the all 18 bytes like the uhci/ohci controllers.  However, I have included
	*  the code below just to show how it could be done.
	*/
	UINT_8 max_packet  = 64;
	UINT_8 dev_address = 1;
	
	// send the "get_descriptor" packet (get 18 bytes)
	if (!ehci_control_in(&dev_desc, 18, max_packet, 0))
	{
		UINT_8* tmp = (UINT_8*)(void*)(&dev_desc);
		printk("Device Desc= ^, ^\n", tmp[0], tmp[1]);
		return 0;
	}
		return FALSE;
	
	// reset the port
	ehci_reset_port(port);
	
	// set address
	ehci_set_address(max_packet, dev_address);
	
	// get the whole packet.
	__IMOS_MemZero(&dev_desc, 18);
	if (!ehci_control_in(&dev_desc, 18, max_packet, dev_address))
		return FALSE;
	
	// print the descriptor
	printk("Found Device Descriptor:\n"
		   "              len: %\n"
		   "             type: %\n"
		   "          version: ^\n"
		   "            class: %\n"
		   "         subclass: %\n"
		   "         protocol: %\n"
		   "  max packet size: %\n"
		   "        vendor id: ^\n"
		   "       product id: ^\n"
		   "      release ver: %\n"
		   "manufacture index: %\n"
		   "    product index: %\n"
		   "     serial index: %\n"
		   "number of configs: %\n",
		dev_desc.len, dev_desc.type, dev_desc.usb_ver >> 8, dev_desc.usb_ver & 0xFF, dev_desc._class, dev_desc.subclass, 
		dev_desc.protocol, dev_desc.max_packet_size, dev_desc.vendorid, dev_desc.productid, 
		(dev_desc.device_rel & 0xF000) >> 12, (dev_desc.device_rel & 0x0F00) >> 8,
		(dev_desc.device_rel & 0x00F0) >> 4,  (dev_desc.device_rel & 0x000F) >> 0,
		dev_desc.manuf_indx, dev_desc.prod_indx, dev_desc.serial_indx, dev_desc.configs
	);
	return TRUE;
}

// enable/disable one of the lists.
// if the async member is set, it disables/enables the asynchronous list, else the periodic list
BOOL ehci_enable_async_list(const BOOL enable) 
{
	UINT_32 command;
	
	// first make sure that both bits are the same
	// should not modify the enable bit unless the status bit has the same value
	command = ehci_read_op_reg(EHC_OPS_USBCommand);
	WaitMiliSecond(500);
	if (ehci_handshake(EHC_OPS_USBStatus, (1<<15), (command & (1<<5)) ? (1<<15) : 0, 800))
	{
		printk("both Cmd and Sts have the same value\n");
		if (!(command & (1<<5)))
		{
			printk("inside second IF in enable_list\n");
			ehci_write_op_reg(EHC_OPS_USBCommand, command | (1<<5));
		}
		printk("just before returning in enable_list\n");
		return ehci_handshake(EHC_OPS_USBStatus, (1<<15), (1<<15), 800);
	}
	printk("main handshake failed\n");
	return FALSE;
}

/* This routine waits for the value read at (base, reg) and'ed by mask to equal result.
 * It returns TRUE if this happens before the alloted time expires
 * returns FALSE if this does not happen
 */
BOOL ehci_handshake(const UINT_32 reg, const UINT_32 mask, const UINT_32 result, unsigned ms) 
{
	do {
		if ((ehci_read_op_reg(reg) & mask) == result)
			return TRUE;
		WaitMiliSecond(10);
	} while (ms-=10);
	printk("handshake failed\n");
	return FALSE;
}

// initialize the async queue list (Control and Bulk TD's)
void ehci_init_stack_frame(const UINT_32 async_base) 
{
	int i;
	UINT_32 cur_addr = async_base;
	
	// the async queue (Control and Bulk TD's) is a round robin set of 16 Queue Heads.
	for (i=0; i<16; i++) 
	{
		ehci_write_phy_mem(cur_addr + EHCI_QH_OFF_HORZ_PTR, (cur_addr + EHCI_QUEUE_HEAD_SIZE) | QH_HS_TYPE_QH | QH_HS_T0);
		ehci_write_phy_mem(cur_addr + EHCI_QH_OFF_ENDPT_CAPS, (0 << 16) | ((i==0) ? (1<<15) : (0<<15)) | QH_HS_EPS_HS | (0<<8) | 0);
		ehci_write_phy_mem(cur_addr + EHCI_QH_OFF_HUB_INFO, (1<<30));
		ehci_write_phy_mem(cur_addr + EHCI_QH_OFF_NEXT_QTD_PTR, QH_HS_T1);
		ehci_write_phy_mem(cur_addr + EHCI_QH_OFF_ALT_NEXT_QTD_PTR, QH_HS_T1);
		cur_addr += EHCI_QUEUE_HEAD_SIZE;
	}
	
	// backup and point the last one at the first one
	cur_addr -= EHCI_QUEUE_HEAD_SIZE;
	ehci_write_phy_mem(cur_addr + EHCI_QH_OFF_HORZ_PTR, (async_base | QH_HS_TYPE_QH | QH_HS_T0));
}

BOOL ehci_stop_legacy(PEHCI ehci, const UINT_32 params) 
{
	UINT_32 eecp = (params >> 8) & 0xFF;
	//.printk("params= ^\n", (UINT_32)params);
	//.printk("eecp= ^\n", (UINT_32)eecp);
	UINT_32 legsup;
    int msec = 1000;
    if (eecp >= 0x40)
    {
        // Disable BIOS legacy support
        legsup = Read((PPCI)ehci->pci_pointer, ehci->bus, ehci->device, ehci->function, eecp);
        if (legsup & USBLEGSUP_HC_BIOS)
        {
            //printk("    In EHCI: disabling BIOS legacy support...");
            Write((PPCI)ehci->pci_pointer, ehci->bus, ehci->device, ehci->function, eecp, 0x01000000);
            while(msec > 0)
            {
                legsup = Read((PPCI)ehci->pci_pointer, ehci->bus, ehci->device, ehci->function, eecp);
                WaitMiliSecond(10);
                msec -= 10;
                if ((legsup & USBLEGSUP_HC_BIOS == 0))
                    break;
            }
            if (legsup & USBLEGSUP_HC_BIOS)
            {
                //printk("disabling BIOS legacy support FAILED!\n");
                return FALSE;
            }
            //else
            //   printk("disabling BIOS legacy support DONE!\n");
        }
    }
	return TRUE;
}

BOOL ehci_set_address(const UINT_8 max_packet, const UINT_8 address) 
{
	// allocate enough memory to hold the out_packet, queue and the TD's
	UINT_32 packet = (UINT_32)( Alloc(64 + EHCI_QUEUE_HEAD_SIZE + (2 * EHCI_TD_SIZE), 64) );
	UINT_32 queue  = packet + 64;
	UINT_32 td0    = queue + EHCI_QUEUE_HEAD_SIZE;
	
	static UINT_8 temp[8] = { 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	temp[2] = address;
	
	ehci_copy_to_phy_mem(packet, temp, sizeof(REQUEST_PACKET));
	
	ehci_queue(queue, td0, 0, max_packet, 0);
	ehci_setup_packet(td0, packet);
	ehci_packet(td0 + EHCI_TD_SIZE, 0, 0, 0, TRUE, 1, EHCI_TD_PID_IN, max_packet);
	
	ehci_insert_queue(queue, QH_HS_TYPE_QH);
	int ret = ehci_wait_interrupt(td0, 2000, NULL);
	ehci_remove_queue(queue);
	
	return (ret == SUCCESS);
}

BOOL ehci_control_in(void *targ, const int len, const int max_packet, const UINT_8 address) 
{
	// allocate enough memory to hold the packet, queue, and the TD's
	UINT_32 packet = (UINT_32)( Alloc(64 + EHCI_QUEUE_HEAD_SIZE + (16 * EHCI_TD_SIZE), 64) );
	UINT_32 queue  = packet + 64;
	UINT_32 td0    = queue + EHCI_QUEUE_HEAD_SIZE;
	
	UINT_32 buffer_addr = (UINT_32)( Alloc(256, 1) ); 
	
	static REQUEST_PACKET temp = { STDRD_GET_REQUEST, GET_DESCRIPTOR, ((0x29 << 8) | 0), 0, 0 }; // 0x29 must be DEVICE ?????
	temp.length = len;
	
	// copy the request packet from local memory to physical address memory
	ehci_copy_to_phy_mem(packet, &temp, sizeof(REQUEST_PACKET));
	
	BOOL spd = 0;
	const int last = 1 + ((len + (max_packet-1)) / max_packet);
	
	ehci_queue(queue, td0, 0, max_packet, address);
	ehci_setup_packet(td0, packet);
	ehci_packet(td0 + EHCI_TD_SIZE, td0 + (last * EHCI_TD_SIZE), buffer_addr, len, FALSE, 1, EHCI_TD_PID_IN, max_packet);
	ehci_packet(td0 + (last * EHCI_TD_SIZE), NULL, NULL, 0, TRUE, 1, EHCI_TD_PID_OUT, max_packet);
	
	ehci_insert_queue(queue, QH_HS_TYPE_QH);
	int ret = ehci_wait_interrupt(td0, 2000, &spd);
	ehci_remove_queue(queue);
	
	if (ret == SUCCESS) 
	{
		// now copy from the physical buffer to the specified buffer
		ehci_copy_from_phy_mem(targ, buffer_addr, len);
		return TRUE;
	}
	
	return FALSE;
}

void ehci_queue(UINT_32 addr, const UINT_32 qtd, UINT_8 endpt, const UINT_16 mps, const UINT_8 address) 
{
	int i;
	
	// clear it to zeros
	ehci_clear_phy_mem(addr, EHCI_QUEUE_HEAD_SIZE);
	
	ehci_write_phy_mem(addr + EHCI_QH_OFF_HORZ_PTR, 1);
	ehci_write_phy_mem(addr + EHCI_QH_OFF_ENDPT_CAPS, (8<<28) | ((mps & 0x7FF) << 16) | (0<<15) | (1<<14) | (2<<12) | ((endpt & 0x0F) << 8) | (0<<7) | (address & 0x7F));
	ehci_write_phy_mem(addr + EHCI_QH_OFF_HUB_INFO, (1<<30) | (0<<23) | (0<<16));
	ehci_write_phy_mem(addr + EHCI_QH_OFF_NEXT_QTD_PTR, qtd);
}

int ehci_setup_packet(const UINT_32 addr, UINT_32 request) 
{
	int i;
	// clear it to zeros
	ehci_clear_phy_mem(addr, EHCI_TD_SIZE);
	
	ehci_write_phy_mem(addr + EHCI_TD_OFF_NEXT_TD_PTR, addr + EHCI_TD_SIZE);
	ehci_write_phy_mem(addr + EHCI_TD_OFF_ALT_NEXT_QTD_PTR, QH_HS_T1);
	ehci_write_phy_mem(addr + EHCI_TD_OFF_STATUS, (0<<31) | (8<<16) | (0<<15) | (0<<12) | (3<<10) | (EHCI_TD_PID_SETUP<<8) | 0x80);
	ehci_write_phy_mem(addr + EHCI_TD_OFF_BUFF0_PTR, request);
	request = (request + 0x1000) & ~0x0FFF;
	ehci_write_phy_mem(addr + EHCI_TD_OFF_BUFF1_PTR, request);
	ehci_write_phy_mem(addr + EHCI_TD_OFF_BUFF2_PTR, request + 0x1000);
	ehci_write_phy_mem(addr + EHCI_TD_OFF_BUFF3_PTR, request + 0x2000);
	ehci_write_phy_mem(addr + EHCI_TD_OFF_BUFF4_PTR, request + 0x3000);
	
	return 1;
}

int ehci_packet(UINT_32 addr, const UINT_32 status_qtd, UINT_32 buffer, const UINT_32 size, const BOOL last, UINT_8 data0, const UINT_8 dir, const UINT_16 mps) 
{
	int cnt = 0, i;
	int sz = size;
	int max_size = (0x1000 - (addr & 0x0FFF)) + (4 * 0x1000);
	if (max_size > mps)
		max_size = mps;
	UINT_32 current = addr;
	
	do {
		// clear it to zeros
		ehci_clear_phy_mem(current, EHCI_TD_SIZE);
	
		ehci_write_phy_mem(current + EHCI_TD_OFF_NEXT_TD_PTR, (current + EHCI_TD_SIZE) | ((last && (sz <= max_size)) ? QH_HS_T1 : 0));
		ehci_write_phy_mem(current + EHCI_TD_OFF_ALT_NEXT_QTD_PTR, (!status_qtd) ? QH_HS_T1 : status_qtd);
		ehci_write_phy_mem(current + EHCI_TD_OFF_STATUS, (data0<<31) | (((sz < max_size) ? sz : max_size)<<16) | (0<<15) | (0<<12) | (3<<10) | (dir<<8) | 0x80);
		ehci_write_phy_mem(current + EHCI_TD_OFF_BUFF0_PTR, buffer);
		if (buffer)
		{
			UINT_32 buff = (buffer + 0x1000) & ~0x0FFF;
			ehci_write_phy_mem(current + EHCI_TD_OFF_BUFF1_PTR, buff);
			ehci_write_phy_mem(current + EHCI_TD_OFF_BUFF2_PTR, buff + 0x1000);
			ehci_write_phy_mem(current + EHCI_TD_OFF_BUFF3_PTR, buff + 0x2000);
			ehci_write_phy_mem(current + EHCI_TD_OFF_BUFF4_PTR, buff + 0x3000);
		}
	
		buffer += max_size;
		data0 ^= 1;
		current += EHCI_TD_SIZE;
		cnt++;
		sz -= max_size;
	} while (sz > 0);
	
	return cnt;
}

void ehci_insert_queue(UINT_32 queue, const UINT_8 type) 
{
	ehci_write_phy_mem(queue + EHCI_QH_OFF_HORZ_PTR, ehci_read_phy_mem(async_base + EHCI_QH_OFF_HORZ_PTR));
	ehci_write_phy_mem(async_base + EHCI_QH_OFF_HORZ_PTR, queue | type);
	ehci_write_phy_mem(queue + EHCI_QH_OFF_PREV_PTR, async_base);
}

BOOL ehci_remove_queue(UINT_32 queue)
{
	UINT_32 temp_addr;
	
	temp_addr = ehci_read_phy_mem(queue + EHCI_QH_OFF_PREV_PTR);
	ehci_write_phy_mem(temp_addr + EHCI_QH_OFF_HORZ_PTR, ehci_read_phy_mem(queue + EHCI_QH_OFF_HORZ_PTR));
	
	temp_addr = ehci_read_phy_mem(queue + EHCI_QH_OFF_HORZ_PTR) & ~EHCI_QUEUE_HEAD_PTR_MASK;
	ehci_write_phy_mem(temp_addr + EHCI_QH_OFF_PREV_PTR, ehci_read_phy_mem(queue + EHCI_QH_OFF_PREV_PTR));
	
	// now wait for the successful "doorbell"
	// set bit 6 in command register (to tell the controller that something has been removed from the schedule)
	// then watch for bit 5 in the status register.  Once it is set, we can assume all removed correctly.
	// We ignore the interrupt on async bit in the USBINTR.  We don't need an interrupt here.
	UINT_32 command = ehci_read_op_reg(EHC_OPS_USBCommand);
	ehci_write_op_reg(EHC_OPS_USBCommand, command | (1<<6));
	if (ehci_handshake(EHC_OPS_USBStatus, (1<<5), (1<<5), 100)) 
	{
		ehci_write_op_reg(EHC_OPS_USBStatus, (1<<5)); // acknowledge the bit
		return TRUE;
	} 
	else
		return FALSE;
}

int ehci_wait_interrupt(UINT_32 addr, const UINT_32 timeout, BOOL *spd) 
{
	int ret = -1;
	UINT_32 status;
	int timer = timeout;
	
	while (timer)
	{
		status = ehci_read_phy_mem(addr + EHCI_TD_OFF_STATUS) & ~1;  // ignore bit 0 (?)
		if ((status & 0x00000080) == 0)
		{
			ret = SUCCESS;
			if ((status & 0x7F) == 0x00) 
			{
				if ((((status & 0x7FFF0000) >> 16) > 0) && (((status & (3<<8))>>8) == 1))
					if (spd) 
						*spd = TRUE;
			} 
			else
			{
				if (status & (1<<6))
					ret = ERROR_STALLED;
				else if (status & (1<<5))
					ret = ERROR_DATA_BUFFER_ERROR;
				else if (status & (1<<4))
					ret = ERROR_BABBLE_DETECTED;
				else if (status & (1<<3))
					ret = ERROR_NAK;
				else if (status & (1<<2))
					ret = ERROR_TIME_OUT;
				else
				{
					printk("USB EHCI wait interrupt qtd->status = ^\n", status);
					ret = ERROR_UNKNOWN;
				}
				return ret;
			}
			if ((((status & 0x7FFF0000) >> 16) > 0) && (((status & (3<<8))>>8) == 1)) 
			{
				if ((ehci_read_phy_mem(addr + EHCI_TD_OFF_ALT_NEXT_QTD_PTR) & 1) == 0) 
				{
					addr = ehci_read_phy_mem(addr + EHCI_TD_OFF_ALT_NEXT_QTD_PTR);
					timer = timeout;
				} 
				else
					return ret;
			} 
			else 
			{
				if ((ehci_read_phy_mem(addr + EHCI_TD_OFF_NEXT_TD_PTR) & 1) == 0) 
				{
					addr  = ehci_read_phy_mem(addr + EHCI_TD_OFF_NEXT_TD_PTR);
					timer = timeout;
				} 
				else
					return ret;
			}
			printk("once came inside ");
		}
		WaitMiliSecond(1);
		timer--;
	}
	if (ret == -1)
	{
		printk("USB EHCI Interrupt wait timed out\n");
		ret = ERROR_TIME_OUT;
	}
	return ret;
}

void ehci_clear_phy_mem(const UINT_32 address, const int len) 
{
	int i;
	UINT_8* tmp = (UINT_8*)((void*)address);
	for (i=0; i<len; i++)
		tmp[i] = 0;
}

void ehci_copy_to_phy_mem(const UINT_32 address, void *src, const int len) 
{
	UINT_8 *s = (UINT_8 *) src;
	int i;
	UINT_8* tmp = (UINT_8*)((void*)address);
	for (i=0; i<len; i++)
		tmp[i] = s[i];
}

void ehci_copy_from_phy_mem(void *targ, const UINT_32 address, const int len) 
{
	UINT_8 *t = (UINT_8 *) targ;
	UINT_8* tmp = (UINT_8*)((void*)address);
	int i;
	for (i=0; i<len; i++)
		t[i] = tmp[i];
}

void ehci_write_phy_mem(const UINT_32 address, UINT_32 val) 
{
	void* tmp = (void*)address;
	*(UINT_32*)tmp = val;
}

UINT_32 ehci_read_phy_mem(const UINT_32 address)
{
	return *(UINT_32*)((void*)address);
}

void ehci_write_cap_reg(const UINT_32 offset, const UINT_32 val) 
{
	UINT_32 tmp = *(UINT_32*)((void*)((UINT_32)ehci_bar0 + offset));
	tmp = val;
}

void ehci_write_op_reg(const UINT_32 offset, const UINT_32 val) 
{
	ehci_write_cap_reg(op_base_off + offset, val);
}

UINT_32 ehci_read_cap_reg(const UINT_32 offset) 
{
	return *(UINT_32*)((void*)((UINT_32)ehci_bar0 + offset));
}

UINT_32 ehci_read_op_reg(const UINT_32 offset) 
{
	return ehci_read_cap_reg(op_base_off + offset);
}

