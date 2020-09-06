#ifndef _MP__H__
#define _MP__H__

#include "LiBOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ CONSTANTS

/* LAPIC REGISTERS */
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/* IOAPIC OFFSETS */
#define IOAPIC_IOREGSEL      0x00
#define IOAPIC_IOWIN         0x10
#define IOAPIC_ID            0x00
#define IOAPIC_VERSION       0x01
#define IOAPIC_ARBITRATIONID 0x02

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/* LAPIC TIMER */
// Counter 0, 1, 2
#define TIMECOUNTER_i8254_FREQU  1193182
#define COUNTER_0_DATAPORT       0x40              // Channel 0 data port (read/write)
#define COUNTER_1_DATAPORT       0x41              // Channel 1 data port (read/write)
#define COUNTER_2_DATAPORT       0x42              // Channel 2 data port (read/write), OUT connected to speaker
#define COMMANDREGISTER          0x43              // Command register (write only, read ignored)
#define COUNTER_2_CONTROLPORT    0x61              // Channel 2 gate: the input can be controlled by IO port 0x61, bit 0.

// command register
// bit0
#define SIXTEEN_BIT_BINARY       0x00
#define FOUR_DIGIT_BCD           0x01
//bit1-3
/*In MODES 0, 1, 4, and 5 the Counter wraps around to the highest count (FFFFh or 9999bcd) and continues counting.
In MODES 2 and 3 (which are periodic) the Counter reloads itself with the initial count and continues counting from there. */
#define MODE0                    0x00              // interrupt on terminal count
#define MODE1                    0x02              // hardware re-triggerable one-shot
#define RATEGENERATOR            0x04              // Mode 2: divide by N counter
#define SQUAREWAVE               0x06              // Mode 3: square-wave mode
#define MODE4                    0x08              // software triggered strobe
#define MODE5                    0x0A              // hardware triggered strobe
//bit4-5
#define RW_LO_MODE               0x10              // Read/2xWrite bits 0..7 of counter value
#define RW_HI_MODE               0x20              // Read/2xWrite bits 8..15 of counter value
#define RW_LO_HI_MODE            0x30              // 2xRead/2xWrite bits 0..7 then 8..15 of counter value
//bit6-7
#define COUNTER_0                0x00              // select counter 0
#define COUNTER_1                0x40              // select counter 1
#define COUNTER_2                0x80              // select counter 2

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ ENUMS

typedef enum _CPU_STATE {
    ENABLED              = 0x01,
    INITIALIZED          = 0x02,
    STARTED_UP           = 0x04,
    LAPIC_CONFIGURED     = 0x08,
    WITH_TRAMPOLINE_CODE = 0x10,
} CPU_STATE;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ DATA STRUCTURES

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
} __LiBOS_PACKED__ MP_FLOATING_POINTER;

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
} __LiBOS_PACKED__ MP_CONFIGURATION_TABLE_HEADER;

typedef struct _MP_CONFIGURATION_TABLE_ENTRY {
	UINT_8  type;
	UINT_8* unused
} __LiBOS_PACKED__ MP_CONFIGURATION_TABLE_ENTRY;

typedef struct _MP_CONFIGURATION_TABLE {
	MP_CONFIGURATION_TABLE_HEADER header;
	MP_CONFIGURATION_TABLE_ENTRY* entries;
} __LiBOS_PACKED__ MP_CONFIGURATION_TABLE;

typedef struct _CPU_INFO {
	UINT_8   type;
	UINT_8   LAPIC_id;
	UINT_8   LAPIC_version;
	UINT_8   CPU_bitfields;
	UINT_32  CPU_signature;
	UINT_32  CPU_feature_flags;
} __LiBOS_PACKED__ CPU_INFO;

typedef struct _IOAPIC_INFO {
	UINT_8   type;
	UINT_8   IOAPIC_id;
	UINT_8   IOAPIC_version;
	UINT_8   IOAPIC_enabled;
	UINT_32  IOAPIC_address;
} __LiBOS_PACKED__ IOAPIC_INFO;

typedef struct _LiBOS_LOGICAL_CPU {
	UINT_8    LAPIC_id;
	UINT_8    LAPIC_version;
	BOOL      BSP_or_AP; // TRUE=BSP, FALSE=AP
	BOOL      enabled;
	CPU_STATE cpu_state;
	UINT_8    reserved[2];
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
	UINT_8                number_of_logical_ioapics;
	LiBOS_LOGICAL_IOAPIC  ioapics[0x10]; // statically 16 CPU support
} LiBOS_IOAPICs;

typedef struct _IO_INTTERUPT_ASSIGNMENT_ENTRY {
    UINT_8 entry_type; // 0x03
    UINT_8 intterupt_type;
    UINT_8 polarity_and_trigger_mode;
    UINT_8 reserved;
    UINT_8 source_bus_id;
    UINT_8 source_bus_irq;
    UINT_8 destination_ioapic_id;
    UINT_8 destination_ioapic_intin;
} __LiBOS_PACKED__ IO_INTTERUPT_ASSIGNMENT_ENTRY;

typedef struct _LOCAL_INTTERUPT_ASSIGNMENT_ENTRY {
    UINT_8 entry_type; // 0x04
    UINT_8 intterupt_type;
    UINT_8 polarity_and_trigger_mode;
    UINT_8 reserved;
    UINT_8 source_bus_id;
    UINT_8 source_bus_irq;
    UINT_8 destination_lapic_id;     //id 0xFF means the signal is connected to all LAPICs
    UINT_8 destination_lapic_lintin; // 0 or 1
} __LiBOS_PACKED__ LOCAL_INTTERUPT_ASSIGNMENT_ENTRY;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ FUNCTION DECLARATIONS

/* finds mpfp by searching memory                                  */  BOOL    query_multiprocessing                (MP_FLOATING_POINTER* mpfp);
/* finds configuration table, CPUs, IOAPICs, LAPICs, IOIAEs, LIAEs */  void    MP_configuration_table               (MP_FLOATING_POINTER* mpfp);
/* gives number_of_logical_CPUs                                    */  UINT_32 query_number_of_logical_CPUs         (void);
/* gives number_of_enabled_logical_CPUs                            */  UINT_32 query_number_of_enabled_logical_CPUs (void);
/* gives all LiBOS CPUs found in MP space                          */  void    query_libos_cpus                     (void);
/* gives all LiBOS IOAPICs found in MP space                       */  void    query_libos_ioapics                  (void);
/* starts multi-processing, disables PIT, and enables LAPIC-timer  */  void    start_multiprocessing                (void);
/* EOI in multi-processing space                                   */  void    mp_send_eoi                          (void);
/* TRUE if we are in APIC mode . FALSE if in IC mode               */  BOOL    apic_mode                            (void);
/* TRUE if the target AP cpu activated, FALSE if not activated     */  BOOL    bsp_initialize_ap                    (LiBOS_LOGICAL_CPU* cpu);

       void cpu_1_process_zone   (void);
       void cpu_2_process_zone   (void);
       void cpu_3_process_zone   (void);
extern void get_trampoline_start (UINT_32*);
extern void get_trampoline_end   (UINT_32*);
extern void get_mp_32_start      (UINT_32*);
extern void get_mp_32_end        (UINT_32*);
extern void set_cpu_id           (UINT_32 id);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	
#endif //! _MP__H__

