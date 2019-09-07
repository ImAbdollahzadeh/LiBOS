#ifndef _DOSSPEC__H__
#define _DOSSPEC__H__ 

#include "IMOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _DOSNAME {
	UINT_8           name[12];
	struct _DOSNAME* next;
} __attribute__ ((packed)) DOSNAME; 

typedef struct _CSTRING_32_LIMIT {
	INT_8 tmp[32]; // we limited ourselves to 32 bytes characters as CString namin
} __attribute__ ((packed)) CSTRING_32_LIMIT;

typedef struct _DOSSTRING_11_LIMIT {
	INT_8 tmp[12]; 
} __attribute__ ((packed))  DOSSTRING_11_LIMIT;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 ReleaseDosPackage         ( DOSNAME* dos );
void    DosPrint                  ( DOSNAME* dos );
UINT_32 DosStringToDosNamePackage ( INT_8* DosPath, DOSNAME* dosNamePackage );
UINT_32 DosStrcmp                 ( INT_8* src, INT_8* trg, UINT_32 length );
INT_8*  CStringAddressToDosString ( INT_8* CStringAddress );

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif // !_DOSSPEC__H__
