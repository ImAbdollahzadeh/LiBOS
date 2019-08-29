#include "../include/AHCI.h"
#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

#define HBA_PxCMD_CR            (1 << 15)  /* CR  - Command list Running       */
#define HBA_PxCMD_FR            (1 << 14)  /* FR  - FIS receive Running        */
#define HBA_PxCMD_FRE           (1 <<  4)  /* FRE - FIS Receive Enable         */
#define HBA_PxCMD_SUD           (1 <<  1)  /* SUD - Spin-Up Device             */
#define HBA_PxCMD_ST            (1 <<  0)  /* ST  - Start (command processing) */
#define ATA_DEV_BUSY            0x80
#define ATA_DEV_DRQ             0x08
#define ATA_CMD_READ_DMA_EX     0x25
#define HBA_PxIS_TFES           (1 << 30)  /* TFES - Task File Error Status */

       PHBA_MEM       abar;
static HBA_CMD_HEADER cmdlist[32] __attribute__((aligned(1024)));
static HBA_FIS        fisstorage  __attribute__((aligned(256)));
static HBA_CMD_TBL    cmdtbls[32] __attribute__((aligned(128)));

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void AHCI_MemZero(void* mem, unsigned int bytes)
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

void PortRebase(PHBA_PORT port, PHBA_CMD_HEADER cl, PHBA_CMD_TBL ctlist, PHBA_FIS fisbase)
{
    int i;
	
	port->cmd &= ~HBA_PxCMD_ST;
	port->cmd &= ~HBA_PxCMD_FRE;
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
    
    port->clb  = (unsigned int)cl;
	port->clbu = 0;
	AHCI_MemZero((void*)port->clb, 1024);
	//.printk("port->clb: ^", (int)((void*)port->clb));
	
    port->fb   = (unsigned int)fisbase;
	port->fbu  = 0;
    AHCI_MemZero((void*)port->fb, 256);
    //.printk(", port->fb: ^", (int)((void*)port->fb));
    
    for (i=0; i<32; i++)
    {
        cl[i].prdtl = 8; 
        cl[i].ctba  = (unsigned int)ctlist;
        cl[i].ctbau = 0;
        AHCI_MemZero((void*)cl[i].ctba, 256);
    }
    
    while (port->cmd & HBA_PxCMD_CR);
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
	
	//.printk("PortRebase done!\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static int CheckType(PHBA_PORT port)
{
	unsigned int  ssts = port->ssts;
	unsigned char ipm  = (ssts >> 8) & 0x0F;
	unsigned char det  = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 
	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

int FindCmdSlot(PHBA_PORT port)
{
	int i;
	
	// If not set in SACT and CI, the slot is free
	unsigned int slots        = (port->sact | port->ci);
	int          num_of_slots = (abar->cap & 0x0f00)>>8;
	for (i=0; i<num_of_slots; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	return -1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

int read(PHBA_PORT port, unsigned int startl, unsigned int starth, unsigned int count, unsigned char* buf)
{
	int i, spin = 0; 
	
	port->is = (unsigned int) -1;
	int slot = FindCmdSlot(port);
	if (slot == -1)
	{
		panic("slot=-1\n");
		return 0;
	}		
 
	PHBA_CMD_HEADER cmdheader = (PHBA_CMD_HEADER)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(unsigned int);	// Command FIS size
	cmdheader->w = 0;		                                    // Read from device
	cmdheader->prdtl = (unsigned short)((count-1)>>4) + 1;	    // PRDT entries count
 
	PHBA_CMD_TBL cmdtbl = (PHBA_CMD_TBL)(cmdheader->ctba);
	AHCI_MemZero(cmdtbl, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
 
	// 8K bytes (16 sectors) per PRDT
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (unsigned int) buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[cmdheader->prdtl-1].dba = (unsigned int) buf;
	cmdtbl->prdt_entry[cmdheader->prdtl-1].dbc = (count<<9)-1;	// 512 bytes per sector
	cmdtbl->prdt_entry[cmdheader->prdtl-1].i = 1;
 
	// Setup command
	PFIS_REG_H2D cmdfis = (PFIS_REG_H2D)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (unsigned char)startl;
	cmdfis->lba1 = (unsigned char)(startl>>8);
	cmdfis->lba2 = (unsigned char)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (unsigned char)(startl>>24);
	cmdfis->lba4 = (unsigned char)starth;
	cmdfis->lba5 = (unsigned char)(starth>>8);
 
	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
		spin++;
	if (spin == 1000000)
	{
		panic("port is hung\n");
		return 0;
	}
 
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			printk("read disk error %\n", 0);
			return 0;
		}
	}
 
	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		printk("read disk error %\n", 1);
		return 0;
	}
 
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void ProbePort(PHBA_MEM abar, int* which_port)
{
	unsigned int pi = abar->pi;
	int i=0, j=0;
	unsigned char main_buffer[40000];
	int status = 0;
	AHCI_MemZero(main_buffer, 40000);
	while (i<32)
	{
		if (pi&1)
		{
			int dt = CheckType(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				printk("SATA drive found at port ^\n", i);
				PortRebase(&abar->ports[i], cmdlist, cmdtbls, &fisstorage);
				*which_port = i;
				//status = read(&abar->ports[i], 0x13E857F8, 0, 8, main_buffer);
				//if(status) 
				//{
				//	clear_screen();
				//	for(j=0;j<100;j++)
				//	{
				//		if(main_buffer[j] != 0)
				//			print_char(main_buffer[j]);
				//	}
				//}
			}
			else if (dt == AHCI_DEV_SATAPI)
				printk("SATAPI drive found at port ^\n", i);
			else if (dt == AHCI_DEV_SEMB)
				printk("SEMB drive found at port ^\n", i);
			else if (dt == AHCI_DEV_PM)
				printk("PM drive found at port ^\n", i);
			else
				printk("No drive found at port ^\n", i);
		}
		pi >>= 1;
		i++;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void AhciBegin( PSATA hd )
{
	void* bar5 = hd->bar5;
	int harddisk_port;
	
	if (!hd || !hd->valid)
	{
		panic("Trying to manipulate an unregistered or a wrong harddrive\n");
		return;
	}
	
	hd->abar             = (PHBA_MEM)bar5;
	abar                 = (PHBA_MEM)hd->abar;
	ProbePort(abar, &harddisk_port);
	hd->sata_port_number = harddisk_port;	
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
