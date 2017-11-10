#include "globals.h"

double float_buffer[BUFFER_LENGTH] = {0.0};

int dsp = 0;
double potato = 0.0;

// delay 
int delay_ptr = 0;
int delay_counter = 0;
double delay_buffer[DELAY_LENGTH] = {0.0};

// chorus 
int chorus_ptr = 0;
double chorus_buffer[CHORUS_LENGTH] = {0.0};

int rx0a_buf[BUFFER_LENGTH] = {0};
int tx1a_buf[BUFFER_LENGTH] = {0};

int rx0a_tcb[8]  = {0,0,0,0, 0, BUFFER_LENGTH, 1, 0};				
int tx1a_tcb[8]  = {0,0,0,0, 0, BUFFER_LENGTH, 1, 0};				

int tx1a_delay_tcb[8]  = {0, 0, 0, 0, 0, BUFFER_LENGTH/2, 1, 0};	
	
int auxDivisor = 0;

int potValue = 0;
int potValuePrev1 = 0;
int potValuePrev2 = 0;

double potArray[NUM_POTS] = {0};

//double debugF[TOGGLE_TIME * 2] = {0.0};

void delay(int times)
{
	int i;

    for(i = times; i > 0; --i)
    	asm("nop;");
}
