#include "delay.h"

// number of interpolation coefficients
#define N 3

double h[N] = {0.3750, 0.7500, -0.1250};
double d = 0.0;
double feedback = 0.0;
double FF = 0.5;
double FB = 0.5;
double BL = 0.5;
int writeIDX = 0;
int readIDX = 0;
double delayLine[DELAY_LINE_LENGTH] = {0.0};


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


// delay with no LFO, otherwise has all bells and whistles
void delayFromIEEE(double delayVal, double feedbackIn, limiter_state* delayLimiter) {
	
	int i = 0;
	double interpolated = 0.0;

	delayVal = (4095.0*2.0 - delayVal);
	// delayVal += 0.0*4096.0;

	// if (delayVal < 5000)
	// 	delayVal = 5000;

	FB = feedbackIn/(4095.0*2.0);
	// FB = 0.7;

	// convolution of input (going forwards) and coeffs (going backwards)
	// for (i = 0 ; i < N; i++)
	// 	interpolated += delay_buffer[(readIDX - N/2 + i) % DELAY_LENGTH] * h[N - 1 - i];
	
	delay_buffer[writeIDX] = iirFilter(potato + FB * delay_buffer[readIDX]);
	// delay_buffer[writeIDX] = potato + FB * delay_buffer[readIDX];

	// double limit(double signal, double threshold, double *delay_line, limiter_state *state)
	delay_buffer[writeIDX] = limit(delay_buffer[writeIDX], 0.8, delayLine, delayLimiter);

	potato = BL * potato + FF * delay_buffer[readIDX];

	// delay_buffer[writeIDX] = potato + FB * interpolated;

	// potato = BL * potato + FF * interpolated;

	writeIDX = (writeIDX + 1) % DELAY_LENGTH;

	readIDX = (writeIDX + (int)delayVal) % DELAY_LENGTH;

	float_buffer[dsp] = potato;
	
	return;
}

void delayLFO(double delayVal, double feedbackIn, limiter_state* delayLimiter, double lfo) {
	
	int i = 0;
	double interpolated = 0.0;

	// y = mx + b --> compress delay knob range then offset it
	delayVal = (double) ( (MAX_POT_VAL - 2*MAX_LFO_AMP) / (MAX_POT_VAL) ) * delayVal + (MAX_LFO_AMP);

	delayVal += lfo;
	delayVal = (MAX_POT_VAL - delayVal);
	// delayVal += 0.0*4096.0;

	FB = feedbackIn/(MAX_POT_VAL);

	// convolution of input (going forwards) and coeffs (going backwards)
	// for (i = 0 ; i < N; i++)
	// 	interpolated += delay_buffer[(readIDX - N/2 + i) % DELAY_LENGTH] * h[N - 1 - i];
	
	delay_buffer[writeIDX] = iirFilter(potato + FB * delay_buffer[readIDX]);
	// delay_buffer[writeIDX] = potato + FB * delay_buffer[readIDX];

	// double limit(double signal, double threshold, double *delay_line, limiter_state *state)
	delay_buffer[writeIDX] = limit(delay_buffer[writeIDX], 0.8, delayLine, delayLimiter);

	potato = BL * potato + FF * delay_buffer[readIDX];

	writeIDX = (writeIDX + 1) % DELAY_LENGTH;

	readIDX = (writeIDX + (int)delayVal) % DELAY_LENGTH;

	float_buffer[dsp] = potato;
	
	return;
}
