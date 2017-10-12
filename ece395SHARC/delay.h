#ifndef _DELAY_H
#define _DELAY_H

#include "globals.h"

void delayHarmonicWithFeedback(int delaySpeed);
void delayPrecisionFeedback(int delaySpeed);
void delayLagrangeNoFeedback(int potVal);
void delayLagrangeWithFeedback(void);
void potTesting(void);
void delayFromIEEE(double delayVal, double feedback);

#endif
