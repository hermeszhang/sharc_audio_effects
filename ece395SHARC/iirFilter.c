#include "iirFilter.h"
#include "coeffsIIR.h"

double iirFilter(double x) {

	int i;
	
	// accumulators for IIR filter
	double acc_iir = 0.0;
	double new_hist = 0.0;

	// the two delay values in each stage
	// turns out things have to be initialized - didn't and errors ensued
	double history1 = 0.0;
	double history2 = 0.0;

	// pointer to beginning of coeffsIIR array
	double * coeffPtr = coeffsIIR;

	// pointers to beginning two spots of history array
	double * hist1Ptr = history;
	double * hist2Ptr = history + 1;

	// initial gain before going through each 2nd order IIR stage
	acc_iir = gain * x;

	for(i = 0; i < stages; i++ ) {

		// fill delay elements with current stage's history values
		history1 = *hist1Ptr;
		history2 = *hist2Ptr;

		//printf("history1 = %f\n",history1);
		//printf("acc_iir = %f\n",acc_iir);

		// poles 
		acc_iir = acc_iir - (*coeffPtr++) * (history1);
		new_hist = acc_iir - (*coeffPtr++) * (history2);

		// zeros
		acc_iir = acc_iir * (*coeffPtr++);
		acc_iir = acc_iir + (*coeffPtr++) * (history1);
		acc_iir = acc_iir + (*coeffPtr++) * (history2);

		//printf("acc_iir = %f\n",acc_iir);

		// shift in new delay element values for this stage
		*hist2Ptr++ = *hist1Ptr;
		*hist1Ptr++ = new_hist;

		// leap frog over by two to be at right spot in next stage
		hist1Ptr++;
		hist2Ptr++;
	}
	
	// output accumulator contains the filtered sample for output
	// potato = acc_iir;

	return acc_iir;
}
