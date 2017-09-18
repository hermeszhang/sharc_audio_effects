#include "format.h"

void formatInput(void){

	// format audio input

	if (rx0a_buf[dsp] & 0x00800000)			// is rx negative?
		rx0a_buf[dsp] |= 0xFF000000;		// sign-extend it

    float_buffer[dsp] = potato = (double)rx0a_buf[dsp];

    // format potentiometer input
    potValue = (int)rx2a_buf[0];	

    return;
}

void formatOutput(void){

	tx1a_buf[dsp] = (int)potato;
	tx1a_buf[dsp] &= 0x00FFFFFF;	

	dsp = (dsp + 1)%BUFFER_LENGTH;

	return;
}
