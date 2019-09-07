#ifndef _XHCI__H__
#define _XHCI__H__

#include "./IMOS_CORE.h"

#define CMND_RING_TRBS   256  // not more than 4096
#define TRBS_PER_RING    256

#define BIT(X) (1<<(X))

#define xCMD_RUN    BIT(0)
#define xCMD_RESET  BIT(1)
#define xCMD_INTE   BIT(2)
#define xCMD_HSEE   BIT(3)
/* Bit 4-6 reserved */
#define xCMD_LHCRST BIT(7)
#define xCMD_CSS    BIT(8)
#define xCMD_CRS    BIT(9)
#define xCMD_EWE    BIT(10)
#define xCMD_EU3S   BIT(11)

#define xSTS_INTMASK 0x041C
#define xSTS_HCH    BIT(0)
/* Bit 1 reserved */
#define xSTS_HSE    BIT(2)
#define xSTS_EINT   BIT(3)
#define xSTS_PCD    BIT(4)
/* Bit 5-7 reserved */
#define xSTS_SSS    BIT(8)
#define xSTS_RSS    BIT(9)
#define xSTS_SRE    BIT(10)
#define xSTS_CNR    BIT(11)
#define xSTS_HCE    BIT(12)

#define xPORT_CCS   BIT(0)
#define xPORT_ED    BIT(1)
#define xPORT_OCA   BIT(3)
#define xPORT_RESET BIT(4)
#define xPORT_LS    0x01E0
#define xPORT_POW   BIT(9)
#define xPORT_SPD   0x3C00
#define xPORT_IC    0xC000
#define xPORT_LWS   BIT(16)
#define xPORT_CSC   BIT(17)
#define xPORT_PEC   BIT(18)
#define xPORT_WRC   BIT(19)
#define xPORT_OCC   BIT(20)
#define xPORT_PRC   BIT(21)
#define xPORT_PLC   BIT(22)
#define xPORT_CEC   BIT(23)
#define xPORT_CAS   BIT(24)
#define xPORT_WCE   BIT(25)
#define xPORT_WDE   BIT(26)
#define xPORT_WOE   BIT(27)
/* Bit 28-29 reserved */
#define xPORT_DR    BIT(30)
#define xPORT_WPR   BIT(31)

#define xPORT_CHANGE_BITS  (BIT(17) | BIT(18) | BIT(20) | BIT(21) | BIT(22))
                             /// Quirk:  TI TUSB7340: sets bit 19 on USB2 ports  ??????????????
#define xHCI_Port_PORTSC 0x00

#define xHC_INTERRUPTER_PRIMARY      0

#define xHC_INTERRUPTER_IMAN      0x00
#define xHC_INTERRUPTER_IMOD      0x04
#define xHC_INTERRUPTER_ERSTSZ    0x08
#define xHC_INTERRUPTER_RESV      0x0C
#define xHC_INTERRUPTER_ERSTBA    0x10
#define xHC_INTERRUPTER_ERDP      0x18

#define xHC_IMAN_IP BIT(0)
#define xHC_IMAN_IE BIT(1)
// xHCI speed values
#define xHCI_SPEED_FULL   1
#define xHCI_SPEED_LOW    2
#define xHCI_SPEED_HIGH   3
#define xHCI_SPEED_SUPER  4

#define xHCI_DIR_NO_DATA  0
#define xHCI_DIR_OUT      2
#define xHCI_DIR_IN       3

#define xHCI_DIR_OUT_B    0
#define xHCI_DIR_IN_B     1

// Endpoint Doorbell numbers
#define xHCI_SLOT_CNTX   0
#define xHCI_CONTROL_EP  1
#define xHCI_EP1_OUT     2
#define xHCI_EP1_IN      3
#define xHCI_EP2_OUT     4
#define xHCI_EP2_IN      5
#define xHCI_EP3_OUT     6
#define xHCI_EP3_IN      7
#define xHCI_EP4_OUT     8
#define xHCI_EP4_IN      9
#define xHCI_EP5_OUT     10
#define xHCI_EP5_IN      11
#define xHCI_EP6_OUT     12
#define xHCI_EP6_IN      13
#define xHCI_EP7_OUT     14
#define xHCI_EP7_IN      15
#define xHCI_EP8_OUT     16
#define xHCI_EP8_IN      17
#define xHCI_EP9_OUT     18
#define xHCI_EP9_IN      19
#define xHCI_EP10_OUT    20
#define xHCI_EP10_IN     21
#define xHCI_EP11_OUT    22
#define xHCI_EP11_IN     23
#define xHCI_EP12_OUT    24
#define xHCI_EP12_IN     25
#define xHCI_EP13_OUT    26
#define xHCI_EP13_IN     27
#define xHCI_EP14_OUT    28
#define xHCI_EP14_IN     29
#define xHCI_EP15_OUT    30
#define xHCI_EP15_IN     31

// Port_info flags
#define xHCI_PROTO_INFO           BIT(0)  // bit 0 set = USB3, else USB2
#define xHCI_PROTO_HSO            BIT(1)  // bit 1 set = is USB 2 and High Speed Only
#define xHCI_PROTO_HAS_PAIR       BIT(2)  // bit 2 set = has a corresponding port. (i.e.: is a USB3 and has USB2 port (a must))
                                          //     clear = does not have a corr. port (i.e.: is a USB2 port and does not have a USB3 port)
#define xHCI_PROTO_ACTIVE         BIT(3)  // is the active port of the pair.

#define xHCI_PROTO_USB2  0
#define xHCI_PROTO_USB3  1

#define xHCI_IS_USB3_PORT(x)        ((port_info[(x)].flags & xHCI_PROTO_INFO) == xHCI_PROTO_USB3)
#define xHCI_IS_USB2_PORT(x)        ((port_info[(x)].flags & xHCI_PROTO_INFO) == xHCI_PROTO_USB2)
#define xHCI_IS_USB2_HSO(x)         ((port_info[(x)].flags & xHCI_PROTO_HSO) == xHCI_PROTO_HSO)
#define xHCI_HAS_PAIR(x)            ((port_info[(x)].flags & xHCI_PROTO_HAS_PAIR) == xHCI_PROTO_HAS_PAIR)
#define xHCI_IS_ACTIVE(x)           ((port_info[(x)].flags & xHCI_PROTO_ACTIVE) == xHCI_PROTO_ACTIVE)

#define xHC_TRB_ID_LINK             6
#define xHC_TRB_ID_NOOP             8
#define xHC_xECP_ID_NONE            0
#define xHC_xECP_ID_LEGACY          1
#define xHC_xECP_ID_PROTO           2
#define xHC_xECP_ID_POWER           3
#define xHC_xECP_ID_VIRT            4
#define xHC_xECP_ID_MESS            5
#define xHC_xECP_ID_LOCAL           6
#define xHC_xECP_ID_DEBUG           10
#define xHC_xECP_ID_EXT_MESS        17
#define xHC_xECP_LEGACY_TIMEOUT     10  // 10 milliseconds
#define xHC_xECP_LEGACY_BIOS_OWNED  BIT(16)
#define xHC_xECP_LEGACY_OS_OWNED    BIT(24)
#define xHC_xECP_LEGACY_OWNED_MASK  (xHC_xECP_LEGACY_BIOS_OWNED | xHC_xECP_LEGACY_OS_OWNED)
	
typedef struct _xHCI_xECP_LEGACY {
	UINT_32 volatile id_next_owner_flags;
	UINT_32 volatile cntrl_status;
} __attribute__ ((packed)) xHC_xECP_LEGACY;

typedef struct _xHCI_xECP_PROTO {
	UINT_8  id;
	UINT_8  next;
	UINT_8  minor;
	UINT_8  major;
	UINT_32 name;
	UINT_8  offset;
	UINT_8  count;
	UINT_16 flags;
} __attribute__ ((packed)) xHC_xECP_PROTO;

#define MAX_CONTEXT_SIZE   64                               // Max Context size in bytes
#define MAX_SLOT_SIZE      (MAX_CONTEXT_SIZE * 32)          // Max Total Slot size in bytes

// Slot State
#define SLOT_STATE_DISABLED_ENABLED  0
#define SLOT_STATE_DEFAULT           1
#define SLOT_STATE_ADRESSED          2
#define SLOT_STATE_CONFIGURED        3

typedef struct _xHCI_SLOT_CONTEXT {
	volatile UINT_32 DWORD0;
	volatile UINT_16 max_exit_latency;
	volatile UINT_8  root_hub_port_number;
	volatile UINT_8  number_of_ports;
	volatile UINT_8  TT_hub_slot_id;
	volatile UINT_8  TT_port_number;
	volatile UINT_16 WORD2;
	volatile UINT_8  usb_device_address;
	volatile UINT_8  WORD_BYTE3[3];
	volatile UINT_32 reserved[4];
} __attribute__ ((packed)) xHCI_SLOT_CONTEXT;

// EP State
#define EP_STATE_DISABLED 0
#define EP_STATE_RUNNING  1
#define EP_STATE_HALTED   2
#define EP_STATE_STOPPED  3
#define EP_STATE_ERROR    4

typedef struct _xHCI_EP_CONTEXT {
	volatile UINT_16 WORD0;
	volatile UINT_8  interval;
	volatile UINT_8  max_esit_payload_hi;
	volatile UINT_8  BYTE1;
	volatile UINT_8  max_burst_size;
	volatile UINT_16 max_packet_size;
	volatile UINT_32 DWORD2;
	volatile UINT_32 TR_dequeue_pointer_hi;
	volatile UINT_16 average_trb_length;
	volatile UINT_16 max_esit_payload_lo;
	volatile UINT_32 reserved[3];
} __attribute__ ((packed)) xHCI_EP_CONTEXT;

typedef struct _xHCI_TRB {
	UINT_64 param;
	UINT_32 status;
	UINT_32 command;
} __attribute__ ((packed)) xHCI_TRB;

// event ring specification
typedef struct _xHCI_EVENT_SEG_TABLE {
	UINT_64 addr;
	UINT_32 size;
	UINT_32 resv;
} __attribute__ ((packed)) xHCI_EVENT_SEG_TABLE;

#define XHCI_DIR_EP_OUT   0
#define XHCI_DIR_EP_IN    1
#define XHCI_GET_DIR(x)      (((x) & (1    <<  7)) >> 7)

#define TRB_GET_STYPE(x)     (((x) & (0x1F << 16)) >> 16)
#define TRB_SET_STYPE(x)     (((x) & 0x1F) << 16)
#define TRB_GET_TYPE(x)      (((x) & (0x3F << 10)) >> 10)
#define TRB_SET_TYPE(x)      (((x) & 0x3F) << 10)
#define TRB_GET_COMP_CODE(x) (((x) & (0x7F << 24)) >> 24)
#define TRB_SET_COMP_CODE(x) (((x) & 0x7F) << 24)
#define TRB_GET_SLOT(x)      (((x) & (0xFF << 24)) >> 24)
#define TRB_SET_SLOT(x)      (((x) & 0xFF) << 24)
#define TRB_GET_TDSIZE(x)    (((x) & (0x1F << 17)) >> 17)
#define TRB_SET_TDSIZE(x)    (((x) & 0x1F) << 17)
#define TRB_GET_EP(x)        (((x) & (0x1F << 16)) >> 16)
#define TRB_SET_EP(x)        (((x) & 0x1F) << 16)

#define TRB_GET_TARGET(x)    (((x) & (0x3FF << 22)) >> 22)
#define TRB_GET_TX_LEN(x)     ((x) & 0x1FFFF)
#define TRB_GET_TOGGLE(x)    (((x) & (1<<1)) >> 1)

#define TRB_DC(x)            (((x) & (1<<9)) >> 9)
#define TRB_IS_IMMED_DATA(x) (((x) & (1<<6)) >> 6)
#define TRB_IOC(x)           (((x) & (1<<5)) >> 5)
#define TRB_CHAIN(x)         (((x) & (1<<4)) >> 4)
#define TRB_SPD(x)           (((x) & (1<<2)) >> 2)
#define TRB_TOGGLE(x)        (((x) & (1<<1)) >> 1)

#define TRB_CYCLE_ON          (1<<0)
#define TRB_CYCLE_OFF         (0<<0)

#define TRB_TOGGLE_CYCLE_ON   (1<<1)
#define TRB_TOGGLE_CYCLE_OFF  (0<<1)

#define TRB_CHAIN_ON          (1<<4)
#define TRB_CHAIN_OFF         (0<<4)

#define TRB_IOC_ON            (1<<5)
#define TRB_IOC_OFF           (0<<5)

#define TRB_LINK_CMND         (TRB_SET_TYPE(xLINK) | TRB_IOC_OFF | TRB_CHAIN_OFF | TRB_TOGGLE_CYCLE_OFF | TRB_CYCLE_ON)

#define XHCI_IRQ_DONE         BIT(31)

// setup packets
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

// device requests
enum { GET_STATUS=0, CLEAR_FEATURE, SET_FEATURE=3, SET_ADDRESS=5, GET_DESCRIPTOR=6, SET_DESCRIPTOR,
        GET_CONFIGURATION, SET_CONFIGURATION,
// interface requests
        GET_INTERFACE, SET_INTERFACE,
// standard endpoint requests
        SYNCH_FRAME,
// Device specific
        GET_MAX_LUNS = 0xFE, BULK_ONLY_RESET
};

// Common TRB types
enum { 
	xNORMAL=1, 
	xSETUP_STAGE, 
	xDATA_STAGE, 
	xSTATUS_STAGE, 
	xISOCH, 
	xLINK = 6,
	xEVENT_DATA, 
	xNO_OP,
    xENABLE_SLOT=9, 
	xDISABLE_SLOT, 
	xADDRESS_DEVICE, 
	xCONFIG_EP, 
	xEVALUATE_CONTEXT, 
	xRESET_EP,
    xSTOP_EP=15, 
	xSET_TR_DEQUEUE, 
	xRESET_DEVICE, 
	xFORCE_EVENT, 
	xDEG_BANDWIDTH, 
	xSET_LAT_TOLERANCE,
    xGET_PORT_BAND=21, 
	xFORCE_HEADER, NO_OP_CMD,  
	// 24 - 31 = reserved
    xTRANS_EVENT=32, 
	xCOMMAND_COMPLETION, 
	xPORT_STATUS_CHANGE,
	xBANDWIDTH_REQUEST, 
	xDOORBELL_EVENT,
    xHOST_CONTROLLER_EVENT=37, 
	xDEVICE_NOTIFICATION, 
	xMFINDEX_WRAP, 
    // 40 - 47 = reserved
    // 48 - 63 = Vendor Defined
};

// event completion codes
enum { 
	xTRB_SUCCESS=1, 
	xDATA_BUFFER_ERROR,
	xBABBLE_DETECTION, 
	xTRANSACTION_ERROR,
	xTRB_ERROR, 
	xSTALL_ERROR,
    xRESOURCE_ERROR=7,
	xBANDWIDTH_ERROR, 
	xNO_SLOTS_ERROR, 
	xINVALID_STREAM_TYPE, 
	xSLOT_NOT_ENABLED, 
	xEP_NOT_ENABLED,
    xSHORT_PACKET=13,
	xRING_UNDERRUN, 
	xRUNG_OVERRUN, 
	xVF_EVENT_RING_FULL, 
	xPARAMETER_ERROR, 
	xBANDWITDH_OVERRUN,
    xCONTEXT_STATE_ERROR=19, 
	xNO_PING_RESPONSE, 
	xEVENT_RING_FULL, 
	xINCOMPATIBLE_DEVICE,
	xMISSED_SERVICE,
    xCOMMAND_RING_STOPPED=24, 
	xCOMMAND_ABORTED, 
	xSTOPPED, 
	xSTOPPER_LENGTH_ERROR,
	xRESERVED, 
	xISOCH_BUFFER_OVERRUN,
    xEVERN_LOST=32, 
	xUNDEFINED, 
	xINVALID_STREAM_ID,
	xSECONDARY_BANDWIDTH, 
	xSPLIT_TRANSACTION
    /* 37 - 191 reserved */
    /* 192 - 223 vender defined errors */
    /* 224 - 225 vendor defined info */
};

// Descriptor types
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

enum { xCONTROL_EP=0, xISOCHRONOUS_EP, xBULK_EP, xINTERRUPT_EP };
typedef struct _xHCI_CAPABILITY {
	volatile UINT_8  caplength; //0
	volatile UINT_8  reserved1; //1
	volatile UINT_16 hciversion;//2
	volatile UINT_32 hcsparams1; //4
	volatile UINT_32 hcsparams2; //8
	volatile UINT_32 hcsparams3; //12
	volatile UINT_32 hccparams1; //16
	volatile UINT_32 dboff;      //20
	volatile UINT_32 rtsoff;     //24
	volatile UINT_32 hccparams2; //28
} __attribute__ ((packed)) xHCI_CAPABILITY;

typedef struct _xHCI_OPERATION {
	volatile UINT_32 usbcmd;          //0x00
	volatile UINT_32 usbsts;          //0x04
	volatile UINT_32 pagesize;        //0x08
	volatile UINT_8  reserved1[8];    //0x0C
	volatile UINT_32 dnctrl;          //0x14
	volatile UINT_64 crcr;            //0x18
	volatile UINT_8  reserved2[16];   //0x20
	volatile UINT_64 dcbaap;          //0x30
	volatile UINT_32 config;          //0x38
	volatile UINT_8  reserved3[964];
	volatile UINT_8  port[255 * 16];  // room for maximum 255 x 16-byte port register set
} __attribute__ ((packed)) xHCI_OPERATION;

typedef volatile struct _xHCI_PORT_REGISTER_SET {
	volatile UINT_32 portsc;    // port status and control
	volatile UINT_32 portpmsc;  // port power management status and control
	volatile UINT_32 portli;    // port port link info
	volatile UINT_32 porthlpmc; // port hardware LPM control (only version 1.1. on version 1.0 is reserved)
} __attribute__ ((packed)) xHCI_PORT_REGISTER_SET;

typedef volatile struct _xHCI_EXTENDED_CAPABILITY {
	volatile UINT_32 DWORD0;
	volatile UINT_32 DWORD1;
	volatile UINT_32 DWORD2;
	volatile UINT_32 DWORD3;
} __attribute__ ((packed)) xHCI_EXTENDED_CAPABILITY;

typedef volatile struct _xHCI_PORT_INFO {
	volatile UINT_8  flags;                // port_info flags below
	volatile UINT_8  other_port_num;       // zero based offset to other speed port
	volatile UINT_8  offset;               // offset of this port within this protocol
	volatile UINT_8  reserved;
} __attribute__ ((packed)) xHCI_PORT_INFO;


typedef struct _USB_DEVICE_CONNECTION {
	volatile BOOL   connected;           // 1: connected; 0: disconnected
	volatile UINT_8 port;
	volatile UINT_8 speed; 
	volatile BOOL   slot_configuration_command;   
	volatile BOOL   slot_configuration;
	volatile BOOL   slot_set_address;
	volatile BOOL   setup_data_status_stages;
	volatile BOOL   setup_data_status_stages_command;
	volatile BOOL   setup_stage;
	volatile BOOL   data_stage;
	volatile BOOL   status_stage;
	volatile UINT_8 slot_id;
	volatile void*  slot_address;
	volatile void*  ep0_address;
} __attribute__((packed)) USB_DEVICE_CONNECTION;

typedef volatile struct _XHCI {
	volatile UINT_32                   base_address_lo;
	volatile UINT_32                   base_address_hi;
	volatile UINT_8                    irq_number;
	volatile xHCI_CAPABILITY*          cap;
	volatile xHCI_OPERATION*           oper;
	volatile xHCI_PORT_REGISTER_SET*   port_reg;
	volatile xHCI_EXTENDED_CAPABILITY* ex_cap;
	
	volatile UINT_32                   context_size;
	volatile UINT_32                   page_size;
	volatile UINT_32                   max_intr;
	volatile UINT_32                   driver_crcr;
	volatile UINT_32                   command_ring_Enqueue;
	volatile UINT_32                   command_ring_PCS;
	volatile UINT_32                   current_event_ring_address;
	volatile UINT_32                   current_event_ring_cycle;
	void*                              ers;
	void*                              erst;
	void*                              pci;
	UINT_16                            bus;
	UINT_16                            device;
	UINT_16                            function;
	USB_DEVICE_CONNECTION*             usb_device;
} __attribute__ ((packed)) XHCI;

typedef struct _DEVICE_DESC {
	volatile UINT_8  len;
	volatile UINT_8  type;
	volatile UINT_16 usb_ver;
	volatile UINT_8  _class;
	volatile UINT_8  subclass;
	volatile UINT_8  protocol;
	volatile UINT_8  max_packet_size;
	volatile UINT_16 vendorid;
	volatile UINT_16 productid;
	volatile UINT_16 device_rel;
	volatile UINT_8  manuf_indx;   // index value
	volatile UINT_8  prod_indx;    // index value
	volatile UINT_8  serial_indx;  // index value
	volatile UINT_8  configs;      // Number of configurations
} __attribute__ ((packed)) DEVICE_DESC;

typedef struct _TRB_TYPES {
	UINT_32 id_number;
	INT_8*  message_type;
} __attribute__ ((packed)) TRB_TYPES;

typedef struct _REQUEST_PACKET {
	UINT_8  request_type;
	UINT_8  request;
	UINT_16 value;
	UINT_16 index;
	UINT_16 length;
} __attribute__ ((packed)) REQUEST_PACKET;

typedef enum _XHCI_ROOT_HUB_PORT_NUMBERS {
	PORT_0 = 0x03,
	PORT_1 = 0x0C,
	PORT_2 = 0x30,
	PORT_3 = 0xC0	
} XHCI_ROOT_HUB_PORT_NUMBERS;

typedef enum _XHCI_ROOT_HUB_PORT_CONNECTION_STATUS {
	USB2_CONNECTION_PORT_DISCONNECT = 0x00,
	USB2_CONNECTION_PORT_CONNECT_01 = 0x01,
	USB2_CONNECTION_PORT_CONNECT_02 = 0x02,
	USB2_CONNECTION_PORT_INVALID    = 0x03,	
	USB3_CONNECTION_PORT_DISCONNECT = 0x00,
	USB3_CONNECTION_PORT_CONNECT_01 = 0x01,
	USB3_CONNECTION_PORT_CONNECT_02 = 0x02,
	USB3_CONNECTION_PORT_INVALID    = 0x03,
} XHCI_ROOT_HUB_PORT_CONNECTION_STATUS;

BOOL   start_xhci_controller        (XHCI* x);
void*  xhci_alloc_memory            (UINT_32 byte, UINT_32 alignment, UINT_32 page_boundary);
void   xhci_free_memory             (void* pointer);
void   xhci_slot_configuration      (XHCI* x, UINT_32 port, UINT_32 speed);
void   xhci_slot_release            (XHCI* x, UINT_32 port, UINT_32 speed);
XHCI*  xhci_instance_to_idt         (void);
UINT_8 xhci_get_critical_event      (void);
void   xhci_slot_set_address        (XHCI* x, BOOL BSR);
void   xhci_setup_data_status_stages(XHCI* x, void* target, UINT_32 length, UINT_32 max_packet, BOOL first_or_second);

#endif
