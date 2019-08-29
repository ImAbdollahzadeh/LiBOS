#ifndef _TIMER__H__
#define _TIMER__H__
#include "../include/IDT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define TIMER_CHANNEL0_REGISTER 0x40
#define TIMER_CHANNEL1_REGISTER 0x41
#define TIMER_CHANNEL2_REGISTER 0x42
#define TIMER_COMMAND_REGISTER  0x43
#define TIMER_INPUT_CLOCK       1193180             //(1.19 MHz internal clock of CPU)
#define TIC                     TIMER_INPUT_CLOCK
#define OS_DEFAULT_TIMER_TICK   1000                // default value is every 1 msec
#define OS_MICROSECOND          1000000
#define OS_NANOSECOND           1000000000
#define OS_FINEST_CLOCK_TICK    830 * OS_NANOSECOND // The minimum programmable time: ~830 ns 

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _TIMER {
	unsigned int clock_Hz;
	void (*CurrentTime)(void);
	void (*CurrentDate)(void);

} __attribute__ ((packed))
TIMER, *PTIMER;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void timer_handler(PREGS r);
unsigned int RegisterTimer(PTIMER timer, unsigned int Hz);
void WaitMicroSecond(unsigned int usec);  
void WaitMiliSecond(unsigned int msec);  
void WaitSecond(unsigned int sec);   

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _TIMER__H__