#include "../include/AHCI.h"
#include "../include/PRINT.h"
#include "../include/TIMER.h"

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

       HBA_MEM*       abar;
static HBA_CMD_HEADER cmdlist[32] __attribute__((aligned(1024)));
static HBA_FIS        fisstorage  __attribute__((aligned(256)));
static HBA_CMD_TBL    cmdtbls[32] __attribute__((aligned(128)));

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void PortRebase(HBA_PORT* port, HBA_CMD_HEADER* cl, HBA_CMD_TBL* ctlist, HBA_FIS* fisbase)
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
    
    port->clb  = (UINT_32)cl;
	port->clbu = 0;
	__LiBOS_MemZero((void*)port->clb, 1024);
	//.printk("port->clb: ^", (int)((void*)port->clb));
	
    port->fb   = (UINT_32)fisbase;
	port->fbu  = 0;
    __LiBOS_MemZero((void*)port->fb, 256);
    //.printk(", port->fb: ^", (int)((void*)port->fb));
    
    for (i=0; i<32; i++)
    {
        cl[i].prdtl = 8; 
        cl[i].ctba  = (UINT_32)ctlist;
        cl[i].ctbau = 0;
        __LiBOS_MemZero((void*)cl[i].ctba, 256);
    }
    
    while (port->cmd & HBA_PxCMD_CR);
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
	
	//.printk("PortRebase done!\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static int CheckType(HBA_PORT* port)
{
	UINT_32  ssts = port->ssts;
	UINT_8 ipm  = (ssts >> 8) & 0x0F;
	UINT_8 det  = ssts & 0x0F;
 
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

int FindCmdSlot(HBA_PORT* port)
{
	int i;
	
	// If not set in SACT and CI, the slot is free
	UINT_32 slots        = (port->sact | port->ci);
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

int read(HBA_PORT* port, UINT_32 startl, UINT_32 starth, UINT_32 count, UINT_8* buf)
{
	INT_32 i, spin = 0; 
	
	port->is = (UINT_32) -1;
	int slot = FindCmdSlot(port);
	//:::if (slot == -1)
	//:::{
	//:::	panic("slot=-1\n");
	//:::	return 0;
	//:::}
 
	HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) >> 2;       // Command FIS size
	cmdheader->w = 0;                                // Read from device
	cmdheader->prdtl = (UINT_16)((count-1)>>4) + 1;	 // PRDT entries count
 
	HBA_CMD_TBL* cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
	__LiBOS_MemZero(cmdtbl, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
 
	// 8K bytes (16 sectors) per PRDT
	INT_32 tmp = cmdheader->prdtl-1;
	for (i=0; i<tmp; i++)
	{
		cmdtbl->prdt_entry[i].dba = (UINT_32) buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024-1; // 8K bytes (this value should always be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4096;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[cmdheader->prdtl-1].dba = (UINT_32) buf;
	cmdtbl->prdt_entry[cmdheader->prdtl-1].dbc = (count<<9)-1;	// 512 bytes per sector
	cmdtbl->prdt_entry[cmdheader->prdtl-1].i = 1;
 
	// Setup command
	FIS_REG_H2D* cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (UINT_8)(startl);
	cmdfis->lba1 = (UINT_8)(startl>>8);
	cmdfis->lba2 = (UINT_8)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (UINT_8)(startl>>24);
	cmdfis->lba4 = (UINT_8)starth;
	cmdfis->lba5 = (UINT_8)(starth>>8);
 
	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) /*&& spin < 1000000*/);
	//:::	spin++;
	//:::if (spin == 1000000)
	//:::{
	//:::	panic("port is hung\n");
	//:::	return 0;
	//:::}
	
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		//:::if (port->is & HBA_PxIS_TFES)	// Task file error
		//:::{
		//:::	printk("read disk error %\n", 0);
		//:::	return 0;
		//:::}
	}
 	
	//:://Check again
	//::if (port->is & HBA_PxIS_TFES)
	//::{
	//::	printk("read disk error %\n", 1);
	//::	return 0;
	//::}
 
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void ProbePort(HBA_MEM* abar, int* which_port)
{
	UINT_32 pi = abar->pi;
	UINT_32 i=0, j=0;
	UINT_8 main_buffer[40000];
	UINT_32 status = 0;
	__LiBOS_MemZero(main_buffer, 40000);
	while (i<32)
	{
		if (pi&1)
		{
			int dt = CheckType(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				printk("SATA drive - at port ^\n", i);
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
				printk("SATAPI drive - at port ^\n", i);
			else if (dt == AHCI_DEV_SEMB)
				printk("SEMB drive - at port ^\n", i);
			else if (dt == AHCI_DEV_PM)
				printk("PM drive - at port ^\n", i);
			else
				printk("No drive - at port ^\n", i);
		}
		pi >>= 1;
		i++;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void AhciBegin( SATA* hd )
{
	void* bar5 = hd->bar5;
	int harddisk_port;
	
	if (!hd || !hd->valid)
	{
		panic("Trying to manipulate an unregistered or a wrong harddrive\n");
		return;
	}
	
	hd->abar             = (HBA_MEM*)bar5;
	abar                 = (HBA_MEM*)hd->abar;
	ProbePort(abar, &harddisk_port);
	hd->sata_port_number = harddisk_port;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
