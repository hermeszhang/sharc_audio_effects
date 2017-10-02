#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <stdio.h>


// Precision Feedback Max Delay Length
// #define MAX_DELAY 4096

// 4096 = 2^12 = Pot Value Max
#define DELAY_LENGTH 32768

#define BUFFER_LENGTH 256
#define POT_BUFFER_LENGTH 2

// ADC PCG register settings
// (24.576 MHz)/(desired freq) = clock divider
// clk and fs are totally indpendent of one another
// frame sync should be every 16 clocks of sample clock as per data sheet
#define CLK_DIVIDER 768	
#define FS_DIVIDER 24576

#define NUM_POTS 2	

// buffer for storing floats	
// double float_buffer[BUFFER_LENGTH] = {0.0};

// buffer for storing floats	
extern double float_buffer[BUFFER_LENGTH];

// ------------------------ DMA buffers ----------------- //

// SPORT0 receive buffer a - also used for transmission
extern int rx0a_buf[BUFFER_LENGTH];

// SPORT1 transmit buffer	
extern int tx1a_buf[BUFFER_LENGTH];

/* 
TCB format:       ECx (length of destination buffer),
				  EMx (destination buffer step size),
				  EIx (destination buffer index (initialized to start address)),
				  GPx ("general purpose"),
				  CPx ("Chain Point register"; points to last address (IIx) of
			   								   next TCB to jump to
				                               upon completion of this TCB.),
				  Cx  (length of source buffer),
				  IMx (source buffer step size),
				  IIx (source buffer index (initialized to start address)) 
*/

extern int rx0a_tcb[8];				// SPORT0 receive a tcb from SPDIF
extern int tx1a_tcb[8];				// SPORT1 transmit a tcb to DAC

extern int tx1a_delay_tcb[8];	// SPORT1 transmit a tcb to DAC	

// ------------------------ general globals ------------- //
// main processing index
extern int dsp;
// each effect plays hot potato with the eventual output sample
extern double potato;

// ------------------------ delay globals --------------- //
extern int potValue;
extern int potValuePrev1;
extern int potValuePrev2;

extern int potArray[NUM_POTS];

// delay buffer index
extern int delay_ptr;
// determines delay speed
extern int delay_counter;
// buffer for storing delay samples
extern double delay_buffer[DELAY_LENGTH];

void delay(int times);

#endif
