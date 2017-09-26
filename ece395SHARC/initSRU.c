#include "initSRU.h"

void clearDAIpins(void);

void initSRU() {

	clearDAIpins();
	
	// use pin 12 on the board for SPDIF in
	// this is the pin second from the power,
	// in the not-ground row
	// DAI pin buffer 12 output to SPDIF receiver as input
	SRU(LOW, DAI_PB12_I);
	SRU(LOW, PBEN12_I);
	SRU(DAI_PB12_O, DIR_I);

	//Power off the DAC
	SRU2(HIGH, DPI_PBEN04_I);
	SRU2(LOW, DPI_PB04_I);
	
	delay(10);
	 
	/* ---------- DAC ----------------- */
    //Attach Main Clocks from SPDIF receiver

	//MCLK
	SRU(DIR_TDMCLK_O, DAI_PB05_I); 
	SRU(HIGH,PBEN05_I);
	
	//BICK
	SRU(DIR_CLK_O, DAI_PB06_I);
	SRU(HIGH,PBEN06_I);
	
	//LRCK
	SRU(DIR_FS_O, DAI_PB03_I);
	SRU(HIGH,PBEN03_I);
	
	//CSN
	SRU2(SPI_FLG0_O, DPI_PB07_I);
    SRU2(SPI_FLG0_PBEN_O, DPI_PBEN07_I);

	//Set MOSI/CDT1 to output
	SRU2(SPI_MOSI_O, DPI_PB01_I);
	SRU2(HIGH, DPI_PBEN01_I);
	
	//Send SPI clock to DPI 3
	SRU2(SPI_CLK_O, DPI_PB03_I);
	SRU2(SPI_CLK_PBEN_O, DPI_PBEN03_I);
	
	//Power back on the DAC
	SRU2(HIGH, DPI_PB04_I);

	delay(10);  

	/* ---------- SPORT -------------- */
	SRU(DAI_PB06_O, SPORT0_CLK_I);
	SRU(DAI_PB06_O, SPORT1_CLK_I);
	SRU(DAI_PB03_O, SPORT0_FS_I);
	SRU(DAI_PB03_O, SPORT1_FS_I);

	// SPORT0 receives from SPDIF 
	SRU(DIR_DAT_O, SPORT0_DA_I);

	// SPORT1 outputs to the DAC
	SRU(SPORT1_DA_O, DAI_PB04_I);
	SRU(HIGH, PBEN04_I);

	/* --------- ADC -------------------------------------------- */

	/* -- outputs from SHARC to inputs on ADC board -- */

	//CSN
	SRU2(SPI_FLG1_O, DPI_PB08_I);
    SRU2(SPI_FLG1_PBEN_O, DPI_PBEN08_I);

	//Set MOSI/CDT1 to output
	SRU2(DPI_PB10_O, SPI_MISO_I);
	SRU2(LOW, DPI_PBEN10_I);
	
	//Send SPI clock to DPI 3
	SRU2(SPI_CLK_O, DPI_PB11_I);
	SRU2(SPI_CLK_PBEN_O, DPI_PBEN11_I);

	// 3 select values for analog mux

	// SRU(PCG_FSA_O, SPORT3_DA_I)

	// SELECT A
	SRU(LOW, DAI_PB11_I);
	SRU(HIGH, DAI_PBEN11_I);

	// SELECT B
	SRU(LOW, DAI_PB10_I);
	SRU(HIGH, DAI_PBEN10_I);

	// SELECT C
	SRU(LOW, DAI_PB13_I);
	SRU(HIGH, DAI_PBEN13_I);	
	
	/* ---------- DEBUG -------------- */
	
	// SRU(SPI_CLK_O, DAI_PB09_I);
	// SRU(HIGH, PBEN09_I);

}

void clearDAIpins(void) {

//  Tie the pin buffer inputs LOW for all DAI pins.  Even though
//  these pins are inputs to the SHARC, tying unused pin buffer inputs
//  LOW is "good coding style" to eliminate the possibility of
//  termination artifacts internal to the IC.  Note that signal
//  integrity is degraded only with a few specific SRU combinations.
//  In practice, this occurs VERY rarely, and these connections are
//  typically unnecessary.  This is GROUP D

    SRU(LOW, DAI_PB01_I);
    SRU(LOW, DAI_PB02_I);
    SRU(LOW, DAI_PB03_I);
    SRU(LOW, DAI_PB04_I);
    SRU(LOW, DAI_PB05_I);
    SRU(LOW, DAI_PB06_I);
    SRU(LOW, DAI_PB07_I);
    SRU(LOW, DAI_PB08_I);
    SRU(LOW, DAI_PB09_I);
    SRU(LOW, DAI_PB10_I);
    SRU(LOW, DAI_PB11_I);
    SRU(LOW, DAI_PB12_I);
    SRU(LOW, DAI_PB13_I);
    SRU(LOW, DAI_PB14_I);
    SRU(LOW, DAI_PB15_I);
    SRU(LOW, DAI_PB16_I);
    SRU(LOW, DAI_PB17_I);
    SRU(LOW, DAI_PB18_I);
    SRU(LOW, DAI_PB19_I);
    SRU(LOW, DAI_PB20_I);

//  Tie the pin buffer enable inputs LOW for all DAI pins so
//  that they are always input pins.  This is GROUP F.

    SRU(LOW, PBEN01_I);
    SRU(LOW, PBEN02_I);
    SRU(LOW, PBEN03_I);
    SRU(LOW, PBEN04_I);
    SRU(LOW, PBEN05_I);
    SRU(LOW, PBEN06_I);
    SRU(LOW, PBEN07_I);
    SRU(LOW, PBEN08_I);
    SRU(LOW, PBEN09_I);
    SRU(LOW, PBEN10_I);
    SRU(LOW, PBEN11_I);
    SRU(LOW, PBEN12_I);
    SRU(LOW, PBEN13_I);
    SRU(LOW, PBEN14_I);
    SRU(LOW, PBEN15_I);
    SRU(LOW, PBEN16_I);
    SRU(LOW, PBEN17_I);
    SRU(LOW, PBEN18_I);
    SRU(LOW, PBEN19_I);
    SRU(LOW, PBEN20_I);
}
