#ifndef _CHORUS_H
#define _CHORUS_H

#include "globals.h"
#include "iirFilter.h"
#include "limiter.h"

struct chorus_params_t {
	double FF;
	double FB;
	double BL;
	int writeIDX;
	int readIDX;
}; 

void chorus(double depth, double rate, limiter_state* chorusLimiter);
void initChorus(void);

#endif
