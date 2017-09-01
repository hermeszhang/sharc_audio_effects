#include "delay.h"

// ----------------------- FEEDBACK DELAY --------------------- //

void delayWithFeedback(int delaySpeed, int * delayCounter, double * delayBuffer, double * floatBuffer, int * delayPtr, 
					   double * potato, int * dsp)
{
	// delayPtr is putting what rx just took in into the delayBuffer.
	// once the delay length is satisfied, dsp pointer is adding to the receive buffer what rx
	// already put there PLUS what's just ahead of where delayPtr is now. this way, 
	// the desired delay time is satisfied constantly.
	delaySpeed = (int)(delaySpeed/127);

	if (delaySpeed > 0)
		*delayCounter =  (*delayCounter + 1) % delaySpeed;

	if  (*delayCounter == 0) {

		//*** write current input + attenuated delay into delay buffer at delayPtr
		delayBuffer[*delayPtr] = 0.5*delayBuffer[*delayPtr] + floatBuffer[*dsp];

		//*** increment delayPtr - now points to oldest spot in delay buffer
		*delayPtr = (*delayPtr + 1)%DELAY_LENGTH;

		//*** put the oldest delay value (from 1 delay ago) into receive float buffer
	    floatBuffer[*dsp] = *potato = delayBuffer[*delayPtr];

	    // printf("delayBuffer[%d] = %f\n", *delayPtr ,delayBuffer[*delayPtr]);
	    // printf("delay: floatBuffer[%d] = %f\n", *dsp , floatBuffer[*dsp]);
	}

    return;
}
