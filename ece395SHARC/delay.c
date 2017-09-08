#include "delay.h"

void delayHarmonicWithFeedback(int delaySpeed) 
{
	// delay_ptr is putting what rx just took in into the delay_buffer.
	// once the delay length is satisfied, dsp pointer is adding to the receive buffer what rx
	// already put there PLUS what's just ahead of where delay_ptr is now. this way, 
	// the desired delay time is satisfied constantly.


	delaySpeed = (int)(delaySpeed/127);

	if (delaySpeed > 0)
	 delay_counter =  (delay_counter + 1) % delaySpeed;

	if  (delay_counter == 0) {

		//*** write current input + attenuated delay into delay buffer at delay_ptr
		delay_buffer[delay_ptr] = 0.5*delay_buffer[delay_ptr] + float_buffer[dsp];

		//*** increment delay_ptr - now points to oldest spot in delay buffer
		delay_ptr = (delay_ptr + 1)%DELAY_LENGTH;

		//*** put the oldest delay value (from 1 delay ago) into receive float buffer
	    float_buffer[dsp] = potato = delay_buffer[ delay_ptr];
	}
	

    return;
}


void delayPrecisionFeedback(int delaySpeed)  {
	//	delaySpeed is the modulus for the delay_buffer
	if (delaySpeed < 100)
		delaySpeed = 100;
	
	// load delay buffer with current input, used for wrap around delay
	delay_buffer[delay_ptr] = (0.5 * delay_buffer[delay_ptr]) + float_buffer[dsp];

	//  add delay to the current input, that's your output
	float_buffer[dsp] = potato = delay_buffer[delay_ptr] + delay_buffer[(delay_ptr + 1) % delaySpeed];

	delay_ptr = (delay_ptr + 1) % delaySpeed;

	return;
}
