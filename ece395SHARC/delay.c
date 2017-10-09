#include "delay.h"

// number of interpolation coefficients
#define N 3

double h[N] = {0.3750, 0.7500, -0.1250};
float d = 0.0;
float feedback = 0.0;
float FF = 0.7;
float FB = 0.5;
float BL = 1.0;
int writeIDX = 0;
int readIDX = 0;

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

void delayLagrangeNoFeedback(int potVal) {

	if (potVal <= 1)
		potVal = 2;

	d = d + (float)(potVal - d) / (48 * 200);

	//if (potVal == DELAY)

	int i = 0;
	double interpolated = 0.0;

	// load delay with current input (for next time)
	delay_buffer[delay_ptr] = float_buffer[dsp];

	// convolution of input (going forwards) and coeffs (going backwards)
	for (i = 0 ; i < N; i++)
		interpolated += delay_buffer[(delay_ptr - N/2 + (int)d + i) % DELAY_LENGTH] * h[N - 1 - i];

	// interpolated = delay_buffer[(delay_ptr + d) % DELAY_LENGTH];

	// add old data to current data
	float_buffer[dsp] = potato = delay_buffer[delay_ptr] + interpolated;

	delay_ptr = (delay_ptr + 1) % DELAY_LENGTH;

	return;
}

/**
  *  potArray is global. potArray[0] is delay time, potArray[1] is feedback
  *  
  * 
  * 
  */
void delayLagrangeWithFeedback(void) {

	//if (potVal <= 1)
	//	potVal = 2;

	if (potArray[0] < 30)
		potArray[0] = 30;

	if (potArray[1] < 30)
		potArray[1] = 30;

	// d is the continually updating value of delay, based on
	// a slope from the old value to a desired target value
	// note: the slope ensures that the target value will be
	// reached in 48 * 20 samples, which corresponds to 
	// 20ms at a sample rate of 48000 Hz
	float d0 = potArray[0] * 3.0;

	// printf("ch.0: %d\tch.1: %d\n", potArray[0], potArray[1]);
	d = d + (float)(d0 - d) / (48 * 300);

	feedback = ((float)potArray[1])/4095.0;


	int i = 0;
	double interpolated = 0.0;
	int dn = (delay_ptr + (int)d) % DELAY_LENGTH;

	// convolution of input (going forwards) and coeffs (going backwards)
	for (i = 0 ; i < N; i++)
		interpolated += delay_buffer[(delay_ptr - N/2 + (int)d + i) % DELAY_LENGTH] * h[N - 1 - i];

	// load delay with current input (for next time)
	delay_buffer[delay_ptr] = feedback * interpolated + float_buffer[dsp];

	// the following line works if lagrange interpolation is not desired
	// delay_buffer[delay_ptr] = feedback * delay_buffer[dn] + float_buffer[dsp];

	// add old data to current data
	float_buffer[dsp] = potato = delay_buffer[delay_ptr];

	delay_ptr = (delay_ptr + 1) % DELAY_LENGTH;

	return;
}

void delayFromIEEE(void) {
	FB = ((float)potArray[1])/4095.0;

	int delayVal = DELAY_LENGTH / 2;

	delay_buffer[writeIDX] = potato + FB * delay_buffer[readIDX];

	potato = BL * potato + FF * delay_buffer[readIDX];

	writeIDX = (writeIDX + 1) % DELAY_LENGTH;

	readIDX = (writeIDX + delayVal) % DELAY_LENGTH;

	return;
}

void potTesting(void) {

	//if (potVal <= 1)
	//	potVal = 2;

	// if (potArray[0] < 1)
	// 	potArray[0] = 1;

	// if (potArray[1] < 1)
	// 	potArray[1] = 1;


	float_buffer[dsp] = potato = float_buffer[dsp] * ((float)potArray[0] / 4095.0);

	// printf("potArray[0] math = %0.2f\n", ((float)potArray[0] / 4095.0));

	float_buffer[dsp] = potato = float_buffer[dsp] * ((4095.0 - (float)potArray[1]) / 4095.0);

	// printf("potArray[0] math = %0.2f\tpotArray[1] math = %0.2f\n", ((float)potArray[0] / 4095.0), ((4095.0 - (float)potArray[1])) / 4095.0);

	return;
}
