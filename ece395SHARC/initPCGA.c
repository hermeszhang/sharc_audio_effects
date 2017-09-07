#include "initPCGA.h"

void initPCGA(void) {

	// in the .h file is the following:
	// #define CLKASOURCE  (BIT_31)        /*External CLK A Source */
	// CLK_DIVIDER defined above

	*pPCG_CTLA0 = ENCLKA | ENFSA | FS_DIVIDER; 

	*pPCG_CTLA1 = CLK_DIVIDER;
}
