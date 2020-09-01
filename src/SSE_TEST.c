#include "../include/SSE.h"
#include "../include/PRINT.h"

void test_see(void)
{
	float src[4] __attribute__((aligned(16)));
	float trg[4] __attribute__((aligned(16)));
	
	src[0] = 8.0; 
	src[1] = 2.0; 
	src[2] = -23.0; 
	src[3] = 14.0; 
	
	trg[0] = 0.0; 
	trg[1] = 0.0; 
	trg[2] = 0.0; 
	trg[3] = 0.0; 
	
	_test(trg, src);
	
	printk("%,%,%,%\n", (unsigned int)trg[0], (unsigned int)trg[1], (unsigned int)trg[2], (unsigned int)trg[3]);
	
	return;
}