#ifndef _GLOBALS_H
#define _GLOBALS_H

#define DELAY_LENGTH 9600
#define BUFFER_LENGTH 256
#define POT_BUFFER_LENGTH 8

// ADC PCG register settings
// (24.576 MHz)/(desired freq) = clock divider
// clk and fs are totally indpendent of one another
// frame sync should be every 16 clocks of sample clock as per data sheet
#define CLK_DIVIDER 768	
#define FS_DIVIDER 24576	

// buffer for storing floats	
// double float_buffer[BUFFER_LENGTH] = {0.0};

// buffer for storing floats	
extern double float_buffer[BUFFER_LENGTH];

// ------------------------ DMA buffers ----------------- //


// SPORT0 receive buffer a - also used for transmission
extern int rx0a_buf[BUFFER_LENGTH];

// SPORT1 transmit buffer	
extern int tx1a_buf[BUFFER_LENGTH];

// SPORT2 receive buffer for pot values
extern int rx2a_buf[POT_BUFFER_LENGTH];

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

extern int rx2a_tcb[8];			// SPSORT2 receive for pot values
	

// ------------------------ general globals ------------- //
// main processing index
extern int dsp;
// each effect plays hot potato with the eventual output sample
extern double potato;

// ------------------------ delay globals --------------- //
extern double potValue;
// delay buffer index
extern int delay_ptr;
// determines delay speed
extern int delay_counter;
// buffer for storing delay samples
extern double delay_buffer[2*DELAY_LENGTH];

void delay(int times);

#endif
