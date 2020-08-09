#ifndef _MP__H__
#define _MP__H__

#include "LiBOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// LAPIC REGISTERS
#define LAPIC_ID_Register                   0x0020
#define LAPIC_ID_Version_Register           0x0030
#define Task_Priority_Register              0x0080
#define Arbitration_Priority_Register       0x0090
#define Processor_Priority_Register         0x00A0
#define EOI_Register                        0x00B0
#define Logical_Destination_Register        0x00D0
#define Destination_Format_Register         0x00E0
#define Spurious_Interrupt_Vector_Register  0x00F0
#define Error_Status_Register               0x0280
#define Interrupt_Command_LOW_Register      0x0300
#define Interrupt_Command_HI_Register       0x0310
#define Local_Vector_Table_TIMER            0x0320
#define Thermal_Sensor                      0x0320
#define Performance_Counter_LVT             0x0340
#define Local_Vector_Table_LINT0            0x0350
#define Local_Vector_Table_LINT1            0x0360
#define Local_Vector_Table_Error            0x0370
#define Initial_Count_Register              0x0380
#define Current_Count_Register              0x0390
#define Timer_Divide_Configuration_Register 0x03E0

// IOAPIC OFFSETS
#define IOAPIC_IOREGSEL      0x00
#define IOAPIC_IOWIN         0x10
#define IOAPIC_ID            0x00
#define IOAPIC_VERSION       0x01
#define IOAPIC_ARBITRATIONID 0x02

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


typedef struct _MP_FLOATING_POINTER {
	UINT_8   signature[4];
	UINT_32  mp_config_pointer;
	UINT_8   length;
	UINT_8   version;
	UINT_8   checksum;
	UINT_8   mp_features_1;
	UINT_8   mp_features_2;
	UINT_8   mp_features_3;
	UINT_8   mp_features_4;
	UINT_8   mp_features_5;
} __attribute__ ((packed)) MP_FLOATING_POINTER;

typedef struct _MP_CONFIGURATION_TABLE_HEADER {
	UINT_8   signature[4];
	UINT_16  base_table_length;
	UINT_8   specification_revision;
	UINT_8   checksum;
	UINT_8   OEM_id[8];
	UINT_8   product_id[12];
	UINT_32  OEM_table_pointer;
	UINT_16  OEM_table_size;
	UINT_16  entry_count;
	UINT_32  address_of_local_apic;
	UINT_16  extended_table_length;
	UINT_8   extended_table_checksum;
	UINT_8   reserved;
} __attribute__ ((packed)) MP_CONFIGURATION_TABLE_HEADER;

typedef struct _MP_CONFIGURATION_TABLE_ENTRY {
	UINT_8  type;
	UINT_8* unused
} __attribute__ ((packed)) MP_CONFIGURATION_TABLE_ENTRY;

typedef struct _MP_CONFIGURATION_TABLE {
	MP_CONFIGURATION_TABLE_HEADER header;
	MP_CONFIGURATION_TABLE_ENTRY* entries;
} __attribute__ ((packed)) MP_CONFIGURATION_TABLE;

typedef struct _CPU_INFO {
	UINT_8   type;
	UINT_8   LAPIC_id;
	UINT_8   LAPIC_version;
	UINT_8   CPU_bitfields;
	UINT_32  CPU_signature;
	UINT_32  CPU_feature_flags;
} __attribute__ ((packed)) CPU_INFO;

typedef struct _IOAPIC_INFO {
	UINT_8   type;
	UINT_8   IOAPIC_id;
	UINT_8   IOAPIC_version;
	UINT_8   IOAPIC_enabled;
	UINT_32  IOAPIC_address;
} __attribute__ ((packed)) IOAPIC_INFO;

typedef struct _LiBOS_LOGICAL_CPU {
	UINT_8 LAPIC_id;
	UINT_8 LAPIC_version;
	BOOL   BSP_or_AP; // TRUE=BSP, FALSE=AP
	BOOL   enabled;
	UINT_8 reserved[6];
} LiBOS_LOGICAL_CPU;

typedef struct _LiBOS_CPUs {
	UINT_32            base_lapic_address;
	UINT_8             number_of_logical_cpus;
	LiBOS_LOGICAL_CPU  cpus[0x10]; // statically 16 CPU support
} LiBOS_CPUs;

typedef struct _LiBOS_LOGICAL_IOAPIC {
	UINT_8  IOAPIC_id;
	UINT_8  IOAPIC_version;
	BOOL    enabled;
	UINT_32 address;
} LiBOS_LOGICAL_IOAPIC;

typedef struct _LiBOS_IOAPICs {
	UINT_8             number_of_logical_ioapics;
	LiBOS_LOGICAL_IOAPIC  ioapics[0x10]; // statically 16 CPU support
} LiBOS_IOAPICs;










BOOL    query_multiprocessing                (MP_FLOATING_POINTER* mpfp);
void    MP_configuration_table               (MP_FLOATING_POINTER* mpfp);
UINT_32 query_number_of_logical_CPUs         (void);
//UINT_32 query_number_of_enabled_logical_CPUs (void);
void    query_libos_cpus                     (void);
void    query_libos_ioapics                  (void);
void    start_multiprocessing                (void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	
#endif //! _MP__H__

