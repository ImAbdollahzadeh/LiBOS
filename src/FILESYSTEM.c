#include "../include/FILESYSTEM.h"
#include "../include/PRINT.h"
#include "../include/IDT.h"
#include "../include/MEMORY.h"
#include "../include/DOSSPEC.h"

#define SWAP_ENDIANNESS(u8_ptr, N) do { *N = (((UINT_32)(u8_ptr)[3]) | ((UINT_32)(u8_ptr[2]) << 8) | ((UINT_32)(u8_ptr[1]) << 16) | ((UINT_32)(u8_ptr[0]) << 24)); } while(0)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

UINT_32 sata_in_use = 0;
UINT_32 RootStartAsDriveEntry = 0; // visible only to FILE.c
UINT_32 RootData = 0; 
UINT_32 RootSectorsPerCluster = 0; 
/**/UINT_32 Fat_start = 0;
/**/UINT_32 bps = 0;
SATA*   RootSata = 0;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 ReadBiosBlock( SATA* hd, UINT_32 partitionOffset )
{
	BIOSPARAMETERBLOCK bpb;
	UINT_32            status;
	UINT_32            fatStart  = 0;
	UINT_32            fatSize   = 0;
	UINT_32            fatCopies = 0;
	UINT_32            dataStart = 0;
	UINT_32            rootStart = 0;
	DIRECTORYENTRY     dirent[16];
    
	status = read(&hd->abar->ports[hd->sata_port_number], partitionOffset, 0, 1 /*sizeof(BIOSPARAMETERBLOCK)*/, (UINT_8*)&bpb);
	if (!status)
	{
		panic("Reading harddrive BIOSPARAMETERBLOCK failed\n");
		return 0;
	}
	
	fatStart  = partitionOffset + bpb.reservedSectors;
	/**/Fat_start = fatStart;
	fatSize   = bpb.tableSize;
	fatCopies = (UINT_32)bpb.fatCopies;
	dataStart = fatStart  + fatSize * bpb.fatCopies;
	rootStart = dataStart + bpb.sectorPerCluster * (bpb.rootCluster - 2);
	/**/bps = bpb.bytesPerSector;

	status = read(&hd->abar->ports[hd->sata_port_number], rootStart, 0, 1 /* that's exactly 1 sector of 512 bytes */, (UINT_8*)&dirent[0]);
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

UINT_32 ReadOperation( HDPARAMS DriveVolume, DESCRIPTOR* descriptor, UINT_8* Buffer, UINT_32 Bytes, UINT_8 ect_size_operation )
{
	UINT_32        status;
	UINT_32        paperCluster  = 0; 
    UINT_32        paperSector   = 0;
	DIRECTORYENTRY dirent[16];
	UINT_32        root          = DriveVolume.root;
	SATA*          hd            = DriveVolume.sata;
	UINT_32        data          = DriveVolume.data;
	UINT_32        secpclus      = DriveVolume.sectorsPerCluster;
	UINT_8         which_dirent;
	int            i             = 0;
	
	/* sanity check */
	UINT_64 tree                  = descriptor->tree;
	UINT_8  descriptor_tree_count = (UINT_8)(tree >> 56);
	UINT_8  validity_descriptor   = (descriptor->validity & 0x0000FF00) >> 8;

	if(descriptor_tree_count != validity_descriptor)
	{
		panic("provided paper descriptor could not pass the sanity-validity\n");
		return 0;
	}
	
	status = read(&hd->abar->ports[hd->sata_port_number], root, 0, 1, (UINT_8*)&dirent[0]);
	if (!status)
	{
		panic("Reading DriveRoot failed\n");
		return 0;
	}

	while(descriptor_tree_count)
	{
		which_dirent = (tree & ((UINT_64)(0x0F) << i)) >> i;
		paperCluster  = (((UINT_32)(dirent[which_dirent].firstClusterHigh) << 16)) | (((UINT_32)dirent[which_dirent].firstClusterLow));
		paperSector   = data + secpclus * (paperCluster - 2);
		//printk("---------------------------\n");
		//printk("---------------------------\n");
		//printk("---------------------------\n");
		
		//printk("SectorPerCluster: %\n", secpclus);
		//printk("BytesPerSector: %\n", (UINT_32)bps);
		if(descriptor_tree_count==1)
		{
			INT_32 SIZE = dirent[which_dirent].size;
			
			// only for ect size retrieve
			if(ect_size_operation == 0x01)
			{
				*(UINT_32*)Buffer = (UINT_32)SIZE;
				//printk("ect size=%\n", (UINT_32)SIZE);
				return 1;
			}
			
			//printk("file size: % bytes\n", (UINT_32)SIZE);
			UINT_32 next_paper_cluster = paperCluster;
			//printk("first cluster:^\n", next_paper_cluster);
			UINT_8 fat_buffer[513];
			UINT_32 n_sector = 0;
			UINT_8* main_buffer = (UINT_8*)(&Buffer[0]);
			UINT_32 sector_offset = 0;
			
			
			
			
			
			/* for <1KB EXT files */
			unsigned int how_many_0x200 =  1 + (SIZE / 0x200);
			//printk("how many 0x200? ~> %\n", how_many_0x200);
			read(&hd->abar->ports[hd->sata_port_number], (paperSector + sector_offset), 0, how_many_0x200, main_buffer);
			goto __END__;
			
			
			
			
			
			
			while(SIZE > 0)
			{
				paperSector = data + (secpclus * (next_paper_cluster - 2));
				sector_offset = 0;
				
				/* for video stuff */
				while(sector_offset < secpclus)
				{
					read(&hd->abar->ports[hd->sata_port_number], (paperSector + sector_offset), 0, 16, main_buffer);
					main_buffer = (UINT_8*)((void*)(PHYSICAL_ADDRESS(main_buffer) + (8192)));
					SIZE -= (8192);
					//n_sector+=16;
					sector_offset+=16;
				}
				
				UINT_32 Fat_sector_for_current_cluster = Fat_start + (next_paper_cluster >> 7);
				read(&hd->abar->ports[hd->sata_port_number], Fat_sector_for_current_cluster, 0, 1, fat_buffer);
				
				UINT_32 Fat_offset_in_sector_for_current_cluster = ((next_paper_cluster << 2) - 1) & 511;
				UINT_8* tmp_ptr = (UINT_8*)((void*)(&fat_buffer[Fat_offset_in_sector_for_current_cluster]));
				//__LiBOS_HexDump((void*)tmp_ptr, 4, "");
				next_paper_cluster = (*(UINT_32*)tmp_ptr) & 0x0FFFFFFF;
				//printk("next cluster:^\n", next_paper_cluster);
			}
		}
		else
			status = read(&hd->abar->ports[hd->sata_port_number], paperSector, 0, 1, (UINT_8*)&dirent[0]);
		
		//:if (!status)
		//:{
		//:	panic("Reading 'PORT & PAPERS' failed\n");
		//:	return 0;
		//:}
		
		i += 4;
		descriptor_tree_count--;
	}
	
__END__:
	//Buffer[dirent[which_dirent].size] = '\0';
	//printk((char*)Buffer);
	//printk("\n");
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

DESCRIPTOR OpenOperation( HDPARAMS DriveVolume, INT_8* DosPath )
{
	DESCRIPTOR      desc                            = { .validity = 0x00000000, .tree = 0ULL };
	UINT_8          LiBOS_OPEN                      = 0xE1;
	UINT_8          LiBOS_TREE_NIBBLE_COUNT         = 0x00;
	UINT_8          LiBOS_NOTHING_FLAG              = 0x00;
	UINT_8          LiBOS_RESERVED                  = 0x88;
	UINT_32         status, j, k, _offs, offs, directory_children = 0;
	DIRECTORYENTRY  dirent[16];            
	UINT_32         root                           = DriveVolume.root;
	SATA*           hd                             = DriveVolume.sata;
	UINT_32         data                           = DriveVolume.data;
	UINT_32         secpclus                       = DriveVolume.sectorsPerCluster;
	DOSNAME         dosNamePackage                 = { .next = 0 };
	DOSNAME*        pointer_to_dos_name            = 0;
	status = read(&hd->abar->ports[hd->sata_port_number], root, 0, 1, (UINT_8*)&dirent[0]);
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
	INT_8* thisPath = (INT_8*)(pointer_to_dos_name->name);
	
	while(pointer_to_dos_name != 0)
	{
		for(j=0;j<16;j++)
		{
			if (dirent[j].name[0] == 0x00)
				break;
			if (dirent[j].name[0] == 0xE5)
				continue;
			
			INT_8 tmp[15];
		    for(k=0;k<11;k++)
				tmp[k] = dirent[j].name[k];
		
			status = DosStrcmp(tmp, thisPath, 11);
			
			if( status )
			{
				//printk(thisPath);
				//printk(" success with j=%\n", j);
				
				desc.tree |= ((UINT_64)j << directory_children);
				directory_children += 4;
				LiBOS_TREE_NIBBLE_COUNT++;
				
			    _offs = ((UINT_32)(dirent[j].firstClusterHigh) << 16) | ((UINT_32)dirent[j].firstClusterLow);
			     offs = data + secpclus * (_offs - 2);
				status = read(&hd->abar->ports[hd->sata_port_number], offs, 0, 1 , (UINT_8*)&dirent[0]);
				if (!status)
				{
					panic("Reading ***subDirectory failed\n");
					return desc;
				}
				break;
			}
		}
		pointer_to_dos_name = pointer_to_dos_name->next;
		thisPath = (INT_8*)(pointer_to_dos_name->name);
	}
	desc.tree    |= ((UINT_64)LiBOS_TREE_NIBBLE_COUNT << 56);
	desc.validity = ((UINT_32)LiBOS_OPEN << 24) | ((UINT_32)LiBOS_NOTHING_FLAG << 16) | ((UINT_32)LiBOS_TREE_NIBBLE_COUNT << 8) | (LiBOS_RESERVED);
	
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

UINT_32 ReadPartition( SATA* hd )
{
	UINT_32          status, next_pointer;
	int              j, k, m, tmp, i;
	MASTERBOOTRECORD mbr;
	//EXTENDEDBOOTRECORD ebr[10];
	
	status = read(&(hd->abar->ports[hd->sata_port_number]), 0, 0, 1, (UINT_8*)&mbr);
	if (!status)
	{
		panic("Reading harddrive MASTERBOOTRECORD failed\n");
		return 0;
	}
	
	//.printk("MBR: ");
	//.for(j=0x1BE;j<=0x01FF;j++)
	//.	printk("^ ", (int)((UINT_8*)&mbr)[j]);
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
		//		status = read(&hd->abar->ports[hd->sata_port_number], next_pointer, 0, 1, (UINT_8*)&ebr[i]);
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
		//		//	tmp = (int)( ((UINT_8*)&ebr[i])[k] );
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
		
		if(mbr.primaryPartitions[j].partition_id == 0x0C) //FAT32
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

UINT_32 RegisterFilesystem( FILESYSTEM* filesystem, SATA* sata )
{
	UINT_32 status;
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
	
	//-printk( "        >>> Filesystem registered successfully <<<\n");
	
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
