#ifndef _XHCI__H__
#define _XHCI__H__

#include "IMOS_CORE.h"

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

#define xPORT_CHANGE_BITS (BIT(17) | BIT(18) | BIT(20) | BIT(21) | BIT(22)) // Quirk:  TI TUSB7340: sets bit 19 on USB2 ports

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

#define xHCI_HOST_ACKNOWLEDGE   0
#define xHCI_DEVICE_ACKNOWLEDGE 1

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
#define TRB_CYCLE_ON         (1<<0)
#define TRB_CYCLE_OFF        (0<<0)
#define TRB_TOGGLE_CYCLE_ON  (1<<1)
#define TRB_TOGGLE_CYCLE_OFF (0<<1)
#define TRB_CHAIN_ON         (1<<4)
#define TRB_CHAIN_OFF        (0<<4)
#define TRB_IOC_ON           (1<<5)
#define TRB_IOC_OFF          (0<<5)
#define TRB_LINK_CMND        (TRB_SET_TYPE(xLINK) | TRB_IOC_OFF | TRB_CHAIN_OFF | TRB_TOGGLE_CYCLE_OFF | TRB_CYCLE_ON)

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
#define CLASS_SET_INTERFACE (HOST_TO_DEV | REQ_TYPE_CLASS  | RECPT_INTERFACE)
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/** structs **/

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

// TRB
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

typedef struct _UNIVERSAL_SLOT_ENDPOINT_ATTRIBUTES {
	UINT_8  config_value;
	UINT_8  config_attribs;
	UINT_8  interface_number;
	UINT_8  number_of_endpoints;
	UINT_8  class_code;
	UINT_8  sub_class;
	UINT_8  protocol;
	UINT_16 des_length;
	UINT_8  endpoint_address;
	UINT_8  endpoint_attribs;
	UINT_16 endpoint_max_packet_size;
	UINT_8  endpoint_interval;
	UINT_8  reserved[0x10 - 0x0E];
} __attribute__ ((packed)) UNIVERSAL_SLOT_ENDPOINT_ATTRIBUTES;

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
	volatile void*  ep_in_address;
	volatile void*  ep_out_address;
	volatile UNIVERSAL_SLOT_ENDPOINT_ATTRIBUTES* uni_config;
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


/** Enumerations **/
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

// device requests
typedef enum _DEVICE_REQUESTS { 
	GET_STATUS        = 0x00,
	CLEAR_FEATURE     = 0x01,
	SET_FEATURE       = 0x03,
	SET_ADDRESS       = 0x05,
	GET_DESCRIPTOR    = 0x06,
	SET_DESCRIPTOR    = 0x07,
	GET_CONFIGURATION = 0x08,
	SET_CONFIGURATION = 0x09,
	// interface requests
	GET_INTERFACE     = 0x0A,
	SET_INTERFACE     = 0x0B,
	// standard endpoint requests
	SYNCH_FRAME       = 0x0C,
	// Device specific
	GET_MAX_LUNS      = 0xFE, 
	BULK_ONLY_RESET   = 0xFF
} DEVICE_REQUESTS;

// HID class requests
typedef enum _HID_CLASS_REQUESTS { 
	HID_CLASS_GET_REPORT   = 0x01,
	HID_CLASS_GET_IDLE     = 0x02,
	HID_CLASS_GET_PROTOCOL = 0x03,
	HID_CLASS_SET_REPORT   = 0x09,
	HID_CLASS_SET_IDLE     = 0x0A,
	HID_CLASS_SET_PROTOCOL = 0x0B
} HID_CLASS_REQUESTS;

// HID protocols
typedef enum _HID_PROTOCOL { 
	HID_BOOT_PROTOCOL   = 0x00,
	HID_REPORT_PROTOCOL = 0x01
} HID_PROTOCOL;

// Common TRB types
typedef enum _TRB_TYPES_ENUM { 
	xNORMAL                = 0x01, 
	xSETUP_STAGE           = 0x02, 
	xDATA_STAGE            = 0x03, 
	xSTATUS_STAGE          = 0x04, 
	xISOCH                 = 0x05, 
	xLINK                  = 0x06,
	xEVENT_DATA            = 0x07, 
	xNO_OP                 = 0x08,
	xENABLE_SLOT           = 0x09, 
	xDISABLE_SLOT          = 0x0A, 
	xADDRESS_DEVICE        = 0x0B, 
	xCONFIG_EP             = 0x0C, 
	xEVALUATE_CONTEXT      = 0x0D, 
	xRESET_EP              = 0x0E,
	xSTOP_EP               = 0x0F, 
	xSET_TR_DEQUEUE        = 0x10, 
	xRESET_DEVICE          = 0x11, 
	xFORCE_EVENT           = 0x12, 
	xDEG_BANDWIDTH         = 0x13, 
	xSET_LAT_TOLERANCE     = 0x14,
	xGET_PORT_BAND         = 0x15, 
	xFORCE_HEADER          = 0x16,
	NO_OP_CMD              = 0x17,  
	// 24 - 31 = reserved
	xTRANS_EVENT           = 0x20, 
	xCOMMAND_COMPLETION    = 0x21, 
	xPORT_STATUS_CHANGE    = 0x22,
	xBANDWIDTH_REQUEST     = 0x23, 
	xDOORBELL_EVENT        = 0x24,
	xHOST_CONTROLLER_EVENT = 0x25, 
	xDEVICE_NOTIFICATION   = 0x26, 
	xMFINDEX_WRAP          = 0x27, 
	// 40 - 47 = reserved
	// 48 - 63 = Vendor Defined
} TRB_TYPES_ENUM;

// Descriptor types
typedef enum _DESCRIPTOR_TYPES { 
	DEVICE                = 0x01, 
	CONFIG                = 0x02, 
	STRING                = 0x03, 
	INTERFACE             = 0x04, 
	ENDPOINT              = 0x05, 
	DEVICE_QUALIFIER      = 0x06,
	OTHER_SPEED_CONFIG    = 0x07, 
	INTERFACE_POWER       = 0x08, 
	OTG                   = 0x09, 
	DEBUG                 = 0x0A, 
	INTERFACE_ASSOSIATION = 0x0B,
	HID                   = 0x21,
	HID_REPORT            = 0x22, 
	HID_PHYSICAL          = 0x23, 
	INTERFACE_FUNCTION    = 0x24,
	ENDPOINT_FUNCTION     = 0x25,
	HUB                   = 0x29
} DESCRIPTOR_TYPES;

// Endpoint types
typedef enum _ENDPOINT_TYPES {
	xCONTROL_EP     = 0x00, 
	xISOCHRONOUS_EP = 0x01, 
	xBULK_EP        = 0x02,
	xINTERRUPT_EP   = 0x03 
} ENDPOINT_TYPES;

/** functions declarations **/
BOOL   xhci_start_controller        (XHCI* x);
void*  xhci_alloc_memory            (UINT_32 byte, UINT_32 alignment, UINT_32 page_boundary);
void   xhci_free_memory             (void* pointer);
void   xhci_slot_configuration      (XHCI* x, UINT_32 port, UINT_32 speed);
void   xhci_slot_release            (XHCI* x, UINT_32 port, UINT_32 speed);
XHCI*  xhci_instance_to_idt         (void);
BOOL   xhci_get_critical_event      (void);
void   xhci_slot_set_address        (XHCI* x, BOOL BSR);
void   xhci_setup_data_status_stages(XHCI* x, void* target, UINT_32 length, UINT_32 max_packet, BOOL first_or_second);
void   xhci_string_descriptor       (XHCI* x, UINT_8 max_packet, UINT_8 level);
void   xhci_configure_endpoint      (XHCI* x);
void   xhci_HID_report              (XHCI* x, UINT_8 max_packet);
void   xhci_set_idle_HID            (XHCI* x);
void   xhci_set_protocol_HID        (XHCI* x);
void   xhci_get_protocol_HID        (XHCI* x);
void   get_contexts                 (XHCI* x, xHCI_SLOT_CONTEXT* global_slot, xHCI_EP_CONTEXT* global_ep0);
void   xhci_reset_endpoint          (XHCI* x, UINT_32 ep);
void   xhci_set_configuration_device(XHCI* x);
void   xhci_hid_test_mouse          (XHCI* x);

#endif
