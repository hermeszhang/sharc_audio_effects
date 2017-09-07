#include "firFilter.h"
#include "coeffs.h"

void firFilter() {

	double acc = 0.0;
	int i = 0;

	// convolution of input (going bakcwards) and coeffs (going forward)
	for (i = 0 ; i < FILTER_LENGTH ; i++)
		acc += float_buffer[ (dsp - i + BUFFER_LENGTH) % BUFFER_LENGTH ] * coeffs[i];

	potato = acc;

	return;              
}
