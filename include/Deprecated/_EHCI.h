#ifndef _EHCI__H__
#define _EHCI__H__

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define MAX_QH 8
#define MAX_TD 32

typedef struct _LINK {
    struct _LINK* prev;
    struct _LINK* next;
} __attribute__ ((packed))
LINK, *PLINK;

typedef struct _CAPABILITY_REGISTERS {
	unsigned char      capLength;
	unsigned char      reserved;
	unsigned short     hciVersion;
	unsigned int       hcsParams;
	unsigned int       hccParams;
	unsigned long long hcspPortRoute;
} __attribute__ ((packed))
CAPABILITY_REGISTERS, *PCAPABILITY_REGISTERS;

// structural_parameters Register
#define HCSPARAMS_N_PORTS_MASK          (15 << 0)   // Number of Ports
#define HCSPARAMS_PPC                   (1 << 4)    // Port Power Control
#define HCSPARAMS_PORT_ROUTE            (1 << 7)    // Port Routing Rules
#define HCSPARAMS_N_PCC_MASK            (15 << 8)   // Number of Ports per Companion Controller
#define HCSPARAMS_N_PCC_SHIFT           8
#define HCSPARAMS_N_CC_MASK             (15 << 12)  // Number of Companion Controllers
#define HCSPARAMS_N_CC_SHIFT            12
#define HCSPARAMS_P_INDICATOR           (1 << 16)   // Port Indicator
#define HCSPARAMS_DPN_MASK              (15 << 20)  // Debug Port Number
#define HCSPARAMS_DPN_SHIFT             20

// capability_parameters Register
#define HCCPARAMS_64_BIT                (1 << 0)    // 64-bit Addressing Capability
#define HCCPARAMS_PFLF                  (1 << 1)    // Programmable Frame List Flag
#define HCCPARAMS_ASPC                  (1 << 2)    // Asynchronous Schedule Park Capability
#define HCCPARAMS_IST_MASK              (15 << 4)   // Isochronous Sheduling Threshold
#define HCCPARAMS_EECP_MASK             (255 << 8)  // EHCI Extended Capabilities Pointer
#define HCCPARAMS_EECP_SHIFT            8

typedef volatile struct _OPERATION_REGISTERS {
	unsigned int   usbCmd;
	unsigned int   usbSts;
	unsigned int   usbIntr;
	unsigned int   frameIndex;
	unsigned int   ctrlDsSegment;
	unsigned int   periodicListBase;
	unsigned int   asyncListAddr;
	unsigned int   reserved[9];
	unsigned int   configFlag;
	unsigned int   ports[];
} __attribute__ ((packed))
OPERATION_REGISTERS, *POPERATION_REGISTERS;

// USB Command Register

#define CMD_RS                          (1 << 0)    // Run/Stop
#define CMD_HCRESET                     (1 << 1)    // Host Controller Reset
#define CMD_FLS_MASK                    (3 << 2)    // Frame List Size
#define CMD_FLS_SHIFT                   2
#define CMD_PSE                         (1 << 4)    // Periodic Schedule Enable
#define CMD_ASE                         (1 << 5)    // Asynchronous Schedule Enable
#define CMD_IOAAD                       (1 << 6)    // Interrupt on Async Advance Doorbell
#define CMD_LHCR                        (1 << 7)    // Light Host Controller Reset
#define CMD_ASPMC_MASK                  (3 << 8)    // Asynchronous Schedule Park Mode Count
#define CMD_ASPMC_SHIFT                 8
#define CMD_ASPME                       (1 << 11)   // Asynchronous Schedule Park Mode Enable
#define CMD_ITC_MASK                    (255 << 16) // Interrupt Threshold Control
#define CMD_ITC_SHIFT                   16

// ------------------------------------------------------------------------------------------------
// USB Status Register

#define STS_USBINT                      (1 << 0)    // USB Interrupt
#define STS_ERROR                       (1 << 1)    // USB Error Interrupt
#define STS_PCD                         (1 << 2)    // Port Change Detect
#define STS_FLR                         (1 << 3)    // Frame List Rollover
#define STS_HSE                         (1 << 4)    // Host System Error
#define STS_IOAA                        (1 << 5)    // Interrupt on Async Advance
#define STS_HCHALTED                    (1 << 12)   // Host Controller Halted
#define STS_RECLAMATION                 (1 << 13)   // Reclamation
#define STS_PSS                         (1 << 14)   // Periodic Schedule Status
#define STS_ASS                         (1 << 15)   // Asynchronous Schedule Status

// ------------------------------------------------------------------------------------------------
// USB Interrupt Enable Register

#define INTR_USBINT                     (1 << 0)    // USB Interrupt Enable
#define INTR_ERROR                      (1 << 1)    // USB Error Interrupt Enable
#define INTR_PCD                        (1 << 2)    // Port Change Interrupt Enable
#define INTR_FLR                        (1 << 3)    // Frame List Rollover Enable
#define INTR_HSE                        (1 << 4)    // Host System Error Enable
#define INTR_IOAA                       (1 << 5)    // Interrupt on Async Advance Enable

// ------------------------------------------------------------------------------------------------
// Frame Index Register

#define FR_MASK                         0x3fff

// ------------------------------------------------------------------------------------------------
// Configure Flag Register

#define CF_PORT_ROUTE                   (1 << 0)    // Configure Flag (CF)

// ------------------------------------------------------------------------------------------------
// Port Status and Control Registers

#define PORT_CONNECTION                 (1 << 0)    // Current Connect Status
#define PORT_CONNECTION_CHANGE          (1 << 1)    // Connect Status Change
#define PORT_ENABLE                     (1 << 2)    // Port Enabled
#define PORT_ENABLE_CHANGE              (1 << 3)    // Port Enable Change
#define PORT_OVER_CURRENT               (1 << 4)    // Over-current Active
#define PORT_OVER_CURRENT_CHANGE        (1 << 5)    // Over-current Change
#define PORT_FPR                        (1 << 6)    // Force Port Resume
#define PORT_SUSPEND                    (1 << 7)    // Suspend
#define PORT_RESET                      (1 << 8)    // Port Reset
#define PORT_LS_MASK                    (3 << 10)   // Line Status
#define PORT_LS_SHIFT                   10
#define PORT_POWER                      (1 << 12)   // Port Power
#define PORT_OWNER                      (1 << 13)   // Port Owner
#define PORT_IC_MASK                    (3 << 14)   // Port Indicator Control
#define PORT_IC_SHIFT                   14
#define PORT_TC_MASK                    (15 << 16)  // Port Test Control
#define PORT_TC_SHIFT                   16
#define PORT_WKCNNT_E                   (1 << 20)   // Wake on Connect Enable
#define PORT_WKDSCNNT_E                 (1 << 21)   // Wake on Disconnect Enable
#define PORT_WKOC_E                     (1 << 22)   // Wake on Over-current Enable
#define PORT_RWC                        (PORT_CONNECTION_CHANGE | PORT_ENABLE_CHANGE | PORT_OVER_CURRENT_CHANGE)

// ------------------------------------------------------------------------------------------------
typedef struct _TRANSFER_DESCRIPTOR {
    volatile unsigned int link;
    volatile unsigned int altLink;
    volatile unsigned int token;
    volatile unsigned int buffer[5];
    volatile unsigned int extBuffer[5];
    // internal fields
    unsigned int          tdNext;
    unsigned int          active;
    unsigned char         pad[4];
} __attribute__ ((packed))
TRANSFER_DESCRIPTOR, *PTRANSFER_DESCRIPTOR;

// TD Link Pointer
#define PTR_TERMINATE                   (1 << 0)
#define PTR_TYPE_MASK                   (3 << 1)
#define PTR_ITD                         (0 << 1)
#define PTR_QH                          (1 << 1)
#define PTR_SITD                        (2 << 1)
#define PTR_FSTN                        (3 << 1)

// TD Token
#define TD_TOK_PING                     (1 << 0)    // Ping State
#define TD_TOK_STS                      (1 << 1)    // Split Transaction State
#define TD_TOK_MMF                      (1 << 2)    // Missed Micro-Frame
#define TD_TOK_XACT                     (1 << 3)    // Transaction Error
#define TD_TOK_BABBLE                   (1 << 4)    // Babble Detected
#define TD_TOK_DATABUFFER               (1 << 5)    // Data Buffer Error
#define TD_TOK_HALTED                   (1 << 6)    // Halted
#define TD_TOK_ACTIVE                   (1 << 7)    // Active
#define TD_TOK_PID_MASK                 (3 << 8)    // PID Code
#define TD_TOK_PID_SHIFT                8
#define TD_TOK_CERR_MASK                (3 << 10)   // Error Counter
#define TD_TOK_CERR_SHIFT               10
#define TD_TOK_C_PAGE_MASK              (7 << 12)   // Current Page
#define TD_TOK_C_PAGE_SHIFT             12
#define TD_TOK_IOC                      (1 << 15)   // Interrupt on Complete
#define TD_TOK_LEN_MASK                 0x7fff0000  // Total Bytes to Transfer
#define TD_TOK_LEN_SHIFT                16
#define TD_TOK_D                        (1 << 31)   // Data Toggle
#define TD_TOK_D_SHIFT                  31

#define USB_PACKET_OUT                  0           // token 0xe1
#define USB_PACKET_IN                   1           // token 0x69
#define USB_PACKET_SETUP                2           // token 0x2d

// ------------------------------------------------------------------------------------------------

typedef struct _USB_DEVICE_REQUEST {
    unsigned char  type;
    unsigned char  req;
    unsigned short value;
    unsigned short index;
    unsigned short len;
} __attribute__ ((packed)) 
USB_DEVICE_REQUEST, *PUSB_DEVICE_REQUEST;

// USB Configuration Descriptor

typedef struct _USB_Configuration_Descriptor {
    unsigned char len;
    unsigned char type;
    unsigned short totalLen;
    unsigned char intfCount;
    unsigned char confValue;
    unsigned char confStr;
    unsigned char attributes;
    unsigned char maxPower;
} __attribute__ ((packed)) 
USB_Configuration_Descriptor, *PUSB_Configuration_Descriptor;

// USB Device Descriptor
typedef struct _USB_DEVICE_DESCRIPTOR {
    unsigned char len;
    unsigned char type;
    unsigned short usbVer;
    unsigned char devClass;
    unsigned char devSubClass;
    unsigned char devProtocol;
    unsigned char maxPacketSize;
    unsigned short vendorId;
    unsigned short productId;
    unsigned short deviceVer;
    unsigned char vendorStr;
    unsigned char productStr;
    unsigned char serialStr;
    unsigned char confCount;
} __attribute__ ((packed)) 
USB_DEVICE_DESCRIPTOR, *PUSB_DEVICE_DESCRIPTOR;

// USB Endpoint Descriptor
typedef struct _USB_ENDPOINT_DESCRIPTOR {
    unsigned char  len;
    unsigned char  type;
    unsigned char  addr;
    unsigned char  attributes;
    unsigned short maxPacketSize;
    unsigned char  interval;
} __attribute__ ((packed)) 
USB_ENDPOINT_DESCRIPTOR, *PUSB_ENDPOINT_DESCRIPTOR;

// USB String Descriptor

typedef struct _USB_STRING_DESCTIPTOR {
    unsigned char len;
    unsigned char type;
    unsigned short str[];
} __attribute__ ((packed))  
USB_STRING_DESCTIPTOR, *PUSB_STRING_DESCTIPTOR;

typedef struct _USB_END_POINT {
    USB_ENDPOINT_DESCRIPTOR desc;
    unsigned int            toggle;
} __attribute__ ((packed))
USB_END_POINT, *PUSB_END_POINT;

// USB Interface Descriptor
typedef struct _USB_INTERFACE_DESCRIPTOR {
    unsigned char len;
    unsigned char type;
    unsigned char intfIndex;
    unsigned char altSetting;
    unsigned char endpCount;
    unsigned char intfClass;
    unsigned char intfSubClass;
    unsigned char intfProtocol;
    unsigned char intfStr;
} __attribute__ ((packed)) 
USB_INTERFACE_DESCRIPTOR, *PUSB_INTERFACE_DESCRIPTOR;

// USB Transfer
typedef struct _USB_TRANSFER {
    PUSB_END_POINT      endp;
    PUSB_DEVICE_REQUEST req;
    void*               data;
    unsigned int        len;
    unsigned int        complete;
    unsigned int        success;
} __attribute__ ((packed))
USB_TRANSFER, *PUSB_TRANSFER;

// USB device
typedef struct _USB_DEVICE {
    struct _USB_DEVICE*      parent;
    struct _USB_DEVICE*      next;
    void*                    hc;
    void*                    drv;
    unsigned int             port;
    unsigned int             speed;
    unsigned int             addr;
    unsigned int             maxPacketSize;
    USB_END_POINT            endp;
    USB_INTERFACE_DESCRIPTOR intfDesc;
    void (*hcControl)(struct _USB_DEVICE* dev, PUSB_TRANSFER t);
    void (*hcIntr)   (struct _USB_DEVICE* dev, PUSB_TRANSFER t);
    void (*drvPoll)  (struct _USB_DEVICE* dev);
} __attribute__ ((packed))
USB_DEVICE, *PUSB_DEVICE;

// Queue Head
typedef struct _QUEUE_HEAD {
    unsigned int          qhlp;       // Queue Head Horizontal Link Pointer
    unsigned int          ch;         // Endpoint Characteristics
    unsigned int          caps;       // Endpoint Capabilities
    volatile unsigned int curLink;
    // matches a transfer descriptor
    volatile unsigned int nextLink;
    volatile unsigned int altLink;
    volatile unsigned int token;
    volatile unsigned int buffer[5];
    volatile unsigned int extBuffer[5];
    // internal fields
    PUSB_TRANSFER         transfer;
    LINK                  qhLink;
    unsigned int          tdHead;
    unsigned int          active;
    unsigned char         pad[20];
} __attribute__ ((packed))
QUEUE_HEAD, *PQUEUE_HEAD;

// Endpoint Characteristics
#define QH_CH_DEVADDR_MASK              0x0000007f  // Device Address
#define QH_CH_INACTIVE                  0x00000080  // Inactive on Next Transaction
#define QH_CH_ENDP_MASK                 0x00000f00  // Endpoint Number
#define QH_CH_ENDP_SHIFT                8
#define QH_CH_EPS_MASK                  0x00003000  // Endpoint Speed
#define QH_CH_EPS_SHIFT                 12
#define QH_CH_DTC                       0x00004000  // Data Toggle Control
#define QH_CH_H                         0x00008000  // Head of Reclamation List Flag
#define QH_CH_MPL_MASK                  0x07ff0000  // Maximum Packet Length
#define QH_CH_MPL_SHIFT                 16
#define QH_CH_CONTROL                   0x08000000  // Control Endpoint Flag
#define QH_CH_NAK_RL_MASK               0xf0000000  // Nak Count Reload
#define QH_CH_NAK_RL_SHIFT              28

// Endpoint Capabilities
#define QH_CAP_INT_SCHED_SHIFT          0
#define QH_CAP_INT_SCHED_MASK           0x000000ff  // Interrupt Schedule Mask
#define QH_CAP_SPLIT_C_SHIFT            8
#define QH_CAP_SPLIT_C_MASK             0x0000ff00  // Split Completion Mask
#define QH_CAP_HUB_ADDR_SHIFT           16
#define QH_CAP_HUB_ADDR_MASK            0x007f0000  // Hub Address
#define QH_CAP_PORT_MASK                0x3f800000  // Port Number
#define QH_CAP_PORT_SHIFT               23
#define QH_CAP_MULT_MASK                0xc0000000  // High-Bandwidth Pipe Multiplier
#define QH_CAP_MULT_SHIFT               30

typedef struct _EHCI {
    	PCAPABILITY_REGISTERS capRegs;
    	POPERATION_REGISTERS  opRegs;
    	unsigned int*         frameList;
    	PQUEUE_HEAD           qhPool;
    	PTRANSFER_DESCRIPTOR  tdPool;
    	PQUEUE_HEAD           asyncQH;
    	PQUEUE_HEAD           periodicQH;
	void*                 bar0;
	void*                 pci_pointer;
	unsigned int   	      valid;
    	unsigned short        bus;
    	unsigned short        device;
	unsigned short        function;
} __attribute__ ((packed))
EHCI, *PEHCI;


// USB Request Type

#define RT_TRANSFER_MASK                0x80
#define RT_DEV_TO_HOST                  0x80
#define RT_HOST_TO_DEV                  0x00

#define RT_TYPE_MASK                    0x60
#define RT_STANDARD                     0x00
#define RT_CLASS                        0x20
#define RT_VENDOR                       0x40

#define RT_RECIPIENT_MASK               0x1f
#define RT_DEV                          0x00
#define RT_INTF                         0x01
#define RT_ENDP                         0x02
#define RT_OTHER                        0x03

// ------------------------------------------------------------------------------------------------
// USB Device Requests

#define REQ_GET_STATUS                  0x00
#define REQ_CLEAR_FEATURE               0x01
#define REQ_SET_FEATURE                 0x03
#define REQ_SET_ADDR                    0x05
#define REQ_GET_DESC                    0x06
#define REQ_SET_DESC                    0x07
#define REQ_GET_CONF                    0x08
#define REQ_SET_CONF                    0x09
#define REQ_GET_INTF                    0x0a
#define REQ_SET_INTF                    0x0b
#define REQ_SYNC_FRAME                  0x0c

// ------------------------------------------------------------------------------------------------
// USB Hub Class Requests

#define REQ_CLEAR_TT_BUFFER             0x08
#define REQ_RESET_TT                    0x09
#define REQ_GET_TT_STATE                0x0a
#define REQ_STOP_TT                     0x0b

// ------------------------------------------------------------------------------------------------
// USB HID Interface Requests

#define REQ_GET_REPORT                  0x01
#define REQ_GET_IDLE                    0x02
#define REQ_GET_PROTOCOL                0x03
#define REQ_SET_REPORT                  0x09
#define REQ_SET_IDLE                    0x0a
#define REQ_SET_PROTOCOL                0x0b

// USB Standard Feature Selectors

#define F_DEVICE_REMOTE_WAKEUP          1   // Device
#define F_ENDPOINT_HALT                 2   // Endpoint
#define F_TEST_MODE                     3   // Device

// USB Hub Feature Seletcors

#define F_C_HUB_LOCAL_POWER             0   // Hub
#define F_C_HUB_OVER_CURRENT            1   // Hub
#define F_PORT_CONNECTION               0   // Port
#define F_PORT_ENABLE                   1   // Port
#define F_PORT_SUSPEND                  2   // Port
#define F_PORT_OVER_CURRENT             3   // Port
#define F_PORT_RESET                    4   // Port
#define F_PORT_POWER                    8   // Port
#define F_PORT_LOW_SPEED                9   // Port
#define F_C_PORT_CONNECTION             16  // Port
#define F_C_PORT_ENABLE                 17  // Port
#define F_C_PORT_SUSPEND                18  // Port
#define F_C_PORT_OVER_CURRENT           19  // Port
#define F_C_PORT_RESET                  20  // Port
#define F_PORT_TEST                     21  // Port
#define F_PORT_INDICATOR                22  // Port

#define USB_STRING_SIZE                 127

// USB Speeds

#define USB_FULL_SPEED                  0x00
#define USB_LOW_SPEED                   0x01
#define USB_HIGH_SPEED                  0x02

// USB Base Descriptor Types

#define USB_DESC_DEVICE                 0x01
#define USB_DESC_CONF                   0x02
#define USB_DESC_STRING                 0x03
#define USB_DESC_INTF                   0x04
#define USB_DESC_ENDP                   0x05

// EECP-based
#define USBLEGSUP                       0x00        // USB Legacy Support Extended Capability
#define UBSLEGCTLSTS                    0x04        // USB Legacy Support Control/Status

// ------------------------------------------------------------------------------------------------
// USB Legacy Support Register

#define USBLEGSUP_HC_OS                 0x01000000  // HC OS Owned Semaphore
#define USBLEGSUP_HC_BIOS               0x00010000  // HC BIOS Owned Semaphore
#define USBLEGSUP_NCP_MASK              0x0000ff00  // Next EHCI Extended Capability Pointer
#define USBLEGSUP_CAPID                 0x000000ff  // Capability ID

typedef struct _USB_DRIVER {
    unsigned int (*init)(PUSB_DEVICE dev);
} __attribute__ ((packed))
USB_DRIVER, *PUSB_DRIVER;

typedef struct _USB_CONTROLLER {
    struct _USB_CONTROLLER* next;
    void*                   hc;
    void (*poll)(struct _USB_CONTROLLER* controller);
} __attribute__ ((packed))
USB_CONTROLLER, *PUSB_CONTROLLER;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void EhciBegin(PEHCI ehci);

#endif //! _EHCI__H__
