#include "../include/PORT.h"
#include "../include/IDT.h"
#include "../include/TIMER.h"
#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

       TIMER   global_OS_timer;                  /* This is the main OS's timer */
       UINT_32 __ProgramTimer(UINT_32 Hz);  /* Forward declaration of internal function */
static UINT_32 timer_ticks = 0;                  /* This is the main OS timer tick counter */
void __CurrentTimeReporter(void);
void __CurrentDateReporter(void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void timer_handler(REGS* r)
{
	/* we don't want to bother with the minimum timer_handler 
	   requirement for the OS. All other stuff related to the timer would be 
	   implemented separately. Here only the timer tick update is enough */
	timer_ticks++ == 0xFFFFFFFF ? timer_ticks = 0 : 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 RegisterTimer(TIMER* timer, UINT_32 Hz)
{
	UINT_32 status = 0;
	
	if(!timer)
		return 0;
	
	status = __ProgramTimer(Hz);
	
	if(!status)
	{
		panic("Timer registered failed\n");
		return 0;
	}
	
	timer->clock_Hz    = Hz;
	timer->CurrentTime = &__CurrentTimeReporter;
	timer->CurrentDate = &__CurrentDateReporter;
	global_OS_timer    = *timer;
	
	__irq_install_handler(0, &timer_handler);
	//.printk( "        >>> Timer registered successfully <<<\n");
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 __ProgramTimer(UINT_32 Hz)
{
	PORT_8 CHANNEL0;
	PORT_8 COMMAND;
	UINT_32 divisor;
	
	if(!Hz)
		return 0;
	
	Port_8_Register(&CHANNEL0);
	Port_8_Register(&COMMAND);
	
	divisor = TIC/Hz;
	COMMAND.Write (TIMER_COMMAND_REGISTER,  0x36);
	CHANNEL0.Write(TIMER_CHANNEL0_REGISTER, (divisor & 0xFF));
	CHANNEL0.Write(TIMER_CHANNEL0_REGISTER, (divisor >> 8));
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void WaitMiliSecond(UINT_32 msec)
{
	UINT_32 msec_reference = global_OS_timer.clock_Hz / 1000;
	UINT_32 elapsed        = msec * msec_reference;
	
	if(!msec)
		return;
	
	_CLI();
	timer_ticks = 0;
	_STI();
	
	while(timer_ticks < elapsed);
} 

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void WaitSecond(UINT_32 sec)
{
	UINT_32 elapsed = sec * global_OS_timer.clock_Hz;
	
	if(!sec)
		return;
	
	_CLI();
	timer_ticks = 0;
	_STI();
	
	while(timer_ticks < elapsed);
	_CLI();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void WaitMicroSecond(UINT_32 usec)
{
	UINT_32 status = 0;
	UINT_32 elapsed;
	
	if(!usec)
		return;
	
	status = __ProgramTimer(OS_MICROSECOND);
	
	if(!status)
	{
		panic("Fine tuning of timer failed\n");
		return;
	}
	
	global_OS_timer.clock_Hz = OS_MICROSECOND;
	elapsed                  = usec * global_OS_timer.clock_Hz;
	
	_CLI();
	timer_ticks = 0;
	_STI();
	
	while(timer_ticks < elapsed);
	
	status = __ProgramTimer(OS_DEFAULT_TIMER_TICK);
	
	if(!status)
	{
		panic("Timer tuning back failed\n");
		return;
	}
	
	global_OS_timer.clock_Hz = OS_DEFAULT_TIMER_TICK;
	
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void __CurrentTimeReporter(void)
{
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void __CurrentDateReporter(void)
{
	return;
}
	
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+