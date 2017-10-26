#include "chorus.h"


double chorusLimiterBuffer[DELAY_LINE_LENGTH] = {0.0};
int n = 0;

struct chorus_params_t c;

void initChorus(void) {
	c.FF = 0.5;
	c.FB = 0.0;
	c.BL = 0.5;
	c.writeIDX = 0;
	c.readIDX = 0;
}

void chorus(double depth, double rate, limiter_state* chorusLimiter) {

	double f;
	double phaseJump;
	double delayReach;

	f = (2*PI/Fs) * ((double)CHORUS_LFO_SPEED / MAX_POT_VAL) * rate;

	depth = ((double)CHORUS_LFO_AMP / MAX_POT_VAL) * depth;

	phaseJump =  depth * sin(f * n);

	// y = mx + b --> compress delay knob range then offset it
	//delayReach = (double) ( (CHORUS_LENGTH - 1 - 2*CHORUS_LFO_AMP) / (MAX_POT_VAL) ) * delayReach + (CHORUS_LFO_AMP);

	delayReach = 960 + phaseJump;

	delayReach = (CHORUS_LENGTH - 1 - delayReach);
	
	chorus_buffer[c.writeIDX] = iirFilter(potato + c.FB * chorus_buffer[c.readIDX]);

	// double limit(double signal, double threshold, double *delay_line, limiter_state *state)
	chorus_buffer[c.writeIDX] = limit(chorus_buffer[c.writeIDX], 0.8, chorusLimiterBuffer, chorusLimiter);

	potato = c.BL * potato + c.FF * chorus_buffer[c.readIDX];

	c.writeIDX = (c.writeIDX + 1) % CHORUS_LENGTH;

	c.readIDX = (c.writeIDX + (int)delayReach) % CHORUS_LENGTH;

	float_buffer[dsp] = potato;

	// n % (Fs/f)
	n = (n + 1) % (int)(Fs / (((double)CHORUS_LFO_SPEED / MAX_POT_VAL) * rate));
	
	return;
}
