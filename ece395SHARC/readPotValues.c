#include "readPotValues.h"

int retVal;

int readPotValues(void)
{
	SELECT_SPI_SLAVE(DS1EN);

	// dummy read, necessary to initiate spi transfer 
	retVal = *pRXSPI;
	
	//Wait for the SPI to indicate that it has finished.
    while ((*pSPISTAT & RXS))

    //Wait for the SPI to indicate that it has finished.
    while (!(*pSPISTAT & SPIFE))

    delay(10);

	retVal = *pRXSPI;

	DESELECT_SPI_SLAVE(DS1EN);

	return retVal;
}
