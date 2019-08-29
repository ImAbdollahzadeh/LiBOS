#ifndef _AHCI__H__
#define _AHCI__H__

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	    0x96690101  // Port multiplier
#define AHCI_DEV_NULL   0
#define AHCI_DEV_SATA   1
#define AHCI_DEV_SEMB   2
#define AHCI_DEV_PM     3
#define AHCI_DEV_SATAPI 4
#define HBA_PORT_IPM_ACTIVE  1
#define HBA_PORT_DET_PRESENT 3

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef enum _FIS_TYPE {
	FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
	FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
	FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
	FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
	FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
} FIS_TYPE;

typedef struct _FIS_REG_H2D {
	// DWORD 0
	unsigned char  fis_type;	// FIS_TYPE_REG_H2D
 
	unsigned char  pmport:4;	// Port multiplier
	unsigned char  rsv0:3;	// Reserved
	unsigned char  c:1;		// 1: Command, 0: Control
 
	unsigned char  command;	// Command register
	unsigned char  featurel;	// Feature register, 7:0
 
	// DWORD 1
	unsigned char  lba0;		// LBA low register, 7:0
	unsigned char  lba1;		// LBA mid register, 15:8
	unsigned char  lba2;		// LBA high register, 23:16
	unsigned char  device;	// Device register
 
	// DWORD 2
	unsigned char  lba3;		// LBA register, 31:24
	unsigned char  lba4;		// LBA register, 39:32
	unsigned char  lba5;		// LBA register, 47:40
	unsigned char  featureh;	// Feature register, 15:8
 
	// DWORD 3
	unsigned char  countl;	// Count register, 7:0
	unsigned char  counth;	// Count register, 15:8
	unsigned char  icc;		// Isochronous command completion
	unsigned char  control;	// Control register
 
	// DWORD 4
	unsigned char  rsv1[4];	// Reserved
} __attribute__ ((packed))
FIS_REG_H2D, *PFIS_REG_H2D;

typedef struct _FIS_REG_D2H {
	// DWORD 0
	unsigned char  fis_type;    // FIS_TYPE_REG_D2H
 
	unsigned char  pmport:4;    // Port multiplier
	unsigned char  rsv0:2;      // Reserved
	unsigned char  i:1;         // Interrupt bit
	unsigned char  rsv1:1;      // Reserved
 
	unsigned char  status;      // Status register
	unsigned char  error;       // Error register
 
	// DWORD 1
	unsigned char  lba0;        // LBA low register, 7:0
	unsigned char  lba1;        // LBA mid register, 15:8
	unsigned char  lba2;        // LBA high register, 23:16
	unsigned char  device;      // Device register
 
	// DWORD 2
	unsigned char  lba3;        // LBA register, 31:24
	unsigned char  lba4;        // LBA register, 39:32
	unsigned char  lba5;        // LBA register, 47:40
	unsigned char  rsv2;        // Reserved
 
	// DWORD 3
	unsigned char  countl;      // Count register, 7:0
	unsigned char  counth;      // Count register, 15:8
	unsigned char  rsv3[2];     // Reserved
 
	// DWORD 4
	unsigned char  rsv4[4];     // Reserved
} __attribute__ ((packed))
FIS_REG_D2H, *PFIS_REG_D2H;

typedef struct _FIS_DATA {
	// DWORD 0
	unsigned char  fis_type;	// FIS_TYPE_DATA
 
	unsigned char  pmport:4;	// Port multiplier
	unsigned char  rsv0:4;	// Reserved
 
	unsigned char  rsv1[2];	// Reserved
 
	// DWORD 1 ~ N
	unsigned int   data[1];	// Payload
} __attribute__ ((packed))
FIS_DATA;

typedef struct _FIS_PIO_SETUP {
	// DWORD 0
	unsigned char  fis_type;	// FIS_TYPE_PIO_SETUP
 
	unsigned char  pmport:4;	// Port multiplier
	unsigned char  rsv0:1;	// Reserved
	unsigned char  d:1;		// Data transfer direction, 1 - device to host
	unsigned char  i:1;		// Interrupt bit
	unsigned char  rsv1:1;
 
	unsigned char  status;	// Status register
	unsigned char  error;		// Error register
 
	// DWORD 1
	unsigned char  lba0;		// LBA low register, 7:0
	unsigned char  lba1;		// LBA mid register, 15:8
	unsigned char  lba2;		// LBA high register, 23:16
	unsigned char  device;		// Device register
 
	// DWORD 2
	unsigned char  lba3;		// LBA register, 31:24
	unsigned char  lba4;		// LBA register, 39:32
	unsigned char  lba5;		// LBA register, 47:40
	unsigned char  rsv2;		// Reserved
 
	// DWORD 3
	unsigned char  countl;	// Count register, 7:0
	unsigned char  counth;	// Count register, 15:8
	unsigned char  rsv3;		// Reserved
	unsigned char  e_status;	// New value of status register
 
	// DWORD 4
	unsigned short tc;		// Transfer count
	unsigned char  rsv4[2];	// Reserved
} __attribute__ ((packed))
FIS_PIO_SETUP, *PFIS_PIO_SETUP;

typedef struct _FIS_DMA_SETUP {
	// DWORD 0
	unsigned char      fis_type;	  // FIS_TYPE_DMA_SETUP
                                      
	unsigned char      pmport:4;	  // Port multiplier
	unsigned char      rsv0:1;		  // Reserved
	unsigned char      d:1;			  // Data transfer direction, 1 - device to host
	unsigned char      i:1;		      // Interrupt bit
	unsigned char      a:1;           // Auto-activate. Specifies if DMA Activate FIS is needed
                       
    unsigned char      rsved[2];      // Reserved
 
	//DWORD 1&2
    unsigned long long DMAbufferID;   /* DMA Buffer Identifier. Used to Identify DMA buffer in host memory. 
	                                     SATA Spec says host specific and not in Spec. Trying AHCI spec might work */
 
    //DWORD 3
    unsigned int 	   rsvd;          //More reserved
 
    //DWORD 4
    unsigned int       DMAbufOffset;  //Byte offset into buffer. First 2 bits must be 0
                       
    //DWORD 5          
    unsigned int       TransferCount; //Number of bytes to transfer. Bit 0 must be 0
                       
    //DWORD 6          
    unsigned int       resvd;         //Reserved
 
} __attribute__ ((packed))
FIS_DMA_SETUP, *PFIS_DMA_SETUP;

typedef volatile struct _HBA_PORT {
	unsigned int clb;		// 0x00, command list base address, 1K-byte aligned
	unsigned int clbu;		// 0x04, command list base address upper 32 bits
	unsigned int fb;		// 0x08, FIS base address, 256-byte aligned
	unsigned int fbu;		// 0x0C, FIS base address upper 32 bits
	unsigned int is;		// 0x10, interrupt status
	unsigned int ie;		// 0x14, interrupt enable
	unsigned int cmd;		// 0x18, command and status
	unsigned int rsv0;		// 0x1C, Reserved
	unsigned int tfd;		// 0x20, task file data
	unsigned int sig;		// 0x24, signature
	unsigned int ssts;		// 0x28, SATA status (SCR0:SStatus)
	unsigned int sctl;		// 0x2C, SATA control (SCR2:SControl)
	unsigned int serr;		// 0x30, SATA error (SCR1:SError)
	unsigned int sact;		// 0x34, SATA active (SCR3:SActive)
	unsigned int ci;		// 0x38, command issue
	unsigned int sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	unsigned int fbs;		// 0x40, FIS-based switch control
	unsigned int rsv1[11];	// 0x44 ~ 0x6F, Reserved
	unsigned int vendor[4];	// 0x70 ~ 0x7F, vendor specific
} __attribute__ ((packed))
HBA_PORT, *PHBA_PORT;

typedef volatile struct _HBA_FIS {
	// 0x00
	FIS_DMA_SETUP	dsfis;		// DMA Setup FIS
	unsigned char	pad0[4];
 
	// 0x20
	FIS_PIO_SETUP	psfis;		// PIO Setup FIS
	unsigned char   pad1[12];
 
	// 0x40
	FIS_REG_D2H	    rfis;		// Register – Device to Host FIS
	unsigned char	pad2[4];
 
	// 0x58
	unsigned char	sdbfis[8];  // Set Device Bit FIS
 
	// 0x60
	unsigned char	ufis[64];
 
	// 0xA0
	unsigned char	rsv[0x100-0xA0];
} __attribute__ ((packed))
HBA_FIS, *PHBA_FIS;

typedef volatile struct _HBA_MEM {
	// 0x00 - 0x2B, Generic Host Control
	unsigned int cap;		// 0x00, Host capability
	unsigned int ghc;		// 0x04, Global host control
	unsigned int is;		// 0x08, Interrupt status
	unsigned int pi;		// 0x0C, Port implemented
	unsigned int vs;		// 0x10, Version
	unsigned int ccc_ctl;	// 0x14, Command completion coalescing control
	unsigned int ccc_pts;	// 0x18, Command completion coalescing ports
	unsigned int em_loc;	// 0x1C, Enclosure management location
	unsigned int em_ctl;	// 0x20, Enclosure management control
	unsigned int cap2;		// 0x24, Host capabilities extended
	unsigned int bohc;		// 0x28, BIOS/OS handoff control and status
 
	// 0x2C - 0x9F, Reserved
	unsigned char  rsv[0xA0-0x2C];
 
	// 0xA0 - 0xFF, Vendor specific registers
	unsigned char  vendor[0x100-0xA0];
 
	// 0x100 - 0x10FF, Port control registers
	HBA_PORT ports[1];	// 1 ~ 32
} __attribute__ ((packed))
HBA_MEM, *PHBA_MEM;

typedef struct _HBA_CMD_HEADER {
	// DW0
	unsigned char  cfl:5;	// Command FIS length in DWORDS, 2 ~ 16
	unsigned char  a:1;		// ATAPI
	unsigned char  w:1;		// Write, 1: H2D, 0: D2H
	unsigned char  p:1;		// Prefetchable
 
	unsigned char  r:1;		// Reset
	unsigned char  b:1;		// BIST
	unsigned char  c:1;		// Clear busy upon R_OK
	unsigned char  rsv0:1;	// Reserved
	unsigned char  pmp:4;	// Port multiplier port
 
	unsigned short prdtl;	// Physical region descriptor table length in entries
 
	// DW1
	volatile
	unsigned int prdbc;		// Physical region descriptor byte count transferred
 
	// DW2, 3
	unsigned int ctba;		// Command table descriptor base address
	unsigned int ctbau;		// Command table descriptor base address upper 32 bits
 
	// DW4 - 7
	unsigned int rsv1[4];	// Reserved
} __attribute__ ((packed))
HBA_CMD_HEADER, *PHBA_CMD_HEADER;

typedef struct _HBA_PRDT_ENTRY {
	unsigned int dba;		// Data base address
	unsigned int dbau;		// Data base address upper 32 bits
	unsigned int rsv0;		// Reserved
 
	// DW3
	unsigned int dbc:22;	// Byte count, 4M max
	unsigned int rsv1:9;	// Reserved
	unsigned int i:1;		// Interrupt on completion
} __attribute__ ((packed))
HBA_PRDT_ENTRY, *PHBA_PRDT_ENTRY;

typedef struct _HBA_CMD_TBL {
	// 0x00
	unsigned char  cfis[64];	  // Command FIS
 
	// 0x40
	unsigned char  acmd[16];      // ATAPI command, 12 or 16 bytes
 
	// 0x50
	unsigned char  rsv[48];	      // Reserved
 
	// 0x80
	HBA_PRDT_ENTRY prdt_entry[1]; // Physical region descriptor table entries, 0 ~ 65535
} __attribute__ ((packed))
HBA_CMD_TBL, *PHBA_CMD_TBL;

typedef struct _SATA {
    unsigned int   valid;
    unsigned int   bus;
    unsigned short device;
    void*          bar5;
    PHBA_MEM       abar;
    unsigned int   sata_port_number;
} __attribute__ ((packed)) 
SATA, *PSATA;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void  AhciBegin            ( PSATA hd );
int   read                 ( PHBA_PORT port, unsigned int startl, unsigned int starth, unsigned int count, unsigned char* buf );
PSATA ReportSATAToFileystem( void );

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _AHCI__H__
