#include "configAK4396.h"

void configAK4396(unsigned int address, unsigned int data)
{
    unsigned short message = 0;

    SELECT_SPI_SLAVE(DS0EN);

    /// MSB                                LSB
    // [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]

    //Read/Write (stay at 1, write only, as per diagram).
    message |= 0x2000; //message[3]
    
    //Register Address
    message |= (address << 8);
        
    message |= data;
    
    *pTXSPI = message;

    //Wait for the SPI to indicate that it has finished.
    while ((*pSPISTAT & TXS))

    //Wait for the SPI to indicate that it has finished.
    while (!(*pSPISTAT & SPIFE))
    
	delay(10);
    
    DESELECT_SPI_SLAVE(DS0EN);

}
