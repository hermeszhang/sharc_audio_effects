#include "initSPI.h"


void initSPI(unsigned int SPI_Flag)
{
	// Configure the SPI Control registers
    // First clear a few registers
    *pSPICTL  =(TXFLSH | RXFLSH) ; //Clear TXSPI and RXSPI
    *pSPIFLG = 0; //Clear the slave select

    //BAUDR is bits 15-1 and 0 is reserved
    *pSPIBAUD = 25;   //SPICLK baud rate = PCLK / (4*BAUDR)
    //PCLK = 200MHz and 200MHz/100 = 2 MHz? - double check this...

    // Setup the SPI Flag register using the Flag specified in the call
    *pSPIFLG = (0xF00|SPI_Flag);

    // Now setup the SPI Control register
    *pSPICTL = (SPIEN | SPIMS | WL16 | MSBF | TIMOD1 | CLKPL | GM);
    
    // SPIEN - SPI System Enable
    // SPIMS - Master Slave Mode Bit - 1 indicates we are a master device
    // WL16 - Word length is 16 bits
    // MSBF - MSB sent/received first
    // (NOT) CPHASE - SPICLK starts toggling at the middle of 1st data bit
    // CLKPL - Clock Polarity - Active low SPICLK (SPICLK high is the idle state)
    // (NOT) SMLS - Seamless Transfer disabled - delay before the next word starts, done to ensure frame on osciliscope, but check if you can change this.
    // GM - Get Data. When RXSPI is full, get more data which overwrites previous data.
}

void initADCSPI(unsigned int SPI_Flag)
{
    // Configure the SPI Control registers
    // First clear a few registers
    *pSPICTL = (TXFLSH | RXFLSH) ; //Clear TXSPI and RXSPI
    *pSPIFLG = 0; //Clear the slave select

    //BAUDR is bits 15-1 and 0 is reserved
    *pSPIBAUD = 25;   //SPICLK baud rate = PCLK / (4*BAUDR)
    //PCLK = 200MHz and 200MHz/100 = 2 MHz? - double check this...

    // Setup the SPI Flag register using the Flag specified in the call
    *pSPIFLG = (0xF00|SPI_Flag);

    // Now setup the SPI Control register
    *pSPICTL = (SPIEN | SPIMS | WL16 | MSBF | CLKPL | GM);
    
    // SPIEN - SPI System Enable
    // SPIMS - Master Slave Mode Bit - 1 indicates we are a master device
    // WL16 - Word length is 16 bits
    // MSBF - MSB sent/received first
    // (NOT) CPHASE - SPICLK starts toggling at the middle of 1st data bit
    // CLKPL - Clock Polarity - Active low SPICLK (SPICLK high is the idle state)
    // (NOT) SMLS - Seamless Transfer disabled - delay before the next word starts, done to ensure frame on osciliscope, but check if you can change this.
    // GM - Get Data. When RXSPI is full, get more data which overwrites previous data.
}
