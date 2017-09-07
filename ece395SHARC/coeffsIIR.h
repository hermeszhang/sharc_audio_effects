#ifndef _COEFFSIIR_H
#define _COEFFSIIR_H

double coeffsIIR[10] = {

	-0.716590,
	0.000000,
	1.000000,
	1.000000,
	0.000000,
	-1.631626,
	0.723069,
	1.000000,
	2.000000,
	1.000000

};

//cutoff = 5000
double gain = 0.00323948552845727780;
int stages = 2;
double history[4] = {0.0};

#endif
