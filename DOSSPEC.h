#ifndef _DOSSPEC__H__
#define _DOSSPEC__H__ 

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _DOSNAME {
	unsigned char    name[12];
	struct _DOSNAME* next;
} __attribute__ ((packed)) 
DOSNAME, *PDOSNAME; 

typedef struct _CSTRING_32_LIMIT {
	char tmp[32]; // we limited ourselves to 32 bytes characters as CString namin
} __attribute__ ((packed)) 
CSTRING_32_LIMIT;

typedef struct _DOSSTRING_11_LIMIT {
	char tmp[12]; 
} __attribute__ ((packed)) 
DOSSTRING_11_LIMIT;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int ReleaseDosPackage         ( PDOSNAME dos );
void         DosPrint                  ( PDOSNAME dos );
unsigned int DosStringToDosNamePackage ( char* DosPath, PDOSNAME dosNamePackage );
unsigned int DosStrcmp                 ( char* src, char* trg, unsigned int length );
char*        CStringAddressToDosString ( char* CStringAddress );

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif // !_DOSSPEC__H__
