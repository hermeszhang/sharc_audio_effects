#ifndef _GLOBALS_H
#define _GLOBALS_H

#define DELAY_LENGTH 9600

#define BUFFER_LENGTH 256

// buffer for storing floats	
// double float_buffer[BUFFER_LENGTH] = {0.0};

// // ------------------------ general globals ------------- //
// // main processing index
// int dsp = 0;
// // when each effect plays hot potato with the eventual output sample
// double potato = 0.0;

// // ------------------------ delay globals --------------- //
// // delay buffer index
// int delay_ptr = 0;
// // determines delay speed
// int delay_counter = 0;
// // buffer for storing delay samples
// double delay_buffer[2*DELAY_LENGTH] = {0};

// // ------------------------ downSample globals --------------- //

// unsigned int idx = 0;

// buffer for storing floats	
extern double float_buffer[BUFFER_LENGTH];

// ------------------------ general globals ------------- //
// main processing index
extern int dsp;
// when each effect plays hot potato with the eventual output sample
extern double potato;

// ------------------------ delay globals --------------- //
// delay buffer index
extern int delay_ptr;
// determines delay speed
extern int delay_counter;
// buffer for storing delay samples
extern double delay_buffer[2*DELAY_LENGTH];

// ------------------------ downSample globals --------------- //

extern unsigned int idx;

#endif
