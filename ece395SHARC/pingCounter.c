#include "pingCounter.h"

void pingCounter() {

	SRU(HIGH, DAI_PB10_I);

	delay(10);
	SRU(LOW, DAI_PB10_I);
	
}
