#ifndef _FILESYSTEM__H__
#define _FILESYSTEM__H__

#include "LiBOS_CORE.h"
#include "AHCI.h"
#include "PCI.h"
#include "TIMER.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _PARTITION {
	UINT_8  bootable;
	UINT_8  start_head;
	UINT_8  start_sector   : 6;
	UINT_16 start_cylinder : 10;
	UINT_8  partition_id;
	UINT_8  end_head;
	UINT_8  end_sector     : 6;
	UINT_16 end_cylinder   : 10;
	UINT_32 start_lba;
	UINT_32 length;
} __attribute__ ((packed))  PARTITION; // 16 bytes

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _EXTENDEDBOOTRECORD {
	UINT_8  bootloader[446];              // must be zero
	PARTITION      extendedPartitions[2];        // <-- main needed entries
	PARTITION      unusedExtendedPartitions[2];  // must be zero
	UINT_16 magicNumber;                  // must be 0xAA55
} __attribute__ ((packed)) EXTENDEDBOOTRECORD;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _MASTERBOOTRECORD {
	UINT_8  bootloader[440];
	UINT_32   signature;
	UINT_16 unused;
	PARTITION      primaryPartitions[4];
	UINT_16 magicNumber; 
} __attribute__ ((packed)) MASTERBOOTRECORD;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _BIOSPARAMETERBLOCK {
	UINT_8  jump[3];
	UINT_8  softName[8];
	UINT_16 bytesPerSector;
	UINT_8  sectorPerCluster;
	UINT_16 reservedSectors;
	UINT_8  fatCopies;
	UINT_16 rootDirEntries;
	UINT_16 totalSectors;
	UINT_8  mediaType;
    UINT_16 fatSectorsCount;
	UINT_16 sectorsPerTrack;
	UINT_16 headCount;
	UINT_32   hiddenSectors;
	UINT_32   totalSectorCount;
	UINT_32   tableSize;
	UINT_16 extFLags;
	UINT_16 fatVersion;
	UINT_32   rootCluster;
	UINT_16 fatInfo;
	UINT_16 backupSector;
	UINT_8  reserved0[12];
	UINT_8  driveNumber;
	UINT_8  reserved;
	UINT_8  bootSignature;
	UINT_32   volumeId;
	UINT_8  volumeLabel[11];
	UINT_8  fatTypeLabel[8];
	UINT_8  ImanOSReserved[422];
} __attribute__ ((packed)) BIOSPARAMETERBLOCK;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _DIRECTORYENTRY {
	UINT_8  name[8];
	UINT_8  ext[3];
	UINT_8  attributes;
	UINT_8  reserved;
	UINT_8  cTimeTenth;
	UINT_16 cTime;
	UINT_16 cDate;
	UINT_16 aTime;
	UINT_16 firstClusterHigh;
	UINT_16 wTime;
	UINT_16 wDate;
	UINT_16 firstClusterLow;
	UINT_32   size;
} __attribute__ ((packed)) DIRECTORYENTRY; // 32 bytes

typedef struct _FILESYSTEM {
	SATA*           sata;
	UINT_32    reserved[3];
} __attribute__ ((packed)) FILESYSTEM;

typedef struct _HDPARAMS {
	SATA*      sata;
	UINT_32    root;
	UINT_32    data;
	UINT_32    sectorsPerCluster;
} __attribute__ ((packed)) HDPARAMS;

typedef struct _DESCRIPTOR {
	UINT_32       validity;
	UINT_64 tree;
} __attribute__ ((packed)) DESCRIPTOR;


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32  RegisterFilesystem( FILESYSTEM* filesystem, SATA* sata );
HDPARAMS DriveEntry(void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _FILESYSTEM__H__
