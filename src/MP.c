#include "../include/MP.h"
#include "../include/PRINT.h"
#include "../include/IDT.h"
#include "../include/MEMORY.h"
#include "../include/TIMER.h"
#include "../include/PORT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

#define EDGE FALSE
#define HIGH FALSE
#define LEVEL TRUE
#define LOW TRUE 

#define BIT(X) (1<<(X))
static UINT_32 number_of_logical_cpus = 0;
static UINT_32 number_of_enabled_cpus = 0;

void spurios_handler              (REGS* r) { return;                                      }
void set_spurios_interrupt_handler(void)    { __irq_install_handler(15, &spurios_handler); }

static LiBOS_CPUs    libos_cpus;
static LiBOS_IOAPICs libos_ioapics;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static UINT_32 LapicRead(UINT_32 offset)
{
	return mread(libos_cpus.base_lapic_address, offset);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void LapicWrite(UINT_32 offset, UINT_32 value)
{
	mwrite(libos_cpus.base_lapic_address, offset, value);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static UINT_32 IOapicRead(UINT_32 offset)
{
	mwrite(libos_ioapics.ioapics[0].address, IOAPIC_IOREGSEL, offset);
	return mread(libos_ioapics.ioapics[0].address, IOAPIC_IOWIN);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void IOapicWrite(UINT_32 offset, UINT_32 value)
{
	mwrite(libos_ioapics.ioapics[0].address, IOAPIC_IOREGSEL, offset);
	mwrite(libos_ioapics.ioapics[0].address, IOAPIC_IOWIN, value);
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
	if(cpu->CPU_bitfields & BIT(0))
	{
		number_of_enabled_cpus++;
		libos_cpus.cpus[libos_cpus.number_of_logical_cpus].enabled   = TRUE;
	}
	if(cpu->CPU_bitfields & BIT(1))
		libos_cpus.cpus[libos_cpus.number_of_logical_cpus].BSP_or_AP = TRUE;
	libos_cpus.cpus[libos_cpus.number_of_logical_cpus].LAPIC_id      = cpu->LAPIC_id;
	libos_cpus.cpus[libos_cpus.number_of_logical_cpus].LAPIC_version = cpu->LAPIC_version;
	libos_cpus.number_of_logical_cpus++;
	
	
	//.printk("LAPIC id=^\n",                cpu->LAPIC_id);
	//.printk("LAPIC version=^\n",           cpu->LAPIC_version);
	//.printk("CPU enabled=^\n",             cpu->CPU_bitfields & BIT(0));
	//.printk("CPU Bootstrap Processor=^\n", cpu->CPU_bitfields & BIT(1));
	//.printk("CPU signature=^\n",           cpu->CPU_signature);
	//.printk("CPU feature flags=^\n",       cpu->CPU_feature_flags);
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
	
	//.printk("IOAPIC id=^\n",      ioapic->IOAPIC_id);
	//.printk("IOAPIC version=^\n", ioapic->IOAPIC_version);
	//.printk("IOAPIC enabled=^\n", ioapic->IOAPIC_enabled & BIT(0));
	//.printk("IOAPIC address=^\n", ioapic->IOAPIC_address);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void MP_configuration_table(MP_FLOATING_POINTER* mpfp)
{
	MP_CONFIGURATION_TABLE* mpct  = (MP_CONFIGURATION_TABLE*)(PHYSICAL_ADDRESS(mpfp->mp_config_pointer));
	libos_cpus.base_lapic_address = mpct->header.address_of_local_apic;
	
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
		else // rest (e.g. BUS, IO_Interrupt_Assignment, and Local_Interrupt_Assignment)
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
	//....for(i=0; i<num; i++)
	//....{
	//....	printk("  **** CPU % ****\n", i);
	//....	printk("lapic_id: %, ", libos_cpus.cpus[i].LAPIC_id);
	//....	printk("lapic_version: %, ", libos_cpus.cpus[i].LAPIC_version);
	//....	if(libos_cpus.cpus[i].BSP_or_AP) { printk("BSP, ");          }
	//....	else                             { printk("AP, ");           }
	//....	if(libos_cpus.cpus[i].enabled)   { printk("enabled\n");      }
	//....	else                             { printk("not enabled\n");  }
	//....}
	//....printk("lapic_address: ^\n", libos_cpus.base_lapic_address);
	//....printk("  *******************\n", i);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void query_libos_ioapics(void)
{
	UINT_8 num = libos_ioapics.number_of_logical_ioapics, i;
	//....for(i=0; i<num; i++)
	//....{
	//....	printk("  **** IOAPIC % ****\n", i);
	//....	printk("ioapic_id: %, ", libos_ioapics.ioapics[i].IOAPIC_id);
	//....	printk("ioapic_version: %, ", libos_ioapics.ioapics[i].IOAPIC_version);
	//....	printk("ioapic address: ^, ", libos_ioapics.ioapics[i].address);
	//....	if(libos_ioapics.ioapics[i].enabled) { printk("enabled\n");      }
	//....	else                                 { printk("not enabled\n");  }
	//....}
	//....printk("  *******************\n", i);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void enable_bsp_lapic(LiBOS_LOGICAL_CPU* cpu)
{
	_CLI();
	
	/* disable PIC */
	UINT_8 irqs = 256, i;
	for(i=0; i<irqs; i++)
		IRQ_set_mask(irqs);
	__irq_remap();
	
	/* set MSR register to enable the local apic */
	UINT_32 ECX = 0x1B;
	UINT_32 EAX = (libos_cpus.base_lapic_address | BIT(8) | BIT(11));
	UINT_32 EDX = 0;
	set_MSR(ECX, EAX, EDX);
	
	LapicWrite(Destination_Format_Register, 0xFFFFFFFF);                                               // Put the APIC into flat delivery mode
    LapicWrite(Logical_Destination_Register, (LapicRead(Logical_Destination_Register) & 0x00FFFFFF));  // LDR mask
	LapicWrite(Logical_Destination_Register, (LapicRead(Logical_Destination_Register) | (0<<24)));       // LDR
    LapicWrite(Task_Priority_Register, 0);  
	
	/* local timer setting */
	LapicWrite(Timer_Divide_Configuration_Register, 0x03);
	LapicWrite(Local_Vector_Table_TIMER, 32 + 0);
	LapicWrite(Initial_Count_Register, 0xFFFFFFFF);

	/* set BSP CPU lapic */
	LapicWrite(Spurious_Interrupt_Vector_Register, (BIT(8) | 15));
	
	/* local INT0 and INT1 */
	LapicWrite(Local_Vector_Table_LINT0, (BIT(8) | BIT(9) | BIT(10) | BIT(15)));
	LapicWrite(Local_Vector_Table_LINT1, BIT(10));
	
	//if(((LapicRead(LAPIC_ID_Version_Register) >> 16) & 0xFF) >= 4)
	//    LapicWrite(Performance_Counter_LVT, 0x10000);

	LapicWrite(Local_Vector_Table_Error, BIT(16));
	LapicWrite(Thermal_Sensor, BIT(16));
	LapicWrite(Performance_Counter_LVT, BIT(10));
	
	LapicWrite(Interrupt_Command_LOW_Register, 0x88500); // BCAST | INIT | LEVEL
	while(LapicRead(Interrupt_Command_LOW_Register) & 0x1000);
	    
	LapicWrite(Task_Priority_Register, 0);
	
	/* repeat local INT0 and INT1 */
	LapicWrite(Local_Vector_Table_LINT0, (BIT(8) | BIT(9) | BIT(10) | BIT(15)));
	LapicWrite(Local_Vector_Table_LINT1, BIT(10));
	
	/* initialize the LAPIC Timer */
	LapicWrite(Initial_Count_Register, 0xFFFFFFFF);
	
	
	/* it uses Spurious-Interrupt Vector Register to enable the lapic */
	set_spurios_interrupt_handler();
	PM_set_timer();
	PM_set_keyboard();
	
	/* enable CPU again */
	_STI();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void ioapic_remapVector(UINT_8 vector, UINT_32 mapped, BOOL level /*f:edge t:level*/, BOOL low /*f:high t:low*/, BOOL disabled)
{
	if (disabled)
		mapped |= BIT(16);
	if (level)
		mapped |= BIT(13);
	if (low)
		mapped |= BIT(15);
	
	UINT_32 apic_id = LapicRead(LAPIC_ID_Register) << 24;
	
	IOapicWrite(0x10 + vector * 2, mapped);
	IOapicWrite(0x11 + vector * 2, apic_id);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void enable_ioapic(LiBOS_LOGICAL_IOAPIC* ioapic, LiBOS_LOGICAL_CPU* cpu)
{
	/* disable PIC */
	disable_PIC();

	/* version */
	UINT_8 ioapicMaxIndexRedirTab = (UINT_8)((IOapicRead(IOAPIC_VERSION) & 0x00FF0000) >> 16); // It is 23
	
	/* remap ISA irqs */
	for(UINT_8 i=0; i<16; i++)
		ioapic_remapVector(i, i + 32, EDGE, HIGH, i==2);
	
	/* activate IOAPIC */
	PORT_8 p;
	Port_8_Register (&p);
	p.Write(0x22, 0x70);
	p.Write(0x23, (p.Read(0x23) | 1));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void send_INIT_IPI(LiBOS_LOGICAL_CPU* cpu)
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void send_SIPI_IPI(LiBOS_LOGICAL_CPU* cpu)
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void start_multiprocessing(void)
{
	/* first the BSP cpu must enable its lapic */
	UINT_8 i = 0;
	LiBOS_LOGICAL_CPU* cpu = 0;
	UINT_8 number_of_avaialable_cpus = libos_cpus.number_of_logical_cpus;
	
	while(number_of_avaialable_cpus--)
	{
		cpu = &libos_cpus.cpus[i];
		if(cpu->BSP_or_AP)
			break;
		i++;
	}
	if(!cpu)
	{
		printk("error-> there is no BSP cpu available in system!\n");
		return;
	}
	enable_bsp_lapic(cpu);
	enable_ioapic(&libos_ioapics.ioapics[0], cpu);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
