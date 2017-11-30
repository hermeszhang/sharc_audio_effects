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
tap_button_state b;

// for computing sin
int n_delay = 0;
SinCos sinCosResult;

double ra_debug[1000] = {0.0};
int ra_idx = 0;

int button_debug = 0;

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

double delayLFO(double delayVal, double feedbackIn, limiter_state* delayLimiter, double rate) {

	int i = 0;
	double interpolated = 0.0;
	double f;
	double phaseJump;
	int max = MAX_POT_VAL;

	if (delayVal > MAX_POT_VAL * 0.99) {
		delayVal = MAX_POT_VAL * 0.99;
	}
	
	// compute lfo sin stuff
	if (rate < 20) {
		phaseJump = 0;
		rate = 10;
	}
	else {
		f = (2*PI/Fs) * (MAX_LFO_SPEED / MAX_POT_VAL) * rate;
		sinCos(f * n_delay, &sinCosResult);
		phaseJump =  MAX_LFO_AMP * sinCosResult.sin;
	}
	
	/*** CHECK IF DELAYVAL IS DRASTICALLY DIFFERENT FROM ITS PREVIOUS VALUE, WHICH WE SHOULD STORE IN DSTRUCT ***/
	/*** that would take care of slewing between both button sessions, and a switch between control modes (button <-> knob) ***/

	// compress range of delay for LFO 
	delayVal = ((double)(DELAY_LENGTH - 2*MAX_LFO_AMP) / (DELAY_LENGTH) ) * delayVal + (MAX_LFO_AMP);

	// add lfo value for modulation
	// delayVal += phaseJump;
	delayVal = (max - delayVal);

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
	
	n_delay = (n_delay + 1) % (int)(Fs / ((MAX_LFO_SPEED / MAX_POT_VAL) * rate));

	return 0;
}

void initDelayButton(void) {
	b.button_val_prev = 4095;
	b.num_pushes = 0;
	b.running_average = 0.0;
	b.timeout = 1.1*((double)DELAY_LENGTH/Fs);
	b.clock_old = clock();
	b.alpha = 0.4;
	b.timeout_flag = 0;
}

void timeoutDelayButton(void) {
	b.button_val_prev = 4095;
	b.num_pushes = 0;
	b.timeout = 1.1*((double)DELAY_LENGTH/Fs);
	b.clock_old = clock();
	b.alpha = 0.4;
	b.timeout_flag = 1;
}

int checkButton(void) {
	double delta_t;
	int retFlag = 0;

	b.clock_new = clock();

	delta_t	= ((double) (b.clock_new - b.clock_old)) / CLOCKS_PER_SEC;

	// if the time elapsed is greater than timeout
	if (delta_t >= b.timeout) {
		timeoutDelayButton();
		return retFlag;
	}

	// if button is pressed (falling edge)
	if ((b.button_val_prev > 4000) && (potArray[3] < 500)) {
		b.clock_old = b.clock_new;

		b.num_pushes++;
		if (b.num_pushes >= 2) {
			if (b.timeout_flag) {
				b.running_average = delta_t;
				b.timeout_flag = 0;
			}
			b.running_average = b.alpha * b.running_average + (1 - b.alpha) * delta_t;
			retFlag = 1;
		}
	}

	// hold onto previous state so we know if falling edge occurs
	b.button_val_prev = potArray[3];

	ra_debug[ra_idx] = b.running_average;
	ra_idx = (ra_idx + 1) % 1000;
	return retFlag;
}

double getButtonDelayReach(){
	if (b.running_average > ((double)(DELAY_LENGTH-1)/Fs))
        b.running_average = (double)(DELAY_LENGTH-1)/Fs;

	return (double)b.running_average * (double)Fs;
}
