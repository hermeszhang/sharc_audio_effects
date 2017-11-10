#include "firFilter.h"
#include "coeffs.h"

// void firFilter() {

// 	double acc = 0.0;
// 	int i = 0;

// 	// convolution of input (going bakcwards) and coeffs (going forward)
// 	for (i = 0 ; i < FILTER_LENGTH ; i++)
// 		acc += float_buffer[ (dsp - i + BUFFER_LENGTH) % BUFFER_LENGTH ] * coeffs[i];

// 	potato = acc;

// 	return;              
// }

double fir_filter(double input, double * history, int history_idx) {

	double acc = 0.0;
	int i = 0;

	history[history_idx] = input;

	// convolution of input (going bakcwards) and coeffs (going forward)
	for (i = 0 ; i < FILTER_LENGTH; i++)
		acc += history[ (history_idx - i + FILTER_LENGTH) % FILTER_LENGTH ] * coeffs[i];

	return acc;
}
