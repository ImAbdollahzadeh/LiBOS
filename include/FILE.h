#ifndef _FILE__H__
#define _FILE__H__

#include "IMOS_CORE.h"
#include "FILESYSTEM.h"
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _BINPAPER {
	INT_8   name[32];
	UINT_32 flags;
	UINT_32 fileLength;
	UINT_32 id;
	UINT_32 eof;
	UINT_32 position;
	UINT_32 currentCluster;
	UINT_32 device;
	UINT_32 interlock;
	UINT_32 sizeInBytes; 
	DESCRIPTOR   paperDescriptor;
} __attribute__ ((packed)) BINPAPER;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BINPAPER PaperOpen  ( INT_8* name_address );
UINT_32  PaperClose ( BINPAPER* paper );	
UINT_32  PaperRead  ( BINPAPER* paper, UINT_8* Buffer, UINT_32 Bytes );

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _FILE__H__
