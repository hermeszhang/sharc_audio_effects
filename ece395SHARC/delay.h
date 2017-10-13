#ifndef _DELAY_H
#define _DELAY_H

#include "globals.h"
#include "iirFilter.h"

void delayHarmonicWithFeedback(int delaySpeed);
void delayLagrangeWithFeedback(void);
void potTesting(void);
void delayFromIEEE(double delayVal, double feedback);

#endif
