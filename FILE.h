#ifndef _FILE__H__
#define _FILE__H__
#include "FILESYSTEM.h"
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _BINPAPER {
	char         name[32];
	unsigned int flags;
	unsigned int fileLength;
	unsigned int id;
	unsigned int eof;
	unsigned int position;
	unsigned int currentCluster;
	unsigned int device;
	unsigned int interlock;
	unsigned int sizeInBytes; 
	DESCRIPTOR   paperDescriptor;
} __attribute__ ((packed))
BINPAPER, *PBINPAPER;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BINPAPER     PaperOpen  ( char* name_address );
unsigned int PaperClose ( PBINPAPER paper );	
unsigned int PaperRead  ( PBINPAPER paper, unsigned char* Buffer, unsigned int Bytes );

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _FILE__H__
