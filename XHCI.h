#ifndef _XHCI__H__
#define _XHCI__H__

typedef unsigned char      bit8u;
typedef unsigned short     bit16u;
typedef unsigned int       bit32u;
typedef unsigned long long bit64u;
typedef unsigned int       bool;

#define TRUE             0x01
#define FALSE            0x00
#define CMND_RING_TRBS   128  // not more than 4096
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
	bit32u volatile id_next_owner_flags;
	bit32u volatile cntrl_status;
} __attribute__ ((packed)) xHC_xECP_LEGACY;

typedef struct _xHCI_xECP_PROTO {
	bit8u  id;
	bit8u  next;
	bit8u  minor;
	bit8u  major;
	bit32u name;
	bit8u  offset;
	bit8u  count;
	bit16u flags;
} __attribute__ ((packed)) xHC_xECP_PROTO;

#define MAX_CONTEXT_SIZE   64                               // Max Context size in bytes
#define MAX_SLOT_SIZE      (MAX_CONTEXT_SIZE * 32)          // Max Total Slot size in bytes

// Slot State
#define SLOT_STATE_DISABLED_ENABLED  0
#define SLOT_STATE_DEFAULT           1
#define SLOT_STATE_ADRESSED          2
#define SLOT_STATE_CONFIGURED        3

typedef struct _xHCI_SLOT_CONTEXT {
	unsigned int entries;
	bit8u        hub;
	bit8u        mtt;
	unsigned int speed;
	bit32u       route_string;
	unsigned int num_ports;
	unsigned int rh_port_num;
	unsigned int max_exit_latency;
	unsigned int int_target;
	unsigned int ttt;
	unsigned int tt_port_num;
	unsigned int tt_hub_slot_id;
	unsigned int slot_state;
	unsigned int device_address;
} __attribute__ ((packed)) xHCI_SLOT_CONTEXT;

// EP State
#define EP_STATE_DISABLED 0
#define EP_STATE_RUNNING  1
#define EP_STATE_HALTED   2
#define EP_STATE_STOPPED  3
#define EP_STATE_ERROR    4

typedef struct _xHCI_EP_CONTEXT {
	unsigned int interval;
	bit8u        lsa;
	unsigned int max_pstreams;
	unsigned int mult;
	unsigned int ep_state;
	unsigned int max_packet_size;
	unsigned int max_burst_size;
	bit8u        hid;
	unsigned int ep_type;
	unsigned int cerr;
	bit64u       tr_dequeue_pointer;
	bit8u        dcs;
	unsigned int max_esit_payload;
	unsigned int average_trb_len;
} __attribute__ ((packed)) xHCI_EP_CONTEXT;

typedef struct _xHCI_TRB {
	bit64u param;
	bit32u status;
	bit32u command;
} __attribute__ ((packed)) xHCI_TRB;

// event ring specification
typedef struct _xHCI_EVENT_SEG_TABLE {
	bit64u addr;
	bit32u size;
	bit32u resv;
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

enum { xCONTROL_EP=0, xISOCHRONOUS_EP, xBULK_EP, xINTERRUPT_EP };
typedef struct _xHCI_CAPABILITY {
	volatile bit8u  caplength; //0
	volatile bit8u  reserved1; //1
	volatile bit16u hciversion;//2
	volatile bit32u hcsparam1; //4
	volatile bit32u hcsparam2; //8
	volatile bit32u hcsparam3; //12
	volatile bit32u hccparam1; //16
	volatile bit32u dboff;     //20
	volatile bit32u rtsoff;    //24
	volatile bit32u hccparam2; //28
} __attribute__ ((packed)) xHCI_CAPABILITY, *PxHCI_CAPABILITY;

typedef struct _xHCI_OPERATION {
	volatile bit32u usbcmd;          //0x00
	volatile bit32u usbsts;          //0x04
	volatile bit32u pagesize;        //0x08
	volatile bit8u  reserved1[8];    //0x0C
	volatile bit32u dnctrl;          //0x14
	volatile bit64u crcr;            //0x18
	volatile bit8u  reserved2[16];   //0x20
	volatile bit64u dcbaap;          //0x30
	volatile bit32u config;          //0x38
	volatile bit8u  reserved3[964];
	volatile bit8u  port[255 * 16];  // room for maximum 255 x 16-byte port register set
} __attribute__ ((packed)) xHCI_OPERATION, *PxHCI_OPERATION;

typedef volatile struct _xHCI_PORT_REGISTER_SET {
	volatile bit32u portsc;    // port status and control
	volatile bit32u portpmsc;  // port power management status and control
	volatile bit32u portli;    // port port link info
	volatile bit32u porthlpmc; // port hardware LPM control (only version 1.1. on version 1.0 is reserved)
} __attribute__ ((packed)) xHCI_PORT_REGISTER_SET, *PxHCI_PORT_REGISTER_SET;

typedef volatile struct _xHCI_EXTENDED_CAPABILITY {
	volatile bit32u DWORD0;
	volatile bit32u DWORD1;
	volatile bit32u DWORD2;
	volatile bit32u DWORD3;
} __attribute__ ((packed)) xHCI_EXTENDED_CAPABILITY, *PxHCI_EXTENDED_CAPABILITY;

typedef volatile struct _xHCI_PORT_INFO {
	volatile bit8u  flags;                // port_info flags below
	volatile bit8u  other_port_num;       // zero based offset to other speed port
	volatile bit8u  offset;               // offset of this port within this protocol
	volatile bit8u  reserved;
} __attribute__ ((packed)) xHCI_PORT_INFO;

typedef volatile struct _XHCI {
	volatile void*                     base_address_lo;
	volatile void*                     base_address_hi;
	volatile void*                     pci;
	volatile bit16u                    bus;
	volatile bit16u                    device;
	volatile bit16u                    function;
	volatile bit8u                     irq_number;
	volatile PxHCI_CAPABILITY          cap;
	volatile PxHCI_OPERATION           oper;
	volatile PxHCI_PORT_REGISTER_SET   port_reg_set;
	volatile PxHCI_EXTENDED_CAPABILITY ext_cap;
} __attribute__ ((packed)) XHCI;

typedef struct _DEVICE_DESC {
	volatile bit8u  len;
	volatile bit8u  type;
	volatile bit16u usb_ver;
	volatile bit8u  _class;
	volatile bit8u  subclass;
	volatile bit8u  protocol;
	volatile bit8u  max_packet_size;
	volatile bit16u vendorid;
	volatile bit16u productid;
	volatile bit16u device_rel;
	volatile bit8u  manuf_indx;   // index value
	volatile bit8u  prod_indx;    // index value
	volatile bit8u  serial_indx;  // index value
	volatile bit8u  configs;      // Number of configurations
} __attribute__ ((packed)) DEVICE_DESC;

void   xhci_print_on_screen  (XHCI* x);
bool   start_xhci_controller (XHCI* x);
void*  xhci_alloc_memory     (bit32u byte, bit32u alignment, bit32u page_boundary);
void   xhci_free_memory      (void* pointer);
//void   xhci_write_oper_reg   (const bit32u base, bit32u offset, bit32u value);
//bit32u xhci_read_oper_reg    (const bit32u base, bit32u offset);

void ___test_NO_OP_TRB___(XHCI*);
#endif
