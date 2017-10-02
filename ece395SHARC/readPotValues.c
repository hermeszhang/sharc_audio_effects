#include "readPotValues.h"

int retVal;

int readPotValues(void)
{
	// also triggers our counter chip to increment the mux
	SELECT_SPI_SLAVE(DS1EN);

	// dummy write to get SPI going
	*pTXSPI = 0x0;
	
	//Wait for the SPI to indicate that it has finished.
    while ((*pSPISTAT & RXS))

    //Wait for the SPI to indicate that it has finished.
    while (!(*pSPISTAT & SPIFE))

   	retVal = *pRXSPI;

    delay(10);

	DESELECT_SPI_SLAVE(DS1EN);

	//delay(10000);

	return retVal;
}
