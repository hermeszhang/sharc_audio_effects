#ifndef _DELAY_H
#define _DELAY_H

#include "globals.h"
#include "iirFilter.h"
#include "limiter.h"

void delayHarmonicWithFeedback(int delaySpeed);
void delayFromIEEE(double delayVal, double feedback, limiter_state* delayLimiter);
void delayLFO(double delayVal, double feedbackIn, limiter_state* delayLimiter, double lfo);


#endif
