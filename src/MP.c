#include "../include/MP.h"
#include "../include/PRINT.h"
#include "../include/IDT.h"
#include "../include/GDT.h"
#include "../include/MEMORY.h"
#include "../include/TIMER.h"
#include "../include/PORT.h"
#include "../include/KEYBOARD.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

/* intterupt types:
    0 INT
    1 NMI
    2 SMI
    3 ExtINT
*/

#define MULTI_PROCESSOR_DEBUG FALSE

static BOOL apic_or_pic              = FALSE; // pic mode
static UINT_16 ready_cpu_for_threads = 0;     // support for up to 16 logical cpu

#define BIT(X) (1<<(X))

#define GET_POLARITY_BIT(X)  (X & 0x03)
#define GET_TRIGGER_BIT(X)  ((X & 0x0C) >> 2)

#define LOGICAL_CPU_ENABLED(CPU)                    ((CPU & BIT(0)) >> 0)
#define LOGICAL_CPU_INITIALIZED(CPU)                ((CPU & BIT(1)) >> 1)
#define LOGICAL_CPU_STARTED_UP(CPU)                 ((CPU & BIT(2)) >> 2)
#define LOGICAL_CPU_LAPIC_CONFIGURED(CPU)           ((CPU & BIT(3)) >> 3)
#define LOGICAL_CPU_LAPIC_WITH_TRAMPOLINE_CODE(CPU) ((CPU & BIT(4)) >> 4)

#define EDGE  FALSE
#define HIGH  FALSE
#define LEVEL TRUE
#define LOW   TRUE 

static UINT_32 number_of_logical_cpus = 0;
static UINT_32 number_of_enabled_cpus = 0;

static LiBOS_CPUs    libos_cpus;
static LiBOS_IOAPICs libos_ioapics;

/* we assume hard-coded 3 AP CPUs */
void (*cpu_1_running_code) (void);
void (*cpu_2_running_code) (void);
void (*cpu_3_running_code) (void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void bsp_lapic_timer(REGS* r) 
{ 
	/* so far no plan */
	return;      
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void multiprocessor_memcopy(void* src, void* trg, UINT_32 bytes)
{
	UINT_32 i;
	INT_8* ssrc = (INT_8*)src;
	INT_8* ttrg = (INT_8*)trg;
	for(i = 0; i < bytes; i++)
		ttrg[i] = ssrc[i];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static UINT_32 lapic_read(const UINT_32 offset)
{
	UINT_32 target_pointer = libos_cpus.base_lapic_address + offset;
	void* target = (void*)target_pointer;
	return *(volatile UINT_32*)target;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static UINT_32 ioapic_read(const UINT_32 offset)
{
	*(volatile UINT_32*)(libos_ioapics.ioapics[0].address) = offset;
	return *(volatile UINT_32*)(libos_ioapics.ioapics[0].address + 0x10);
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void lapic_write(const UINT_32 offset, const UINT_32 value)
{
	UINT_32 target_pointer = libos_cpus.base_lapic_address + offset;
	void* target = (void*)target_pointer;
	*(volatile UINT_32*)target = value;
	lapic_read(LAPIC_ID_Register); /* sanity check */
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void ioapic_write(const UINT_32 offset, const UINT_32 value)
{
	*(volatile UINT_32*)(libos_ioapics.ioapics[0].address) = offset;
	*(volatile UINT_32*)(libos_ioapics.ioapics[0].address + 0x10) = value; 
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL mp_strcmp(INT_8* src, INT_8* trg, UINT_32 bytes)
{
	UINT_32 i;
	for(i=0; i<bytes; i++)
		if(*src++ != *trg++)
			return FALSE;
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static BOOL _find_mpfp(void* begin_ptr, void* end_ptr, MP_FLOATING_POINTER* mpfp)
{
	MP_FLOATING_POINTER* m = 0;
	UINT_8* beg = (UINT_8*)begin_ptr;
	UINT_8* end = (UINT_8*)end_ptr;
	UINT_8* i = 0;
	
	for (i = beg; i < end; i += sizeof(MP_FLOATING_POINTER) /*a.k.a. 16 bytes */)
	{
		m = (MP_FLOATING_POINTER*)i;
		UINT_8* mem = 0;
		if ( mp_strcmp(m->signature, "_MP_", 4) )
		{
			*mpfp = *m;
			return TRUE;
		}
	}
	return FALSE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL query_multiprocessing(MP_FLOATING_POINTER* mpfp)
{
	__LiBOS_MemZero((void*)&libos_cpus, sizeof(LiBOS_CPUs));
	__LiBOS_MemZero((void*)&libos_ioapics, sizeof(LiBOS_IOAPICs));
	
	BOOL result = FALSE;
	result = _find_mpfp((void*)0x9FC00, (void*)0xA0000, mpfp);
	if(result)
		return result;
	result = _find_mpfp((void*)0xF0000, (void*)0xFFFFF, mpfp);
	if(result)
		return result;
	result = _find_mpfp((void*)0x80000, (void*)0x9FFFF, mpfp);
	if(result)
		return result;
	return result;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void CPU_info(CPU_INFO* cpu)
{
	number_of_logical_cpus++;
	if((cpu->CPU_bitfields & BIT(0)))
	{
		number_of_enabled_cpus++;
		libos_cpus.cpus[libos_cpus.number_of_logical_cpus].enabled    = TRUE;
		libos_cpus.cpus[libos_cpus.number_of_logical_cpus].cpu_state |= ENABLED;
	}
	if((cpu->CPU_bitfields & BIT(1)))
		libos_cpus.cpus[libos_cpus.number_of_logical_cpus].BSP_or_AP  = TRUE;
	libos_cpus.cpus[libos_cpus.number_of_logical_cpus].LAPIC_id       = cpu->LAPIC_id;
	libos_cpus.cpus[libos_cpus.number_of_logical_cpus].LAPIC_version  = cpu->LAPIC_version;
	
	libos_cpus.number_of_logical_cpus++;
	
	if(MULTI_PROCESSOR_DEBUG)
	{
		printk("LAPIC id=^\n",                cpu->LAPIC_id);
		printk("LAPIC version=^\n",           cpu->LAPIC_version);
		printk("CPU enabled=^\n",             cpu->CPU_bitfields & BIT(0));
		printk("CPU Bootstrap Processor=^\n", cpu->CPU_bitfields & BIT(1));
		printk("CPU signature=^\n",           cpu->CPU_signature);
		printk("CPU feature flags=^\n",       cpu->CPU_feature_flags);
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void IOAPIC_info(IOAPIC_INFO* ioapic)
{
	if(ioapic->IOAPIC_enabled & BIT(0))
		libos_ioapics.ioapics[libos_ioapics.number_of_logical_ioapics].enabled    = TRUE;
	libos_ioapics.ioapics[libos_ioapics.number_of_logical_ioapics].IOAPIC_id      = ioapic->IOAPIC_id;
	libos_ioapics.ioapics[libos_ioapics.number_of_logical_ioapics].IOAPIC_version = ioapic->IOAPIC_version;
	libos_ioapics.ioapics[libos_ioapics.number_of_logical_ioapics].address        = ioapic->IOAPIC_address;
	libos_ioapics.number_of_logical_ioapics++;
	
	if(MULTI_PROCESSOR_DEBUG)
	{
		printk("IOAPIC id=^\n",      ioapic->IOAPIC_id);
		printk("IOAPIC version=^\n", ioapic->IOAPIC_version);
		printk("IOAPIC enabled=^\n", ioapic->IOAPIC_enabled & BIT(0));
		printk("IOAPIC address=^\n", ioapic->IOAPIC_address);
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void IO_INTTERUPT_ASSIGNMENT_ENTRY_info(IO_INTTERUPT_ASSIGNMENT_ENTRY* ioiae)
{
	if(MULTI_PROCESSOR_DEBUG)
	{
		printk("IO_IAE id=% ",       ioiae->entry_type);
		printk("IOIAE IntType=% ",   ioiae->intterupt_type);
		printk("pol=% "  ,           GET_POLARITY_BIT(ioiae->polarity_and_trigger_mode));
		printk("trig=% ",            GET_TRIGGER_BIT(ioiae->polarity_and_trigger_mode));
		printk("sbusID=% ",          ioiae->source_bus_id);
		printk("sbusIRQ=% ",         ioiae->source_bus_irq);
		printk("dstIOapicID=% ",     ioiae->destination_ioapic_id);
		printk("dstIOapicINTIN=%\n", ioiae->destination_ioapic_intin);
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void LOCAL_INTTERUPT_ASSIGNMENT_ENTRY_info(LOCAL_INTTERUPT_ASSIGNMENT_ENTRY* liae)
{
	if(MULTI_PROCESSOR_DEBUG)
	{
		printk("LIAE id=% ",         liae->entry_type);
		printk("LIAE IntType=% ",    liae->intterupt_type);
		printk("pol=% ",             GET_POLARITY_BIT(liae->polarity_and_trigger_mode));
		printk("trig=% ",            GET_TRIGGER_BIT(liae->polarity_and_trigger_mode));
		printk("sbusID=% ",          liae->source_bus_id);
		printk("sbusIRQ=% ",         liae->source_bus_irq);
		printk("dstLapicID=% ",      liae->destination_lapic_id);
		printk("dstLapicLINTIN=%\n", liae->destination_lapic_lintin);
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void MP_configuration_table(MP_FLOATING_POINTER* mpfp)
{
	MP_CONFIGURATION_TABLE* mpct  = (MP_CONFIGURATION_TABLE*)(PHYSICAL_ADDRESS(mpfp->mp_config_pointer));
	libos_cpus.base_lapic_address = mpct->header.address_of_local_apic;
	
	if(MULTI_PROCESSOR_DEBUG)
	{
	printk("    MP_CONFIGURATION_TABLE    \n");
	__LiBOS_ChrDump (mpct->header.signature, 4);
	printk("base_table_length=^\n", mpct->header.base_table_length);
	printk("specification_revision=^\n", mpct->header.specification_revision);
	__LiBOS_ChrDump (mpct->header.OEM_id, 8);
	__LiBOS_ChrDump (mpct->header.product_id, 12);
	printk("OEM_table_pointer=^\n", mpct->header.OEM_table_pointer);
	printk("OEM_table_size=^\n", mpct->header.OEM_table_size);
	printk("entry_count=^\n", mpct->header.entry_count);
	printk("address_of_local_apic=^\n", mpct->header.address_of_local_apic);
	printk("extended_table_length=^\n", mpct->header.extended_table_length);
	}
	
	UINT_32                       entry   = mpct->header.entry_count, i;
	MP_CONFIGURATION_TABLE_ENTRY* entries = (MP_CONFIGURATION_TABLE_ENTRY*)((INT_8*)mpct + sizeof(MP_CONFIGURATION_TABLE_HEADER));
	for(i=0; i<entry; i++)
	{
		//printk("type:^\n", entries->type);
		if(!entries->type) // CPU
		{
			CPU_info((CPU_INFO*)entries);
			entries = (MP_CONFIGURATION_TABLE_ENTRY*)((INT_8*)entries + 20);
		}
		else if(entries->type == 2) // IOAPIC
		{
			IOAPIC_info((IOAPIC_INFO*)entries);
			entries = (MP_CONFIGURATION_TABLE_ENTRY*)((INT_8*)entries + 8);
		}
		else if(entries->type == 3) // IO_INTTERUPT_ASSIGNMENT_ENTRY
		{
			IO_INTTERUPT_ASSIGNMENT_ENTRY_info((IO_INTTERUPT_ASSIGNMENT_ENTRY*)entries);
			entries = (MP_CONFIGURATION_TABLE_ENTRY*)((INT_8*)entries + 8);
		}
		else if(entries->type == 4) // LOCAL_INTTERUPT_ASSIGNMENT_ENTRY
		{
			LOCAL_INTTERUPT_ASSIGNMENT_ENTRY_info((LOCAL_INTTERUPT_ASSIGNMENT_ENTRY*)entries);
			entries = (MP_CONFIGURATION_TABLE_ENTRY*)((INT_8*)entries + 8);
		}
		else // rest (e.g. BUS)
		{
			entries = (MP_CONFIGURATION_TABLE_ENTRY*)((INT_8*)entries + 8);
		}
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 query_number_of_logical_CPUs(void)
{
	return number_of_logical_cpus;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 query_number_of_enabled_logical_CPUs(void)
{
	return number_of_enabled_cpus;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void query_libos_cpus(void)
{
	UINT_8 num = libos_cpus.number_of_logical_cpus, i;
	
	if(MULTI_PROCESSOR_DEBUG)
	{
		for(i=0; i<num; i++)
		{
			printk("  **** CPU % ****\n", i);
			printk("lapic_id: %, ", libos_cpus.cpus[i].LAPIC_id);
			printk("lapic_version: %, ", libos_cpus.cpus[i].LAPIC_version);
			if(libos_cpus.cpus[i].BSP_or_AP) { printk("BSP, ");          }
			else                             { printk("AP, ");           }
			if(libos_cpus.cpus[i].enabled)   { printk("enabled\n");      }
			else                             { printk("not enabled\n");  }
		}
		printk("lapic_address: ^\n", libos_cpus.base_lapic_address);
		printk("  *******************\n", i);
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void query_libos_ioapics(void)
{
	UINT_8 num = libos_ioapics.number_of_logical_ioapics, i;
	
	if(MULTI_PROCESSOR_DEBUG)
	{
		for(i=0; i<num; i++)
		{
			printk("  **** IOAPIC % ****\n", i);
			printk("ioapic_id: %, ", libos_ioapics.ioapics[i].IOAPIC_id);
			printk("ioapic_version: %, ", libos_ioapics.ioapics[i].IOAPIC_version);
			printk("ioapic address: ^, ", libos_ioapics.ioapics[i].address);
			if(libos_ioapics.ioapics[i].enabled) { printk("enabled\n");      }
			else                                 { printk("not enabled\n");  }
		}
		printk("  *******************\n", i);
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void enable_bsp_lapic(LiBOS_LOGICAL_CPU* cpu)
{
	_CLI();
	
	/* disable pic */
	disable_pic();
	
	/* set MSR register to enable the local apic */
	UINT_32 ECX  = 0x1B;
	UINT_32 addr = (libos_cpus.base_lapic_address | BIT(8) | BIT(11));
	UINT_32 EAX  = addr;
	UINT_32 EDX  = 0;
	set_MSR(ECX, EAX, EDX);
	
	/* set BSP CPU lapic */
	UINT_32 activation = 0x00000000 | BIT(8) | BIT(11) | (47 + 32);
	lapic_write(Spurious_Interrupt_Vector_Register, activation);
	
	lapic_write(Destination_Format_Register, 0xFFFFFFFF);                                                           // Put the APIC into flat delivery mode
	lapic_write(Logical_Destination_Register, (lapic_read(Logical_Destination_Register) & 0x00FFFFFF) | 1);         // LDR mask
	UINT_32 logical_apicid = cpu->LAPIC_id;
	lapic_write(Logical_Destination_Register, (lapic_read(Logical_Destination_Register) | (logical_apicid << 24))); // LDR
	
	/* local timer setting */
	irq_install_handler(70, &bsp_lapic_timer);
	lapic_write(Timer_Divide_Configuration_Register, 0x0000000B);
	UINT_32 clock_activation = 0x00020000 | (32 + 70);
	lapic_write(Local_Vector_Table_TIMER, clock_activation);
	lapic_write(Initial_Count_Register, 10000000);
	
	lapic_write(Task_Priority_Register, 0);  
	
	/* local INT0 and INT1 */
	lapic_write(Local_Vector_Table_LINT0, (BIT(8) | BIT(9) | BIT(10))); // ExtInt
	lapic_write(Local_Vector_Table_LINT1, BIT(10)); // NMI
	
	/* set an error handler */
	lapic_write(Local_Vector_Table_Error, (32 + 16));
	
	/* Clear error status register (requires back-to-back writes) */
	lapic_write(Error_Status_Register, 0);
	lapic_write(Error_Status_Register, 0);
	
	/* sanity: if there is any non-acknowledged interrupt */
	lapic_write(EOI_Register, 0);
	
	/* Send an Init Level De-Assert to synchronise arbitration ID's */
	lapic_write(Interrupt_Command_HI_Register, 0);
	lapic_write(Interrupt_Command_LOW_Register, 0x00080000 | 0x00000500 | 0x00008000);
	while( (lapic_read(Interrupt_Command_LOW_Register) & 0x00001000) );

	/* Enable interrupts on the APIC (but not on the processor) */
	lapic_write(Task_Priority_Register, 0);
	
	/* set the logical cpu state as LAPIC_CONFIGURED */
	cpu->cpu_state |= LAPIC_CONFIGURED;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void enable_ap_lapic(LiBOS_LOGICAL_CPU* cpu)
{
	_CLI();

	/* set the logical cpu state as LAPIC_CONFIGURED */
	cpu->cpu_state |= LAPIC_CONFIGURED;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void ioapic_remap_vector(UINT_8 vector, UINT_32 mapped, UINT_32 tigger_mode, UINT_32 pin_polarity, BOOL mask, LiBOS_LOGICAL_CPU* cpu)
{
	if (mask)
		mapped |= BIT(16);
	if (tigger_mode)
		mapped |= BIT(13);
	if (pin_polarity)
		mapped |= BIT(15);
	
	UINT_32 apic_id = cpu->LAPIC_id;

	ioapic_write( (0x10 + 2 * vector), mapped  );
	ioapic_write( (0x11 + 2 * vector), apic_id );
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void enable_ioapic(LiBOS_LOGICAL_IOAPIC* ioapic, LiBOS_LOGICAL_CPU* cpu)
{
	/* disable pic */
	disable_pic();
	
	/* version */
	UINT_8 ioapicMaxIndexRedirTab = (UINT_8)((ioapic_read(IOAPIC_VERSION) >> 16) & 0xFF);
	
	/* sanity: first disable all vectors */
	UINT_8 i;
	for(i=0; i<ioapicMaxIndexRedirTab; i++)
		ioapic_remap_vector(i, 0, LEVEL, LOW, TRUE, cpu);
	
	/* remap ISA irqs */
	for(i=0; i<16; i++)
		ioapic_remap_vector(i, i + 32, EDGE, HIGH, FALSE, cpu);
	
	/* I know that IRQ0 mapped to INTIN2 of I/O APIC */
	ioapic_remap_vector(2, 32, EDGE, HIGH, FALSE, cpu);
	
	apic_or_pic = TRUE;
	apic_eoi();
	_STI();
	apic_eoi();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static BOOL send_INIT_IPI(LiBOS_LOGICAL_CPU* cpu)
{
	UINT_32 apic_id = (UINT_32)cpu->LAPIC_id << 24;
	UINT_32 flags   = (5 << 8);
	lapic_write(Interrupt_Command_HI_Register, apic_id);
	lapic_write(Interrupt_Command_LOW_Register, flags);
	cpu->cpu_state |= INITIALIZED;
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static BOOL send_SIPI_IPI(LiBOS_LOGICAL_CPU* cpu)
{
	UINT_32 apic_id = (UINT_32)cpu->LAPIC_id << 24;
	UINT_32 flags   = (6 << 8) | (0x7000 >> 12); /* we know trampoline codes resides @ address 0x7000 */
	lapic_write(Interrupt_Command_HI_Register, apic_id);
	lapic_write(Interrupt_Command_LOW_Register, flags);
	cpu->cpu_state |= STARTED_UP;
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL bsp_initialize_ap(LiBOS_LOGICAL_CPU* cpu)
{
	if(!cpu)
		return FALSE;
	
	if( !send_INIT_IPI(cpu) )
	{
		panic("INIT_IPI failed\n");
		return FALSE;
	}
	WaitMiliSecond(20);
	if( !send_SIPI_IPI(cpu) )
	{
		panic("SIPI_IPI failed\n");
		return FALSE;
	}
	WaitMiliSecond(10);
	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void start_multiprocessing(void)
{
	initilalize_cpu_zones();
	
	/* first the BSP cpu must enable its lapic */
	UINT_8 i = 0;
	LiBOS_LOGICAL_CPU* cpu = 0;
	UINT_8 number_of_avaialable_cpus = libos_cpus.number_of_logical_cpus;
	
	while(number_of_avaialable_cpus--)
	{
		cpu = &libos_cpus.cpus[i];
		if(cpu->BSP_or_AP)
			goto FOUND_BSP;
		i++;
	}
FOUND_BSP:
	if(!cpu)
	{
		printk("error-> there is no BSP cpu available in system!\n");
		return;
	}
	enable_bsp_lapic(cpu);
	ready_cpu_for_threads |= BIT(i);
	enable_ioapic(&libos_ioapics.ioapics[0], cpu);

	/* copy trampoline code (real mode @ 0x7000 and protected mode @ 0x8000) */
	UINT_32 start_real_mode      = 0;
	UINT_32 end_real_mode        = 0;
	UINT_32 start_protected_mode = 0;
	UINT_32 end_protected_mode   = 0;
	
	get_trampoline_start(&start_real_mode);
	get_trampoline_end  (&end_real_mode);
	get_mp_32_start     (&start_protected_mode);
	get_mp_32_end       (&end_protected_mode);
	
	multiprocessor_memcopy((void*)start_real_mode,      (void*)0x7000, end_real_mode - start_real_mode);
	multiprocessor_memcopy((void*)start_protected_mode, (void*)0x8000, end_protected_mode - start_protected_mode);
	
	/* Wake up all AP cpus */
	i   = 0;
	cpu = 0;
	number_of_avaialable_cpus = libos_cpus.number_of_logical_cpus;
	while(i < number_of_avaialable_cpus)
	{
		cpu = &libos_cpus.cpus[i];
		if(!cpu->BSP_or_AP) // accept only AP cpus
		{
			set_cpu_id(i);
			
			if( !bsp_initialize_ap(cpu) )
			{
				panic("AP cpu initialization by BSP cpu failed\n");
				return;
			}
			ready_cpu_for_threads |= BIT(i);
		}
		i++;
	}
	
	apic_eoi(); /* sanity check */
	_STI();     /* enable interrupts on all cpus */
	apic_eoi(); /* sanity check */
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void apic_eoi(void)
{
	lapic_write(EOI_Register, 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BOOL apic_mode(void)
{
	return apic_or_pic;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


void __initial_LiBOS_process_zone(void)
{ ; }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void initilalize_cpu_zones(void)
{
	cpu_1_running_code = &__initial_LiBOS_process_zone;
	cpu_2_running_code = &__initial_LiBOS_process_zone;
	cpu_3_running_code = &__initial_LiBOS_process_zone;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void cpu_1_process_zone(void)
{
	while(1)
		cpu_1_running_code();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void cpu_2_process_zone(void)
{
	while(1)
		cpu_2_running_code();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void cpu_3_process_zone(void)
{
	while(1)
		cpu_3_running_code();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


