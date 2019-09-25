#ifndef _AHCI__H__
#define _AHCI__H__

#include "LiBOS_CORE.h"

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
	UINT_8  fis_type;	// FIS_TYPE_REG_H2D
	UINT_8  pmport:4;	// Port multiplier
	UINT_8  rsv0:3;	// Reserved
	UINT_8  c:1;		// 1: Command, 0: Control
	UINT_8  command;	// Command register
	UINT_8  featurel;	// Feature register, 7:0
	// DWORD 1
	UINT_8  lba0;		// LBA low register, 7:0
	UINT_8  lba1;		// LBA mid register, 15:8
	UINT_8  lba2;		// LBA high register, 23:16
	UINT_8  device;	// Device register
	// DWORD 2
	UINT_8  lba3;		// LBA register, 31:24
	UINT_8  lba4;		// LBA register, 39:32
	UINT_8  lba5;		// LBA register, 47:40
	UINT_8  featureh;	// Feature register, 15:8
	// DWORD 3
	UINT_8  countl;	// Count register, 7:0
	UINT_8  counth;	// Count register, 15:8
	UINT_8  icc;		// Isochronous command completion
	UINT_8  control;	// Control register
	// DWORD 4
	UINT_8  rsv1[4];	// Reserved
} __attribute__ ((packed)) FIS_REG_H2D;

typedef struct _FIS_REG_D2H {
	// DWORD 0
	UINT_8  fis_type;    // FIS_TYPE_REG_D2H
	UINT_8  pmport:4;    // Port multiplier
	UINT_8  rsv0:2;      // Reserved
	UINT_8  i:1;         // Interrupt bit
	UINT_8  rsv1:1;      // Reserved
	UINT_8  status;      // Status register
	UINT_8  error;       // Error register
	// DWORD 1
	UINT_8  lba0;        // LBA low register, 7:0
	UINT_8  lba1;        // LBA mid register, 15:8
	UINT_8  lba2;        // LBA high register, 23:16
	UINT_8  device;      // Device register
	// DWORD 2
	UINT_8  lba3;        // LBA register, 31:24
	UINT_8  lba4;        // LBA register, 39:32
	UINT_8  lba5;        // LBA register, 47:40
	UINT_8  rsv2;        // Reserved
	// DWORD 3
	UINT_8  countl;      // Count register, 7:0
	UINT_8  counth;      // Count register, 15:8
	UINT_8  rsv3[2];     // Reserved
	// DWORD 4
	UINT_8  rsv4[4];     // Reserved
} __attribute__ ((packed)) FIS_REG_D2H;

typedef struct _FIS_DATA {
	// DWORD 0
	UINT_8  fis_type;	// FIS_TYPE_DATA
	UINT_8  pmport:4;	// Port multiplier
	UINT_8  rsv0:4;	// Reserved
	UINT_8  rsv1[2];	// Reserved
	// DWORD 1 ~ N
	UINT_32   data[1];	// Payload
} __attribute__ ((packed)) FIS_DATA;

typedef struct _FIS_PIO_SETUP {
	// DWORD 0
	UINT_8  fis_type;	// FIS_TYPE_PIO_SETUP
	UINT_8  pmport:4;	// Port multiplier
	UINT_8  rsv0:1;	// Reserved
	UINT_8  d:1;		// Data transfer direction, 1 - device to host
	UINT_8  i:1;		// Interrupt bit
	UINT_8  rsv1:1;
	UINT_8  status;	// Status register
	UINT_8  error;		// Error register
	// DWORD 1
	UINT_8  lba0;		// LBA low register, 7:0
	UINT_8  lba1;		// LBA mid register, 15:8
	UINT_8  lba2;		// LBA high register, 23:16
	UINT_8  device;		// Device register
	// DWORD 2
	UINT_8  lba3;		// LBA register, 31:24
	UINT_8  lba4;		// LBA register, 39:32
	UINT_8  lba5;		// LBA register, 47:40
	UINT_8  rsv2;		// Reserved
	// DWORD 3
	UINT_8  countl;	// Count register, 7:0
	UINT_8  counth;	// Count register, 15:8
	UINT_8  rsv3;		// Reserved
	UINT_8  e_status;	// New value of status register
	// DWORD 4
	UINT_16 tc;		// Transfer count
	UINT_8  rsv4[2];	// Reserved
} __attribute__ ((packed)) FIS_PIO_SETUP;

typedef struct _FIS_DMA_SETUP {
	// DWORD 0
	UINT_8      fis_type;	  // FIS_TYPE_DMA_SETUP                                 
	UINT_8      pmport:4;	  // Port multiplier
	UINT_8      rsv0:1;		  // Reserved
	UINT_8      d:1;			  // Data transfer direction, 1 - device to host
	UINT_8      i:1;		      // Interrupt bit
	UINT_8      a:1;           // Auto-activate. Specifies if DMA Activate FIS is needed                 
    UINT_8      rsved[2];      // Reserved
	//DWORD 1&2
    UINT_64 DMAbufferID;   /* DMA Buffer Identifier. Used to Identify DMA buffer in host memory. 
	                          SATA Spec says host specific and not in Spec. Trying AHCI spec might work */
    //DWORD 3
    UINT_32 	   rsvd;          //More reserved
    //DWORD 4
    UINT_32       DMAbufOffset;  //Byte offset into buffer. First 2 bits must be 0
    //DWORD 5          
    UINT_32       TransferCount; //Number of bytes to transfer. Bit 0 must be 0
    //DWORD 6          
    UINT_32       resvd;         //Reserved
 
} __attribute__ ((packed)) FIS_DMA_SETUP;

typedef volatile struct _HBA_PORT {
	UINT_32 clb;		// 0x00, command list base address, 1K-byte aligned
	UINT_32 clbu;		// 0x04, command list base address upper 32 bits
	UINT_32 fb;		// 0x08, FIS base address, 256-byte aligned
	UINT_32 fbu;		// 0x0C, FIS base address upper 32 bits
	UINT_32 is;		// 0x10, interrupt status
	UINT_32 ie;		// 0x14, interrupt enable
	UINT_32 cmd;		// 0x18, command and status
	UINT_32 rsv0;		// 0x1C, Reserved
	UINT_32 tfd;		// 0x20, task file data
	UINT_32 sig;		// 0x24, signature
	UINT_32 ssts;		// 0x28, SATA status (SCR0:SStatus)
	UINT_32 sctl;		// 0x2C, SATA control (SCR2:SControl)
	UINT_32 serr;		// 0x30, SATA error (SCR1:SError)
	UINT_32 sact;		// 0x34, SATA active (SCR3:SActive)
	UINT_32 ci;		// 0x38, command issue
	UINT_32 sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	UINT_32 fbs;		// 0x40, FIS-based switch control
	UINT_32 rsv1[11];	// 0x44 ~ 0x6F, Reserved
	UINT_32 vendor[4];	// 0x70 ~ 0x7F, vendor specific
} __attribute__ ((packed)) HBA_PORT;

typedef volatile struct _HBA_FIS {
	// 0x00
	FIS_DMA_SETUP	dsfis;		// DMA Setup FIS
	UINT_8	pad0[4];
	// 0x20
	FIS_PIO_SETUP	psfis;		// PIO Setup FIS
	UINT_8   pad1[12];
	// 0x40
	FIS_REG_D2H	    rfis;		// Register – Device to Host FIS
	UINT_8	pad2[4];
	// 0x58
	UINT_8	sdbfis[8];  // Set Device Bit FIS
	// 0x60
	UINT_8	ufis[64];
	// 0xA0
	UINT_8	rsv[0x100-0xA0];
} __attribute__ ((packed)) HBA_FIS;

typedef volatile struct _HBA_MEM {
	// 0x00 - 0x2B, Generic Host Control
	UINT_32 cap;		// 0x00, Host capability
	UINT_32 ghc;		// 0x04, Global host control
	UINT_32 is;		// 0x08, Interrupt status
	UINT_32 pi;		// 0x0C, Port implemented
	UINT_32 vs;		// 0x10, Version
	UINT_32 ccc_ctl;	// 0x14, Command completion coalescing control
	UINT_32 ccc_pts;	// 0x18, Command completion coalescing ports
	UINT_32 em_loc;	// 0x1C, Enclosure management location
	UINT_32 em_ctl;	// 0x20, Enclosure management control
	UINT_32 cap2;		// 0x24, Host capabilities extended
	UINT_32 bohc;		// 0x28, BIOS/OS handoff control and status
	// 0x2C - 0x9F, Reserved
	UINT_8  rsv[0xA0-0x2C];
	// 0xA0 - 0xFF, Vendor specific registers
	UINT_8  vendor[0x100-0xA0];
	// 0x100 - 0x10FF, Port control registers
	HBA_PORT ports[1];	// 1 ~ 32
} __attribute__ ((packed)) HBA_MEM;

typedef struct _HBA_CMD_HEADER {
	// DW0
	UINT_8  cfl:5;	// Command FIS length in DWORDS, 2 ~ 16
	UINT_8  a:1;		// ATAPI
	UINT_8  w:1;		// Write, 1: H2D, 0: D2H
	UINT_8  p:1;		// Prefetchable
	UINT_8  r:1;		// Reset
	UINT_8  b:1;		// BIST
	UINT_8  c:1;		// Clear busy upon R_OK
	UINT_8  rsv0:1;	// Reserved
	UINT_8  pmp:4;	// Port multiplier port
	UINT_16 prdtl;	// Physical region descriptor table length in entries
	// DW1
	volatile
	UINT_32 prdbc;		// Physical region descriptor byte count transferred
	// DW2, 3
	UINT_32 ctba;		// Command table descriptor base address
	UINT_32 ctbau;		// Command table descriptor base address upper 32 bits
	// DW4 - 7
	UINT_32 rsv1[4];	// Reserved
} __attribute__ ((packed)) HBA_CMD_HEADER;

typedef struct _HBA_PRDT_ENTRY {
	UINT_32 dba;		// Data base address
	UINT_32 dbau;		// Data base address upper 32 bits
	UINT_32 rsv0;		// Reserved
	// DW3
	UINT_32 dbc:22;	// Byte count, 4M max
	UINT_32 rsv1:9;	// Reserved
	UINT_32 i:1;		// Interrupt on completion
} __attribute__ ((packed)) HBA_PRDT_ENTRY;

typedef struct _HBA_CMD_TBL {
	// 0x00
	UINT_8  cfis[64];	  // Command FIS
 
	// 0x40
	UINT_8  acmd[16];      // ATAPI command, 12 or 16 bytes
 
	// 0x50
	UINT_8  rsv[48];	      // Reserved
 
	// 0x80
	HBA_PRDT_ENTRY prdt_entry[1]; // Physical region descriptor table entries, 0 ~ 65535
} __attribute__ ((packed)) HBA_CMD_TBL;

typedef struct _SATA {
    UINT_32  valid;
    UINT_32  bus;
    UINT_16  device;
    void*    bar5;
    HBA_MEM* abar;
    UINT_32  sata_port_number;
} __attribute__ ((packed)) SATA;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void   AhciBegin            ( SATA* hd );
INT_32 read                 ( HBA_PORT* port, UINT_32 startl, UINT_32 starth, UINT_32 count, UINT_8* buf );
SATA*  ReportSATAToFileystem( void );

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _AHCI__H__
