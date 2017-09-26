#include "muxSelect.h"
	
int b0, b1, b2;

void muxSelect(int selectCounter) {

	b0 = selectCounter & 0x00000001;
	b1 = (selectCounter & 0x00000002) >> 1;
	b2 = (selectCounter & 0x00000004) >> 2;
	
	//SRU(LOW, DAI_PB11_I);
	//SRU(LOW, DAI_PB10_I);
	//SRU(LOW, DAI_PB13_I);
	
	if (b0)
		SRU(HIGH, DAI_PB11_I);
	else
		SRU(LOW, DAI_PB11_I);

	if (b1)
		SRU(HIGH, DAI_PB10_I);
	else
		SRU(LOW, DAI_PB10_I);

	if (b2)
		SRU(HIGH, DAI_PB13_I);
	else
		SRU(LOW, DAI_PB13_I);
	
	


	// printf("b2b1b0 = %d%d%d\n", b2, b1, b0);

}
