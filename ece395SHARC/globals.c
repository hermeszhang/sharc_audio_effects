#include "globals.h"

double float_buffer[BUFFER_LENGTH] = {0.0};

int dsp = 0;
double potato = 0.0;

int delay_ptr = 0;
int delay_counter = 0;
double delay_buffer[2*DELAY_LENGTH] = {0.0};

int rx0a_buf[BUFFER_LENGTH] = {0};
int tx1a_buf[BUFFER_LENGTH] = {0};
int rx2a_buf[POT_BUFFER_LENGTH] = {0};

int rx0a_tcb[8]  = {0,0,0,0, 0, BUFFER_LENGTH, 1, 0};				
int tx1a_tcb[8]  = {0,0,0,0, 0, BUFFER_LENGTH, 1, 0};				

int tx1a_delay_tcb[8]  = {0, 0, 0, 0, 0, BUFFER_LENGTH/2, 1, 0};	

int rx2a_tcb[8]  = {0,0,0,0, 0, POT_BUFFER_LENGTH, 1, 0};
	
int auxDivisor = 0;

double potValue = 0.0;

void delay(int times)
{
	int i;

    for(i = times; i > 0; --i)
    	asm("nop;");
}
