#ifndef _DELAY_H
#define _DELAY_H

#include "globals.h"
#include "iirFilter.h"
#include "limiter.h"

void delayHarmonicWithFeedback(int delaySpeed);
void delayFromIEEE(double delayVal, double feedback, limiter_state* delayLimiter);
double delayLFO(double delayVal, double feedbackIn, limiter_state* delayLimiter, double rate);
int checkButton(void);
void timeoutDelayButton(void);
void initDelayButton(void);
double getButtonDelayReach(void);

#endif
