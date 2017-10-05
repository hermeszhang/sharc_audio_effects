#include "pingCounter.h"

void pingCounter() {

	delay(10);
	SRU(HIGH, DAI_PB10_I);

	delay(10);
	SRU(LOW, DAI_PB10_I);
	
}
