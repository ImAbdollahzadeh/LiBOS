#ifndef _ACPI__H__
#define _ACPI__H__

#include "LiBOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _RSDP_Descriptor {
 INT_8   signature[8];
 UINT_8  checksum;
 INT_8   OEM_id[6];
 UINT_8  revision;
 UINT_32 rsdt_address;
} __attribute__ ((packed)) RSDP_Descriptor;

typedef struct _RSDP_Descriptor_2_0 {
 RSDP_Descriptor rsdp_1_0; 
 UINT_32         length;
 UINT_64         xsdt_address;
 UINT_8          extended_checksum;
 UINT_8          reserved[3];
} __attribute__ ((packed)) RSDP_Descriptor_2_0;

typedef struct _ACPI_SDT_Header {
  INT_8   signature[4];
  UINT_32 length;
  UINT_8  revision;
  UINT_8  checksum;
  INT_8   OEM_id[6];
  INT_8   OEM_table_id[8];
  UINT_32 OEM_revision;
  UINT_32 creator_id;
  UINT_32 creator_revision;
} __attribute__ ((packed)) ACPI_SDT_Header;

typedef struct _RSDT {
  ACPI_SDT_Header h;
  UINT_32*        pointer_to_otgher_SDT;
} __attribute__ ((packed)) RSDT;


BOOL  acpi_strcmp (INT_8* src, INT_8* trg, UINT_32 bytes);
BOOL  query_rsdp   (RSDP_Descriptor_2_0* rsdp);
void* find_FACP    (UINT_32 rsdt);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	
#endif //! _ACPI__H__

