#ifndef _FIRFILTER_H
#define _FIRFILTER_H

#include "globals.h"

//void firFilter(void);
double fir_filter(double input, double * history, int history_idx);

#endif
