#ifndef _DELAY_H
#define _DELAY_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DELAY_LENGTH 9600

void delayWithFeedback(int delaySpeed, int * delayCounter, double * delayBuffer, double * floatBuffer, int * delayPtr, 
					   double * potato, int * dsp);


#endif
