#include "../include/FILESYSTEM.h"
#include "../include/PRINT.h"
#include "../include/IDT.h"
#include "../include/MEMORY.h"
#include "../include/DOSSPEC.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

unsigned int sata_in_use = 0;
unsigned int RootStartAsDriveEntry = 0; // visible only to FILE.c
unsigned int RootData = 0; 
unsigned int RootSectorsPerCluster = 0; 
PSATA        RootSata = 0;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void Filesystem_MemZero(void* mem, unsigned int bytes)
{
    unsigned int        blocks = bytes >> 3;
    unsigned char       remain = bytes &  7;
    unsigned long long* dst    = (unsigned long long*)mem; 
    unsigned char*      ddst   = 0; 
    while(blocks--) { *dst++ = 0ULL; }
    ddst = (unsigned char*)dst;
    while(remain--) { *ddst++ = 0; } 
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int ReadBiosBlock( PSATA hd, unsigned int partitionOffset )
{
	BIOSPARAMETERBLOCK bpb;
	unsigned int       status;
	unsigned int       fatStart  = 0;
	unsigned int       fatSize   = 0;
	unsigned int       fatCopies = 0;
	unsigned int       dataStart = 0;
	unsigned int       rootStart = 0;
	DIRECTORYENTRY     dirent[16];
    
	status = read(&hd->abar->ports[hd->sata_port_number], partitionOffset, 0, 1 /*sizeof(BIOSPARAMETERBLOCK)*/, (unsigned char*)&bpb);
	if (!status)
	{
		panic("Reading harddrive BIOSPARAMETERBLOCK failed\n");
		return 0;
	}
	
	fatStart  = partitionOffset + bpb.reservedSectors;
	fatSize   = bpb.tableSize;
	fatCopies = (unsigned int)bpb.fatCopies;
	dataStart = fatStart  + fatSize * bpb.fatCopies;
	rootStart = dataStart + bpb.sectorPerCluster * (bpb.rootCluster - 2);

	status = read(&hd->abar->ports[hd->sata_port_number], rootStart, 0, 1 /* that's exactly 1 sector of 512 bytes */, (unsigned char*)&dirent[0]);
	if (!status)
	{
		panic("Reading harddrive Directory failed\n");
		return 0;
	}
	
	RootStartAsDriveEntry = rootStart;
	RootData              = dataStart;
	RootSectorsPerCluster = bpb.sectorPerCluster;
	RootSata              = hd;
	
	if (!RootStartAsDriveEntry || !RootData || !RootSectorsPerCluster || !RootSata)
	{
		panic("Initializing DriveVolume failed\n");
		return 0;
	}
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int ReadOperation( HDPARAMS DriveVolume, PDESCRIPTOR descriptor, unsigned char* Buffer, unsigned int Bytes )
{
	unsigned int   status;
	unsigned int   paperCluster  = 0; 
    unsigned int   paperSector   = 0;
	DIRECTORYENTRY dirent[16];
	unsigned int   root          = DriveVolume.root;
	PSATA          hd            = DriveVolume.sata;
	unsigned int   data          = DriveVolume.data;
	unsigned int   secpclus      = DriveVolume.sectorsPerCluster;
	unsigned char  which_dirent;
	int            i             = 0;
	
	/* sanity check */
	unsigned long long tree                  = descriptor->tree;
	unsigned char      descriptor_tree_count = (unsigned char)(tree >> 56);
	unsigned char      validity_descriptor   = (descriptor->validity & 0x0000FF00) >> 8;
    
	if(descriptor_tree_count != validity_descriptor)
	{
		panic("provided paper descriptor could not pass the sanity-validity\n");
		return 0;
	}
	
	status = read(&hd->abar->ports[hd->sata_port_number], root, 0, 1, (unsigned char*)&dirent[0]);
	if (!status)
	{
		panic("Reading DriveRoot failed\n");
		return 0;
	}
	
	while(descriptor_tree_count)
	{
		which_dirent = (tree & ((unsigned long long)(0x0F) << i)) >> i;
		paperCluster  = (((unsigned int)(dirent[which_dirent].firstClusterHigh) << 16)) | (((unsigned int)dirent[which_dirent].firstClusterLow));
		paperSector   = data + secpclus * (paperCluster - 2);
		status       = (descriptor_tree_count==1) ? read(&hd->abar->ports[hd->sata_port_number], paperSector, 0, 1, Buffer)
			                                      : read(&hd->abar->ports[hd->sata_port_number], paperSector, 0, 1, (unsigned char*)&dirent[0]);
		if (!status)
		{
			panic("Reading 'PORT & PAPERS' failed\n");
			return 0;
		}
		i += 4;
		descriptor_tree_count--;
	}
	
	
	Buffer[dirent[which_dirent].size] = '\0';
	printk((char*)Buffer);
	printk("\n");
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

DESCRIPTOR OpenOperation( HDPARAMS DriveVolume, char* DosPath )
{
	DESCRIPTOR      desc                            = { .validity = 0x00000000, .tree = 0ULL };
	unsigned char   IMOS_OPEN                      = 0xE1;
	unsigned char   IMOS_TREE_NIBBLE_COUNT         = 0x00;
	unsigned char   IMOS_NOTHING_FLAG              = 0x00;
	unsigned char   IMOS_RESERVED                  = 0x88;	
	unsigned int    status, j, k, _offs, offs, directory_children = 0;
	DIRECTORYENTRY  dirent[16];            
	unsigned int    root                           = DriveVolume.root;
	PSATA           hd                             = DriveVolume.sata;
	unsigned int    data                           = DriveVolume.data;
	unsigned int    secpclus                       = DriveVolume.sectorsPerCluster;
	DOSNAME         dosNamePackage                 = { .next = 0 };
	PDOSNAME        pointer_to_dos_name            = 0;
	status = read(&hd->abar->ports[hd->sata_port_number], root, 0, 1, (unsigned char*)&dirent[0]);
	if (!status)
	{
		panic("Reading DriveRoot failed\n");
		return desc;
	}
	
	/* here we construct the naming tree linked list */
	status = DosStringToDosNamePackage(DosPath, &dosNamePackage);
	if (!status)
	{
		panic("Assigning DosNamePackage failed\n");
		return desc;
	}
    
	pointer_to_dos_name = &dosNamePackage;
	char* thisPath = (char*)(pointer_to_dos_name->name);
	
	while(pointer_to_dos_name != 0)
	{
		for(j=0;j<16;j++)
		{
			if (dirent[j].name[0] == 0x00)
				break;		
			if (dirent[j].name[0] == 0xE5)
				continue;
			
			char tmp[15];
		    for(k=0;k<11;k++)
		    	tmp[k] = dirent[j].name[k];
		
			status = DosStrcmp(tmp, thisPath, 11);
			
			if( status )
			{
				//printk(thisPath);
				//printk(" success with j=%\n", j);
				
				desc.tree |= ((unsigned long long)j << directory_children);
				directory_children += 4;
				IMOS_TREE_NIBBLE_COUNT++;
				
			    _offs = ((unsigned int)(dirent[j].firstClusterHigh) << 16) | ((unsigned int)dirent[j].firstClusterLow);
			     offs = data + secpclus * (_offs - 2);
				status = read(&hd->abar->ports[hd->sata_port_number], offs, 0, 1 , (unsigned char*)&dirent[0]);
				if (!status)
				{
					panic("Reading ***subDirectory failed\n");
					return desc;
				}
				break;
			}
		}
		pointer_to_dos_name = pointer_to_dos_name->next;
		thisPath = (char*)(pointer_to_dos_name->name);
	}
	desc.tree    |= ((unsigned long long)IMOS_TREE_NIBBLE_COUNT << 56);
	desc.validity = ((unsigned int)IMOS_OPEN << 24) | ((unsigned int)IMOS_NOTHING_FLAG << 16) | ((unsigned int)IMOS_TREE_NIBBLE_COUNT << 8) | (IMOS_RESERVED);
	
    status = ReleaseDosPackage(&dosNamePackage);
    if (!status)
	{
		panic("Releasing DosNamePackage failed\n");
        desc.tree     = 0ULL;
        desc.validity = 0;
		return desc;
	}
    
	return desc;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int ReadPartition( PSATA hd )
{
	unsigned int       status, next_pointer;
	int                j, k, m, tmp, i;
	MASTERBOOTRECORD   mbr;
	//EXTENDEDBOOTRECORD ebr[10];
	
	status = read(&(hd->abar->ports[hd->sata_port_number]), 0, 0, 1, (unsigned char*)&mbr);
	if (!status)
	{
		panic("Reading harddrive MASTERBOOTRECORD failed\n");
		return 0;
	}
	
	//.printk("MBR: ");
	//.for(j=0x1BE;j<=0x01FF;j++)
	//.	printk("^ ", (int)((unsigned char*)&mbr)[j]);
	//.printk("\n");
	
	if (mbr.magicNumber != 0xAA55)
	{
		panic("Illegal MBR\n");
		return 0;
	}
	
	for(j=0;j<4;j++)
	{
		if(mbr.primaryPartitions[j].partition_id == 0x00)
			continue;
	
		//printk("Partition % is ", j);
		//if(mbr.primaryPartitions[j].bootable == 0x80)
		//	printk("Bootable with Type ");
		//else 
		//	printk("Not bootable with Type ");
		//printk("^\n", (mbr.primaryPartitions[j].partition_id));
		
		//if(mbr.primaryPartitions[j].partition_id == 0x05) //extended partition
		//{
		//	i = 0;
		//	next_pointer = mbr.primaryPartitions[j].start_lba;
		//	while(i<10)
		//	{
		//		status = read(&hd->abar->ports[hd->sata_port_number], next_pointer, 0, 1, (unsigned char*)&ebr[i]);
		//		if (!status)
		//		{
		//			panic("Reading harddrive 1st extended partition failed\n");
		//			return 0;
		//		}
		//		
		//		printk("% extended partition: LBA %, length %\n", i, ebr[i].extendedPartitions[0].start_lba, ebr[i].extendedPartitions[0].length);
		//		//for(k=0x1BE;k<0x1CE;k++)
		//		//{
		//		//	//linux_swap partition 0x82 partition_id or linux 0x83
		//		//	tmp = (int)( ((unsigned char*)&ebr[i])[k] );
		//		//	printk("^ ", tmp);
		//		//}
		//		//printk("\n");
		//		
		//		/* remember that the next address is relative to the main extended partition !! */
		//		next_pointer = (mbr.primaryPartitions[j].start_lba) + ebr[i].extendedPartitions[1].start_lba;
		//		
		//		//WaitSecond(10);
		//		//clear_screen();
		//		//status = ReadBiosBlock(hd, mbr.primaryPartitions[j].start_lba + ebr0.extendedPartitions[0].start_lba);
		//		//if (!status)
		//		//{
		//		//	panic("Reading bios block failed\n");
		//		//	return 0;
		//		//}
		//		
		//		if (ebr[i].extendedPartitions[1].start_lba == 0)
		//			break;
		//		i++;
		//	}
		//}
		
		if(mbr.primaryPartitions[j].partition_id == 0x0B) //FAT32
		{
			status = ReadBiosBlock(hd, mbr.primaryPartitions[j].start_lba);
			if (!status)
			{
				panic("Reading FAT32 drive failed\n");
				return 0;
			}
		}
	}
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int RegisterFilesystem( PFILESYSTEM filesystem, PSATA sata )
{
	unsigned int status;	
	if(!sata)
	{
		panic("Invalid SATA object\n");
		return 0;
	}
	
	if(!filesystem)
	{
		panic("Invalid FILESYSTEM object passed\n");
		return 0;
	}
	
	if(!sata_in_use)
	{
		filesystem->sata = sata;
		status = ReadPartition(filesystem->sata);
		if(!status)
		{
			panic("Filesystem failed to start reading harddisk partition\n");
			return 0;
		}
		sata_in_use = 1;
	}
	else
	{
		panic("Only and only one single FILESYSTEM can be registered\n");
		return 0;
	}	
	
	printk( "        >>> Filesystem registered successfully <<<\n");
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

HDPARAMS DriveEntry(void)
{
	HDPARAMS hdparsms = {
		.sata = RootSata,
		.root = RootStartAsDriveEntry,
		.data = RootData,
		.sectorsPerCluster = RootSectorsPerCluster
	};
	return hdparsms;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
