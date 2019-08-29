#ifndef _FILESYSTEM__H__
#define _FILESYSTEM__H__

#include "../include/AHCI.h"
#include "../include/PCI.h"
#include "../include/TIMER.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _PARTITION {
	unsigned char  bootable;
	unsigned char  start_head;
	unsigned char  start_sector   : 6;
	unsigned short start_cylinder : 10;
	unsigned char  partition_id;
	unsigned char  end_head;
	unsigned char  end_sector     : 6;
	unsigned short end_cylinder   : 10;
	unsigned int   start_lba;
	unsigned int   length;
} __attribute__ ((packed)) 
PARTITION, *PPARTITION; // 16 bytes

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _EXTENDEDBOOTRECORD {
	unsigned char  bootloader[446];              // must be zero
	PARTITION      extendedPartitions[2];        // <-- main needed entries
	PARTITION      unusedExtendedPartitions[2];  // must be zero
	unsigned short magicNumber;                  // must be 0xAA55
} __attribute__ ((packed)) 
EXTENDEDBOOTRECORD, *PEXTENDEDBOOTRECORD;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _MASTERBOOTRECORD {
	unsigned char  bootloader[440];
	unsigned int   signature;
	unsigned short unused;
	PARTITION      primaryPartitions[4];
	unsigned short magicNumber; 
} __attribute__ ((packed)) 
MASTERBOOTRECORD, *PMASTERBOOTRECORD;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _BIOSPARAMETERBLOCK {
	unsigned char  jump[3];
	unsigned char  softName[8];
	unsigned short bytesPerSector;
	unsigned char  sectorPerCluster;
	unsigned short reservedSectors;
	unsigned char  fatCopies;
	unsigned short rootDirEntries;
	unsigned short totalSectors;
	unsigned char  mediaType;
        unsigned short fatSectorsCount;
	unsigned short sectorsPerTrack;
	unsigned short headCount;
	unsigned int   hiddenSectors;
	unsigned int   totalSectorCount;
	unsigned int   tableSize;
	unsigned short extFLags;
	unsigned short fatVersion;
	unsigned int   rootCluster;
	unsigned short fatInfo;
	unsigned short backupSector;
	unsigned char  reserved0[12];
	unsigned char  driveNumber;
	unsigned char  reserved;
	unsigned char  bootSignature;
	unsigned int   volumeId;
	unsigned char  volumeLabel[11];
	unsigned char  fatTypeLabel[8];
	unsigned char  ImanOSReserved[422];
} __attribute__ ((packed)) 
BIOSPARAMETERBLOCK, *PBIOSPARAMETERBLOCK;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _DIRECTORYENTRY {
	unsigned char  name[8];
	unsigned char  ext[3];
	unsigned char  attributes;
	unsigned char  reserved;
	unsigned char  cTimeTenth;
	unsigned short cTime;
	unsigned short cDate;
	unsigned short aTime;
	unsigned short firstClusterHigh;
	unsigned short wTime;
	unsigned short wDate;
	unsigned short firstClusterLow;
	unsigned int   size;
} __attribute__ ((packed)) 
DIRECTORYENTRY, *PDIRECTORYENTRY; // 32 bytes

typedef struct _FILESYSTEM {
	PSATA           sata;
	unsigned int    reserved[3];
} __attribute__ ((packed)) 
FILESYSTEM, *PFILESYSTEM;

typedef struct _HDPARAMS {
	PSATA           sata;
	unsigned int    root;
	unsigned int    data;
	unsigned int    sectorsPerCluster;
} __attribute__ ((packed)) 
HDPARAMS, *PHDPARAMS;

typedef struct _DESCRIPTOR {
	unsigned int       validity;
	unsigned long long tree;
} __attribute__ ((packed)) 
DESCRIPTOR, *PDESCRIPTOR;


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int RegisterFilesystem( PFILESYSTEM filesystem, PSATA sata );
HDPARAMS     DriveEntry(void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _FILESYSTEM__H__
